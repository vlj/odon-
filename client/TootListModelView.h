#pragma once
#include "pch.h"

namespace client
{
	[Windows::UI::Xaml::Data::BindableAttribute]
	public ref class TootListModelView sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
	{
		Windows::System::Threading::ThreadPoolTimer^ periodicTimer;
		int statuses_current_max_id;
		int notifications_current_max_id;
	public:
		TootListModelView();

		void SetTimer();
		void SuspendTimer();

		property Windows::Foundation::Collections::IObservableVector<Toot^>^ TimelineToots
		{
			Windows::Foundation::Collections::IObservableVector<Toot^>^ get()
			{
				return _timeline;
			}
		}

		property Windows::Foundation::Collections::IObservableVector<Notification^> ^ Notifications
		{
			Windows::Foundation::Collections::IObservableVector<Notification^>^ get()
			{
				return _notifications;
			}
		}

		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		void refresh();

	private:
		Platform::Collections::Vector<Toot^>^ _timeline;
		Platform::Collections::Vector<Notification^>^ _notifications;

		concurrency::task<void> fetchStatuses(const Mastodon::InstanceConnexion instance);
		concurrency::task<void> fetchNotifications(const Mastodon::InstanceConnexion instance);
	};
}
