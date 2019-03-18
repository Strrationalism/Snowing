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

		// 表示文本框中的单个字符
		struct Charater final
		{
			Snowing::Graphics::FontSprite Sprite;
			std::unique_ptr<TextAnimation> Animation;
			FontRenderer* Renderer;

			float LifeTime;			// 总生命时间
			float SinceFadeInTime;	// 若为负数，则为“距离被正常显示还有”，若为正数，则为“距离正常显示已经过去”
			float WaitTime;			// 从创建到被完整显示时间以外的偏移时间
		};

	public:

		// TextWindow的状态
		enum class State
		{
			EmptyTextWindow,
			FadeInText,
			TextShow,
			FadeOutText,
		};

		// 此类用于创建文字的动画效果
		class TextAnimation
		{
		public:
			// 每帧更新一次
			virtual void Update() = 0;
			
			// 用户点击鼠标要求快速完成淡入效果时
			virtual void FastFadeIn() = 0;

			// 当要求被淡出时
			virtual void FadeOut() = 0;

			// 当窗口被显示/隐藏时
			virtual void OnHide() = 0;
			virtual void OnShow() = 0;
		};

		// 此类用于用户配置对话框系统，使用者需要继承于此类，子对象传递给TextWindow
		class TextWindowContent
		{
		public:
			// 获取对话框位置
			virtual Snowing::Math::Coordinate2DRect GetTextWindowRectInWindowCoord() const = 0;

			// 根据ID获取着色器Tech对应的FontRenderer
			virtual std::map<size_t, FontRenderer*>& BorrowFontRenderers() = 0;

			// 根据ID创建文字动画效果
			virtual std::unique_ptr<TextAnimation> CreateAnimationByID(Charater* ch,size_t id) = 0;

			// 当窗口被显示/隐藏时
			virtual void OnHide() = 0;
			virtual void OnShow() = 0;
		};

	private:
		

	public:

		TextWindow(
			TextWindowFontStyle defaultStyle,
			TextWindowContent* content
		);

		// 强制清除
		void Clear();

		// 渐出清除
		void FadeClear();

		// 追加文字
		void AppendText(
			std::wstring_view text,
			const TextWindowFontStyle& style,
			float wait);

		// 停止淡出效果
		void FastFadeOut();

		// 设置窗口是否显示/隐藏
		void SetVisible(bool);

		// 获取状态
		State GetState() const;
	};
}
