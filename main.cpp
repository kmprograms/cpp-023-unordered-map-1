#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <functional>

// -----------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------
// --> Sequence Containers
// a. implementuja struktury, do ktorych mamy sekwencyjny dostep
// b. std::array, std::vector, std::dequeue, std::forward_list, std::list

// --> Ordered Associative Containers
// a. implementuja posortowane struktury danych, ktore mozna bardzo szybko przeszukiwac
// b. std::set, std::multiset, std::map, std::multimap

// --> Unordered Associative Containers
// a. implementuja nieposortowane struktury danych ktore mozna szybko przeszukiwac
// b. std::unordered_set, std::unordered_map, std::unordered_multiset, std::unordered_multimap

// -----------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------
// Jak dziala hash table oraz hash function

/*
    Zalozmy ze mamy tablice napisow jak ponizej:

    OLA         EWA         JAN         IZA         IGA
    0           1           2           3           4

    Zeby w takiej tablicy znalezc np IZA musisz kolejno przegladac elementy w tablicy
    i sprawdzac czy aktualnie przegladany element jest IZA czy nie. Dla duzych tablic
    takie podejscie nie jest optymalne.

    Z drugiej strony jezeli chcesz dowiedziec sie, jaki element znajduje sie pod
    konkretnym indeksem po prostu odwolujesz sie do tablicy, podajesz indeks i
    bardzo szybko mozesz uzyskac wartosc elementu pod tym indeksem.

    Problemem jest ustalenie, pod ktory indeks nalezy sie udac, zeby znalezc interesujacy
    nas element.
    Mozemy go rozwiazac poprzez wprowadzenie odpowiedniego mapowania, ktore konkretnej
    wartosci bedzie w stanie przyporzadkowac indeks.
    Stosujac taki pomysl zacznijmy jeszcze raz wypelniac nasza tablice.

    Wykorzystamy tutaj napisy z wczesniejszej tablicy i zastosujemy pewne kryterium
    przyporzadkowania, ktore wyznacza indeks jako wynik dzielenia calkowitego
    sumy kodow znakow w napisie przez ilosc elementow w tablicy

    JAN -> 74 + 65 + 78 -> 197 % 5 = 2

    IZA -> 73 + 90 + 65 -> 228 % 5 = 3

    IGA -> 73 + 71 + 65 -> 209 % 5 = 4

    OLA -> 79 + 76 + 65 -> 220 % 5 = 0

    EWA -> 69 + 87 + 65 -> 221 % 5 = 1


    OLA         EWA         JAN         IZA         IGA
    0           1           2           3           4

    Teraz o tym na ktorej pozycji znajdzie sie konkretna wartosc decyduja pewna funkcja:
    f(v) = sum_codes(v) % size
    Taka funkcje nazywamy HASH FUNCTION

    Majac taka funkcje szukanie elementow w tablicy jest teraz bardzo szybkie.
    Znajdzmy np EWA
    Wyliczamy wartosc funkcji f(EWA) ktora wynosi 1 i od razu wiemy ze mozemy
    znalezc ta wartosc odnoszac sie do indeksu 1

    Oczywiscie elementem takiej tablicy nie musi byc napis. Moze byc nim nawet caly
    obiekt, ktory zawiera rozne informacje. Mozesz tez umieszczac w takiej tablicy
    pary klucz - wartosc, gdzie klucz jest unikalny i pozwala identyfikowac pare
    oraz wykorzystywan jest do ustalenia pozycji pary w tablicy, wartosc przechowuje
    dodatkowe potrzebne informacje.


    Teraz wyjasniamy sobie czym sa kolizje.
    Do naszej tablicy:

    OLA         EWA         JAN         IZA         IGA
    0           1           2           3           4

    dodajemy teraz ALA. Wynik funkcji hashujacej w tym przypadku to:
    ALA -> 65 + 76 + 65 -> 206 % 5 = 1

    Jak rozwiazac kolizje? Jest kilka sposobow, ale my omowimy CHAINING.
    Teraz elementem tablicy bedzie wskaznik do listy, w ktorej umieszczczamy
    elementy posiadajace taka sama wartosc funkcji hash

    0   -> OLA

    1   -> EWA -> ALA

    2   -> JAN

    3   -> IZA

    4   -> IGA

    Teraz kiedy chcesz odnalezc ALA najpierw wyliczana jest wartosc funkcji
    hash i wyniesie ona 1. Trafiamy do listy elementow przyporzadkowanej
    temu indeksowy w tablicy a nastepnie liniowo przeszukujemy liste w celu
    odnalezenia elementu ALA.

    Jakie sa cechy dobrej funckji hash?
    a. minimalizacja kolizji
    b. dazenie do uzyskania wartosci ukladajacych sie w rozklad jednostajny
    c. latwa do wyliczenia, szybka
*/


class Person {
    std::string name;
    int age;
public:
    explicit Person(std::string name = "DEFAULT_NAME", const int age = 18) : name{std::move(name)}, age{age} {}

    [[nodiscard]] std::string get_name() const {
        return name;
    }

    [[nodiscard]] int get_age() const {
        return age;
    }

    bool operator==(const Person &person) const {
        return name == person.name && age == person.age;
    }

    friend std::ostream &operator<<(std::ostream &out, const Person &person) {
        return out << person.name << " IS " << person.age;
    }
};

std::size_t person_hash_value(const Person& person) {
    using std::size_t;
    using std::hash;
    using std::string;

    size_t res = 17;
    res = res * 31 + hash<string>()(person.get_name());
    return res * 31 + hash<int>()(person.get_age());
}

int main() {

    Person p1{"PER_A", 15};
    Person p2{"PER_A", 15};
    Person p3{"PER_B", 10};
    Person p4{"PER_B", 10};
    Person p5{"PER_C", 21};
    Person p6{"PER_C", 21};

    std::unordered_map<Person, std::string, std::function<size_t(const Person& person)>> people_with_hobbies(100, person_hash_value);

    people_with_hobbies.try_emplace(p1, "SPORT");
    people_with_hobbies.insert_or_assign(p2, "BOOKS");
    people_with_hobbies.insert(std::make_pair(p3, "MUSIC"));
    people_with_hobbies[p4] = "MUSIC";
    people_with_hobbies[p5] = "MUSIC";
    people_with_hobbies[p6] = "MUSIC";

    std::for_each(
            people_with_hobbies.begin(),
            people_with_hobbies.end(),
            [](const auto &pair) { std::cout << pair.first << " AND HAS HOBBY: " << pair.second << std::endl; }
    );

    std::cout << people_with_hobbies.bucket_count() << std::endl;

    return 0;
}


