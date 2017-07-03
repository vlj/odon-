#pragma once
#include "pch.h"

namespace client
{
	ref class DeferredTimeline sealed: public DeferredList<Toot>
	{
	public:
		virtual Windows::Foundation::IAsyncOperation<Windows::UI::Xaml::Data::LoadMoreItemsResult> ^ LoadMoreItemsAsync(unsigned int count) override
		{
			return concurrency::create_async([this]() { return callback(); });
		}

	internal:
		concurrency::task<Windows::UI::Xaml::Data::LoadMoreItemsResult> callback()
		{
			auto&& idRange = Mastodon::range{};
			if (nextMinTarget != nullptr) idRange = idRange.set_max(nextMinTarget->Value);
			const auto& timelineresult = co_await Util::getInstance().timeline_home(idRange);

			nextMinTarget = std::get<1>(timelineresult).value().since_id.has_value() ?
				ref new Platform::Box<int>(std::get<1>(timelineresult).value().since_id.value()) :
				nullptr;
			const auto& statuses = std::get<0>(timelineresult);

			int OldSize;
			int NewSize;
			co_await Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Low,
				ref new Windows::UI::Core::DispatchedHandler([&]()
			{
				OldSize = Size;
				for (const auto& toot : statuses)
				{
					Append(ref new Toot(toot));
				}
				NewSize = Size;
			}));

			Windows::UI::Xaml::Data::LoadMoreItemsResult res;
			res.Count = NewSize - OldSize;
			return res;
		}
	};

	ref class DeferredNotifications sealed : public DeferredList<Notification>
	{
	public:
		virtual Windows::Foundation::IAsyncOperation<Windows::UI::Xaml::Data::LoadMoreItemsResult> ^ LoadMoreItemsAsync(unsigned int count) override
		{
			return concurrency::create_async([this]() { return callback(); });
		}

	internal:
		concurrency::task<Windows::UI::Xaml::Data::LoadMoreItemsResult> callback()
		{
			auto&& idRange = Mastodon::range{};
			if (nextMinTarget != nullptr) idRange = idRange.set_max(nextMinTarget->Value);
			const auto& timelineresult = co_await Util::getInstance().notifications(idRange);

			nextMinTarget = std::get<1>(timelineresult).value().since_id.has_value() ?
				ref new Platform::Box<int>(std::get<1>(timelineresult).value().since_id.value()) :
				nullptr;
			const auto& statuses = std::get<0>(timelineresult);

			int OldSize;
			int NewSize;
			co_await Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Low,
				ref new Windows::UI::Core::DispatchedHandler([&]()
			{
				OldSize = Size;
				for (const auto& toot : statuses)
				{
					Append(ref new Notification(toot));
				}
				NewSize = Size;
			}));

			Windows::UI::Xaml::Data::LoadMoreItemsResult res;
			res.Count = NewSize - OldSize;
			return res;
		}
	};

	[Windows::UI::Xaml::Data::Bindable]
	public ref class TootListModelView sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
	{
		Windows::System::Threading::ThreadPoolTimer^ periodicTimer;
		int statuses_current_max_id;
		int notifications_current_max_id;
	public:
		TootListModelView();

		void SetTimer();
		void SuspendTimer();

		property Windows::UI::Xaml::Interop::IBindableObservableVector^ TimelineToots
		{
			Windows::UI::Xaml::Interop::IBindableObservableVector^ get()
			{
				return _timeline;
			}
		}

		property Windows::UI::Xaml::Interop::IBindableObservableVector^ Notifications
		{
			Windows::UI::Xaml::Interop::IBindableObservableVector^ get()
			{
				return _notifications;
			}
		}

		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		void refresh();

	private:
		DeferredTimeline^ _timeline;
		DeferredNotifications^ _notifications;

		concurrency::task<void> fetchStatuses();
		concurrency::task<void> fetchNotifications();
	};
}
