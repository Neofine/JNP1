#include "geometry.h"
#include <utility>
#include <cassert>

// enumerator pomocniczy, pozwalający lepiej zrozumieć zwracane przez funkcję
// canIMerge wartości
enum merging {
    cannot = 0, horizontally, vertically
};

//============================ AffineSpace =====================================
const int &AffineSpace::x() const { return mX; }

const int &AffineSpace::y() const { return mY; }

AffineSpace::~AffineSpace() {};

AffineSpace::AffineSpace(int x, int y) : mX(x), mY(y) {}

AffineSpace::AffineSpace(const Position &pos) : mX(pos.x()), mY(pos.y()) {}

AffineSpace::AffineSpace(const Vector &vec) : mX(vec.x()), mY(vec.y()) {}

//============================= Position =======================================
Position::Position(int x, int y) : AffineSpace::AffineSpace(x, y) {}

Position::Position(const Vector &vec) : AffineSpace::AffineSpace(vec) {}

Position &Position::operator+=(const Vector &vec) {
    mX += vec.x();
    mY += vec.y();

    return *this;
}

bool Position::operator==(const Position &other) const {
    return mX == other.mX && mY == other.mY;
}

Position Position::reflection() const { return Position(mY, mX); }

const Position &Position::origin() {
    static Position mOrigin = Position(0, 0);
    return mOrigin;
}

//============================= Vector =========================================
Vector::Vector(int x, int y) : AffineSpace::AffineSpace(x, y) {}

Vector::Vector(const Position &pos) : AffineSpace::AffineSpace(pos) {}

Vector &Vector::operator+=(const Vector &vec) {
    mX += vec.x();
    mY += vec.y();

    return *this;
}

bool Vector::operator==(const Vector &other) const {
    return mX == other.mX && mY == other.mY;
}

Vector Vector::reflection() const { return Vector(mY, mX); }

//============================ Rectangle =======================================
Rectangle::Rectangle(int width, int height, const Position& botLeft)
        : mWidth(width), mHeight(height), mPosition(botLeft) {
    // szerokość i wysokość muszą być dodatnie
    assert(width > 0 && height > 0);
}

bool Rectangle::operator==(const Rectangle &secondOne) const {
    // klasa Position ma własny operator przyrównania więc możemy w taki
    // prosty sposób to zapisać
    return mPosition == secondOne.pos() && mWidth == secondOne.width() &&
           mHeight == secondOne.height();
}

bool Rectangle::operator!=(const Rectangle &secondOne) const {
    // funkcja przeciwna do == więc możemy zastosować linijkę poniżej
    return !(*this == secondOne);
}

Rectangle &Rectangle::operator+=(const Vector &toAdd) {
    // klasa Position ma wbudowany własny operator += więc możemy tu
    // tak minimalistycznie napisać
    mPosition += toAdd;
    return *this;
}

Rectangle Rectangle::reflection() const {
    return Rectangle(mHeight, mWidth, mPosition.reflection());
}

long long Rectangle::area() const { return static_cast<long long>(mHeight) * mWidth; }

unsigned int Rectangle::width() const { return mWidth; }

unsigned int Rectangle::height() const { return mHeight; }

Position Rectangle::pos() const { return mPosition; }

//============================== Rectangles ====================================
Rectangles::Rectangles(std::initializer_list<Rectangle> iList) noexcept:
        rects(iList) {}

const Rectangle &Rectangles::operator[](size_t index) const {
    // index nie może przekroczyć wielości kolekcji
    assert(index < rects.size());
    return rects.at(index);
}

Rectangle &Rectangles::operator[](size_t index) {
    // index nie może przekroczyć wielości kolekcji
    assert(index < rects.size());
    return rects.at(index);
}

size_t Rectangles::size() const { return rects.size(); }

bool Rectangles::operator==(const Rectangles &secondList) const {
    // jeżeli wielkości kolekcji się różnią to oczywiśnie nie są takie same
    if (size() != secondList.size())
        return false;

    // element na i-tej pozycji musi być dokładnie taki sam jak w drugim zbiorze
    // permutacje nawet tych samych elementów tu |nie| są sobie równe
    for (size_t i = 0; i < size(); i++)

        if (rects[i] != secondList[i])
            return false;

    return true;
}

Rectangles &Rectangles::operator+=(const Vector &toAdd) {
    // liniowo dodaję do każdego elementu z kolekcji podany wektor
    for (size_t i = 0; i < size(); i++) {
        rects[i] += toAdd;
    }
    return *this;
}

