#include "Application.h"

int main()
{
	if (nadpher::Application::init(1024, 768))
	{
		return 1;
	}

	nadpher::Application::run();

	return 0;
}