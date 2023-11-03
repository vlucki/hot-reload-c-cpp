#ifndef GAME_HPP
#define GAME_HPP

#ifdef WINDOWS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif
               
struct context_t
{
	unsigned long long totalCallCount;
};

extern "C"
{
	void EXPORT init_context(context_t** ctx);
	void EXPORT deinit_context(context_t** ctx);
	void EXPORT load_name_printer(void (*namePrinter)(char const* const name));
	void EXPORT load_greetings_printer(void (*greetingsPrinter)());
	void EXPORT update(context_t* ctx);
}

#endif // GAME_HPP
