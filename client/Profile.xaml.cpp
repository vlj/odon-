//
// Profile.xaml.cpp
// Implementation of the Profile class
//

#include "pch.h"
#include "Profile.xaml.h"
#include "FocusedToot.xaml.h"

using namespace client;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

Profile::Profile()
{
	InitializeComponent();
}

void client::Profile::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	auto profile = dynamic_cast<Account^>(e->Parameter);

	DataContext = profile;
	statuses = ref new DeferredProfile(profile->id);
	ProfileToot->DataContext = statuses;
	getStatuses(profile->id);
}

void client::Profile::ProfileToot_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto toot = dynamic_cast<Toot^>(e->ClickedItem);
	Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(FocusedToot::typeid), toot);
}


void client::Profile::tootviewer_OnImagePressed(client::TootViewer^ c, Account^ acc)
{
	Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(Profile::typeid), acc);
}

concurrency::task<void> client::Profile::getStatuses(const int & id)
{
	const auto& s = co_await Util::getInstance().statuses(id);
	co_await Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,
		ref new Windows::UI::Core::DispatchedHandler([&]()
		{
			for (const auto& toot : std::get<0>(s)) {
				statuses->Append(ref new Toot(toot));
			}
		}
	));
}
