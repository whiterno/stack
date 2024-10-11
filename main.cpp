#include <stdio.h>
#include <time.h>

#include "./stack_hcpp/stack.h"

int main(){
    Stack* st = stackCtor(INIT(st) 0);

    for (int i = 0; i < 100; i++){
        stackPush(st, i);
    }

    // if you define any of written below, decomment the section
    // P.S. define visibly

    // #ifdef NDEBUG
    // printf("No encapsulation: capacity = %lu\n", st->capacity);
    // printf("No canaries\n");
    // printf("No hash\n");
    // printf("No asserts in stack.cpp\n");
    // st->data[5] = 123;
    // return 0;
    // #endif

    #ifndef NDEBUG_CANARY
    printf("Yes canaries\n");
    st->left_canary = 0;
    stackDtor(&st);
    printf("Look in dump.txt\n");
    #endif

    // #ifndef NDEBUG_HASH
    // printf("Yes hash\n");
    // st->data[1] = 110;
    // stackDtor(&st);
    // printf("Look in dump.txt\n");
    // #endif

}
