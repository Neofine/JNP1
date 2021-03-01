#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <stdexcept>
#include <array>

#ifndef JNP1_4_COMPUTER_H
#define JNP1_4_COMPUTER_H

// typ reprezentujący hasz, którym zmieniamy tekst z funkcji Id na liczbę
using id_hash = uint64_t;

// Komenda Id z treści, jest funkcją z powodu ograniczenia technicznego,
// jeżeli na wejściu dostaje zgodny z poleceniem tekst to zmienia go na hasz
// tego słowa (ignorując przy tym różnicę między wielkimi i małymi literami)
constexpr id_hash Id(const char *what) {
    size_t it = 0;
    id_hash hash = 0;
    while (what[it] != '\0') {
        if(!((what[it] >= 'a' && what[it] <= 'z') ||
             (what[it] >= 'A' && what[it] <= 'Z') ||
             (what[it] >= '0' && what[it] <= '9')))
            throw std::length_error
                    ("Letters and numbers are only allowed!");

        it++;
    }
    if(it == 0 || it > 6)
        throw std::length_error
                ("Words can't be empty or exceed 6 letters length!");
    const int p = 67;
    for (size_t i = 0; i < it; i++) {
        char c = what[i];
        if (what[i] >= 'A' && what[i] <= 'Z')
            c = static_cast<char>(what[i] + 'a' - 'A');
        hash = hash * p + c;
    }
    return hash;
}

// Poniżej wszystkie structy są puste, zrobiliśmy tak, gdyż jeszcze niżej mamy
// klasę Executors który specjalizuje jeden template wokół tych pustych structów

template<typename type>
struct Mem {
};

template<id_hash i>
struct Lea {
};

template<id_hash i>
struct Label {
};

template<id_hash i>
struct Jmp {
};

template<id_hash i>
struct Js {
};

template<id_hash i>
struct Jz {
};

template<id_hash i, typename val>
struct D {
};

template<auto inner>
struct Num {
};

template<typename Arg1, typename Arg2>
struct Mov {
};

template<typename Arg1, typename Arg2>
struct Add {
};

template<typename Arg1, typename Arg2>
struct Sub {
};

template<typename Arg>
struct Inc {
};

template<typename Arg>
struct Dec {
};

template<typename Arg1, typename Arg2>
struct And {
};

template<typename Arg1, typename Arg2>
struct Or {
};

template<typename Arg>
struct Not {
};

template<typename Arg1, typename Arg2>
struct Cmp {
};

template<typename ... args>
struct Program {
};

template<typename first_type, typename ... args>
struct Program<first_type, args ...> {
    using first = first_type;
    using others = Program<args ...>;
};

template<typename first_type>
struct Program<first_type> {
    using first = first_type;
    using others = void;
};

template<>
struct Program<> {
    using first = void;
    using others = void;
};

// Klasa zawierająca funkcje wykonujące podane z wejścia komendy.
template<typename value_t, size_t mem_size>
class executors {

    // Enumerator do tablicy pomocniczej, w którym:
    // ZFlag oznacza indeks pod którym się znajduje "zero flag" programu
    // SFlag oznacza indeks pod którym się znajduje "signed flag" programu
    // LastVar oznacza indeks pod którym znajduje się zmienna przedstawiająca
    // na którym miejscu w tablicy nie ma jeszcze nic zadeklarowanego.
    enum global {
        ZFlag = mem_size, SFlag, LastVar
    };

    // Typ który przedstawia wygląd tablicy która służy jako pamięć komputera.
    using memory_t = std::array<value_t, mem_size>;

    // Typ przedstawiający tablicę pomocniczą na której trzymam hasze
    // zadeklarowanych zmiennych, flagi oraz zmiennną globalną przedstawiającą
    // indeks pierwszego miejsca na którym jeszcze nie ma żadnej zmiennej
    // zadeklarowanej.
    using misc_t = std::array<id_hash, mem_size + 3>;

    // Tutaj deklarujemu ogólny struct executor który będziemy
    // specjalizowac ponizej.
    template<typename operation>
    struct executor;

