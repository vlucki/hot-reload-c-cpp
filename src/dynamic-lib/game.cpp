#include "game.hpp"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void(*s_namePrinterFunc)(char const* const name);
static void(*s_helloPrinterFunc)();

void init_context(context_t** ctx)
{
    *ctx = (context_t*)malloc(sizeof(context_t));
	memset(*ctx, 0, sizeof(context_t));
}

void deinit_context(context_t** ctx)
{
    if (ctx && *ctx)
    {
        free(*ctx);
        *ctx = nullptr;
    }
}

void load_name_printer(void (*namePrinter)(char const* const name))
{
    s_namePrinterFunc = namePrinter;
}

void load_hello_printer(void(*helloPrinter)())
{
    s_helloPrinterFunc = helloPrinter;
}

void update(context_t* ctx)
{
    // Demonstrate a way to keep some values across reloads
    static long long countSinceLastLoad = 0;
    ++countSinceLastLoad;
    ++(ctx->totalCallCount);
    printf("\rgame::update (total calls: %lld, calls since last load: %lld): ", ctx->totalCallCount, countSinceLastLoad);
   
    // Demonstrate calling functions provided by the code which loaded the lib

    s_helloPrinterFunc();
    printf("... ");
    s_namePrinterFunc("World!");
}
