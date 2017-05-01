//
// Timeline.xaml.cpp
// Implémentation de la classe Timeline
//

#include "pch.h"
#include "Timeline.xaml.h"
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

// Pour plus d'informations sur le modèle d'élément Page vierge, consultez la page https://go.microsoft.com/fwlink/?LinkId=234238

Timeline::Timeline()
{
	InitializeComponent();

	auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
	auto&& instance = Mastodon::InstanceConnexion(dynamic_cast<String^>(localSettings->Values->Lookup("client_id"))->Data(),
		dynamic_cast<String^>(localSettings->Values->Lookup("client_secret"))->Data(),
		dynamic_cast<String^>(localSettings->Values->Lookup("access_token"))->Data());

	instance.timeline_home()
		.then([this](const std::vector<Mastodon::Status>& v)
		{
			auto tootscol = ref new Platform::Collections::Vector<Toot^>();
			for (const auto& toot : v)
			{
				tootscol->Append(ref new Toot(toot));
			}
			Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low, ref new Windows::UI::Core::DispatchedHandler([this, tootscol]()
			{
				this->timelinesection->DataContext = tootscol;
			}));
		});

	instance.notifications()
		.then([this](const std::vector<Mastodon::Notifications>& v)
		{
			auto list = ref new Platform::Collections::Vector<Notification^>();
			for (const auto& n : v)
			{
				list->Append(ref new Notification(n));
			}
			Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,
				ref new Windows::UI::Core::DispatchedHandler([this, list]() {
				mentionsection->DataContext = list;
			}));
		});
}

void client::Timeline::ListView_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto toot = dynamic_cast<Toot^>(e->ClickedItem);
	Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(FocusedToot::typeid), toot->Id);
}
