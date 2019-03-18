#include <Coordinate2D.h>
#include <PlatformImpls.h>
#include "TextWindowFontStyle.h"
#include "TextTyper.h"

namespace Yukimi
{
	class TextWindow final : 
		public virtual Snowing::Scene::Object
	{
	public:
		class TextAnimation;

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
			virtual void Update(Charater&) = 0;
			
			// �û�������Ҫ�������ɵ���Ч��ʱ
			virtual void FastFadeIn(Charater&) = 0;

			// ��Ҫ�󱻵���ʱ
			virtual void FadeOut(Charater&) = 0;

			// �����ڱ���ʾ/����ʱ
			virtual void OnHide(Charater&) = 0;
			virtual void OnShow(Charater&) = 0;
		};

		// ���������û����öԻ���ϵͳ��ʹ������Ҫ�̳��ڴ��࣬�Ӷ��󴫵ݸ�TextWindow
		class TextWindowUserAdapter
		{
		public:
			// ��ȡ�Ի���λ��
			virtual Snowing::Math::Vec4f GetTextWindowBox() const = 0;

			// ����ID��ȡ��ɫ��Tech��Ӧ��FontRenderer
			virtual FontRenderer* GetFontRendererByShaderName(Snowing::BKDRHash) = 0;

			// ����������л��Ʋ���
			virtual void FlushDrawCall() = 0;

			// ��ȡ����
			virtual const Snowing::Graphics::Font& GetFont() const = 0;
			
			// ����ID�������ֶ���Ч��
			virtual std::unique_ptr<TextAnimation> CreateAnimationByName(Charater& ch,Snowing::BKDRHash) = 0;

			// �����ڱ���ʾ/����ʱ
			virtual void OnHide() = 0;
			virtual void OnShow() = 0;
		};

	private:
		std::map<wchar_t, Snowing::Math::Vec2f> fix_;
		TextWindowUserAdapter* const userAdapter_;

		std::vector<Charater> text_;
		TextTyper typer_;
		void appendCharater(wchar_t ch, const TextWindowFontStyle& style, float wait);

	public:

		TextWindow(
			TextWindowUserAdapter* userAdapter
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
		void FastFadeIn();

		// ���ô����Ƿ���ʾ/����
		void SetVisible(bool);

		// ��ȡ״̬
		State GetState() const;

		bool Update() override;
	};
}
