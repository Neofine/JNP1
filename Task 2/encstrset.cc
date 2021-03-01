// Projekt 2, grupa numer 5
// Autorzy:
// Jakub Panasiuk, jp418362
// Paweł Sarzyński, ps418407
#include "encstrset.h"
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <iomanip>

namespace {
    using std::string;

    // wszystkie wypisywane rzeczy do których będę adresował poniżej jako
    // komunikaty (lub wiadomości) są wypisywane na stderr, żeby je wyłączyć
    // trzeba skompilować program z flagą -DNDEBUG

    // jeżeli program został skompilowany z flagą -DNDEBUG to ustawiam zmienną
    // pomocniczą żeby później w prosty sposób monitorować czy potrzebuję
    // wypisywać komunikaty czy też nie
#ifdef NDEBUG
    static const bool debug = false;
#else  // !NDEBUG
    static const bool debug = true;
#endif // NDEBUG

    // typ który stosuję do przechowywania wartości identyfikatorów zbiorów
    // jest on zawsze nieujemy
    using Idset = unsigned long;

    // typ który dostaję z argumentów funkcji które miałem zaimplementować
    // zmieniam go później na stringi dla wygody oraz zachowania prostoty
    // struktury programu
    using Argvalue = const char*;

    // typ który stosuję do przechowywania wartości, szyfru oraz zaszyfrowanego
    // już kodu, jest on typu const, gdyż jak te stringi zostaną raz ustalone
    // to nie zmieniam ich już później
    using Constvalue = const string&;

    // typ odpowiadający pojedynczemu setowi na którym wykonywane są operacje
    // ten set będzie przechowywany na mapie
    using Singleset = std::unordered_set<string>;

    // typ z którego korzystamy do inicjalizowania i odwoływania się
    // do mapy setów na której przechowujemy aktualny stan programu
    using Mapofsets = std::unordered_map<Idset, Singleset>;

    // typ odpowiadający iteratorowi Mapofsets
    using Mapiter = Mapofsets::iterator;

    // typ służący odwoływaniu się do różnych wielkości stringów/setów
    using Setsize = size_t;

    // funkcja która zwraca mapę na której przechowujemy sety, zapisana jest
    // w taki sposób żeby zapobiegać static initialisation problem
    static Mapofsets &get_map() {
        static Mapofsets map;
        return map;
    }

    // funkcja która zwraca mapę identyfikator największego obecnie posiadanego
    // setu, zapisana jest w taki sposób żeby zapobiec static initialisation problem
    static Idset &get_new_id() {
        static Idset newId = 0;
        return newId;
    }

    // funkcja zwracająca strumień na który będę wypisywał komunikaty
    // jeżeli zmienna debug jest ustawiona na true to wypisuję je na
    // wyjście cerr, w przeciwnym wypadku na ostream(nullptr) czyli wypisuję
    // do pustki (nigdzie nie będzie widać tych komunikatów)
    static std::ostream &get_stream() {
        static std::ios_base::Init init;
        static std::ostream *global_stream =
                debug ? &std::cerr : new std::ostream(nullptr);

        return *global_stream;
    }

    // przeciążam operator xor na stringach żeby zwrócić wynik
    // xorowania jaki był żądany od nas w treści zadania
    static string operator ^(Constvalue value, Constvalue cipher) {
        if (cipher.empty())
            return value;

        string outcome;
        for (Setsize i = 0; i < value.size(); i++) {
            outcome += value[i] ^ cipher[i % cipher.size()];
        }

        return outcome;
    }

    // wypisuję zaszyfrowany już tekst na wyjście, wypisywany jest w kodzie
    // hexadecymalnym i każda wartość odpowiada wartości kodu ascii (hex)
    static void print_hex(Constvalue text) {
        for (Setsize i = 0; i < text.length(); i++) {
            get_stream() << std::setw(2) << std::setfill('0') <<
                         std::uppercase << std::hex <<
                         static_cast<unsigned int>
                         (static_cast<unsigned char>(text[i]));
            if (i < text.length() - 1)
                get_stream() << " ";
        }
    }

    // sprawdza czy iterator znajduje się na mapie
    static bool in_map(Mapiter it) {
        return it != get_map().end();
    }

    // sprawdza czy na mapie znajduje się zbiór o numerze grugiego argumentu
    // jeżeli tak wypisuje odpowiedni komunikat i zwaca iterator na niego
    static auto check_if_exists(Constvalue name, Idset nr) {
        auto it = get_map().find(nr);
        if (!in_map(it))
            get_stream() << "encstrset_" << name << ": set #"
                         << nr << " does not exist\n";

        return it;
    }

    // funkcja dla podanego wskaźnika na tablicę charów wypisuje
    // "NULL", jeżeli wskaźnik na nic nie wskazuje lub wypisuje ją w
    // formacie "\"wypisywana_tablica"\"
    static void show_string(Argvalue value) {
        if (nullptr == value)
            get_stream() << "NULL";
        else
            get_stream() << "\"" << value << "\"";
    }

