#pragma once
#include "pch.h"

namespace client
{
	template<typename T>
	ref class DeferredList : Windows::UI::Xaml::Interop::IBindableObservableVector,
		Windows::UI::Xaml::Data::ISupportIncrementalLoading
	{
	protected private:
		Platform::Collections::Vector<Platform::Object^>^ _internalVector;
		int currentMaxId;
		int currentMinId;
		Platform::IBox<int>^ nextMinTarget;

		DeferredList()
		{
			_internalVector = ref new Platform::Collections::Vector<Platform::Object^>();
			_internalVector->VectorChanged += ref new Windows::Foundation::Collections::VectorChangedEventHandler<Platform::Object^>(
				this, &DeferredList::OnVectorChanged);
			currentMaxId = std::numeric_limits<int>::min();
			currentMinId = std::numeric_limits<int>::max();
		}
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
			auto toot = dynamic_cast<T^>(value);
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
			auto toot = dynamic_cast<T^>(value);
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

		virtual Windows::Foundation::IAsyncOperation<Windows::UI::Xaml::Data::LoadMoreItemsResult> ^ LoadMoreItemsAsync(unsigned int count)
		{
			throw ref new Platform::NotImplementedException();
		}

	};

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

		concurrency::task<void> fetchStatuses(const Mastodon::InstanceConnexion instance);
		concurrency::task<void> fetchNotifications(const Mastodon::InstanceConnexion instance);
	};
}
