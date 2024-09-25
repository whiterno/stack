#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "stack.h"

#define STACK_ASSERT(st)                                \
{                                                       \
    if (!stackErr(st)){                                 \
        stackDump(st, __FILE__, __func__, __LINE__);    \
    }                                                   \
}                                                       \

int stackCtor(Stack* stk){
    STACK_ASSERT(stk)

    stk->size = 0;
    stk->capacity = 0;
    stk->data = NULL;

    STACK_ASSERT(stk)
    return 0;
}

int stackPush(Stack* stk, StackElem value){
    STACK_ASSERT(stk)

    if (stk->capacity == 0){
        stk->data = (StackElem*)calloc(1, sizeof(StackElem));
        stk->capacity = 1;
    }
    if (stk->size == stk->capacity){
        stk->capacity *= 2;
        printf("%d\n", stk->data[0]);
        if (!realloc(stk->data, sizeof(StackElem) * stk->capacity)){
            printf("Shish\n");
            return 1;
        }
        printf("%d\n", stk->data[0]);
    }
    stk->data[stk->size++] = value;

    STACK_ASSERT(stk)
    return 0;
}

int stackPop(Stack* stk, StackElem* value){
    STACK_ASSERT(stk)

    if (stk->size == 0){
        return 1;
    }
    if (stk->size == stk->capacity / 4){
        stk->capacity /= 2;
        if (!realloc(stk->data, sizeof(StackElem) * stk->capacity)){
            return 1;
        }
    }

    stk->size--;
    *value = stk->data[stk->size];

    STACK_ASSERT(stk)
    return 0;
}

int stackDtor(Stack* stk){

    free(stk->data);
    stk->size = 0;
    stk->capacity = 0;

    return 0;
}

int stackErr(Stack* stk){
    if (stk == NULL){
        return STACK_NOT_FOUND;
    }
    if (stk->size > stk->capacity){
        return SIZE_BIGGER_CAPACITY;
    }
    if (stk->data == NULL && stk->size != 0){
        return LOST_DATA;
    }

    return NO_ERROR;
}

int stackDump(const Stack* stk, const char* filename, const char* funcname, const size_t line){
    printf("Stack [%p] name = %s at %s:%lu born at %s:%lu (%s)\n", stk, stk->name, filename, line, stk->filename, stk->line, stk->funcname);
    printf("{\n");
    printf("\t%s.size     = %lu\n", stk->name, stk->size);
    printf("\t%s.capacity = %lu\n", stk->name, stk->capacity);
    printf("\t%s.data =\n", stk->name);
    printf("\t{\n");
    for (size_t elem = 0; elem < stk->capacity; elem++){
        if (elem >= stk->size){
            printf("\t\t [%lu]: -1 (POISON)\n", elem);
            continue;
        }
        printf("\t\t*[%lu]: %d\n", elem, stk->data[elem]);
    }
    printf("\t}\n");
    printf("}\n");

    return 0;
}