    template<typename type>
    struct isNumeric {
        constexpr static bool value = false;
    };

    template<auto i>
    struct isNumeric<Num<i>> {
        constexpr static bool value = true;
    };

    template<typename type>
    struct isValidRvalue {
        constexpr static bool value = false;
    };

    template<typename inner>
    struct isValidRvalue<Mem<inner>> {
        constexpr static bool value = true;
    };

    template<id_hash i>
    struct isValidRvalue<Lea<i>> {
        constexpr static bool value = true;
    };

    template<auto inner>
    struct isValidRvalue<Num<inner>> {
        constexpr static bool value = true;
    };

    template<typename type>
    struct isValidLvalue {
        constexpr static bool value = false;
    };

    template<typename type>
    struct isValidLvalue<Mem<type>> {
        constexpr static bool value = true;
    };

    template<typename type>
    constexpr static void testRvalue() {
        static_assert(isValidRvalue<type>::value, "Invalid rvalue");
    }

    template<typename type>
    constexpr static void testLvalue() {
        static_assert(isValidLvalue<type>::value, "Invalid rvalue");
    }

    // Sprawdza, czy dany typ jest instrukcja wykonywalna bezposrednio
    // przez program,  a nie np. taka, ktora moze sie pojawic wylacznie jako
    // podinstrukcja innej instrukcji.
    template<typename type>
    struct isCallable {
        constexpr static bool value = true;
    };

    template<typename type>
    struct isCallable<Mem<type>> {
        constexpr static bool value = false;
    };

    template<id_hash i>
    struct isCallable<Lea<i>> {
        constexpr static bool value = false;
    };

    template<auto inner>
    struct isCallable<Num<inner>> {
        constexpr static bool value = false;
    };

    template<typename type>
    constexpr static void testCallability() {
        static_assert(isCallable<type>::value, "Invalid operation");
    }

    // Sprawdza poprawnosc argumentow w operacjach Mov, Sub, Or itp.
    template<typename Arg1, typename Arg2>
    constexpr static void assertArithmeticValidity() {
        testLvalue<Arg1>();
        testRvalue<Arg2>();
    }

    // Executor dla komendy Mem<inner_t>.
    template<typename inner_t>
    struct executor<Mem<inner_t>> {

        // Funkcja która zapisuje na miejsce Mem<inner_t> podaną wartość what.
        constexpr static void save(memory_t &memory, misc_t &misc_things,
                                   value_t what) {
            id_hash idx = executor<inner_t>::value(memory, misc_things);

            if (static_cast<size_t>(idx) >= mem_size)
                throw std::length_error("Out of borders");

            memory[idx] = what;
        }

        // Funkcja która zwraca wartość w tablicy pod komórką inner_t.
        constexpr static value_t value(memory_t &memory, misc_t &misc_things) {
            id_hash idx = executor<inner_t>::value(memory, misc_things);
            testRvalue<inner_t>();
            if (static_cast<size_t>(idx) >= mem_size)
                throw std::length_error("Out of borders");

            return memory[idx];
        }

    };

    // executor dla komendy Num<inner_t>
    template<auto inner_t>
    struct executor<Num<inner_t>> {

        // jeżeli podane tablice są poprawne zwraca wartość podaną w komendzie
        constexpr static auto value(memory_t &memory, misc_t &misc_things) {
            static_assert(std::is_lvalue_reference<decltype(memory)>::value);
            static_assert
                    (std::is_lvalue_reference<decltype(misc_things)>::value);
            return inner_t;
        }

    };

    // funkcja dla argumentu który właśnie został wyliczony zmienia flagi
    // programu będące na tablicy misc_things
    constexpr static void raise_flags(misc_t &misc_things, value_t outcome) {
        if (outcome == 0) {
            misc_things[SFlag] = 0;
            misc_things[ZFlag] = 1;
        }
        else if (outcome < 0) {
            misc_things[SFlag] = 1;
            misc_things[ZFlag] = 0;
        }
        else {
            misc_things[SFlag] = 0;
            misc_things[ZFlag] = 0;
        }
    }

