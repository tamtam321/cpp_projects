#ifndef HYPERLOGLOG_HPP
#define HYPERLOGLOG_HPP

#include <functional>
#include <iostream>
#include <cmath>

//#include <bitset>
// std::bitset<64> b

double szumma(size_t cap, size_t m[])
{
    double back = 0;

    for(size_t i = 0; i < cap; i++)
    {
        int harmonia_sum_exponent_part = -(m[i] + 1);
        double harmonia_sum_part = pow(2, harmonia_sum_exponent_part);
        back = back + harmonia_sum_part;
    }

    return back;
}

/// Megjegyzés harmónia közép:
///  alfaM megvan adva azt szorzom M^2 ami a capacity és ezt négyzetre emelni
/// Szumma rész: inverse(sum(2^-(adott partícióhoz tartozó maxja+1)))

double Harmonikus_kozep(size_t cap, size_t m[])
{
    double alfaM = 0.72134;

    double back = 0;
    double sum;

    sum = szumma(cap,m);
    back = alfaM * std::pow(cap,2) * 1/sum;

    return back;
}

template <typename ForwardIterator>

size_t hyperloglog(ForwardIterator begin, ForwardIterator end)
{
    // definition of the value type:
    using value_type = typename std::iterator_traits<ForwardIterator>::value_type;

    // from this point we can use value_type as a type referring to the type we
    // get when dereferencing the iterators

    // we can use std::hash as a hash function:
    // hash_func is functor. From this point
    // we can think of it as a function.
    std::hash<value_type> hash_func {};

    // hash_value fogja eltárolni a hashelt értéket
    size_t hash_value;

    // capacity mondja meg a table méretét.
    size_t capacity = (1 << 14);

    // A table-ben tárolom majd hashelés után a nullák db számát.
    size_t table[(1 << 14)] = {0};

    for(ForwardIterator it = begin; it != end; it++)
    {
        hash_value = hash_func(*it);

        // key tárolja a 64 bit első 14 bitjét, ami a partíció
        int key = (hash_value >> 50u);

        // val_zeros_numb tárolja a maradék 50 bitet, ami az érték, de
        // __builtin_clzll() segítségével megszámolom hogy hány darab nullával kezdődik
        size_t val_zeros_numb = __builtin_clzl((hash_value << 14u));

        if(table[key] < val_zeros_numb)
        {
            table[key] = val_zeros_numb;
        }
    }

    // Megnézem table értékét
    /*for(size_t i : table)
    {
        std::cout << i << ", ";
    }*/

    //std::cout << sol << "\n";

    // Ez tárolja a max db nulla értéket
    // Ezzel térek vissza
    double sol;
    sol = Harmonikus_kozep(capacity, table);

    return sol;
}

#endif /* ifndef HYPERLOGLOG_HPP */
