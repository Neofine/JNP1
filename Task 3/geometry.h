#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>

class Position;

class Vector;

class Rectangle;

class Rectangles;

// Klasa nadrzędna dla pozycji i wektora - udostępnia podstawowe gettery i konstruktory
class AffineSpace {
protected:
    int mX;
    int mY;

public:
    const int &x() const;

    const int &y() const;

    // Konstruktor przyjmujący współrzędne (x, y) i tworzący pozycję/wektor
    AffineSpace(int, int);

    // Konstruktor przyjmujący jako argument pozycję
    explicit AffineSpace(const Position &);

    // Konstruktor przyjmujący jako argument wektor
    explicit AffineSpace(const Vector &);

    // Destruktor czyniący klasę pure virtual
    virtual ~AffineSpace() = 0;
};

// Klasa opisująca pozycję na dwuwymiarowym układzie współrzędnych kartezjańskich
class Position : public AffineSpace {
public:
    // Konstruktor na podstawie współrzędnych (x, y)
    Position(int, int);

    // Konstruktor kopii dla pozycji
    Position(const Position &) = default;

    // Przypisanie kopii dla pozycji
    Position &operator=(const Position &) = default;

    // Konstruktor na podstawie wektora
    explicit Position(const Vector &);

    // Operator przemieszczający pozycję o dany wektor
    Position &operator+=(const Vector &);

    // Operator sprawdzający równość dwóch pozycji
    bool operator==(const Position &) const;

    // Funkcja zwracająca odbicie danej pozycji względem prostej f(x) = x
    Position reflection() const;

    // Funkcja udostępniająca niemodyfikowalny obiekt Position(0, 0)
    static const Position &origin();

    // Destruktor pozycji
    ~Position() = default;
};

// Klasa opisująca wektor na dwuwymiarowym układzie współrzędnych kartezjańskich
class Vector : public AffineSpace {
public:
    // Konstruktor na podstawie współrzędnych (x, y)
    Vector(int, int);

    // Konstruktor kopii dla wektora
    Vector(const Vector &) = default;

    // Przypisanie kopii dla wektora
    Vector &operator=(const Vector &) = default;

    // Konstruktor na podstawie pozycji
    explicit Vector(const Position &);

    // Operator zmieniający wektor o sumę z drugim wektorem
    Vector &operator+=(const Vector &);

    // Operator sprawdzajacy sumę dwóch wektorów
    bool operator==(const Vector &) const;

    // Funkcja zwracająca odbicie danego wektora względem prostej f(x) = x
    Vector reflection() const;

    // Destruktor wektora
    ~Vector() = default;
};

// Klasa Rectangle opisuje prostokąt, jego położenie i wymiary, posiada
// konstruktor, funkcje które na tej klasie można wykonać oraz pewne operatory
class Rectangle {
private:
    // Szerokość prostokąta
    unsigned int mWidth;
    // Wysokość prostokąta
    unsigned int mHeight;
    // Pozycja lewego dolnego punktu prostokąta
    Position mPosition;

public:
    // Konstruktor klasy Rectangle, przyjmujący 2 lub 3 argumenty, z których pierwsze
    // 2 (przymusowe do poprawnego zainicjowania konstruktora) są odpowiednio
    // szerokością oraz wysokością prostokąta, trzeci argument, opcjonalny, jest
    // to pozycja lewego dolnego jego boku, jeżeli nie jest podany to program
    // przyjmuje jego pozycję jako (0, 0)
    Rectangle(int width, int height, const Position& botLeft = Position(0, 0));

    // Konstruktor kopii dla prostokąta
    Rectangle(const Rectangle &) = default;

    // Przypisanie kopii dla prostokąta
    Rectangle &operator=(const Rectangle &) = default;

    // Operator przyrównujący jeden obiekt klasy rectangle do drugiego, zwraca
    // prawdę jeżeli wysokość, szerokość oraz lewy dolny punkt są takie same
    // w obu obiektach, w przeciwnym wypadku fałsz
    bool operator==(const Rectangle &) const;

    // Operator przyrównujący jeden obiekt klasy rectangle do drugiego, zwraca
    // fałsz jeżeli wysokość, szerokość oraz lewy dolny punkt są takie same
    // w obu obiektach, w przeciwnym wypadku prawdę
    bool operator!=(const Rectangle &) const;

    // Operator zmieniający położenie obieku na którym się go zastosuje, o
    // podany wektor
    Rectangle &operator+=(const Vector &);

    // Funkcja zwraca szerokość prostokąta
    unsigned int width() const;

    // Funkcja zwraca wysokość prostokąta
    unsigned int height() const;

    // Funkcja zwraca pozycję prostokąta
    Position pos() const;

    // Funkcja zwraca pole prostokąta.
    long long area() const;

    // Funkcja zwraca kopię prostokąta odbitą symetrycznie według funkcji y = x
    // według posiadanego w wywoływanej klasie prostokąta
    Rectangle reflection() const;

    // Destruktor rectangle
    ~Rectangle() = default;
};

// Klasa Rectangles opisuje zbiór prostokątów z klasy Rectangle,  posiada
// konstruktory, funkcje które na tej klasie można wykonać oraz pewne operatory
class Rectangles {
private:
    // Wektor który trzyma prostokąty podane w konstruktorze
    std::vector<Rectangle> rects;

public:
    // Konstruktor który pozwala wywołać klasę która nie
    // posiada w środku prostokątów
    Rectangles() = default;