    // funkcja dla argumentu który właśnie został wyliczony (z komend typu:
    // And, Or, Not) zmienia flagę ZFlag programu będącą na tablicy misc_things
    constexpr static void check_zero_flag(misc_t &misc_things,
                                          value_t outcome) {
        if (outcome == 0)
            misc_things[ZFlag] = 1;
        else misc_things[ZFlag] = 0;
    }

    // Przy inicjalizacji nalezy zignorowac wszystkie instrukcje poza D, ale
    // jednoczesnie sprawdzic poprawnosc wszystkich instrukcji.
    template<typename type>
    struct initializer{

        constexpr static void initialize(memory_t &memory, misc_t &misc_things){
            static_assert(std::is_lvalue_reference<decltype(memory)>::value);
            static_assert
                    (std::is_lvalue_reference<decltype(misc_things)>::value);
        }

    };

    // cmp

    template<typename Arg1, typename Arg2>
    struct initializer<Cmp<Arg1, Arg2>>{

        constexpr static void initialize(memory_t &memory, misc_t &misc_things){
            testRvalue<Arg1>();
            testRvalue<Arg2>();
            static_assert(std::is_lvalue_reference<decltype(memory)>::value);
            static_assert
                    (std::is_lvalue_reference<decltype(misc_things)>::value);
        }

    };

    template<typename Arg1>
    struct initializer<Not<Arg1>>{

        constexpr static void initialize(memory_t &memory, misc_t &misc_things){
            testLvalue<Arg1>();
            static_assert(std::is_lvalue_reference<decltype(memory)>::value);
            static_assert
                    (std::is_lvalue_reference<decltype(misc_things)>::value);
        }

    };

    template<typename Arg1>
    struct initializer<Inc<Arg1>>{

        constexpr static void initialize(memory_t &memory, misc_t &misc_things){
            testLvalue<Arg1>();
            static_assert(std::is_lvalue_reference<decltype(memory)>::value);
            static_assert
                    (std::is_lvalue_reference<decltype(misc_things)>::value);
        }

    };

    template<typename Arg1>
    struct initializer<Dec<Arg1>>{

        constexpr static void initialize(memory_t &memory, misc_t &misc_things){
            testLvalue<Arg1>();
            static_assert(std::is_lvalue_reference<decltype(memory)>::value);
            static_assert
                    (std::is_lvalue_reference<decltype(misc_things)>::value);
        }

    };

    template<typename Arg1, typename Arg2>
    struct initializer<Add<Arg1, Arg2>>{

        constexpr static void initialize(memory_t &memory, misc_t &misc_things){
            assertArithmeticValidity<Arg1, Arg2>();
            static_assert(std::is_lvalue_reference<decltype(memory)>::value);
            static_assert
                    (std::is_lvalue_reference<decltype(misc_things)>::value);
        }

    };

    template<typename Arg1, typename Arg2>
    struct initializer<Mov<Arg1, Arg2>>{

        constexpr static void initialize(memory_t &memory, misc_t &misc_things){
            assertArithmeticValidity<Arg1, Arg2>();
            static_assert(std::is_lvalue_reference<decltype(memory)>::value);
            static_assert
                    (std::is_lvalue_reference<decltype(misc_things)>::value);
        }

    };

    template<typename Arg1, typename Arg2>
    struct initializer<Sub<Arg1, Arg2>>{

        constexpr static void initialize(memory_t &memory, misc_t &misc_things){
            assertArithmeticValidity<Arg1, Arg2>();
            static_assert(std::is_lvalue_reference<decltype(memory)>::value);
            static_assert
                    (std::is_lvalue_reference<decltype(misc_things)>::value);
        }

    };

    template<typename Arg1, typename Arg2>
    struct initializer<And<Arg1, Arg2>>{

        constexpr static void initialize(memory_t &memory, misc_t &misc_things){
            assertArithmeticValidity<Arg1, Arg2>();
            static_assert(std::is_lvalue_reference<decltype(memory)>::value);
            static_assert
                    (std::is_lvalue_reference<decltype(misc_things)>::value);
        }

    };

