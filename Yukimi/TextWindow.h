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
			FontRenderer* Renderer;

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

			// �����ڱ���ʾ/����ʱ
			virtual void OnHide() = 0;
			virtual void OnShow() = 0;
		};

		// ���������û����öԻ���ϵͳ��ʹ������Ҫ�̳��ڴ��࣬�Ӷ��󴫵ݸ�TextWindow
		class TextWindowContent
		{
		public:
			// ��ȡ�Ի���λ��
			virtual Snowing::Math::Coordinate2DRect GetTextWindowRectInWindowCoord() const = 0;

			// ����ID��ȡ��ɫ��Tech��Ӧ��FontRenderer
			virtual std::map<size_t, FontRenderer*>& BorrowFontRenderers() = 0;

			// ����ID�������ֶ���Ч��
			virtual std::unique_ptr<TextAnimation> CreateAnimationByID(Charater* ch,size_t id) = 0;

			// �����ڱ���ʾ/����ʱ
			virtual void OnHide() = 0;
			virtual void OnShow() = 0;
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
		void AppendText(
			std::wstring_view text,
			const TextWindowFontStyle& style,
			float wait);

		// ֹͣ����Ч��
		void FastFadeOut();

		// ���ô����Ƿ���ʾ/����
		void SetVisible(bool);

		// ��ȡ״̬
		State GetState() const;
	};
}
