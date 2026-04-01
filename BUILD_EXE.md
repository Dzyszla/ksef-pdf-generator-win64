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
ksef-pdf-generator.exe -i invoice.xml -o invoice.pdf -t invoice --nrKSeF "1111111111-20251107-080080679C57-14" --qrCode "https://qr.ksef.mf.gov.pl/invoice/{nip}/{p1}/{hash}"
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

### Pomoc:
```bash
ksef-pdf-generator.exe --help
```

## Parametry

- `-i, --input <ścieżka>` - Ścieżka do pliku XML wejściowego (wymagane w trybie plikowym)
- `-o, --output <ścieżka>` - Ścieżka do pliku PDF wyjściowego (opcjonalne, tylko w trybie plikowym, domyślnie: nazwa pliku wejściowego z rozszerzeniem .pdf)
- `-t, --type <typ>` - Typ dokumentu: `invoice` lub `upo` (wymagane)
- `--nrKSeF <wartość>` - Numer KSeF (wymagane dla faktur)
- `--qrCode <url>` - URL kodu QR (wymagane dla faktur)
- `--qr2Code <url>` - URL kodu QR2 (dla faktur w trybie OFFLINE)
- `--stream` - Tryb strumieniowy: XML ze stdin, PDF do stdout
- `-h, --help` - Wyświetla pomoc

## Uwagi

- **Wymagany Node.js**: Aplikacja wymaga zainstalowanego Node.js (v18+) na systemie docelowym
- **Zależności**: Wszystkie zależności muszą być zainstalowane przez `npm install` przed uruchomieniem
- **Tryb strumieniowy**: Tryb strumieniowy (`--stream`) jest idealny do integracji z innymi aplikacjami
- **Komunikaty błędów**: W trybie strumieniowym wszystkie komunikaty błędów są zapisywane do stderr, a dane wyjściowe (PDF) do stdout



