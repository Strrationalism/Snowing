#include <Snowing.h>
#include <Task.h>

int main()
{
	// ��������ʵ��
	auto engine = 
		Snowing::PlatformImpls::WindowsImpl::MakeEngine(
			L"HelloWorld",		// ���ڱ���
			{ 400,300 },		// ���ڴ�С
			true);				// �Ƿ񴰿ڻ���Ϊfalse��ȫ����

	// ��ӡLog
	Snowing::Log("HelloWorld");

	// ����һ������
	Snowing::Engine::Get().RunObject(
		// ����Task������3���ִ��lambda���ʽ������
		Snowing::Scene::Task
		{
			3.0f,
			[]{
				// �˳�����
				Snowing::Engine::Get().Exit();
			}
		}
	);

	return 0;
}