    template<typename Arg1, typename Arg2>
    struct initializer<Or<Arg1, Arg2>>{

        constexpr static void initialize(memory_t &memory, misc_t &misc_things){
            assertArithmeticValidity<Arg1, Arg2>();
            static_assert(std::is_lvalue_reference<decltype(memory)>::value);
            static_assert
                    (std::is_lvalue_reference<decltype(misc_things)>::value);
        }

    };

    template<id_hash Arg1, typename Arg2>
    struct initializer<D<Arg1, Arg2>>{

        // Przy inicjalizacji wykonuje tylko instrukcje D,
        // czyli tylko tą funkcję
        constexpr static void initialize(memory_t &memory, misc_t &misc_things){

            static_assert(isNumeric<Arg2>::value);

            if (misc_things[LastVar] >= mem_size)
                throw std::length_error
                        ("You want to save too much and have too little memory");

            misc_things[misc_things[LastVar]] = Arg1;
            memory[misc_things[LastVar]] =
                    executor<Arg2>::value(memory, misc_things);

            misc_things[LastVar]++;
        }

    };

    // executor komendy Add<Arg1, Arg2>
    template<typename Arg1, typename Arg2>
    struct executor<Add<Arg1, Arg2>> {

        // funkcja (o ile Arg1 oraz Arg2 są poprawnymi i zgodnymi z treścią
        // typename'ami) wykonuje dodawanie oraz jego wynik zapisuje na
        // miejscu pierwszego argumentu.
        constexpr static void execute(memory_t &memory, misc_t &misc_things) {
            assertArithmeticValidity<Arg1, Arg2>();
            value_t outcome = executor<Arg1>::value(memory, misc_things) +

                              executor<Arg2>::value(memory, misc_things);

            raise_flags(misc_things, outcome);
            executor<Arg1>::save(memory, misc_things, outcome);
        }

    };

    // executor komendy Sub<Arg1, Arg2>
    template<typename Arg1, typename Arg2>
    struct executor<Sub<Arg1, Arg2>> {

        // funkcja (o ile Arg1 oraz Arg2 są poprawnymi i zgodnymi z treścią
        // typename'ami) wykonuje odejmowanie oraz jego wynik zapisuje na
        // miejscu pierwszego argumentu.
        constexpr static void execute(memory_t &memory, misc_t &misc_things) {
            assertArithmeticValidity<Arg1, Arg2>();
            value_t outcome = executor<Arg1>::value(memory, misc_things) -

                              executor<Arg2>::value(memory, misc_things);

            raise_flags(misc_things, outcome);
            executor<Arg1>::save(memory, misc_things, outcome);
        }

    };

    // executor komendy Cmp<Arg1, Arg2>
    template<typename Arg1, typename Arg2>
    struct executor<Cmp<Arg1, Arg2>> {

        // funkcja (o ile Arg1 oraz Arg2 są poprawnymi i zgodnymi z treścią
        // typename'ami) wykonuje odejmowanie podnosząc odpowiednie flagi, lecz
        // nie zapisuje nigdzie wyniku.
        constexpr static void execute(memory_t &memory, misc_t &misc_things) {
            testRvalue<Arg1>();
            testRvalue<Arg2>();
            value_t outcome =
                    executor<Arg1>::value(memory, misc_things) -

                    executor<Arg2>::value(memory, misc_things);
            raise_flags(misc_things, outcome);
        }

    };

    // executor komendy Inc<Arg1>
    template<typename Arg1>
    struct executor<Inc<Arg1>> {

        // funkcja (o ile Arg1 jest poprawnym i zgodnym z treścią
        // typename'em) zwiększa jego wartość o 1.
        constexpr static void execute(memory_t &memory, misc_t &misc_things) {
            testLvalue<Arg1>();
            value_t outcome = executor<Arg1>::value(memory, misc_things) + 1;
            raise_flags(misc_things, outcome);
            executor<Arg1>::save(memory, misc_things, outcome);
        }

    };

    // executor komendy Dec<Arg1>
    template<typename Arg1>
    struct executor<Dec<Arg1>> {

