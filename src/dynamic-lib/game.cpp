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

struct context 
{
	unsigned long long totalCallCount = 0;
};

void update(unsigned char* ctx)
{
    context* actualCtx = (context*)ctx;
    // Demonstrate a way to keep some values across reloads
    static long long countSinceLastLoad = 0;
    ++countSinceLastLoad;
    ++(actualCtx->totalCallCount);
    printf("\rgame::update (total calls: %lld, calls since last load: %lld): ", actualCtx->totalCallCount, countSinceLastLoad);
   
    // Demonstrate calling functions provided by the code which loaded the lib

    s_helloPrinterFunc();
    printf("... ");
    s_namePrinterFunc("World!");
}
