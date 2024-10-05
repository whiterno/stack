#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "stack.h"

static int popResize(Stack* stk);
static int pushResize(Stack* stk);
static Errors stackErr(Stack* stk);
static int stackDump(Stack* stk, const char* filename, const char* funcname, const size_t line);
static void putDataCanaries(Stack* stk);
static const char* errorToString(Errors err);
static unsigned int hashAlgorithm(char* key, size_t len);
static unsigned int hashStack(Stack* stk);

static const size_t CANARY = 0xDEADDEAD;
static FILE* fp = fopen(DUMP_FILENAME, "w");

int stackCtor(Stack* stk, size_t count, ...){
    va_list argptr;
    va_start(argptr, count);

    stk->size = 0;
    stk->capacity = 0;
    stk->data = NULL;
    ON_DEBUG(stk->left_canary = CANARY)
    ON_DEBUG(stk->right_canary = CANARY)
    ON_DEBUG(stk->hash = hashStack(stk))

    for (size_t elem = 0; elem < count; elem++){
        stackPush(stk, va_arg(argptr, StackElem));
    }

    va_end(argptr);

    STACK_ASSERT(stk)
    return 0;
}

int stackPush(Stack* stk, StackElem value){
    //stackDump(NULL, __FILE__, __func__, __LINE__);

    STACK_ASSERT(stk)

    pushResize(stk);
    stk->data[stk->size++] = value;

    ON_DEBUG(stk->hash = hashStack(stk))
    STACK_ASSERT(stk)
    return NO_ERROR;
}

int stackPop(Stack* stk, StackElem* value){
    STACK_ASSERT(stk)

    if (popResize(stk) == STACK_UNDERFLOW){
        return STACK_UNDERFLOW;
    }

    stk->size--;
    stk->data[stk->size] = POISON;
    *value = stk->data[stk->size];

    ON_DEBUG(stk->hash = hashStack(stk))
    STACK_ASSERT(stk)
    return NO_ERROR;
}

int stackDtor(Stack* stk){
    STACK_ASSERT(stk)

    free((char*)stk->data DBG(- sizeof(size_t)));
    stk->size = 0;
    stk->capacity = 0;

    return 0;
}

static Errors stackErr(Stack* stk){
    if (stk == NULL){
        return STACK_NOT_FOUND;
    }
    if (stk->size > stk->capacity){
        return STACK_OVERFLOW;
    }
    if (stk->data == NULL && stk->size != 0){
        return LOST_DATA;
    }
    for (size_t elem = stk->size; elem < stk->capacity; elem++){
        if (stk->data[elem] != POISON){
            return  POISON_TOUCHED;
        }
    }
    if (stk->left_canary != CANARY){
        return LEFT_CANARY_TOUCHED;
    }
    if (stk->right_canary != CANARY){
        return RIGHT_CANARY_TOUCHED;
    }
    if (stk->data != NULL && *((size_t*)stk->data - 1) != CANARY){
        return LEFT_DATA_CANARY_TOUCHED;
    }
    if (stk->data != NULL && *(size_t*)(stk->data + stk->capacity)!= CANARY){
        return RIGHT_DATA_CANARY_TOUCHED;
    }
    if (stk->hash != hashStack(stk)){
        return HASH_DOES_NOT_MATCH;
    }

    return NO_ERROR;
}

static int stackDump(Stack* stk, const char* filename, const char* funcname, const size_t line){
    Errors err = stackErr(stk);

    fprintf(fp, "-----------------------------------------------------------\n");
    fprintf(fp, "CODE ERROR: %s\n", errorToString(err));
    if (err == STACK_NOT_FOUND){
        fprintf(fp, "Stack not found.\n");
        return err;
    }
    fprintf(fp, "From %s stack [%p] name = %s at %s:%lu born at %s:%lu (%s)\n", funcname, stk, stk->name, filename, line, stk->filename, stk->line, stk->funcname);
    fprintf(fp, "{\n");
    fprintf(fp, "\t%s.left_canary[%p] = %#lX\n", stk->name, &stk->left_canary, stk->left_canary);
    fprintf(fp, "\t%s.size[%p]        = %lu\n", stk->name, &stk->size, stk->size);
    fprintf(fp, "\t%s.capacity[%p]    = %lu\n", stk->name, &stk->capacity, stk->capacity);
    if (err == STACK_OVERFLOW){
        fprintf(fp, "\tSize is bigger than capacity.\n");
        return err;
    }
    if (err == LOST_DATA){
        fprintf(fp, "\tSize is not 0, but data has nothing in it.\n");
        return err;
    }
    fprintf(fp, "\n");
    if (stk->data != NULL){
        fprintf(fp, "\tleft_data_canary[%p] = %#lX\n", (size_t*)stk->data - 1, *((size_t*)stk->data - 1));
    }
    fprintf(fp, "\t%s.data[%p] =\n", stk->name, stk->data);
    fprintf(fp, "\t{\n");
    for (size_t elem = 0; elem < stk->capacity; elem++){
        if (elem >= stk->size){
            fprintf(fp, "\t\t [%lu]: %d (POISON)\n", elem, stk->data[elem]);
            continue;
        }
        fprintf(fp, "\t\t*[%lu]: %d\n", elem, stk->data[elem]);
    }
    fprintf(fp, "\t}\n");
    if (stk->data != NULL){
        fprintf(fp, "\tright_data_canary[%p] = %#lX\n", stk->data + stk->capacity, *(size_t*)(stk->data + stk->capacity));
    }
    fprintf(fp, "\t%s.right_canary = %#lX\n", stk->name, stk->right_canary);
    fprintf(fp, "}\n");

    return err;
}

