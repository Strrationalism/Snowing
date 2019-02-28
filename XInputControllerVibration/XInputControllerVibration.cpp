#include <Snowing.h>
#include <XInputController.h>

using namespace Snowing;

/* ��Ч����д���� */
/* ��Ч����һ��ʹ��ANSI�����CSV�ļ�������6�С� */
/* ��һ���Ǽ��ʱ�䣬�����д�˴��ϸ���������ǰ������ʱ��������λΪ�룬����С���� */
/* �ڶ����ǲ���������������������: */
/*     Tween - ��ĳ����ǿ�Ȼ����ı任������һ����ǿ�� */
/*     Set   - ����Ϊĳ����ǿ�� */
/*     Stop  - ֹͣ�� */
/*     Loop  - ���ص���ʼ��ѭ��ִ�У���ʱ��Ҫ����VibratePlayer��Stop��ֹͣ���� */
/* ��������Ҫ���������𶯿����������������������ݣ� (Loop����������*/
/*     L - ��ߵ��𶯿����� */
/*     R - �ұߵ��𶯿����� */
/*     All - ���ߵ��𶯿����� */
/* ��������Ҫ�����õ���ǿ�ȣ���Set��Tween�����룬��Χ��0~1֮�䣬����С���� */
/* �������ǻ����仯��ʱ�䳤�ȣ���λΪ�룬����С������Tween���������롣 */
/* �������ǻ����仯ʱʹ�õĻ������ߣ�����ʹ�õ��������Ͳμ�Snowing::Scene::TweenFX�����ռ䣬��Tween�����롣 */

int main()
{
	// ��������
	auto engine = PlatformImpls::WindowsImpl::MakeEngine(
		L"XInputControllerVibration",
		{ 400,300 },
		true);

	// ����Ѿ����ӵ�XInput�������б�
	auto controllers = PlatformImpls::WindowsXInput::GetConnectedControllers();

	// ���û��XInput���������˳�
	if (controllers.empty())
		return 0;

	// ��ȡ��һ��XInput������
	auto controller = std::move(controllers.front());

	// ������Ч��
	Scene::VibrateEffect effect{ LoadAsset("VibratorEffect.csv") };

	// �����𶯲�����
	Scene::VibratePlayer player{ &controller,&effect };

	// ����������ѭ������ִ��player��������
	Engine::Get().RunObject(player);

	return 0;
}
