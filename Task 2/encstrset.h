// Projekt 2, grupa numer 5
// Autorzy:
// Jakub Panasiuk, jp418362
// Paweł Sarzyński, ps418407
#ifndef JNP1_ENCSTRSET_H
#define JNP1_ENCSTRSET_H

#ifdef __cplusplus
#include <cstddef>
extern "C" {
    namespace jnp1 {
#else // !__cplusplus
    #include <stdbool.h>
    #include <stdlib.h>
#endif // __cplusplus

// funkcja tworzy nowy set i zwraca jego identyfikator, ten
// identyfikator jest inkrementowany pod koniec funkcji (nawet jak usunę
// jakiś set to nigdy ponownie jego identyfikatora nie przypiszę do innego)
unsigned long encstrset_new();

// jeżeli zbiór o podanym numerze istnieje to usuwa go z mapy wraz
// z odpowiednim komunikatem
void encstrset_delete(unsigned long id);

// zwraca wielkość zbioru o podanym numerze, jeżeli na mapie nie istnieje
// taki to zwraca 0, wypisuje odpowiedni komunikat
size_t encstrset_size(unsigned long id);

// funkcja (jeżeli takiego elementu nie ma na secie) na set o
// numerze id wstawia string value zaszyfrowany kluczem key
bool encstrset_insert(unsigned long id, const char* value, const char* key);

// funkcja usuwa (jeżeli taki element znajduje się na secie) z setu o
// numerze id string value zaszyfrowany kluczem key
bool encstrset_remove(unsigned long id, const char* value, const char* key);

// funkcja sprawdza czy string value zaszyfrowany kluczem key znajduje
// sie na secie o numerze id czy też nie, wypisuje odpowiedni komunikat
bool encstrset_test(unsigned long id, const char* value, const char* key);

// funkcja usuwa wszystkie elementy z setu o numerze id
void encstrset_clear(unsigned long id);

// funkcja przekopiowuje elementy, z setu o numerze src_id do setu o
// numerze dest_id, które jeszcze na nim się nie znalazły, wypisując podczas
// działania odpowiednie komunikaty o sukcesie lub niepowodzeniu kopiowania
// danego elementu
void encstrset_copy(unsigned long src_id, unsigned long dst_id);

#ifdef __cplusplus
    }
}
#endif // __cplusplus

#endif // JNP1_ENCSTRSET_H
