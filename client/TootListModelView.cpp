#include "pch.h"
#include "TootListModelView.h"
#include <limits>

using namespace Platform;

client::TootListModelView::TootListModelView() : statuses_current_max_id(0), notifications_current_max_id(0)
{
	_timeline = ref new DeferredList();
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

concurrency::task<void> client::TootListModelView::fetchStatuses(const Mastodon::InstanceConnexion instance)
{
	const auto& statuses = co_await instance.timeline_home(Mastodon::range{ _timeline->MaxId });
	Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
		Windows::UI::Core::CoreDispatcherPriority::Low,
		ref new Windows::UI::Core::DispatchedHandler([this, statuses]()
	{
		unsigned int position = 0;
		for (const auto& toot : std::get<0>(statuses))
		{
			_timeline->MaxId = std::max<int>(_timeline->MaxId, toot.id);
			_timeline->MinId = std::min<int>(_timeline->MinId, toot.id);
			_timeline->InsertAt(position++, ref new Toot(toot));
		}
		//PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("TimelineToots"));
	}));
}

concurrency::task<void> client::TootListModelView::fetchNotifications(const Mastodon::InstanceConnexion instance)
{
	auto notifications = co_await instance.notifications(Mastodon::range{ notifications_current_max_id });

	auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
	auto ptrvalue = localSettings->Values->Lookup("last_notification");
	const auto& lastId = ptrvalue == nullptr ? 0 : (int)ptrvalue;
	auto newLastId = lastId;

	auto toastNotifier = Windows::UI::Notifications::ToastNotificationManager::CreateToastNotifier();
	const auto notify = [&](const auto& n)
	{
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
			<text>Mastoduck</text>
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

	};
	for (const auto& n : notifications)
	{
		if (n.id <= lastId)
			continue;
		newLastId = std::max<int>(newLastId, n.id);
		notify(n);
	}
	Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
		Windows::UI::Core::CoreDispatcherPriority::Low,
		ref new Windows::UI::Core::DispatchedHandler([this, lastId, newLastId, notifications]() {
		if (lastId != newLastId)
		{
			auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
			const auto& lastId = localSettings->Values->Insert("last_notification", newLastId);
		}
		for (const auto& n : notifications)
		{
			_notifications->Append(ref new Notification(n));
			notifications_current_max_id = std::max<int>(notifications_current_max_id, n.id);
		}
		//PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Notifications"));
	}));
}

void client::TootListModelView::refresh()
{
	try {
		const auto& instance = Util::getInstance();
		fetchStatuses(instance);
		fetchNotifications(instance);
	}
	catch (...)
	{
		return;
	}
}

client::DeferredList::DeferredList()
{
	_internalVector = ref new Collections::Vector<Platform::Object^>();
	_internalVector->VectorChanged += ref new Windows::Foundation::Collections::VectorChangedEventHandler<Platform::Object^>(
		this, &DeferredList::OnVectorChanged);
	currentMaxId = -1;
}

void client::DeferredList::OnVectorChanged(Windows::Foundation::Collections::IObservableVector<Platform::Object^>^ sender, Windows::Foundation::Collections::IVectorChangedEventArgs ^ e)
{
	VectorChanged(this, e);
}

bool client::DeferredList::HasMoreItems::get()
{
	if (currentMinId.has_value()) return *currentMinId;
	return true;
}

Windows::Foundation::IAsyncOperation<Windows::UI::Xaml::Data::LoadMoreItemsResult> ^ client::DeferredList::LoadMoreItemsAsync(unsigned int count)
{
	const auto& f = Util::getInstance().timeline_home(Mastodon::range{ std::make_optional<int>(), currentMinId })
		.then([this](const std::tuple<std::vector<Mastodon::Status>, std::optional<Mastodon::range>>& timelineresult)
		{
			Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Low,
				ref new Windows::UI::Core::DispatchedHandler([=]()
			{
				const auto& statuses = std::get<0>(timelineresult);
				currentMinId = std::get<1>(timelineresult).value().max_id.value();
				for (const auto& toot : statuses)
				{
					const auto& already_here = [&]() {
						for (auto&& i = 0u; i < Size; ++i)
						{
							if (dynamic_cast<Toot^>(GetAt(i))->Id == toot.id) return true;
						}
						return false;
					}();
					if (already_here) continue;
					Append(ref new Toot(toot));
				}
			}));

			Windows::UI::Xaml::Data::LoadMoreItemsResult res;
			res.Count = 0;// statuses.size();
			return res;
		});
	return concurrency::create_async([f]() {return f.get(); });
}
