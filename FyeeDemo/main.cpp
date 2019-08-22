#include <Snowing.h>
#include <Task.h>

using namespace Snowing;

int main()
{
	// 创建引擎实例
	auto engine =
		Snowing::PlatformImpls::WindowsImpl::MakeEngine(
			L"HelloWorld",		// 窗口标题
			{ 400,300 },		// 窗口大小
			true);				// 是否窗口化（为false则全屏）

	// 打印Log
	Snowing::Log("HelloWorld");

	// 运行一个对象
	Snowing::Engine::Get().Run([]{});

	return 0;
}