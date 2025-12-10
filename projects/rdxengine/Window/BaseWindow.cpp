#include "BaseWindow.h"
#include "ServiceLayer.h"

using namespace rdx;

BaseWindow::~BaseWindow() {}

void BaseWindow::PollEvents()
{
	ServiceLayer::InputService()->SwapKeys();
	PollEventsImpl();
}

void BaseWindow::OnKeyPress(KeyCode const key)
{
	ServiceLayer::InputService()->OnKeyPress(key);
}

void BaseWindow::OnKeyRelease(KeyCode const key)
{
	ServiceLayer::InputService()->OnKeyRelease(key);
}

void BaseWindow::OnMousePress(MouseCode const button)
{
	ServiceLayer::InputService()->OnMousePress(button);
}

void BaseWindow::OnMouseRelease(MouseCode const button)
{
	ServiceLayer::InputService()->OnMouseRelease(button);
}

void BaseWindow::OnMouseMove(double const xpos, double const ypos)
{
	ServiceLayer::InputService()->OnMouseMove(glm::vec2{xpos, ypos});
}

void BaseWindow::OnScroll(double const yoffset)
{
	ServiceLayer::InputService()->OnScroll(static_cast<float>(yoffset));
}