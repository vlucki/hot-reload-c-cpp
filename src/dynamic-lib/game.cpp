#include "game.hpp"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void (*s_namePrinterFunc)(char const* const name);
static void (*s_greetingsPrinterFunc)();

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

void load_greetings_printer(void (*greetingsPrinter)())
{
	s_greetingsPrinterFunc = greetingsPrinter;
}

void update(context_t* ctx)
{
	// Demonstrate a way to keep some values across reloads
	static long long countSinceLastLoad = 0;
	++countSinceLastLoad;
	++(ctx->totalCallCount);
	printf("\rgame::update calls: total = %lld, since last load = %lld): ", ctx->totalCallCount, countSinceLastLoad);

	// Demonstrate calling functions provided by the code which loaded the lib

	s_greetingsPrinterFunc();
	printf("... ");
	s_namePrinterFunc("World!");
}
