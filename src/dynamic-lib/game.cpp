#include "game.hpp"
#include <stdio.h>

void(*s_namePrinterFunc)(char const* const name);
void(*s_helloPrinterFunc)();

void load_name_printer(void (*namePrinter)(char const* const name))
{
    s_namePrinterFunc = namePrinter;
}

void load_hello_printer(void(*helloPrinter)())
{
    s_helloPrinterFunc = helloPrinter;
}

void update(context* ctx)
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
