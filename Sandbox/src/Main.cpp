#include <mcspch.hpp>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "Game.hpp"

class Foo
{
public:
	Foo() { }
	~Foo() { }

	template<typename T>
	inline void RandomFunc()
	{

	}

private:

};

int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Foo foo;
	int d = 4;
	foo.RandomFunc<decltype(d)>();
	
	auto app = FY_NEW MCS::Game();
	app->Run();
	delete app;

	return 0;
}
