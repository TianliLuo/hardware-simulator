#include <stdlib.h>

#include "uthread.h"
#include "uthread_mutex_cond.h"


void method1(){
    print("this is the method1");
}
void method2(){
    print("this is the method2");
}

int main (int argc, char** argv){
    uthread_init(2);
    uthread_t t1 = uthread_create(method1, NULL);
    uthread_t t2 = uthread_create(method2,NULL);



}