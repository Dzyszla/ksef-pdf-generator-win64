# Instrukcja kompilacji i uruchamiania aplikacji CLI

## Wymagania

- Node.js w wersji 18 lub wyższej
- npm

## Kroki kompilacji

1. **Zainstaluj zależności** (jeśli jeszcze nie zostały zainstalowane):
   ```bash
   npm install
   ```

2. **Skompiluj aplikację**:
   ```bash
   npm run build
   ```

## Użycie skompilowanej aplikacji

### Użycie pliku .exe (Windows)

#### Generowanie faktury PDF:
```bash
ksef-pdf-generator.exe -i invoice.xml -o invoice.pdf -t invoice --nrKSeF "1111111111-20251107-080080679C57-14" --acquisitionDate "07.11.2025 08:15:30" --qrCode "https://qr.ksef.mf.gov.pl/invoice/{nip}/{p1}/{hash}" --watermark "Faktura"
```

#### Generowanie faktury PDF dla trybu OFFLINE:
```bash
ksef-pdf-generator.exe -i invoice.xml -o invoice.pdf -t invoice --nrKSeF "123-2025-ABC" --qrCode "https://qr.ksef.mf.gov.pl/invoice/{nip}/{p1}/{hash}" --qr2Code "https://qr.ksef.mf.gov.pl/certificate/Nip/1111111111/{nip}/01F20A5D352AE590/..."
```

#### Generowanie UPO PDF:
```bash
ksef-pdf-generator.exe -i upo.xml -t upo -o upo.pdf
```

#### Generowanie faktury PDF w strumieniu z użyciem parametrów dla kodu QR:
```bash
ksef-pdf-generator.exe --stream -t invoice --nrKSeF "123-2025-ABC" --qrCode "https://qr.ksef.mf.gov.pl/invoice/{nip}/{p1}/{hash}" < invoice.xml > invoice.pdf
```

#### Generowanie faktury PDF w innym języku:
```bash
ksef-pdf-generator.exe ... --lang "lang\en.json"
```
Plik z tłumaczeniami może być zlokalizowany w dowolnym miejscu. Musi być on zgodny ze strukturą, jaka znajduje się w repozytorium pod adresem [src/lib-public/i18n/lang/](https://github.com/Dzyszla/ksef-pdf-generator-win64/tree/main/src/lib-public/i18n/lang]


### Pomoc:
```bash
ksef-pdf-generator.exe --help
```

## Parametry

- `-i, --input <ścieżka>` - Ścieżka do pliku XML wejściowego (wymagane w trybie plikowym)
- `-o, --output <ścieżka>` - Ścieżka do pliku PDF wyjściowego (opcjonalne, tylko w trybie plikowym, domyślnie: nazwa pliku wejściowego z rozszerzeniem .pdf)
- `-t, --type <typ>` - Typ dokumentu: `invoice` lub `upo` (wymagane). `invoice` obsługuje FA(1), FA(2), FA(3), FA_RR(1) oraz PEF (UBL `Invoice`/`CreditNote` - faktura podstawowa, korygująca i specyfikacja); rodzaj rozpoznawany jest automatycznie.
- `--nrKSeF <wartość>` - Numer KSeF. Dla faktury w trybie plikowym parametr można pominąć, jeżeli nazwa pliku XML (bez rozszerzenia) jest numerem KSeF, np. `1111111111-20251107-080080679C57-14.xml`.
- `--acquisitionDate <data>` - Data nadania numeru KSeF (opcjonalne dla faktur)
- `--qrCode <url>` - URL kodu QR (wymagane dla faktur); obsługuje parametry `{hash}`, `{nip}`, `{p1}`. Dla PEF `{nip}` to NIP sprzedawcy z `AccountingSupplierParty` (pierwszy 10-cyfrowy identyfikator z `PartyTaxScheme/CompanyID`, `PartyLegalEntity/CompanyID` lub `EndpointID`, bez prefiksu `PL`), a `{p1}` to `IssueDate`.
- `--qr2Code <url>` - URL kodu QR2 (dla faktur w trybie OFFLINE)
- `--stream` - Tryb strumieniowy: XML ze stdin, PDF do stdout
- `--watermark <tekst>` - Tekst umieszczany w tle strony (znak wodny) - dla faktur i UPO
- `--creator <tekst>` - Nazwa aplikacji tworzącej dokument, zapisywana w metadanych PDF (pole `Creator`; domyślnie „Brak danych”). Pole `Producer` generator ustawia automatycznie.
- `--lang <ścieżka>` - Generowanie faktury w innym języku - w parametrze <ścieżka> plik json z tłumaczeniami dla dokumentu (przykładowy plik w [src/lib-public/i18n/lang/](https://github.com/Dzyszla/ksef-pdf-generator-win64/tree/main/src/lib-public/i18n/lang]). Pliki `pl.json` i `en.json` są generowane ze źródeł `pl.ts` / `en.ts` poleceniem `node scripts/generate-lang-json.cjs` (lub `generate-lang-json.bat`) - uruchom je po aktualizacji z upstream.
- `-h, --help` - Wyświetla pomoc

## Uwagi

- **Wymagany Node.js**: Aplikacja wymaga zainstalowanego Node.js (v18+) na systemie docelowym
- **Zależności**: Wszystkie zależności muszą być zainstalowane przez `npm install` przed uruchomieniem
- **Tryb strumieniowy**: Tryb strumieniowy (`--stream`) jest idealny do integracji z innymi aplikacjami
- **Komunikaty błędów**: W trybie strumieniowym wszystkie komunikaty błędów są zapisywane do stderr, a dane wyjściowe (PDF) do stdout
