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

	public ref class Util sealed
	{
	internal:
		static Windows::UI::Xaml::Controls::RichTextBlock^ convertParagraph(const utility::string_t& str);
		static Platform::String^ emojify(const utility::string_t& str);
		static Mastodon::InstanceConnexion getInstance();
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
				auto&& instance = Mastodon::InstanceConnexion(U("https://oc.todon.fr/"), dynamic_cast<Platform::String^>(localSettings->Values->Lookup("access_token"))->Data());

				instance.account_follow(_account.id);
			});
		}

		Delegate^ _onClick;
	public:
		property Platform::String^ TootCount
		{
			Platform::String^ get()
			{
				return ref new Platform::String(std::to_wstring(_account.statuses_count).data());
			}
		}

		property Platform::String^ FollowerCount
		{
			Platform::String^ get()
			{
				return ref new Platform::String(std::to_wstring(_account.followers_count).data());
			}
		}

		property Platform::String^ FollowingCount
		{
			Platform::String^ get()
			{
				return ref new Platform::String(std::to_wstring(_account.following_count).data());
			}
		}

		property Platform::String^ Username
		{
			Platform::String^ get()
			{
				return ref new Platform::String(_account.acct.data());
			}
		}

		property Platform::String^ DisplayName
		{
			Platform::String^ get()
			{
				if (!_account.display_name.empty())
					return Util::emojify(_account.display_name.data());
				return Util::emojify(_account.username.data());
			}
		}

		property Platform::String^ Avatar
		{
			Platform::String^ get()
			{
				return ref new Platform::String(_account.avatar.data());
			}
		}

		property Platform::String^ Header
		{
			Platform::String^ get()
			{
				return ref new Platform::String(_account.header.data());
			}
		}

		property Windows::UI::Xaml::Input::ICommand^ OnClick
		{
			Windows::UI::Xaml::Input::ICommand^ get()
			{
				return _onClick;
			}
		}

		property int id
		{
			int get()
			{
				return _account.id;
			}
		}

		property Windows::UI::Xaml::Controls::RichTextBlock^ Note
		{
			Windows::UI::Xaml::Controls::RichTextBlock^ get()
			{
				return Util::convertParagraph(_account.note);
			}
		}
	};

	public ref class Attachment sealed
	{
	internal:
		Attachment(const Mastodon::Attachment& att)
		{
			previewUrl = ref new Platform::String(att.preview_url.data());
			url = ref new Platform::String(att.url.data());
		}

		Platform::String^ previewUrl;
		Platform::String^ url;
	public:
		property Platform::String^ PreviewUrl
		{
			Platform::String^ get()
			{
				return previewUrl;
			}
		}

		property Platform::String^ Url
		{
			Platform::String^ get()
			{
				return url;
			}
		}
	};

	[Windows::UI::Xaml::Data::Bindable]
	public ref class Toot sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
	{
	internal:
		Mastodon::Status status;

		Toot() {}
		
		Toot(const Mastodon::Status& _status) : status(_status)
		{
			_account = ref new Account(status._account);
			if (_status.spoiler_text)
				_spoiler_text = ref new Platform::String(_status.spoiler_text->c_str());

			const auto& DispatcherPropertyChanged = ref new Windows::UI::Core::DispatchedHandler([this]() {
				PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(""));
			});

			_favourite = ref new Delegate([=]() {
				auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
				auto&& instance = Mastodon::InstanceConnexion(U("https://oc.todon.fr/"), dynamic_cast<Platform::String^>(localSettings->Values->Lookup("access_token"))->Data());

				const auto& httprequest = [&]() {
					if (status.favourited)
						return instance.status_unfavourite(status.id);
					return instance.status_favourite(status.id);
				};

				httprequest().then([&](const Mastodon::Status& newStatus) {
					status.favourited = newStatus.favourited;
					status.favourites_count = newStatus.favourites_count;
					Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
						Windows::UI::Core::CoreDispatcherPriority::Low, DispatcherPropertyChanged);
				});
			});

			_reblog = ref new Delegate([=]() {
				auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
				auto&& instance = Mastodon::InstanceConnexion(U("https://oc.todon.fr/"), dynamic_cast<Platform::String^>(localSettings->Values->Lookup("access_token"))->Data());

				const auto& httprequest = [&]() {
					if (status.reblogged)
						return instance.status_unreblog(status.id);
					return instance.status_reblog(status.id);
				};

				httprequest().then([&](const Mastodon::Status& newStatus) {
					status.reblogged = newStatus.reblogged;
					status.reblogs_count = newStatus.reblogs_count;
					Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
						Windows::UI::Core::CoreDispatcherPriority::Low, DispatcherPropertyChanged);
				});
			});
		}
	private:
		Account^ _account;
		Platform::String^ _spoiler_text;
		bool _sensitive;
		Delegate^ _favourite;
		Delegate^ _reblog;
	public:
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		property bool Sensitive
		{
			bool get()
			{
				return _spoiler_text != nullptr;
			}
		}

		property bool IsReblog
		{
			bool get()
			{
				return status.reblog.get() != nullptr;
			}
		}

		property Platform::String^ RebloggedBy
		{
			Platform::String^ get()
			{
				if (!status._account.display_name.empty())
					return Util::emojify(status._account.display_name.data());
				return Util::emojify(status._account.username.data());
			}
		}

		property int Id
		{
			int get()
			{
				return status.id;
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
				return Util::convertParagraph(status.reblog.get() == nullptr ? status.content : status.reblog->content);
			}
		}

		property Account^ Author
		{
			Account^ get()
			{
				return status.reblog.get() == nullptr ? _account : ref new Account(status.reblog->_account);
			}
		}

		property bool HasMedia
		{
			bool get()
			{
				return !status.media_attachments.empty();
			}
		}

		property Windows::Foundation::Collections::IObservableVector<Attachment^>^ Medias
		{
			Windows::Foundation::Collections::IObservableVector<Attachment^>^ get()
			{
				auto images = ref new Platform::Collections::Vector<Attachment^>();
				for (const auto& media : status.media_attachments)
				{
					images->Append(ref new Attachment(media));
				}
				return images;
			}
		}

		property Platform::String^ FavoriteCount
		{
			Platform::String^ get()
			{
				return ref new Platform::String(std::to_wstring(status.favourites_count).data());
			}
		}

		property Platform::String^ RebloggedCount
		{
			Platform::String^ get()
			{
				return ref new Platform::String(std::to_wstring(status.reblogs_count).data());
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
				if (!notification.account.display_name.empty())
					return Util::emojify(notification.account.display_name.data());
				return Util::emojify(notification.account.username.data());
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

		property int Id
		{
			int get()
			{
				return notification.id;
			}
		}
	};
}

