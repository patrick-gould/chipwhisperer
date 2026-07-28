#ifndef LAZART_API_HPP
#define LAZART_API_HPP

#ifdef LAZART

#include <klee/klee.h>

/*#define LAZART_ORACLE(oracle)               \
    if(oracle) {                            \
        klee_assume(oracle);                \
    } else {                                \
        klee_assume(0 != 0 );               \
    }*/

#define LAZART_ORACLE(oracle) klee_assume(oracle)

#else
#define LAZART_ORACLE(oracle)
#endif // LAZART

#endif // LAZART_API_HPP
