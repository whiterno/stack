#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "stack.h"

#ifndef NOT_ENCAPSULATION

struct Stack{
    DBG_CANARY(size_t left_canary);
    DBG(const char* name);
    DBG(const char* filename);
    DBG(const char* funcname);
    DBG(size_t line);

    size_t size;
    size_t capacity;
    StackElem* data;

    DBG_CANARY(size_t right_canary);
    DBG_HASH(unsigned int hash);
};

#endif

static int popResize(Stack* stk);
static int pushResize(Stack* stk);

DBG(static Errors stackErr(Stack* stk));
DBG(static int stackDump(Stack* stk, const char* filename, const char* funcname, const size_t line));
DBG(static const char* errorToString(Errors err));
DBG_CANARY(static void putDataCanaries(Stack* stk));
DBG_HASH(static unsigned int hashAlgorithm(char* key, size_t len));
DBG_HASH(static unsigned int hashStack(Stack* stk));
DBG_SECURE_PTR(static Stack* ptrXOR(Stack* stk));

DBG(static FILE* fp_bukkshit = fopen(DUMP_FILENAME, "w"));
DBG(static FILE* fp = (fp_bukkshit == NULL) ? stderr : fp_bukkshit);
DBG_CANARY(static size_t STRUCT_CANARY = time(NULL));
DBG_CANARY(static size_t counter_struct_canary = 0);
DBG_CANARY(static size_t DATA_CANARY = time(NULL));
DBG_CANARY(static size_t counter_data_canary = 0);
DBG_SECURE_PTR(static size_t KEY = 0);
DBG_SECURE_PTR(static size_t counter_key = 0);

Stack* stackCtor(INIT_ARGS, ...){
    va_list argptr;
    va_start(argptr, count);

    Stack* stk = (Stack*)calloc(1, sizeof(Stack));
    stk->size = 0;
    stk->capacity = 0;
    stk->data = NULL;

    PUT_INIT_ARGS(stk);

    INIT_STRUCT_CANARY;
    DBG_CANARY(stk->left_canary = STRUCT_CANARY);
    DBG_CANARY(stk->right_canary = STRUCT_CANARY);

    DBG_HASH(stk->hash = hashStack(stk));
    INIT_XOR_KEY;

    DBG_SECURE_PTR(stk = ptrXOR(stk));

    for (size_t elem = 0; elem < count; elem++){
        stackPush(stk, va_arg(argptr, StackElem));
    }

    va_end(argptr);

    DBG_SECURE_PTR(stk = ptrXOR(stk));
    STACK_ASSERT(stk);
    DBG_SECURE_PTR(stk = ptrXOR(stk));
    return stk;
}

int stackPush(Stack* stk, StackElem value){
    DBG_SECURE_PTR(stk = ptrXOR(stk));

    STACK_ASSERT(stk);

    pushResize(stk);
    stk->data[stk->size++] = value;
    DBG_HASH(stk->hash = hashStack(stk));

    STACK_ASSERT(stk);
    return NO_ERROR;
}

int stackPop(Stack* stk, StackElem* value){
    DBG_SECURE_PTR(stk = ptrXOR(stk));

    STACK_ASSERT(stk);

    if (popResize(stk) == STACK_UNDERFLOW){
        return STACK_UNDERFLOW;
    }

    stk->size--;
    *value = stk->data[stk->size];
    stk->data[stk->size] = POISON;
    DBG_HASH(stk->hash = hashStack(stk));

    STACK_ASSERT(stk);
    return NO_ERROR;
}

int stackDtor(Stack** stk){
    DBG_SECURE_PTR(*stk = ptrXOR(*stk));
    STACK_ASSERT(*stk);

    free((char*)(*stk)->data DBG_CANARY(- sizeof(size_t)));
    free(*stk);
    *stk = NULL;

    return NO_ERROR;
}

static int pushResize(Stack* stk){
    if (stk->size == stk->capacity){
        stk->capacity = (stk->capacity == 0) ? 1 : stk->capacity * PUSH_RESIZE_MULTIPLIER;
        size_t new_size = sizeof(StackElem) * stk->capacity DBG_CANARY(+ 2 * sizeof(size_t));

        stk->data = (StackElem*)realloc((stk->capacity == 1) ? NULL : (char*)stk->data DBG_CANARY(- sizeof(size_t)), new_size);
        DBG_CANARY(putDataCanaries(stk));

        for (size_t elem = stk->size; elem < stk->capacity; elem++){
            stk->data[elem] = POISON;
        }
    }

    DBG_HASH(stk->hash = hashStack(stk));
    STACK_ASSERT(stk);
    return NO_ERROR;
}

