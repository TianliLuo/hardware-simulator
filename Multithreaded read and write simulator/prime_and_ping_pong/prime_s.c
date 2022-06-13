#include <stdlib.h>
#include <stdio.h>

long is_prime(long p) {
    for (long i=2; i<p; i++)
        if (p % i == 0)
            return 0;
    return 1;
}

long nth_prime(int n) {
    long p;
    for (long i=2, c=0; c<n; i++)
        if (is_prime(i)) {
            p = i;
            c += 1;
        }
    return p;
}

int main(int argc, char** argv) {
    int n = atoi(argv[1]);
    long p0, p1;

    // TODO: run nth_prime calls in parallel using uthreads
    //    uthread API:
    //        uthread_init(2);
    //        uthread_t uthread_create(void* (*procedure)(void*), void* parameter);
    //        void uthread_join(uthread_t, void** result);
    //    steps:
    //        (1) change nth_prime to have right type signature
    //        (2) add uthreads

    p0 = nth_prime (n);
    p1 = nth_prime (n+1);

    printf("%ld x %ld = %ld\n", p0, p1, p0 * p1);
}