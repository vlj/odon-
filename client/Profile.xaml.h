//
// Profile.xaml.h
// Declaration of the Profile class
//

#pragma once

#include "Profile.g.h"
#include "tootviewer.xaml.h"
#include "TootListModelView.h"

namespace client
{
	ref class DeferredProfile sealed : public DeferredList<Toot>
	{
		int id;
	public:
		virtual Windows::Foundation::IAsyncOperation<Windows::UI::Xaml::Data::LoadMoreItemsResult> ^ LoadMoreItemsAsync(unsigned int count) override
		{
			return concurrency::create_async([this]() { return callback(); });
		}

		DeferredProfile(int id) : id(id)
		{}

	internal:
		concurrency::task<Windows::UI::Xaml::Data::LoadMoreItemsResult> callback()
		{
			const auto& nativeNextMinTarget = (nextMinTarget == nullptr) ? std::optional<int>() : std::make_optional<int>(nextMinTarget->Value);
			const auto& timelineresult = co_await Util::getInstance().statuses(id, Mastodon::range{ std::make_optional<int>(), nativeNextMinTarget });

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


	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Profile sealed
	{
		DeferredProfile^ statuses;
	public:
		Profile();

		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

	private:
		void ProfileToot_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void tootviewer_OnImagePressed(client::tootviewer^ c, Account^ acc);
		concurrency::task<void> getStatuses(const int& id);
	};
}
