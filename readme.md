# Biblioteka do generowania wizualizacji PDF faktur i UPO

Biblioteka do generowania wizualizacji PDF faktur oraz UPO na podstawie plików XML po stronie klienta.

---

## 0. EXE

Plik EXE dołączany jest do publikowanych [wydań (releases)](https://github.com/Dzyszla/ksef-pdf-generator-win64/releases) jako załącznik.

Informacje o kompilacji dla Windows znajdują się w pliku [BUILD_EXE.md](BUILD_EXE.md)

---

## 1. Główne ustalenia

    Biblioteka zawiera następujące funkcjonalności:
    - Generowanie wizualizacji PDF faktur
    - Generowanie wizualizacji PDF UPO

---

## 2. Budowanie bibliotki

Uwaga! Informacje o kompilacji dla Windows znajdują się w pliku [BUILD_EXE.md](BUILD_EXE.md)

---

## 3. Lokalizacja - i18next

Biblioteka wspiera lokalizację, poprzez użycie biblioteki i18next. Pliki z tłumaczeniami należy umieścic w folderze
** src/lib-public/i18n/lang **. Dokumentacja samej biblioteki i18next znajduje się pod adresem https://www.i18next.com/.

---

### 1. Nazewnictwo zmiennych i metod

- **Polsko-angielskie nazwy** stosowane w zmiennych, typach i metodach wynikają bezpośrednio ze struktury pliku schemy
  faktury.  
  Takie podejście zapewnia spójność i ujednolicenie nazewnictwa z definicją danych zawartą w schemie XML.

### 2. Struktura danych

- Struktura danych interfejsu FA odzwierciedla strukturę danych źródłowych pliku XML, zachowując ich logiczne powiązania
  i hierarchię
  w bardziej czytelnej formie.

### 3. Typy i interfejsy

- Typy odzwierciedlają strukturę danych pobieranych z XML faktur oraz ułatwiają generowanie PDF
- Typy i interfejsy są definiowane w folderze types oraz plikach z rozszerzeniem types.ts.

---

## Uwagi

- Upewnij się, że pliki XML są poprawnie sformatowane zgodnie z odpowiednią schemą.
- W przypadku problemów z Node.js, rozważ użycie menedżera wersji Node, np. [nvm](https://github.com/nvm-sh/nvm).