static int popResize(Stack* stk){
    if (stk->size == 0){
        STACK_ASSERT(stk);
        return STACK_UNDERFLOW;
    }

    if (stk->size == stk->capacity / POP_MAX_CAPACITY_DIVIDE_SIZE_VALUE){
        size_t new_size = sizeof(StackElem) * stk->capacity DBG_CANARY(+ 2 * sizeof(size_t));

        stk->capacity /= POP_RESIZE_DIVIDER;
        stk->data = (StackElem*)realloc((char*)stk->data DBG_CANARY(- sizeof(size_t)), new_size);
        DBG_CANARY(putDataCanaries(stk));
    }

    DBG_HASH(stk->hash = hashStack(stk));
    STACK_ASSERT(stk);
    return NO_ERROR;
}

#ifndef NDEBUG

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

    #ifndef NDEBUG_CANARY
    if (stk->left_canary != STRUCT_CANARY){
        return LEFT_CANARY_TOUCHED;
    }
    if (stk->right_canary != STRUCT_CANARY){
        return RIGHT_CANARY_TOUCHED;
    }
    if (stk->data != NULL && *((size_t*)stk->data - 1) != DATA_CANARY){
        return LEFT_DATA_CANARY_TOUCHED;
    }
    if (stk->data != NULL && *(size_t*)(stk->data + stk->capacity)!= DATA_CANARY){
        return RIGHT_DATA_CANARY_TOUCHED;
    }
    #endif

    #ifndef NDEBUG_HASH
    if (stk->hash != hashStack(stk)){
        return HASH_DOES_NOT_MATCH;
    }
    #endif

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
    DBG_CANARY(fprintf(fp, "\t%s.left_canary[%p] = %#lX\n", stk->name, &stk->left_canary, stk->left_canary));
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
        DBG_CANARY(fprintf(fp, "\tleft_data_canary[%p] = %#lX\n", (size_t*)stk->data - 1, *((size_t*)stk->data - 1)));
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
        DBG_CANARY(fprintf(fp, "\tright_data_canary[%p] = %#lX\n", stk->data + stk->capacity, *(size_t*)(stk->data + stk->capacity)));
    }
    DBG_CANARY(fprintf(fp, "\t%s.right_canary = %#lX\n", stk->name, stk->right_canary));
    DBG_HASH(fprintf(fp, "\t%s.hash = %#X\n", stk->name, stk->hash));
    DBG_HASH(if (err == HASH_DOES_NOT_MATCH){fprintf(fp, "(does not match with %#X)\n", hashStack(stk));});
    fprintf(fp, "}\n");

    return err;
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

#ifndef NDEBUG_CANARY
static void putDataCanaries(Stack* stk){
    INIT_DATA_CANARY

    *(size_t*)stk->data = DATA_CANARY;
    stk->data = (StackElem*)((size_t*)stk->data  + 1);
    *(size_t*)(stk->data + stk->capacity) = DATA_CANARY;
}
#endif

#ifndef NDEBUG_HASH
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
    const size_t length_struct = sizeof(stk->data) + sizeof(stk->size) + sizeof(stk->capacity) DBG_CANARY(+ sizeof(size_t));
    char* ptr_struct = (char*)(&stk->size);

    unsigned int hash1 = hashAlgorithm(ptr_struct, length_struct);

    unsigned int hash2 = 0;
    if (stk->data != NULL){
        size_t length_data = stk->capacity * sizeof(StackElem) DBG_CANARY(+ 2 * sizeof(size_t));
        char* ptr_data = (char*)(stk->data) DBG_CANARY(- sizeof(size_t));
        hash2 = hashAlgorithm(ptr_data, length_data);
    }

    return hash1 + hash2;
}
#endif

#ifndef NDEBUG_SECURE_PTR
static Stack* ptrXOR(Stack* stk){
    stk = (Stack*)((size_t) stk ^ KEY);

    return stk;
}
#endif

#endif
