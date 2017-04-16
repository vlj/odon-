#pragma once

namespace client
{
	public ref class Toot sealed
	{
		Platform::String^ _username;
		Platform::String^ _content;
	public:
		property Platform::String^ Username
		{
			Platform::String^ get()
			{
				return  _username;
			}
		}

		property Platform::String^ Content
		{
			Platform::String^ get()
			{
				return _content;
			}
		}

		Toot(Platform::String^ username, Platform::String^ content)
		{
			_username = username;
			_content = content;
		}
	};
}

