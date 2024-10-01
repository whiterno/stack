#include <stdio.h>

#include "stack.h"

int main(){
    Stack st = {INIT(st)};
    stackCtor(&st, 2, 1, 2);
    int x = 0;
    stackPush(&st, 100);
    stackPush(&st, 200);
    stackPush(&st, 300);
    stackPush(&st, 400);
    stackPush(&st, 500);
    stackPush(&st, 100);
    stackPush(&st, 200);
    stackPush(&st, 300);
    stackPush(&st, 400);
    stackPush(&st, 500);
    stackPush(&st, 100);
    stackPop(&st, &x);
    stackPop(&st, &x);
    stackPop(&st, &x);
    stackPop(&st, &x);
    stackPop(&st, &x);
    stackPop(&st, &x);
    stackPop(&st, &x);
    stackPop(&st, &x);
    stackPop(&st, &x);
    stackPop(&st, &x);
    stackPop(&st, &x);

    stackDtor(&st);
}
