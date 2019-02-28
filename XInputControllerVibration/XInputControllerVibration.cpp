#include <Snowing.h>
#include <XInputController.h>

using namespace Snowing;

/* 震动效果书写规则 */
/* 震动效果是一个使用ANSI编码的CSV文件，它有6列。 */
/* 第一列是间隔时间，这里编写了从上个操作到当前操作的时间间隔，单位为秒，允许小数。 */
/* 第二列是操作，可以填入以下内容: */
/*     Tween - 从某个震动强度缓慢的变换到另外一个震动强度 */
/*     Set   - 设置为某个震动强度 */
/*     Stop  - 停止震动 */
/*     Loop  - 返回到开始处循环执行，这时需要调用VibratePlayer的Stop来停止播放 */
/* 第三列是要被操作的震动控制器，可以填入以下内容： (Loop操作不填此项）*/
/*     L - 左边的震动控制器 */
/*     R - 右边的震动控制器 */
/*     All - 两边的震动控制器 */
/* 第四列是要被设置的震动强度，仅Set和Tween可填入，范围在0~1之间，允许小数。 */
/* 第五列是缓慢变化的时间长度，单位为秒，允许小数，仅Tween操作可填入。 */
/* 第六列是缓慢变化时使用的缓动曲线，可以使用的曲线类型参见Snowing::Scene::TweenFX命名空间，仅Tween可填入。 */

int main()
{
	// 创建引擎
	auto engine = PlatformImpls::WindowsImpl::MakeEngine(
		L"XInputControllerVibration",
		{ 400,300 },
		true);

	// 获得已经连接的XInput控制器列表
	auto controllers = PlatformImpls::WindowsXInput::GetConnectedControllers();

	// 如果没有XInput控制器则退出
	if (controllers.empty())
		return 0;

	// 获取第一个XInput控制器
	auto controller = std::move(controllers.front());

	// 加载震动效果
	Scene::VibrateEffect effect{ LoadAsset("VibratorEffect.csv") };

	// 创建震动播放器
	Scene::VibratePlayer player{ &controller,&effect };

	// 启动引擎主循环，并执行player场景对象
	Engine::Get().RunObject(player);

	return 0;
}