    // Konstruktor kopii dla prostokątów
    Rectangles(const Rectangles &) = default;

    // Konstruktor move dla prostokątów
    Rectangles(Rectangles &&) noexcept = default;

    // Konstruktor który pozwala wywołać klasę w następujący sposób:
    // Rectangles({rect1, rect2, ..., rectn}), gdzie wszystkie obiekty w
    // liście inicjalizującej muszą być z klasy Rectangle
    Rectangles(std::initializer_list<Rectangle>) noexcept;

    // Przypisanie kopii dla prostokątów
    Rectangles &operator=(const Rectangles &) = default;

    // Przypisanie move dla prostokątów
    Rectangles &operator=(Rectangles &&) noexcept = default;

    // Operator który pozwala odwoływać się do i-tego trzymanego prostokąta w
    // klasie, jeżeli podany argument jest większy niż liczba prostokątów
    // to program to wyłapie podnosząc odpowiedniednią assercję
    const Rectangle &operator[](size_t) const;

    // Operator który pozwala odwoływać się do i-tego trzymanego prostokąta w
    // klasie, jeżeli podany argument jest większy niż liczba prostokątów
    // to program to wyłapie podnosząc odpowiedniednią assercję, jest on inny
    // niż funkcja powyżej żeby umożliwić kompilatorowi wybranie odpowiadającego
    // mu operatora w różnych sytuacjach
    Rectangle &operator[](size_t);

    // Operator przyrównujący jeden obiekt klasy rectangles do drugiego, zwraca
    // prawdę wielkości klas są równe oraz element na i-tej pozycji pierwszej
    // klasy jest równy elementowi na i-tej pozycji drugiej klasy, dla każdego
    // możliwego iteratora 'i' mieszczącego się w rozmiarze klas
    bool operator==(const Rectangles &) const;

    // Operator zmieniający położenie wszystkich obiektów trzymanych
    // na (std::)wektorze rects o podany wektor (naszą klasę)
    Rectangles &operator+=(const Vector &);

    // Funkcja zwraca ilość prostokątów przetrzymywanych w klasie
    size_t size() const;

    // Destruktor rectangles
    ~Rectangles() = default;
};

// Operator zwracający pozycję przesuniętą o wektor
Position operator+(Position, const Vector &);

// Operator zwracający pozycję przesuniętą o wektor
Position operator+(const Vector &, Position);

// Operator zwracający sumę dwóch wektorów
Vector operator+(Vector, const Vector &);

// Operator dodawania, zwraca klasę Rectangle będącą wynikiem dodawania
// podanego obiektu klasy rectangle oraz wektora, klasa wynikowa będzie
// obiektem klasy rectangle przesuniętym o podany wektor
Rectangle operator+(Rectangle, const Vector &);

// Operator dodawania, taki jak wyżej lecz chcemy obsługiwać sytuację jakby
// po prawej stronie dodawania był wektor natomiast po lewej
// obiekt klasy rectangle
Rectangle operator+(const Vector &, Rectangle);

// Operator dodawania, zwraca klasę Rectangles będącą wynikiem dodawania
// podanego obiektu klasy rectangles oraz wektora, klasa wynikowa będzie
// obiektem klasy rectangles z każdym elementów z (std::)wektora przesuniętym
// o wektor (naszą klasę)
Rectangles operator+(Rectangles, const Vector &);

// Operator dodawania, taki jak wyżej lecz chcemy obsługiwać sytuację jakby
// po prawej stronie dodawania był wektor natomiast po lewej
// obiekt klasy rectangles
Rectangles operator+(const Vector &, Rectangles);

// Funkcja łączy dwa obiekty klasy Rectangle ze wspólną krawędzią,
// będącą górną krawędzią pierwszego obiektu klasy Rectangle i
// dolną krawędzią drugiego obiektu klasy Rectangle, jeżeli prostokąty
// na siebie się nakładają, jeden byłby węższy lub nie byłyby w odpowiedniej
// w stosunku do siebie pozycji to program podnosi asercję
Rectangle merge_horizontally(const Rectangle &, const Rectangle &);

// Funkcja łączy dwa obiekty klasy Rectangle ze wspólną krawędzią, będącą
// prawą krawędzią pierwszego obiektu klasy Rectangle i lewą krawędzią
// drugiego obiektu klasy Rectangle, jeżeli prostokąty
// na siebie się nakładają, jeden byłby wyższy lub nie byłyby w odpowiedniej
// w stosunku do siebie pozycji to program podnosi asercję
Rectangle merge_vertically(const Rectangle &, const Rectangle &);

// Funkcja powoduje złączenie obiektów klasy Rectangles po kolei, na przykład
// jeśli w kolekcji byłyby cztery prostokąty, to operacja ta zwraca
// następujący obiekt typu Rectangle
// merge_?(merge_?(merge_?(rect1,rect2), rect3), rect4),
// gdzie zamiast ? wstawiamy horizontally albo vertically – za każdym razem
// wybierając to złączenie, które jest możliwe. Jeżeli w jakimkolwiek przypadku
// złączenie nie jest możliwe lub kolekcja jest pusta to funkcja podniesie
// odpowiednią asercję
Rectangle merge_all(const Rectangles &);

#endif // GEOMETRY_H