        // funkcja (o ile Arg1 jest poprawnym i zgodnym z treścią
        // typename'em) zmniejsza jego wartość o 1.
        constexpr static void execute(memory_t &memory, misc_t &misc_things) {
            testLvalue<Arg1>();
            value_t outcome = executor<Arg1>::value(memory, misc_things) - 1;
            raise_flags(misc_things, outcome);
            executor<Arg1>::save(memory, misc_things, outcome);
        }

    };


    // executor komendy And<Arg1, Arg2>
    template<typename Arg1, typename Arg2>
    struct executor<And<Arg1, Arg2>> {

        // funkcja (o ile Arg1 oraz Arg2 są poprawnymi i zgodnymi z treścią
        // typename'ami) wykonuje operację bitową AND, zapisując wynik
        // na miejscu pierwszego argumentu.
        constexpr static void execute(memory_t &memory, misc_t &misc_things) {
            assertArithmeticValidity<Arg1, Arg2>();
            value_t outcome = executor<Arg1>::value(memory, misc_things) &
                              executor<Arg2>::value(memory, misc_things);
            check_zero_flag(misc_things,outcome);
            executor<Arg1>::save(memory, outcome);
        }

    };

    // executor komendy Or<Arg1, Arg2>
    template<typename Arg1, typename Arg2>
    struct executor<Or<Arg1, Arg2>> {

        // funkcja (o ile Arg1 oraz Arg2 są poprawnymi i zgodnymi z treścią
        // typename'ami) wykonuje operację bitową OR, zapisując wynik
        // na miejscu pierwszego argumentu.
        constexpr static void execute(memory_t &memory, misc_t &misc_things) {
            assertArithmeticValidity<Arg1, Arg2>();
            value_t outcome = executor<Arg1>::value(memory, misc_things) |
                              executor<Arg2>::value(memory, misc_things);
            check_zero_flag(misc_things, outcome);
            executor<Arg1>::save(memory, outcome);
        }

    };

    // executor komendy Not<Arg1>
    template<typename Arg>
    struct executor<Not<Arg>> {

        // funkcja (o ile Arg1 jest poprawnym i zgodnym z treścią
        // typename'em) wykonuje na nim operację bitową NOT.
        constexpr static void execute(memory_t &memory, misc_t &misc_things) {
            testLvalue<Arg>();
            value_t outcome = !executor<Arg>::value(memory, misc_things);
            check_zero_flag(misc_things, outcome);
            executor<Arg>::save(memory, outcome);
        }

    };

    // Executor komendy Lea<id_hash>, generalnie w treści powinno działać
    // Lea<Id(tekst)>, lecz nasza komenda Id(tekst) zwraca zahaszowany tekst
    // więc tutaj przyjmujemy id_hash.
    template<id_hash hash>
    struct executor<Lea<hash>> {

        // zwraca indeks pod którym pierwsza zmienna która ma ten sam hasz
        // została zadeklarowana, jeżeli takiej nie ma to rzuca wyjatek. Zawsze
        // wywołując tą komendę musimy być pewni że jest taka zmienna.
        constexpr static size_t value(memory_t &memory, misc_t &misc_things) {
            static_assert(std::is_lvalue_reference<decltype(memory)>::value);
            static_assert
                    (std::is_lvalue_reference<decltype(misc_things)>::value);
            for (size_t i = 0; i < mem_size; i++) {
                if (misc_things[i] == hash)
                    return i;
            }
            throw std::logic_error("No text found");
        }

    };

    // executor komendy Mov<Arg1, Arg2>.
    template<typename Arg1, typename Arg2>
    struct executor<Mov<Arg1, Arg2>> {

        // funkcja przekopiowuje wartość będącą pod Arg2 na pozycję Arg1
        // jeżeli Arg1 lub Arg2 są niepoprawnymi typename'ami wywoła się
        // static assert.
        constexpr static void execute(memory_t &memory, misc_t &misc_things) {
            assertArithmeticValidity<Arg1, Arg2>();

            executor<Arg1>::save
                    (memory, misc_things, executor<Arg2>::value(memory, misc_things));
        }

    };

