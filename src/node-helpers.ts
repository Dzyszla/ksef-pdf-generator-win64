import { execFileSync } from 'child_process';
import { existsSync, readFileSync } from 'fs';
import { basename, isAbsolute, join } from 'path';
import { xml2js } from 'xml-js';
import { generateFA1 } from './lib-public/FA1-generator';
import { Faktura as Faktura1 } from './lib-public/types/fa1.types';
import { generateFA2 } from './lib-public/FA2-generator';
import { Faktura as Faktura2 } from './lib-public/types/fa2.types';
import { generateFA3 } from './lib-public/FA3-generator';
import { Faktura as Faktura3 } from './lib-public/types/fa3.types';
import { generateFARR } from './lib-public/FARR-generator';
import { Faktura as FakturaRR } from './lib-public/types/faRR.types';
import { stripPrefix } from './shared/XML-parser';
import { AdditionalDataTypes } from './lib-public/types/common.types';
import pdfMake from 'pdfmake/build/pdfmake';
import pdfFonts from 'pdfmake/build/vfs_fonts';
import { Upo } from './lib-public/types/upo-v4_2.types';
import { TDocumentDefinitions } from 'pdfmake/interfaces';
import { generateStyle } from './shared/PDF-functions';
import { generateNaglowekUPO } from './lib-public/generators/UPO4_2/Naglowek';
import { generateDokumentUPO } from './lib-public/generators/UPO4_2/Dokumenty';
import { Position } from './shared/enums/common.enum';
import { configureFonts } from './lib-public/configure-fonts';

type FontStyle = 'normal' | 'bold' | 'italics' | 'bolditalics';

interface WindowsFontRegistryEntry {
  name: string;
  fileName: string;
}

// Inicjalizacja pdfmake z czcionkami (wymagane dla Node.js)
(pdfMake as any).addVirtualFileSystem(pdfFonts);

function getWindowsFontRegistryEntries(registryPath: string): WindowsFontRegistryEntry[] {
  try {
    const output = execFileSync('reg.exe', ['query', registryPath], {
      encoding: 'utf8',
      windowsHide: true,
      stdio: ['ignore', 'pipe', 'ignore'],
    });

    return output
      .split(/\r?\n/)
      .map(line => line.match(/^\s*(.+?)\s{2,}REG_\w+\s{2,}(.*?)\s*$/))
      .filter((match): match is RegExpMatchArray => match !== null)
      .map(match => ({ name: match[1], fileName: match[2] }));
  } catch {
    return [];
  }
}

function getWindowsFontStyle(registryName: string, familyName: string): FontStyle | undefined {
  const nameWithoutType = registryName.replace(/\s+\([^)]*\)\s*$/, '').trim();
  if (nameWithoutType.localeCompare(familyName, undefined, { sensitivity: 'accent' }) === 0) {
    return 'normal';
  }

  if (!nameWithoutType.toLocaleLowerCase().startsWith(`${familyName.toLocaleLowerCase()} `)) {
    return undefined;
  }

  const suffix = nameWithoutType.substring(familyName.length).trim();
  if (/^(regular|book|roman)$/i.test(suffix)) {
    return 'normal';
  }

  const isBold = /\b(bold|semi[- ]?bold|demi[- ]?bold|black|heavy)\b/i.test(suffix);
  const isItalic = /\b(italic|oblique)\b/i.test(suffix);
  if (isBold && isItalic) {
    return 'bolditalics';
  }
  if (isBold) {
    return 'bold';
  }
  if (isItalic) {
    return 'italics';
  }

  return undefined;
}

function resolveWindowsFontFile(fileName: string): string | undefined {
  const normalizedFileName = fileName.trim().replace(/^"|"$/g, '');
  if (isAbsolute(normalizedFileName)) {
    return existsSync(normalizedFileName) ? normalizedFileName : undefined;
  }

  const windowsDirectory = process.env.WINDIR ?? process.env.SystemRoot ?? 'C:\\Windows';
  const fontDirectories = [join(windowsDirectory, 'Fonts')];
  if (process.env.LOCALAPPDATA) {
    fontDirectories.push(join(process.env.LOCALAPPDATA, 'Microsoft', 'Windows', 'Fonts'));
  }

  return fontDirectories.map(directory => join(directory, normalizedFileName)).find(existsSync);
}

/**
 * Rejestruje rodzinę czcionek zainstalowaną w Windows jako domyślną czcionkę PDF.
 * Nazwa rodziny musi odpowiadać nazwie widocznej w ustawieniach czcionek Windows,
 * np. "Arial" lub "Aptos".
 */
