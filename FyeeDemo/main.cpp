#include <Snowing.h>
#include <Task.h>

using namespace Snowing;

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
	Snowing::Engine::Get().Run([]{});

	return 0;
}