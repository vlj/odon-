#pragma once
#include "pch.h"

namespace client
{
	public ref class DeferredList sealed : Windows::UI::Xaml::Interop::IBindableObservableVector,
		Windows::UI::Xaml::Data::ISupportIncrementalLoading
	{
	private:
		Platform::Collections::Vector<Platform::Object^>^ _internalVector;
		int currentMaxId;
		int currentMinId;
		std::optional<int> nextMinTarget;
	public:
		property int MaxId
		{
			int get()
			{
				return currentMaxId;
			}
		}

		property int MinId
		{
			int get()
			{
				return currentMinId;
			}
		}

		virtual Windows::UI::Xaml::Interop::IBindableIterator ^ First()
		{
			return dynamic_cast<Windows::UI::Xaml::Interop::IBindableIterator^>(_internalVector->First());
		}

		virtual property unsigned int Size
		{
			unsigned int get()
			{
				return _internalVector->Size;
			}
		}


		virtual Platform::Object ^ GetAt(unsigned int index)
		{
			return _internalVector->GetAt(index);
		}

		virtual Windows::UI::Xaml::Interop::IBindableVectorView ^ GetView()
		{
			return dynamic_cast<Windows::UI::Xaml::Interop::IBindableVectorView^>(_internalVector->GetView());
		}

		virtual bool IndexOf(Platform::Object ^value, unsigned int *index)
		{
			return _internalVector->IndexOf(value, index);
		}

		virtual void SetAt(unsigned int index, Platform::Object ^value)
		{
			_internalVector->SetAt(index, value);
		}

		virtual void InsertAt(unsigned int index, Platform::Object ^value)
		{
			auto toot = dynamic_cast<client::Toot^>(value);
			if (toot->Id < currentMaxId && toot->Id > currentMinId) return;
			currentMinId = std::min<int>(currentMinId, toot->Id);
			currentMaxId = std::max<int>(currentMaxId, toot->Id);
			_internalVector->InsertAt(index, value);
		}

		virtual void RemoveAt(unsigned int index)
		{
			_internalVector->RemoveAt(index);
		}


		virtual void Append(Platform::Object ^value)
		{
			auto toot = dynamic_cast<client::Toot^>(value);
			if (toot->Id <= currentMaxId && toot->Id >= currentMinId) return;
			currentMinId = std::min<int>(currentMinId, toot->Id);
			currentMaxId = std::max<int>(currentMaxId, toot->Id);
			_internalVector->Append(value);
		}

		virtual void RemoveAtEnd()
		{
			_internalVector->RemoveAtEnd();
		}

		virtual void Clear()
		{
			_internalVector->Clear();
		}


		virtual event Windows::UI::Xaml::Interop::BindableVectorChangedEventHandler ^ VectorChanged;

		DeferredList()
		{
			_internalVector = ref new Platform::Collections::Vector<Platform::Object^>();
			_internalVector->VectorChanged += ref new Windows::Foundation::Collections::VectorChangedEventHandler<Platform::Object^>(
				this, &DeferredList::OnVectorChanged);
			currentMaxId = std::numeric_limits<int>::min();
			currentMinId = std::numeric_limits<int>::max();
		}

		void OnVectorChanged(Windows::Foundation::Collections::IObservableVector<Platform::Object^>^ sender, Windows::Foundation::Collections::IVectorChangedEventArgs ^ e)
		{
			VectorChanged(this, e);
		}

		virtual property bool HasMoreItems
		{
			bool get()
			{
				return currentMinId > 0;
			}
		}

		virtual Windows::Foundation::IAsyncOperation<Windows::UI::Xaml::Data::LoadMoreItemsResult> ^ LoadMoreItemsAsync(unsigned int count);
	internal:
		concurrency::task<Windows::UI::Xaml::Data::LoadMoreItemsResult> callback();
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
		DeferredList^ _timeline;
		Platform::Collections::Vector<Notification^>^ _notifications;

		concurrency::task<void> fetchStatuses(const Mastodon::InstanceConnexion instance);
		concurrency::task<void> fetchNotifications(const Mastodon::InstanceConnexion instance);
	};
}
