#ifndef MEMORY_H
#define MEMORY_H

// typ słowa w komórce pamięci
using WordType = int_fast64_t;

// typ indeksu w komórce pamięci
using MemType = uint_fast64_t;

// klasa pamięć, którą komputer posiada
class Memory {
private:
    // wektor trzymający pamięć naszego komputera
    std::vector<WordType> memory;
    // mapa zapisująca pod jakim indeksem znajduje się
    // dany string (do funkcji data)
    std::map<std::string, MemType> wordPlace;
    // ilość zadeklarowanych zmiennych
    MemType declaredVars;

public:
    explicit Memory(MemType size) :
        memory(size, 0),  wordPlace(),declaredVars(0) {}

    // dla danego argumentu x zwraca co w pamięci pod indeksem x się znajduje
    // albo rzuca wyjątek jak x jest większe od rozmiaru pamięci
    inline WordType valueAt(MemType x) const {
        if (x >= memory.size())
            throw std::out_of_range("Index out of bounds");

        return memory[x];
    }

    // dla danego stringu name, zwraca pod jakim indeksem string
    // został zadeklarowany lub wyjątek jeżeli nie znajduje się on na mapie
    inline MemType getVarAddr(const std::string& name) {
        if (wordPlace.find(name) == wordPlace.end())
            throw std::logic_error("Program called an undeclared variable");

        return wordPlace[name];
    }

    // ustawia wartość w pamięci pod indeksem where na wartość to, jeżeli
    // where jest większe od rozmiaru pamięci to rzuca wyjątkiem
    inline void setValue(MemType where, WordType to) {
        if (where >= memory.size())
            throw std::out_of_range("Index out of bounds");

        memory[where] = to;
    }

    // dodaje zmienną do pamięci, oraz zapisuje ją na mapę, jeżeli w pamięci
    // jest już maksymalna możliwa ilość zmiennych to rzuca wyjątek
    inline void addVariable(const std::string& id,  WordType value) {
        if (declaredVars + 1 > memory.size())
            throw std::logic_error("Declared more variables than can fit in the memory");

        memory[declaredVars] = value;

        // Z forum: Deklaracja z tą samą nazwą rezerwuje i inicjuje kolejne miejsce w pamięci,
        // przy odwołaniu odwołujemy się do pierwszego adresu.
        if (wordPlace.find(id) == wordPlace.end())
            wordPlace[id] = declaredVars;

        declaredVars++;
    }

    // funkcja pomocnicza do memory_dump
    inline void memDump(std::stringstream& str_stream) const {
        for (auto i: memory)
            str_stream << i << " ";
    }

    // przygotowuje pamięc do wykonania programu
    inline void reset() {
        wordPlace.clear();
        declaredVars = 0;
        for (auto &i : memory)
            i = 0;
    }
};

#endif //MEMORY_H
