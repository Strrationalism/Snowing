#include <Snowing.h>
#include <Task.h>

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
	Snowing::Engine::Get().RunObject(
		// 创建Task对象，在3秒后执行lambda表达式的内容
		Snowing::Scene::Task
		{
			3.0f,
			[]{
				// 退出引擎
				Snowing::Engine::Get().Exit();
			}
		}
	);

	return 0;
}