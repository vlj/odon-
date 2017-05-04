#pragma once
#include <functional>

namespace client
{
	public ref class Delegate sealed : Windows::UI::Xaml::Input::ICommand
	{
	internal:
		std::function<void(void)> _callback;
		Delegate(std::function<void(void)>&& callback) : _callback(callback) {}
	public:
		Delegate() {}

		virtual event Windows::Foundation::EventHandler<Platform::Object ^> ^ CanExecuteChanged;
		virtual bool CanExecute(Platform::Object ^parameter)
		{
			return true;
		}
		virtual void Execute(Platform::Object ^parameter)
		{
			_callback();
		}
	};

	[Windows::UI::Xaml::Data::Bindable]
	public ref class Account sealed
	{
	internal:
		Mastodon::Account _account;

		Account(const Mastodon::Account& v) : _account(v)
		{
			_onClick = ref new Delegate([=]() {
				auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
				auto&& instance = Mastodon::InstanceConnexion(dynamic_cast<Platform::String^>(localSettings->Values->Lookup("client_id"))->Data(),
					dynamic_cast<Platform::String^>(localSettings->Values->Lookup("client_secret"))->Data(),
					dynamic_cast<Platform::String^>(localSettings->Values->Lookup("access_token"))->Data());

				instance.account_follow(_account.id);
			});
		}

		Delegate^ _onClick;
	public:
		property Platform::String^ Username
		{
			Platform::String^ get()
			{
				return ref new Platform::String(_account.username.data());
			}
		}

		property Platform::String^ Avatar
		{
			Platform::String^ get()
			{
				return ref new Platform::String(_account.avatar.data());
			}
		}

		property Windows::UI::Xaml::Input::ICommand^ OnClick
		{
			Windows::UI::Xaml::Input::ICommand^ get()
			{
				return _onClick;
			}
		}

		property size_t id
		{
			size_t get()
			{
				return _account.id;
			}
		}
	};

	[Windows::UI::Xaml::Data::Bindable]
	public ref class Toot sealed
	{
	internal:
		Mastodon::Status status;
		
		Toot(const Mastodon::Status& _status) : status(_status)
		{
			_account = ref new Account(status._account);
			if (_status.spoiler_text)
				_spoiler_text = ref new Platform::String(_status.spoiler_text->c_str());
			_sensitive = _status.sensitive;
			_id = _status.id;
			_favourite = ref new Delegate([=]() {
				auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
				auto&& instance = Mastodon::InstanceConnexion(dynamic_cast<Platform::String^>(localSettings->Values->Lookup("client_id"))->Data(),
					dynamic_cast<Platform::String^>(localSettings->Values->Lookup("client_secret"))->Data(),
					dynamic_cast<Platform::String^>(localSettings->Values->Lookup("access_token"))->Data());
				if (status.favourited)
					instance.status_unfavourite(status.id);
				else
					instance.status_favourite(status.id);
			});

			_reblog = ref new Delegate([=]() {
				auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
				auto&& instance = Mastodon::InstanceConnexion(dynamic_cast<Platform::String^>(localSettings->Values->Lookup("client_id"))->Data(),
					dynamic_cast<Platform::String^>(localSettings->Values->Lookup("client_secret"))->Data(),
					dynamic_cast<Platform::String^>(localSettings->Values->Lookup("access_token"))->Data());
				if (status.reblogged)
					instance.status_unreblog(status.id);
				else
					instance.status_reblog(status.id);
			});
		}
	private:
		Account^ _account;
		Platform::String^ _content;
		Platform::String^ _spoiler_text;
		bool _sensitive;
		size_t _id;
		Delegate^ _favourite;
		Delegate^ _reblog;

		static Windows::UI::Xaml::Controls::RichTextBlock^ convertParagraph(const utility::string_t& str);
	public:
		property bool Sensitive
		{
			bool get()
			{
				return _spoiler_text != nullptr;
			}
		}

		property size_t Id
		{
			size_t get()
			{
				return _id;
			}
		}

		property bool NotSensitive
		{
			bool get()
			{
				return _spoiler_text == nullptr;
			}
		}

		property bool IsFave
		{
			bool get()
			{
				return status.favourited;
			}
		}

		property bool IsNotFave
		{
			bool get()
			{
				return !status.favourited;
			}
		}

		property bool IsReblogged
		{
			bool get()
			{
				return status.reblogged;
			}
		}

		property bool IsNotReblogged
		{
			bool get()
			{
				return !status.reblogged;
			}
		}

		property Platform::String^ SpoilerText
		{
			Platform::String^ get()
			{
				return _spoiler_text;
			}
		}

		property Windows::UI::Xaml::Controls::RichTextBlock^ reb
		{
			Windows::UI::Xaml::Controls::RichTextBlock^ get()
			{
				return convertParagraph(status.content);
			}
		}

		property Account^ Author
		{
			Account^ get()
			{
				return _account;
			}
		}

		property Windows::UI::Xaml::Input::ICommand^ Favourite
		{
			Windows::UI::Xaml::Input::ICommand^ get()
			{
				return _favourite;
			}
		}

		property Windows::UI::Xaml::Input::ICommand^ Reblog
		{
			Windows::UI::Xaml::Input::ICommand^ get()
			{
				return _reblog;
			}
		}
	};

	[Windows::UI::Xaml::Data::Bindable]
	public ref class Notification sealed
	{
		Mastodon::Notifications notification;
	internal:
		Notification(const Mastodon::Notifications& v) : notification(v)
		{

		}
	public:
		property Platform::String^ User
		{
			Platform::String^ get()
			{
				return ref new Platform::String(notification.account.username.data());
			}
		}

		property Toot^ toot
		{
			Toot^ get()
			{
				if (notification.status.has_value())
					return ref new Toot(notification.status.value());
				else
					return nullptr;
			}
		}

		property bool IsMention
		{
			bool get()
			{
				return notification.type == Mastodon::NotificationType::mention;
			}
		}

		property bool IsReblog
		{
			bool get()
			{
				return notification.type == Mastodon::NotificationType::reblog;
			}
		}

		property bool IsFavourited
		{
			bool get()
			{
				return notification.type == Mastodon::NotificationType::favourite;
			}
		}

		property bool IsFollow
		{
			bool get()
			{
				return notification.type == Mastodon::NotificationType::follow;
			}
		}
	};
}

