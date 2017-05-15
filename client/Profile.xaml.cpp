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
	const auto& profile_id = (int)(e->Parameter);

	auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
	const auto& instance = Mastodon::InstanceConnexion{
		dynamic_cast<String^>(localSettings->Values->Lookup("client_id"))->Data(),
		dynamic_cast<String^>(localSettings->Values->Lookup("client_secret"))->Data(),
		dynamic_cast<String^>(localSettings->Values->Lookup("access_token"))->Data()
	};
	instance.statuses(profile_id)
		.then([this](const std::vector<Mastodon::Status>& v) {
			auto list = ref new Platform::Collections::Vector<Toot^>();
			for (const auto& toot : v) {
				list->Append(ref new Toot(toot));
			}
			Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,
				ref new Windows::UI::Core::DispatchedHandler([this, list]() {
				ProfileToot->DataContext = list;
			}));
		});

	instance.account(profile_id)
		.then([this](const Mastodon::Account& account) {

		Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,
			ref new Windows::UI::Core::DispatchedHandler([this, account]()
		{
			const auto& answerTag = U("@") + account.username;
			tootWriter->Text = ref new Platform::String(answerTag.data());
		}));
	});
}

void client::Profile::ProfileToot_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto toot = dynamic_cast<Toot^>(e->ClickedItem);
	Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(FocusedToot::typeid), toot->Id);
}


void client::Profile::tootviewer_OnImagePressed(client::tootviewer^ c, default::int32 Id)
{
	Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(Profile::typeid), Id);
}
