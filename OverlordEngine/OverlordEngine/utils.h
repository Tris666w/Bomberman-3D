#pragma once

namespace utils
{
	template <typename T>
	void SafeDelete(T*& pointer)
	{
		if (pointer)
		{
			delete pointer;
			pointer = nullptr;
		}
	}
}
