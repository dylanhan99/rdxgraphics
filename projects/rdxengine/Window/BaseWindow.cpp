#include "BaseWindow.h"
#include "ServiceLayer.h"

using namespace rdx;

BaseWindow::BaseWindow() {}
BaseWindow::~BaseWindow() {}

void BaseWindow::OnKeyPress(KeyCode const key)
{
	ServiceLayer::InputService()->OnKeyPress(key);
}

void BaseWindow::OnKeyRelease(KeyCode const key)
{
	ServiceLayer::InputService()->OnKeyRelease(key);
}
