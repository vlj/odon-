#pragma once

namespace client
{
	public ref class Delegate sealed : Windows::UI::Xaml::Input::ICommand
	{
		int _id;
	public:
		Delegate(int id)
		{
			_id = id;
		}
		// Inherited via ICommand
		virtual event Windows::Foundation::EventHandler<Platform::Object ^> ^ CanExecuteChanged;
		virtual bool CanExecute(Platform::Object ^parameter) 
		{
			return true;
		}
		virtual void Execute(Platform::Object ^parameter)
		{
			auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
			auto&& instance = Mastodon::InstanceConnexion(dynamic_cast<Platform::String^>(localSettings->Values->Lookup("client_id"))->Data(),
				dynamic_cast<Platform::String^>(localSettings->Values->Lookup("client_secret"))->Data(),
				dynamic_cast<Platform::String^>(localSettings->Values->Lookup("access_token"))->Data());

			instance.account_follow(_id);
		}
	};

	[Windows::UI::Xaml::Data::Bindable]
	public ref class Account sealed
	{
		Platform::String^ _username;
		Platform::String^ _avatar;
		Delegate^ _onClick;
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

		property Windows::UI::Xaml::Input::ICommand^ OnClick
		{
			Windows::UI::Xaml::Input::ICommand^ get()
			{
				return _onClick;
			}
		}

		Account(Platform::String^ username, Platform::String^ avatar, size_t id)
		{
			_username = username;
			_avatar = avatar;
			_onClick = ref new Delegate(id);
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

