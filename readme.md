# Biblioteka do generowania wizualizacji PDF faktur i UPO

Biblioteka do generowania wizualizacji PDF faktur oraz UPO na podstawie plików XML po stronie klienta.

---

### Informacje wstępne

Niniejsza wersja Biblioteki stanowi wydanie wstępne, zawierające zbudowaną paczkę JavaScript oraz aplikacje służące do
testowania rozwiązania.
Wersja pełna, obejmująca kompletny kod źródłowy w TypeScript, możliwość budowania biblioteki oraz generowania typów TS,
zostanie udostępniona w dniu 14 listopada 2025 r.

## 1. Jak uruchomić aplikację

1. Zainstaluj Node.js w wersji **22.14.0**  
   Możesz pobrać Node.js z oficjalnej strony: [https://nodejs.org](https://nodejs.org)

2. Sklonuj repozytorium i przejdź do folderu projektu:
   ```bash
   git clone https://github.com/CIRFMF/ksef-pdf-generator#
   cd ksef-pdf-generator
   ```

3. Zainstaluj zależności:
   ```bash
   npm install
   ```

4. Uruchom aplikację:
   ```bash
   npm run start
   ```

Aplikacja uruchomi się domyślnie pod adresem: [http://localhost:5173/](http://localhost:5173/)

---

## 2. Jak wygenerować fakturę

1. Po uruchomieniu aplikacji przejdź do **Wygeneruj wizualizacje faktury PDF**.
2. Wybierz plik XML zgodny ze schemą **FA(1), FA(2) lub FA(3)**.
3. Przykładowy plik znajduje się w folderze:
   ```
   examples/invoice.xml
   ```  
4. Po wybraniu pliku, PDF zostanie wygenerowany.

---

## 3. Jak wygenerować UPO

1. Po uruchomieniu aplikacji przejdź do **Wygeneruj wizualizacje UPO PDF**.
2. Wybierz plik XML zgodny ze schemą **UPO v4_2**.
3. Przykładowy plik znajduje się w folderze:
   ```
   examples/upo.xml
   ```  
4. Po wybraniu pliku, PDF zostanie wygenerowany.

---

## Uwagi

- Upewnij się, że pliki XML są poprawnie sformatowane zgodnie z odpowiednią schemą.
- W przypadku problemów z Node.js, rozważ użycie menedżera wersji Node, np. [nvm](https://github.com/nvm-sh/nvm).
