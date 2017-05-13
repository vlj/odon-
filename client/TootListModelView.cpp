#include "pch.h"
#include "TootListModelView.h"

using namespace Platform;

client::TootListModelView::TootListModelView()
{
	_timeline = ref new Collections::Vector<Toot^>();
	_notifications = ref new Collections::Vector<Notification^>();
	refresh();
	SetTimer();
}

void client::TootListModelView::SetTimer()
{
	if (periodicTimer != nullptr)
		return;
	Windows::Foundation::TimeSpan timespan;
	timespan.Duration = 10 * 10000000;
	periodicTimer = Windows::System::Threading::ThreadPoolTimer::CreatePeriodicTimer(
		ref new Windows::System::Threading::TimerElapsedHandler([this](auto)
	{
		refresh();
	}), timespan);
}

void client::TootListModelView::SuspendTimer()
{
	if (periodicTimer == nullptr)
		return;
	periodicTimer->Cancel();
	periodicTimer = nullptr;
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
		auto toastNotifier = Windows::UI::Notifications::ToastNotificationManager::CreateToastNotifier();
		auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
		auto ptrvalue = localSettings->Values->Lookup("last_notification");
		const auto& lastId = ptrvalue == nullptr ? 0 : (unsigned int)ptrvalue;
		auto newLastId = lastId;
		for (const auto& n : v)
		{
			refreshednotifications->Append(ref new Notification(n));

			if (n.id <= lastId)
				continue;
			newLastId = std::max<unsigned int>(newLastId, n.id);
			const auto& removeHtml = [](const auto& content)
			{
				return std::wstring{
					Windows::Data::Html::HtmlUtilities::ConvertToText(
					ref new Platform::String(content.data())
				)->Data() };
			};

			std::wstring&& toastVisual =
				LR"(<toast launch="app-defined-string"><visual>
				<binding template='ToastGeneric'>
				<text>Ducktodon</text>
				<text>)";
			if (n.type == Mastodon::NotificationType::favourite)
			{
				toastVisual += n.account.display_name + LR"( has favorited your toot :</text>)";
				toastVisual += LR"(<text>)" + removeHtml(n.status->content) + LR"(</text>)";
			}
			else if (n.type == Mastodon::NotificationType::reblog)
			{
				toastVisual += n.account.display_name + LR"( has reblogged your toot :</text>)";
				toastVisual += LR"(<text>)" + removeHtml(n.status->content) + LR"(</text>)";
			}
			else if (n.type == Mastodon::NotificationType::mention)
			{
				toastVisual += n.account.display_name + LR"( has mentioned you :</text>)";
				toastVisual += LR"(<text>)" + removeHtml(n.status->content) + LR"(</text>)";
			}
			else if (n.type == Mastodon::NotificationType::follow)
			{
				toastVisual += n.account.display_name + LR"( has followed you</text>)";
			}
			toastVisual += LR"(</binding>
				</visual></toast>)";
			auto xml = ref new Windows::Data::Xml::Dom::XmlDocument();
			xml->LoadXml(ref new Platform::String(toastVisual.data()));
			auto notif = ref new Windows::UI::Notifications::ToastNotification(xml);
			toastNotifier->Show(notif);
		}
		Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
			Windows::UI::Core::CoreDispatcherPriority::Low,
			ref new Windows::UI::Core::DispatchedHandler([this, lastId, newLastId, refreshednotifications]() {
			if (lastId != newLastId)
			{
				auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
				const auto& lastId = localSettings->Values->Insert("last_notification", newLastId);
			}
			_notifications = refreshednotifications;
			PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Notifications"));
		}));
	});
}
