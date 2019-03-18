#include <Coordinate2D.h>
#include <PlatformImpls.h>
#include <TextWindowFontStyle.h>

namespace Yukimi
{
	class TextWindow final : public Snowing::Scene::Object
	{
	public:
		class TextAnimation;

	private:

		using FontRenderer = Snowing::Graphics::FontRenderer<
			Snowing::Graphics::Sprite,
			Snowing::Graphics::SpriteSheet,
			64>;

		// ��ʾ�ı����еĵ����ַ�
		struct Charater final
		{
			Snowing::Graphics::FontSprite Sprite;
			std::unique_ptr<TextAnimation> Animation;
			TextWindowFontStyle Style;

			float LifeTime;			// ������ʱ��
			float SinceFadeInTime;	// ��Ϊ��������Ϊ�����뱻������ʾ���С�����Ϊ��������Ϊ������������ʾ�Ѿ���ȥ��
			float WaitTime;			// �Ӵ�������������ʾʱ�������ƫ��ʱ��
		};

	public:

		// TextWindow��״̬
		enum class State
		{
			EmptyTextWindow,
			FadeInText,
			TextShow,
			FadeOutText,
		};

		// �������ڴ������ֵĶ���Ч��
		class TextAnimation
		{
		public:
			// ÿ֡����һ��
			virtual void Update() = 0;
			
			// �û�������Ҫ�������ɵ���Ч��ʱ
			virtual void FastFadeIn() = 0;

			// ��Ҫ�󱻵���ʱ
			virtual void FadeOut() = 0;

			// �����ڱ�����ʱ
			virtual void VisableChanged(bool visible) = 0;
		};

		// ���������û����öԻ���ϵͳ��ʹ������Ҫ�̳��ڴ��࣬�Ӷ��󴫵ݸ�TextWindow
		class TextWindowContent : public Snowing::Scene::Object
		{
		public:
			// ��ȡ�Ի���λ��
			virtual Snowing::Math::Coordinate2DRect GetTextWindowRectInWindowCoord() const = 0;

			// ����ID��ȡ��ɫ��Tech��Ӧ��FontRenderer
			virtual std::map<size_t, FontRenderer*>& BorrowFontRenderers() = 0;

			// ����ID�������ֶ���Ч��
			virtual std::unique_ptr<TextAnimation> CreateAnimationByID(Charater* ch,size_t id) = 0;

			// �����������ƻ�ȡ���������
			virtual TextWindowFontStyle* BorrowFontStyle(std::wstring_view) = 0;

			// ����һ֡����TextWindow����֮ǰ��ִ�У������ڴ���Ⱦ���������÷���false
			bool Update() override = 0;

			// ���ô����Ƿ���ʾ
			virtual void VisableChanged(bool visable) = 0;

			// ��ȡһ������������󽫻��������һ������������TextWindow���º���£������ڴ���ȾUI���ö��󲻵�����
			virtual Snowing::Scene::Object* GetAfterObject() = 0;
		};

	private:
		

	public:

		TextWindow(
			TextWindowFontStyle defaultStyle,
			TextWindowContent* content
		);

		// ǿ�����
		void Clear();

		// �������
		void FadeClear();

		// ׷������
		void AppendText(std::wstring_view text, std::wstring_view* styleNames, size_t styleNameCount,float wait);

		// ֹͣ����Ч��
		void FastFadeOut();

		// ��ȡ״̬
		State GetState() const;
	};
}
