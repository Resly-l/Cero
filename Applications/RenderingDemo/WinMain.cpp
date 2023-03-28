#include "Window/Window.h"

class DemoWindow : public Window
{
public:
	DemoWindow()
		:
		Window(1600, 900, L"Software Rendering")
	{}

	void Run()
	{
		while (ProcessMessage())
		{
			renderer->BeginFrame();
			renderer->EndFrame();
		}
	}
};

int __stdcall wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int)
{
	DemoWindow{}.Run();

	return 0;
}