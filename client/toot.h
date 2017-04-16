#pragma once

namespace client
{
	public ref class Toot sealed
	{
	internal:
		std::vector<float> test;
	private:
		Platform::String^ _username;
		Platform::String^ _content;
		Platform::String^ _avatar;
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

		property Platform::String^ Avatar
		{
			Platform::String^ get()
			{
				return _avatar;
			}
		}

		Toot(Platform::String^ username, Platform::String^ content, Platform::String^ avatar)
		{
			_username = username;
			_content = content;
			_avatar = avatar;
		}
	};
}