static void putDataCanaries(Stack* stk){
    *(size_t*)stk->data = CANARY;
    stk->data = (StackElem*)((size_t*)stk->data  + 1);
    *(size_t*)(stk->data + stk->capacity) = CANARY;
}

#define _DESCR(str) case str: return #str;
static const char* errorToString(Errors err){
    switch(err){
        _DESCR(NO_ERROR)
        _DESCR(STACK_NOT_FOUND)
        _DESCR(STACK_OVERFLOW)
        _DESCR(LOST_DATA)
        _DESCR(STACK_UNDERFLOW)
        _DESCR(POISON_TOUCHED)
        _DESCR(LEFT_CANARY_TOUCHED)
        _DESCR(RIGHT_CANARY_TOUCHED)
        _DESCR(LEFT_DATA_CANARY_TOUCHED)
        _DESCR(RIGHT_DATA_CANARY_TOUCHED)
        _DESCR(HASH_DOES_NOT_MATCH)
    }
}
#undef _DESCR

static int pushResize(Stack* stk){
    if (stk->capacity == 0){
        stk->data = (StackElem*)calloc(1, sizeof(StackElem) DBG(+ 2 * sizeof(size_t)));
        stk->capacity = 1;
        ON_DEBUG(putDataCanaries(stk))
        stk->data[0] = POISON;
    }

    if (stk->size == stk->capacity){
        stk->capacity *= 2;
        stk->data = (StackElem*)realloc((char*)stk->data - sizeof(size_t), sizeof(StackElem) * stk->capacity DBG(+ 2 * sizeof(size_t)));
        ON_DEBUG(putDataCanaries(stk))

        for (size_t elem = stk->size; elem < stk->capacity; elem++){
            stk->data[elem] = POISON;
        }
    }

    ON_DEBUG(stk->hash = hashStack(stk))
    STACK_ASSERT(stk)
    return NO_ERROR;
}

static int popResize(Stack* stk){
    if (stk->size == 0){
        STACK_ASSERT(stk)
        return STACK_UNDERFLOW;
    }

    if (stk->size == stk->capacity / 4){
        stk->capacity /= 2;
        stk->data = (StackElem*)realloc((char*)stk->data - sizeof(size_t),
                                        sizeof(StackElem) * stk->capacity DBG(+ 2 * sizeof(size_t)));
        ON_DEBUG(putDataCanaries(stk))
    }

    ON_DEBUG(stk->hash = hashStack(stk))
    STACK_ASSERT(stk)
    return NO_ERROR;
}

static unsigned int hashAlgorithm(char* key, size_t len){
    unsigned int m = 0x5bd1e995;
    unsigned int seed = 0x9164bf0a;
    int r = 24;

    int h = seed * len;

    unsigned char* data = (unsigned char*)key;
    unsigned int k = 0x93fab1c3;

    while (len >= 4)
    {
        k  = data[0];
        k |= data[1] << 8;
        k |= data[2] << 16;
        k |= data[3] << 24;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }
    switch (len)
    {
        case 3:{
            h ^= data[2] << 16;
        }
        case 2:{
            h ^= data[1] << 8;
        }
        case 1:{
            h ^= data[0];
            h *= m;
        }
    }

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

return h;
}

static unsigned int hashStack(Stack* stk){
    size_t length_struct = sizeof(stk->data) + sizeof(stk->size) + sizeof(stk->capacity) + DBG(+ sizeof(size_t));
    char* ptr_struct = (char*)(&stk->size);

    unsigned int hash1 = hashAlgorithm(ptr_struct, length_struct);

    unsigned int hash2 = 0;
    if (stk->data != NULL){
        size_t length_data = stk->capacity * sizeof(StackElem) DBG(+ 2 * sizeof(size_t));
        char* ptr_data = (char*)(stk->data) DBG(- sizeof(size_t));

        hash2 = hashAlgorithm(ptr_data, length_data);
    }

    return hash1 + hash2;
}