#include <stdlib.h>
#include <stdio.h>
#include "uthread.h"

long is_prime(long p) {
    for (long i=2; i<p; i++)
        if (p % i == 0)
            return 0;
    return 1;
}

// long nth_prime(int n) {
//     long p;
//     for (long i=2, c=0; c<n; i++)
//         if (is_prime(i)) {
//             p = i;
//             c += 1;
//         }
//     return p;
// }

void* nth_prime(void* npv) {
    int* np = npv;
    long p;
    for (long i=2, c=0; c<*np; i++)
        if (is_prime(i)) {
            p = i;
            c += 1;
        }
    long* rp = malloc(sizeof(*rp));
    *rp = p;
    return rp;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: prime n\n");
        exit(EXIT_FAILURE);
    }
    int n = atoi(argv[1]);
    int n1 = n + 1;
    long *p0p, *p1p;

    uthread_init(2);

    // long p0, p1;
    // p0 = nth_prime (n);
    // p1 = nth_prime (n+1);
    uthread_t t0 = uthread_create(nth_prime, &n);
    uthread_t t1 = uthread_create(nth_prime, &n1);
    uthread_join(t0, (void**) &p0p);
    uthread_join(t1, (void**) &p1p);

    // printf("%ld x %ld = %ld\n", p0, p1, p0 * p1);
    printf("%ld x %ld = %ld\n", *p0p, *p1p, *p0p * *p1p);

    free(p0p);
    free(p1p);
    // TODO: run nth_prime calls in parallel using uthreads
    //    uthread API:
    //        uthread_init(2);
    //        uthread_t uthread_create(void* (*procedure)(void*), void* parameter);
    //        void uthread_join(uthread_t, void** result);
    //    steps:
    //        (1) change nth_prime to have right type signature
    //        (2) add uthreads
}


// QUESTION
// 
// A) Got it
// B) Got close, but ran out of time
// C) Sort of get it, but a little confused
// D) Kind of get it, but can't translate into code
// E) Don't really get it