    // funkcja wypisuje komunikat wyświetlający nazwę wykonywanej teraz
    // operacji, identyfikator setu na którym operujemy i zawartość tablic
    // value oraz key
    static void common_show(Constvalue name, Idset id,
                            Argvalue value, Argvalue key) {
        get_stream() << "encstrset_" << name << "(" << id << ", ";
        show_string(value);
        get_stream() << ", ";
        show_string(key);
        get_stream() << ")\n";
    }

    // funkcja która dla poprawnych danych zwraca mapę pod indeksem id oraz
    // wypisuje (oprócz common_show) wartość val zaszyfrowaną kluczem key
    // w systemie szesnastkowym, dodatkowo zapisuje (już nie w systemie
    // szesnastkowym) ją na string res, dla niepoprawnych danych kończy
    // się opowiednim komunikatem
    static auto code(Constvalue nm, string &res, Idset id,
                     Argvalue value, Argvalue key) {
        common_show(nm, id, value, key);
        auto end = get_map().end();
        if (nullptr == value) {
            get_stream() << "encstrset_" << nm <<
                         ": invalid value (NULL)\n";
            return end;
        }

        auto it = check_if_exists(nm, id);
        if (!in_map(it))
            return end;

        string valueCopy = value;
        string keyCopy = (nullptr == key) ? "" : key;
        res = valueCopy ^ keyCopy;

        get_stream() << "encstrset_" << nm <<
                     ": set #" << id << ", cypher \"";
        print_hex(res);

        return it;
    }

    // funkcja jest wywoływana gdy przepisywany szyfr był już w drugim secie
    // wywołuje odpowiedni komunikat
    static void cypher_not_copied(Constvalue cypher, Idset dst) {
        get_stream() << "encstrset_copy: copied cypher \"";
        print_hex(cypher);
        get_stream() << "\" was already present in set #" << dst << "\n";
    }

    // funkcja jest wywoływana gdy z sukcesem przepisano szyfr na drugi set
    // wywołuje odpowiedni komunikat
    static void cypher_copied(Constvalue cypher, Idset src, Idset dst) {
        get_stream() << "encstrset_copy: cypher \"";
        print_hex(cypher);
        get_stream() << "\" copied from set #" << src <<
                     " to set #" << dst << "\n";
    }
} // namespace

namespace jnp1 {
    Idset encstrset_new() {
        get_stream() << "encstrset_new()\n";

        Singleset new_set;
        get_map().insert({get_new_id(), new_set});

        get_stream() << "encstrset_new: set #" << get_new_id() << " created\n";

        return get_new_id()++;
    }

    void encstrset_delete(Idset id) {
        get_stream() << "encstrset_delete(" << id << ")\n";

        auto it = check_if_exists("delete", id);
        if (in_map(it)) {
            get_map().erase(it);
            get_stream() << "encstrset_delete: set #" << id << " deleted\n";
        }
    }

    Setsize encstrset_size(Idset id) {
        get_stream() << "encstrset_size(" << id << ")\n";

        auto it = check_if_exists("size", id);
        if (in_map(it)) {
            Setsize result = it->second.size();
            get_stream() << "encstrset_size: set #" << id <<
                         " contains " << result << " element(s)\n";

            return result;
        }

        return 0;
    }

    bool encstrset_insert(Idset id, Argvalue value, Argvalue key) {
        string result;
        auto it = code("insert", result, id, value, key);
        if (!in_map(it))
            return false;

        bool inserted = it->second.insert(result).second;
        if (!inserted)
            get_stream() << "\" was already present\n";
        else
            get_stream() << "\" inserted\n";

        return inserted;
    }

    bool encstrset_remove(Idset id, Argvalue value, Argvalue key) {
        string result;
        auto it = code("remove", result, id, value, key);
        if (!in_map(it))
            return false;

        bool erased = it->second.erase(result);
        if (!erased)
            get_stream() << "\" was not present\n";
        else
            get_stream() << "\" removed\n";

        return erased;
    }

    bool encstrset_test(Idset id, Argvalue value, Argvalue key) {
        string result;
        auto it = code("test", result, id, value, key);
        if (it == get_map().end())
            return false;

        bool exists = it->second.find(result) != it->second.end();

        get_stream() << "\" is ";
        if (!exists)
            get_stream() << "not ";
        get_stream() << "present\n";

        return exists;
    }

    void encstrset_clear(Idset id) {
        get_stream() << "encstrset_clear(" << id << ")\n";
        auto it = check_if_exists("clear", id);
        if (!in_map(it))
            return;

        it->second.clear();

        get_stream() << "encstrset_clear: set #" << id << " cleared\n";
    }

    void encstrset_copy(Idset src_id, Idset dst_id) {
        get_stream() << "encstrset_copy(" << src_id <<
                     ", " << dst_id << ")\n";

        auto srcIt = check_if_exists("copy", src_id);
        if (!in_map(srcIt))
            return;

        auto dstIt = check_if_exists("copy", dst_id);
        if (!in_map(dstIt))
            return;

        for (Constvalue it : srcIt->second) {
            if (dstIt->second.find(it) != dstIt->second.end()) {
                cypher_not_copied(it, dst_id);
            } else {
                dstIt->second.insert(it);
                cypher_copied(it, src_id, dst_id);
            }
        }
    }
} // namespace jnp1