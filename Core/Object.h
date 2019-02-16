#pragma once

namespace Snowing::Scene
{
	class[[nodiscard]] Object
	{
	public:
		virtual inline ~Object() {}

		// 更新一帧，返回true为正常存活，false为已死亡。
		virtual inline bool Update() { return true; }
	};
}
