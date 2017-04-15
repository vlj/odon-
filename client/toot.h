#pragma once

namespace client
{
	public ref class Toot sealed
	{
		Platform::String^ str;
	public:
		property Platform::String^ Title
		{
			Platform::String^ get()
			{
				return this->str;
			}
		}

		Toot(Platform::String^ _str)
		{
			this->str = _str;
		}
	};
}

