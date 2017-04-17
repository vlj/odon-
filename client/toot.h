#pragma once

namespace client
{
	[Windows::UI::Xaml::Data::Bindable]
	public ref class Account sealed
	{
		Platform::String^ _username;
		Platform::String^ _avatar;

	public:

		property Platform::String^ Username
		{
			Platform::String^ get()
			{
				return  _username;
			}
		}

		property Platform::String^ Avatar
		{
			Platform::String^ get()
			{
				return _avatar;
			}
		}

		Account(Platform::String^ username, Platform::String^ avatar)
		{
			_username = username;
			_avatar = avatar;
		}

	};

	public ref class Toot sealed
	{
	internal:
		std::vector<float> test;
	private:
		Account^ _account;
		Platform::String^ _content;
	public:

		property Platform::String^ Content
		{
			Platform::String^ get()
			{
				return _content;
			}
		}

		property Account^ Author
		{
			Account^ get()
			{
				return _account;
			}
		}

		Toot(Account^ user, Platform::String^ content)
		{
			_account = user;
			_content = content;
		}
	};
}

