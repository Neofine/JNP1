Zadanie polega na zaimplementowaniu klas obiektów geometrycznych:
Position   – punktu (pozycji) na płaszczyźnie,
Vector     – wektora na płaszczyźnie,
Rectangle  – prostokąta,
Rectangles – kolekcji prostokątów.

Position:

- Position można stworzyć, przekazując współrzędne, np. Position(100, 200). Nie
  powinno być możliwe skonstruowanie obiektu Position bez podawania argumentów
  (tj. Position()).
- Position można porównywać za pomocą operatora ==.
- Position ma metody x() i y(), które wyciągają odpowiednie współrzędne.
- Position ma metodę reflection(), która zwraca pozycję odbitą względem osi
  x = y.
- Position implementuje operator "+=(vec)", gdzie vec jest obiektem klasy
  Vector. Operator powinien zwrócić referencję do wołanego obiektu.
- Position implementuje statyczną metodę origin(), która udostępnia
  niemodyfikowalny obiekt Position(0, 0).

Vector:

- Vector można stworzyć, przekazując współrzędne, np. Vector(100, 200). Nie
  powinno być możliwe skonstruowanie obiektu Vector bez podawania argumentów
  (tj. Vector()).
- Vector można porównywać za pomocą operatora ==.
- Vector ma metody x() i y(), które wyciągają odpowiednie współrzędne.
- Vector ma metodę reflection(), która zwraca wektor odbity względem osi x = y.
- Vector implementuje operator "+=(vec)", gdzie vec jest obiektem klasy Vector.
  Operator powinien zwrócić referencję do wołanego obiektu.

Rectangle:

- Rectangle można stworzyć, przekazując jego rozmiar i pozycję lewego dolnego
  rogu: Rectangle(width, height, pos) lub Rectangle(width, height) – wtedy
  pozycja jest ustalona na (0, 0). Nie powinno być możliwe skonstruowanie
  obiektu Rectangle bez podawania argumentów (tj. Rectangle()).
- Rectangle musi mieć dodatnią szerokość i dodatnią wysokość.
- Rectangle można porównywać za pomocą operatora ==.
- Rectangle ma metody width(), height() i pos(), które wyciągają odpowiednie
  współrzędne i pozycję lewego dolnego rogu.
- Rectangle ma metodę reflection(), która zwraca prostokąt odbity względem osi
  x = y.
- Rectangle implementuje operator "+=(vec)", gdzie vec jest obiektem klasy
  Vector. Operator powinien zwrócić referencję do wołanego obiektu.
- Rectangle implementuje metodę area(), która wylicza jego pole powierzchni.

Rectangles:

- Kolekcję prostokątów Rectangles można stworzyć w następujący sposób:
  Rectangles({rect1, rect2, ..., rectn}). Powinno być możliwe skonstruowanie
  pustej kolekcji Rectangles, jeśli nie podano argumentów (tj. Rectangles()).
- Wywołanie recs[i] zwraca referencję do i-tego prostokąta w kolekcji.
- Metoda size() zwraca liczbę prostokątów w kolekcji.
- Rectangles można porównywać za pomocą operatora ==.
- Rectangles implementuje operator "+=(vec)", gdzie vec jest obiektem klasy
  Vector. Operator powinien zwrócić referencję do wołanego obiektu.

Dodatkowo powinny być możliwe następujące operacje:

  Position = Position + Vector
  Position = Vector + Position
  Vector = Vector + Vector
  Rectangle = Rectangle + Vector
  Rectangle = Vector + Rectangle
  Rectangles = Rectangles + Vector
  Rectangles = Vector + Rectangles

z naturalną semantyką.

Należy też zaimplementować operacje

- merge_horizontally(rect1, rect2)
- merge_vertically(rect1, rect2)

łączące dwa prostokąty odpowiednio wzdłuż osi X lub Y oraz zwracające wynik typu
Rectangle. Funkcja merge_horizontally łączy dwa prostokąty ze wspólną krawędzią,
będącą górną krawędzią pierwszego prostokąta i dolną krawędzią drugiego
prostokąta, a funkcja merge_vertically łączy dwa prostokąty ze wspólną
krawędzią, będącą prawą krawędzią pierwszego prostokąta i lewą krawędzią
drugiego prostokąta.

Dla niepustej kolekcji prostokątów operacja

- merge_all(Rectangles)

powoduje złączenie ich po kolei, na przykład jeśli w kolekcji byłyby cztery
prostokąty, to operacja ta zwraca następujący obiekt typu Rectangle
merge_?(merge_?(merge_?(rect1,rect2), rect3), rect4),
gdzie zamiast ? wstawiamy horizontally albo vertically – za każdym razem
wybierając to złączenie, które jest możliwe.

Powinny być dopuszczone jedynie jawne konwersje z Position na Vector i z Vector
na Position, czyli powinien kompilować się następujący kod:

    Position p1(1, 2);
    Vector v1(p1);
    Vector v2(2, 3);
    Position p2(v2);
    p2 = Position(v1);
    v1 = Vector(p2);

Natomiast niejawne konwersje powinny być zablokowane, czyli przy powyższych
deklaracjach następujące instrukcje nie powinny się kompilować:

    p2 = v1;
    v1 = p2;

Wszystkie klasy powinny umieć się kopiować i przypisywać na siebie w efektywny
sposób. Wartości współrzędnych i wyniki operacji na współrzędnych mieszczą się
w zakresie od -2^31 do 2^31 - 1. Należy sprawdzać poprawność argumentów.
Nieprawidłowe argumenty powinny powodować zakończenie programu z błędem.

Należy zadbać o właściwe wstawianie słowa kluczowego const.

Przy wszystkich operacjach należy zadbać o efektywne działanie na obiektach
tymczasowych, np. w poniższym kodzie należy uniknąć kopiowania obiektów typu
Rectangles.

    Rectangles recs1({...});
    Rectangles recs2 = std::move(recs1) + Vector(1, 1);
    Rectangles recs3 = Vector(1, 1) + std::move(recs2);

Rozwiązanie powinno zawierać pliki geometry.h oraz geometry.cc. Pliki te należy
umieścić w repozytorium w katalogu

grupaN/zadanie3/ab123456+cd123456

lub

grupaN/zadanie3/ab123456+cd123456+ef123456
gdzie N jest numerem grupy, a ab123456, cd123456, ef123456 są identyfikatorami
członków zespołu umieszczającego to rozwiązanie. Katalog z rozwiązaniem nie
powinien zawierać innych plików, ale może zawierać podkatalog prywatne, gdzie
można umieszczać różne pliki, np. swoje testy. Pliki umieszczone w tym
podkatalogu nie będą oceniane. Nie wolno umieszczać w repozytorium plików
dużych, binarnych, tymczasowych (np. *.o) ani innych zbędnych.

Rozwiązanie będzie kompilowane na maszynie students poleceniem

g++ -c -Wall -Wextra -O2 -std=c++17 geometry.cc
