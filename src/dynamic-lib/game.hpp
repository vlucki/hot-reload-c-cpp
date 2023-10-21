#ifndef GAME_HPP
#define GAME_HPP

struct context
{
	unsigned long long totalCallCount = 0;
};
extern "C"
{
	void load_name_printer(void (*namePrinter)(char const* const name));
	void load_hello_printer(void (*helloPrinter)());
	void update(context* ctx);
}

#endif // GAME_HPP
