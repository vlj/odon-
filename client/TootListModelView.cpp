#include "pch.h"
#include "TootListModelView.h"

using namespace Platform;

client::TootListModelView::TootListModelView()
{
	_timeline = ref new Collections::Vector<Toot^>();
	_notifications = ref new Collections::Vector<Notification^>();
	refresh();
}

void client::TootListModelView::refresh()
{
	auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
	auto&& instance = Mastodon::InstanceConnexion(dynamic_cast<String^>(localSettings->Values->Lookup("client_id"))->Data(),
		dynamic_cast<String^>(localSettings->Values->Lookup("client_secret"))->Data(),
		dynamic_cast<String^>(localSettings->Values->Lookup("access_token"))->Data());

	instance.timeline_home()
		.then([this](const std::vector<Mastodon::Status>& v)
	{
		auto refreshedtimeline = ref new Platform::Collections::Vector<Toot^>();
		for (const auto& toot : v)
		{
			refreshedtimeline->Append(ref new Toot(toot));
		}
		_timeline = refreshedtimeline;
		Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
			Windows::UI::Core::CoreDispatcherPriority::Low,
			ref new Windows::UI::Core::DispatchedHandler([this]()
		{
			PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("TimelineToots"));
		}));
	});

	instance.notifications()
		.then([this](const std::vector<Mastodon::Notifications>& v)
	{
		auto refreshednotifications = ref new Platform::Collections::Vector<Notification^>();
		for (const auto& n : v)
		{
			refreshednotifications->Append(ref new Notification(n));
		}
		_notifications = refreshednotifications;
		Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
			Windows::UI::Core::CoreDispatcherPriority::Low,
			ref new Windows::UI::Core::DispatchedHandler([this]() {
			PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Notifications"));
		}));
	});
}
