#include "BaseWindow.h"

namespace rdx
{
	BaseWindow::BaseWindow() {}
	BaseWindow::~BaseWindow() {}

	void BaseWindow::OnKeyPress(const KeyCode key)
	{
		std::cout << (int)key << " Down\n";
		//Input::OnKeyPress(key);
	}

	void BaseWindow::OnKeyRelease(const KeyCode key)
	{
		std::cout << (int)key << " Up\n";
		//Input::OnKeyRelease(key);
	}
}