    // executor komendy D<id_hash, Arg2>, argumentacja dlaczego pierwszy
    // argument jest typu id_hash jest taka sama jak przy Lea.
    template<id_hash Arg1, typename Arg2>
    struct executor<D<Arg1, Arg2>> {

        // Funkcja sprawdza jedynie poprawność tablic, nic nie wykonuje, gdyż
        // D powinno się wykonać przed wszystkimi komendami na samym początku
        // - do tego celu ma stosowny initializer.
        constexpr static void execute(memory_t &memory, misc_t &misc_things) {
            static_assert(std::is_lvalue_reference<decltype(memory)>::value);
            static_assert(std::is_lvalue_reference<decltype(misc_things)>::value);
        }

    };

    template<>
    struct executor<void> {

        constexpr static void execute(memory_t &memory, misc_t &misc_things) {
            static_assert(std::is_lvalue_reference<decltype(memory)>::value);
            static_assert(std::is_lvalue_reference<decltype(misc_things)>::value);
        }

    };

    template<id_hash i>
    struct executor<Label<i>> {

        constexpr static void execute(memory_t &memory, misc_t &misc_things) {
            static_assert(std::is_lvalue_reference<decltype(memory)>::value);
            static_assert
                    (std::is_lvalue_reference<decltype(misc_things)>::value);
        }

    };

public:

    // Funkcje wywolywane z klasy Computer, wywolujace z kolei wykonania
    // odpowiednich instrukcji / inicjalizacji.
    template<typename type>
    constexpr static void genericExecute(memory_t &memory, misc_t &misc_things){
        executor<type>::execute(memory, misc_things);
    }

    template<typename type>
    constexpr static void genericInit(memory_t &memory, misc_t &misc_things){
        testCallability<type>();
        initializer<type>::initialize(memory, misc_things);
    }
};


template<size_t mem_size, typename value_t>
class Computer {
private:
    static_assert(mem_size > 0, "Invalid memory size");
    static_assert(std::is_integral<value_t>::value, "Invalid word type");

    // enum i using'i tak jak w klasie Executors, nie możemy tego niestety
    // uwspólnić nie łącząc tych dwóch klas, gdyż potrzebujemy template'a
    // mem_size zarówno w Computerze jak i w Executorze
    enum global {
        ZFlag = mem_size, SFlag
    };

    using memory_t = std::array<value_t, mem_size>;
    using misc_t = std::array<id_hash, mem_size + 3>;

    // Inicjalizacja zmiennych programu i sprawdzenie poprawnosci operacji.s
    template<typename input>
    constexpr static void initialize(memory_t &memory, misc_t &misc_things) {
        static_assert(std::is_lvalue_reference<decltype(memory)>::value);
        static_assert(std::is_lvalue_reference<decltype(misc_things)>::value);
        executors<value_t, mem_size>::template
        genericInit<typename input::first>(memory, misc_things);
        initialize<typename input::others>(memory, misc_things);
    }

    template<>
    constexpr static void initialize<void>(memory_t &memory,
                                           misc_t &misc_things) {
        static_assert(std::is_lvalue_reference<decltype(memory)>::value);
        static_assert(std::is_lvalue_reference<decltype(misc_things)>::value);
    }

    template<typename type>
    struct isJump {
        constexpr static bool value = false;
    };

    template<id_hash i>
    struct isJump<Jmp<i>> {
        constexpr static bool value = true;
    };

    template<id_hash i>
    struct isJump<Jz<i>> {
        constexpr static bool value = true;
    };

    template<id_hash i>
    struct isJump<Js<i>> {
        constexpr static bool value = true;
    };

    template<typename type>
    struct isZeroJump {
        constexpr static bool value = false;
    };

    template<id_hash i>
    struct isZeroJump<Jz<i>> {
        constexpr static bool value = true;
    };

    template<typename type>
    struct isSignJump {
        constexpr static bool value = false;
    };

    template<id_hash i>
    struct isSignJump<Js<i>> {
        constexpr static bool value = true;
    };

    template<typename type>
    struct isUnconditionalJump {
        constexpr static bool value = false;
    };

