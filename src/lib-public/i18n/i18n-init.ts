import i18next from 'i18next';
import fs from 'fs/promises';
import path from 'path';
import { pl } from './lang/pl';

export async function i18nReady(
  filePath?: string,
  lng: string = 'pl'
): Promise<void> {
  if (!i18next.isInitialized) {
	  let translations = pl;
	
	  if (filePath) {
	    try {
	      const fullPath = path.resolve(filePath);
	      const fileContent = await fs.readFile(fullPath, 'utf-8');
	      translations = JSON.parse(fileContent);
	    } catch (err) {
	      process.stderr.write('i18n: nie uda³o siê wczytaæ pliku', err);
	    }
	  }
	
	  const resources = {
	    [lng]: {
	      translation: translations,
	    },
	  };
	
    await i18next.init({
      lng,
      fallbackLng: lng,
      showSupportNotice: false,
      debug: false,
      resources,
    });
  }
}
