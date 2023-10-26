#ifndef GAME_HPP
#define GAME_HPP

#ifdef WINDOWS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif
               
extern "C"
{
	void EXPORT load_name_printer(void (*namePrinter)(char const* const name));
	void EXPORT load_hello_printer(void (*helloPrinter)());
	void EXPORT update(unsigned char* ctx);
}

#endif // GAME_HPP