//===================== Definicje nonclass z .h ================================
Rectangle merge_horizontally(const Rectangle &bottom, const Rectangle &top) {
    // czy mają podstawę tej samej długości
    assert(bottom.width() == top.width());
    // czy (lewe dolne punkty) mają tą samą współrzędną x
    assert(bottom.pos().x() == top.pos().x());
    // czy (lewy dolny punkt) niższego jest o równo height() niższy do wyższego
    assert(static_cast<int>(bottom.pos().y() + bottom.height()) == top.pos().y());

    // castuję na inta gdyż normalnie byłyby unsigned intami
    return {static_cast<int>(bottom.width()),
            static_cast<int>(bottom.height() + top.height()), bottom.pos()};
}

Rectangle merge_vertically(const Rectangle &left, const Rectangle &right) {
    // czy mają podstawę tej samej długości
    assert(left.height() == right.height());
    // czy (lewe dolne punkty) mają tą samą współrzędną y
    assert(left.pos().y() == right.pos().y());
    // czy (lewy dolny punkt) lewego jest o równo width() bliżej osi OX od prawego
    assert(static_cast<int>(left.pos().x() + left.width()) == right.pos().x());

    // castuję na inta gdyż normalnie byłyby unsigned intami
    return {static_cast<int>(left.width() + right.width()),
            static_cast<int>(left.height()), left.pos()};
}

// funkcja pomocnicza zdefiniowana niżej
merging canIMerge(const Rectangle &a, const Rectangle &b);

// funkcja pomocnicza zdefiniowana niżej
Rectangle merge(const Rectangle &a, const Rectangle &b);

Rectangle merge_all(const Rectangles &rects) {
    // kolekcja nie może być pusta
    assert(rects.size() != 0);
    // złączenie kolekcji 1-elementowej jest tą samą kolekcją
    if (rects.size() == 1)
        return rects[0];

    // czy mogę jakkolwiek połączyć element 0-owy i 1-szy
    assert(canIMerge(rects[0], rects[1]));
    Rectangle outcome = merge(rects[0], rects[1]);

    for (size_t i = 2; i < rects.size(); i++) {
        assert(canIMerge(outcome, rects[i]));
        outcome = merge(outcome, rects[i]);
    }

    return outcome;
}

//==================== Definicje funkcji pomocniczych ==========================
// funkcja sprawdzająca w jaki sposób mogę połączyć dwa elementy klasy rectangle
// zwraca enumerator, jeżeli zwróci 'horizontally' to mogę horyzontalnie
// połączyć, jeżeli zwróci 'vertically' to mogę wertyklanie, natomiast
// jeżeli wynikiem będzie 'cannot' to nie mogę ich nijak połączyć
merging canIMerge(const Rectangle &a, const Rectangle &b) {
    if (a.width() == b.width() && a.pos().x() == b.pos().x() &&
        static_cast<int>(a.pos().y() + a.height()) == b.pos().y())
        return horizontally;

    if (a.height() == b.height() && a.pos().y() == b.pos().y() &&
        static_cast<int>(a.pos().x() + a.width()) == b.pos().x())
        return vertically;

    return cannot;
}

// funkcja łącząca 2 elementy klasy rectangle, wywoływana na etapie na którym
// wiadomo, że mogę je w jakiś sposób połączyć
Rectangle merge(const Rectangle &a, const Rectangle &b) {
    if (canIMerge(a, b) == horizontally)
        return merge_horizontally(a, b);
    return merge_vertically(a, b);
}

//==================== Definicje operatorów dodawania ==========================
Position operator+(Position pos, const Vector &vec) {
    pos += vec;
    return pos;
}

Position operator+(const Vector &vec, Position pos) {
    pos += vec;
    return pos;
}

Vector operator+(Vector vec1, const Vector &vec2) {
    vec1 += vec2;
    return vec1;
}

Rectangle operator+(Rectangle recs, const Vector &vec) {
    recs += vec;
    return recs;
}

Rectangle operator+(const Vector &vec, Rectangle recs) {
    recs += vec;
    return recs;
}

// mimo że nie jest to oczywiste to kompilator sam rozumie kiedy może dać
// rvalue a kiedy lvalue więc ze strony programistycznej taka funkcja będzie
// w 100% wystarczająca do poprawnego i szybkiego działania programu
Rectangles operator+(Rectangles recs, const Vector &vec) {
    recs += vec;
    return recs;
}

// jak wyżej, ale chcę żeby klasa Rectangles będąca po obu stronach znaku '+'
// była wyczytana jako ta operacja
Rectangles operator+(const Vector &vec, Rectangles recs) {
    recs += vec;
    return recs;
}