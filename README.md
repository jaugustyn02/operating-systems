# Systemy Operacyjne - zestawy zadań
## Zestaw 1 - Zarządzanie pamięcią, biblioteki, pomiar czasu
### 1. (25%) Alokacja tablicy ze wskaźnikami na bloki pamięci zawierające znaki
Zaprojektuj i przygotuj zestaw funkcji (bibliotekę) do zarządzania tablicą bloków, w których zapisywane są rezultaty operacji zliczania lini, słów i znaków (poleceniem wc) w plikach przekazywanych jako odpowiedni parametr.
### 2. (25%) Program korzystający z biblioteki
Napisz program testujący działanie funkcji z biblioteki z zadania 1.
### 3. (50%) Testy i pomiary
## Zestaw 2 - Operacje na plikach i katalogach, pomiar czasu
### 1. (25%) Napisz program, który przyjmuje 4 argumenty wiersza poleceń:
  - znak ASCII, który należy znaleźć w pliku wejściowym
  - znak ASCII, na który należy zamienić wszystkie wystąpienia pierwszego argumentu
  - nazwa pliku wejściowego, w którym należy znaleźć pierwszy argument
  - nazwa pliku wyjściowego, do którego należy zapisać zawartość pliku wejściowego z zamienionym znakami argv[1] na argv[2].
### 2. (25%) Napisz program, który kopiuje zawartość jednego pliku do drugiego, odwróconą bajt po bajcie.
### 3. (25%) Napisz program, który będzie przeglądał bieżący katalog, korzystając z funkcji opendir(), readdir() i stat(). Dla każdego znalezionego pliku, który nie jest katalogiem, czyli !S_ISDIR(bufor_stat.st_mode), należy wypisać rozmiar i nazwę pliku. Ponadto na koniec należy wypisać sumaryczny rozmiar wszystkich plików. Nie należy przeglądać podkatalogów! Sumaryczny rozmiar plików należy przechowywać w zmiennej typu long long i wypisywać ją przez format %lld.
### 4. (25%) Napisz program, który będzie przeglądał katalog podany jako argument wywołania i jego podkatalogi, korzystając z funkcji ftw() (uproszczonej wersji funkcji nftw()). Dla każdego znalezionego pliku, który nie jest katalogiem, czyli !S_ISDIR(bufor_stat.st_mode), należy wypisać rozmiar i nazwę pliku. Ponadto na koniec należy wypisać sumaryczny rozmiar wszystkich plików. Dobra wiadomość: funkcja ftw() przyjmuje ścieżki i bezwzględne, i względne.
