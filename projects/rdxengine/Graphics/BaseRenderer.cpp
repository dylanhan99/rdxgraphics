#include "BaseRenderer.h"

using namespace rdx;

void BaseRenderer::Draw() 
{ 
	DrawImpl(); 
}

void BaseRenderer::SetDepthTest(bool flag)
{
	
}