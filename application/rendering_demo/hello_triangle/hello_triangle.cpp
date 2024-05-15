#include "hello_triangle.h"
#include "utility/byte_buffer.h"
#include "core/math/vector.h"

using namespace io::graphics;

HelloTriangle::HelloTriangle()
	: Application(GraphicsAPIType::Vulkan)
{
	Window::SetTitle("Hello Triangle");
	
}

void HelloTriangle::Update()
{
}