export function configureWindowsFont(familyName: string): void {
  if (process.platform !== 'win32') {
    throw new Error('Parametr --font jest dostępny tylko w systemie Windows.');
  }

  const normalizedFamilyName = familyName.trim();
  if (!normalizedFamilyName) {
    throw new Error('Parametr --font wymaga nazwy rodziny czcionek.');
  }

  const registryEntries = [
    ...getWindowsFontRegistryEntries('HKCU\\Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts'),
    ...getWindowsFontRegistryEntries('HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts'),
  ];
  const fontFiles: Partial<Record<FontStyle, string>> = {};

  for (const entry of registryEntries) {
    const style = getWindowsFontStyle(entry.name, normalizedFamilyName);
    if (!style || fontFiles[style]) {
      continue;
    }

    const fontFile = resolveWindowsFontFile(entry.fileName);
    if (fontFile) {
      fontFiles[style] = fontFile;
    }
  }

  if (!fontFiles.normal) {
    throw new Error(`Nie znaleziono odmiany zwykłej czcionki Windows "${normalizedFamilyName}".`);
  }

  const normalFontFile = fontFiles.normal;
  const styles: FontStyle[] = ['normal', 'bold', 'italics', 'bolditalics'];
  const vfs: Record<string, Base64URLString> = {};
  const fonts: Record<string, Record<FontStyle, string>> = { [normalizedFamilyName]: {} as Record<FontStyle, string> };

  for (const style of styles) {
    const fontFile: string = fontFiles[style] ?? normalFontFile;
    const extension: string = basename(fontFile).match(/\.[^.]+$/)?.[0] ?? '.ttf';
    const vfsName = `windows-font-${normalizedFamilyName.replace(/[^a-z0-9]/gi, '_')}-${style}${extension}`;
    vfs[vfsName] = readFileSync(fontFile).toString('base64') as Base64URLString;
    fonts[normalizedFamilyName][style] = vfsName;
  }

  configureFonts({ vfs, fonts });
}

/**
 * Parsuje XML z ciągu znaków (dla Node.js)
 */
export function parseXMLString(xmlString: string): any {
  try {
    const jsonDoc = xml2js(xmlString, {
      compact: true,
      cdataKey: '_text',
      trim: true,
      elementNameFn: stripPrefix,
      attributeNameFn: stripPrefix,
    });

    return jsonDoc;
  } catch (error) {
    throw new Error(`Błąd parsowania XML: ${error}`);
  }
}
/**
 * Generuje PDF faktury z ciągu XML (dla Node.js)
 */
export async function generateInvoiceNode(
  xmlString: string,
  additionalData: AdditionalDataTypes
): Promise<Buffer> {
  const xml = parseXMLString(xmlString);
  const wersja: any = (xml as any)?.Faktura?.Naglowek?.KodFormularza?._attributes?.kodSystemowy;

  switch (wersja) {
    case 'FA (1)':
      return (generateFA1((xml as any).Faktura as Faktura1, additionalData) as any).getBuffer();
    case 'FA (2)':
      return (generateFA2((xml as any).Faktura as Faktura2, additionalData) as any).getBuffer();
    case 'FA (3)':
      return (generateFA3((xml as any).Faktura as Faktura3, additionalData) as any).getBuffer();
    case 'FA_RR(1)':
    case 'FA_RR (1)':
      return (generateFARR((xml as any).Faktura as FakturaRR, additionalData) as any).getBuffer();
    default:
      throw new Error(`Nieobsługiwana wersja faktury: ${wersja}`);
  }
}

/**
 * Generuje PDF UPO z ciągu XML (dla Node.js)
 */
export async function generatePDFUPONode(xmlString: string): Promise<Buffer> {
  const upo = parseXMLString(xmlString) as Upo;
  const docDefinition: TDocumentDefinitions = {
    content: [generateNaglowekUPO(upo.Potwierdzenie!), generateDokumentUPO(upo.Potwierdzenie!)],
    ...generateStyle(),
    pageSize: 'A4',
    pageOrientation: 'landscape',
    footer: function (currentPage: number, pageCount: number) {
      return {
        text: currentPage.toString() + ' z ' + pageCount,
        alignment: Position.RIGHT,
        margin: [0, 0, 20, 0],
      };
    },
  };

  return (pdfMake.createPdf(docDefinition) as any).getBuffer();
}
