#pragma once
#include <Coordinate2D.h>
#include <PlatformImpls.h>
#include "TextWindowFontStyle.h"
#include "TextTyper.h"

namespace Yukimi
{
	class TextWindow final : 
		public Snowing::Scene::Object
	{
	public:
		class TextAnimation;

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
			FadingInText,
			Displaying,
			FadingOutText	// 注意在这个状态下，不能AppendText
		};

		// 此类用于创建文字的动画效果
		class TextAnimation
		{
		public:

			// 动画状态
			enum class AnimationState
			{
				Ready,			// 尚未显示
				FadingIn,		// 正在淡入
				Displaying,		// 目前在正常显示
				FadingOut,		// 正在淡出
				Killed			// 已死亡
			};

			// 每帧更新一次，当文字彻底消失后返回false
			virtual void Update(Charater&) = 0;

			// 获取动画状态
			virtual AnimationState GetState(const Charater&) const = 0;
			
			// 用户点击鼠标要求快速完成淡入效果时
			virtual void FastFadeIn(Charater&) = 0;

			// 当要求被淡出时
			virtual void FadeOut(Charater&) = 0;

			// 当窗口被显示/隐藏时
			virtual void SetVisible(Charater&,bool vis) = 0;
		};

		// 此类用于用户配置对话框系统，使用者需要继承于此类，子对象传递给TextWindow
		class TextWindowUserAdapter
		{
		public:
			// 获取对话框位置
			virtual Snowing::Math::Vec4f GetTextWindowBox() const = 0;

			// 根据ID获取着色器Tech对应的FontRenderer
			virtual FontRenderer* GetFontRendererByShaderName(Snowing::BKDRHash) = 0;

			// 立即完成所有绘制操作
			virtual void FlushDrawCall() = 0;

			// 获取字体
			virtual const Snowing::Graphics::Font& GetFont() const = 0;
			
			// 根据ID创建文字动画效果
			virtual std::unique_ptr<TextAnimation> CreateAnimationByName(Charater& ch,Snowing::BKDRHash) = 0;

			// 当窗口被显示/隐藏时
			virtual void SetVisible(bool) = 0;
		};

	private:
		std::map<wchar_t, Snowing::Math::Vec2f> fix_;
		TextWindowUserAdapter* const userAdapter_;

		bool fadingOut_ = false;
		bool visible_ = true;
		std::vector<Charater> text_;
		TextTyper typer_;
		float currentTimeLineEnd_;
		void appendCharater(wchar_t ch, const TextWindowFontStyle& style, float wait);

	public:

		TextWindow(
			TextWindowUserAdapter* userAdapter
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
		void FastFadeIn();

		// 设置窗口是否显示/隐藏
		void SetVisible(bool);

		// 获取状态
		State GetState() const;

		bool Update() override;
	};
}
