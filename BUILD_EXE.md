# Instrukcja kompilacji i uruchamiania aplikacji CLI

## Wymagania

- Node.js w wersji 18 lub wyższej
- npm lub yarn

## Kroki kompilacji

1. **Zainstaluj zależności** (jeśli jeszcze nie zostały zainstalowane):
   ```bash
   npm install
   ```

2. **Skompiluj aplikację**:
   ```bash
   npm run build:cli
   ```

   To polecenie:
   - Skompiluje kod TypeScript do JavaScript (CommonJS)
   - Utworzy pliki JavaScript w katalogu `dist-cli/`

## Użycie skompilowanej aplikacji

### Sposób 1: Użycie pliku .exe (Windows)

Najprostszy sposób na Windows - użyj pliku `ksef-pdf-generator.bat`:

#### Generowanie faktury PDF:
```bash
ksef-pdf-generator.exe -i invoice.xml -t invoice --nrKSeF "123-2025-ABC" --qrCode "https://example.com/qr"
```

#### Generowanie UPO PDF:
```bash
ksef-pdf-generator.exe -i upo.xml -t upo -o output.pdf
```

### Sposób 2: Bezpośrednie uruchomienie przez Node.js

#### Generowanie faktury PDF:
```bash
node dist-cli/cli.min.js -i invoice.xml -t invoice --nrKSeF "123-2025-ABC" --qrCode "https://example.com/qr"
```

#### Generowanie UPO PDF:
```bash
node dist-cli/cli.min.js -i upo.xml -t upo -o output.pdf
```

### Tryb strumieniowy (--stream)

W trybie strumieniowym aplikacja czyta XML ze standardowego wejścia (stdin) i zapisuje PDF do standardowego wyjścia (stdout). Błędy są zapisywane do stderr.

#### Generowanie faktury PDF (tryb strumieniowy):
```bash
# Windows PowerShell
Get-Content invoice.xml | node dist-cli/cli.min.js --stream -t invoice --nrKSeF "123-2025-ABC" --qrCode "https://example.com/qr" > output.pdf

# Windows CMD
type invoice.xml | node dist-cli/cli.min.js --stream -t invoice --nrKSeF "123-2025-ABC" --qrCode "https://example.com/qr" > output.pdf

# Linux/Mac
cat invoice.xml | node dist-cli/cli.min.js --stream -t invoice --nrKSeF "123-2025-ABC" --qrCode "https://example.com/qr" > output.pdf
```

#### Generowanie UPO PDF (tryb strumieniowy):
```bash
# Windows PowerShell
Get-Content upo.xml | node dist-cli/cli.min.js --stream -t upo > output.pdf

# Windows CMD
type upo.xml | node dist-cli/cli.min.js --stream -t upo > output.pdf

# Linux/Mac
cat upo.xml | node dist-cli/cli.min.js --stream -t upo > output.pdf
```

### Pomoc:
```bash
node dist-cli/cli.min.js --help
```

## Parametry

- `-i, --input <ścieżka>` - Ścieżka do pliku XML wejściowego (wymagane w trybie plikowym)
- `-o, --output <ścieżka>` - Ścieżka do pliku PDF wyjściowego (opcjonalne, tylko w trybie plikowym, domyślnie: nazwa pliku wejściowego z rozszerzeniem .pdf)
- `-t, --type <typ>` - Typ dokumentu: `invoice` lub `upo` (wymagane)
- `--nrKSeF <wartość>` - Numer KSeF (wymagane dla faktur)
- `--qrCode <url>` - URL kodu QR (wymagane dla faktur)
- `--stream` - Tryb strumieniowy: XML ze stdin, PDF do stdout
- `-h, --help` - Wyświetla pomoc

## Uwagi

- **Wymagany Node.js**: Aplikacja wymaga zainstalowanego Node.js (v18+) na systemie docelowym
- **Rozmiar**: Skompilowane pliki JavaScript zajmują tylko ~2.9 MB (vs ~36 MB dla pakowanego .exe)
- **Zależności**: Wszystkie zależności muszą być zainstalowane przez `npm install` przed uruchomieniem
- **Plik .bat**: Plik `ksef-pdf-generator.bat` ułatwia uruchamianie na Windows
- **Tryb strumieniowy**: Tryb strumieniowy (`--stream`) jest idealny do integracji z innymi aplikacjami
- **Komunikaty błędów**: W trybie strumieniowym wszystkie komunikaty błędów są zapisywane do stderr, a dane wyjściowe (PDF) do stdout



