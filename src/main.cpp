#include "Application.h"

int main()
{
#ifdef _DEBUG
	spdlog::set_level(spdlog::level::debug);
#else
	spdlog::set_level(spdlog::level::info);
#endif

	if (!nadpher::Application::init(1024, 768))
	{
		return 1;
	}

	nadpher::Application::run();

	return 0;
}