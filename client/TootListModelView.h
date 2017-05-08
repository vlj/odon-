#pragma once
#include "pch.h"

namespace client
{
	[Windows::UI::Xaml::Data::BindableAttribute]
	public ref class TootListModelView sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
	{
	public:
		TootListModelView();

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
	};
}