    template<id_hash i>
    struct isUnconditionalJump<Jmp<i>> {
        constexpr static bool value = true;
    };

    // Struktura pomocnicza do sprawdzania, do jakiej etykiety ma
    // sie odbyc dany skok
    template<typename inner>
    struct getLabelId{};

    template<id_hash i>
    struct getLabelId<Jmp<i>> {
        constexpr static id_hash value = i;
    };

    template<id_hash i>
    struct getLabelId<Jz<i>> {
        constexpr static id_hash value = i;
    };

    template<id_hash i>
    struct getLabelId<Js<i>> {
        constexpr static id_hash value = i;
    };

    // Struktura pomocnicza do wykonywania instrukcji programu.
    template<typename ... types>
    struct runner {

    };

    template<typename first>
    constexpr static bool canSignJump(misc_t &misc_things){
        if constexpr (isSignJump<first>::value){
            if(misc_things[SFlag]) return true;
            else return false;
        }
        else return false;
    }

    template<typename first>
    constexpr static bool canZeroJump(misc_t &misc_things){
        if constexpr (isZeroJump<first>::value){
            if(misc_things[ZFlag]) return true;
            else return false;
        }
        else return false;
    }

    template<typename input, typename original>
    struct runner<input, original>{

        constexpr static void run(memory_t &memory, misc_t &misc_things) {
            using first = typename input::first;
            using others = typename input::others;
            if constexpr (isJump<first>::value){
                // Instrukcja skoku - szukanie etykiety zaczyna sie
                // od poczatku programu.
                if (canSignJump<first>(misc_things)) {
                    runner<original, original,
                            Label<getLabelId<first>::value>>::run
                            (memory, misc_things);
                }
                else if (canZeroJump<first>(misc_things)){
                    runner<original, original,
                            Label<getLabelId<first>::value>>::run
                            (memory, misc_things);
                }
                else if(isUnconditionalJump<first>::value){
                    runner<original, original,
                            Label<getLabelId<first>::value>>::run
                            (memory, misc_things);
                }
                else{
                    runner<others, original>::run(memory, misc_things);
                }

            }
            else {
                // Komputer wykonuje pierwsza instrukcje i wywoluje
                // rekurencyjnie wykonanie reszty.
                executors<value_t, mem_size>::template
                genericExecute<typename input::first>(memory, misc_things);
                runner<others, original>::run(memory, misc_things);
            }
        }

    };

    // Specjalizacja sluzaca do przewijania do danej etykiety
    // (od poczatku programu)
    template<typename input, typename original, id_hash i>
    struct runner<input, original, Label<i>>{

        constexpr static void run(memory_t &memory, misc_t &misc_things) {
            if (std::is_void<input>::value){
                throw std::logic_error("Jump to a non-existent label");
            }
            else {
                using first = typename input::first;
                using others = typename input::others;
                // Sprawdzamy, czy dotarlismy do etykiety.
                if constexpr (std::is_same<first, Label<i>>::value) {
                    runner<others, original>::run(memory, misc_things);
                }
                else {
                    runner<others, original, Label<i>>::run(memory, misc_things);
                }
            }
        }
    };

    // Specjalizacja konczaca rekursje.
    template<typename original>
    struct runner<void, original> {

        constexpr static void run(memory_t &memory, misc_t &misc_things) {
            static_assert(std::is_lvalue_reference<decltype(memory)>::value);
            static_assert
                    (std::is_lvalue_reference<decltype(misc_things)>::value);
        }
    };

public:

    template<typename input>
    constexpr static std::array<value_t, mem_size> boot() {
        memory_t memory{0};
        misc_t helper{0};

        if (mem_size > std::numeric_limits
                <typename std::make_unsigned<value_t>::type>::max())
            throw std::logic_error("Can't declare that much memory");

        // Komputer najpierw przeglada caly program i inicjalizuje zmienne,
        // a potem wykonuje pozostale instrukcje.
        initialize<input>(memory, helper);
        runner<input, input>::run(memory, helper);
        return memory;
    }

};

#endif //JNP1_4_COMPUTER_H
