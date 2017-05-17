//
// FocusedToot.xaml.cpp
// Implementation of the FocusedToot class
//

#include "pch.h"
#include "tootviewer.xaml.h"
#include "FocusedToot.xaml.h"
#include "Profile.xaml.h"

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

FocusedToot::FocusedToot()
{
	InitializeComponent();
}

void FocusedToot::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	auto id = (int)(e->Parameter);
	auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
	auto&& instance = Mastodon::InstanceConnexion(dynamic_cast<String^>(localSettings->Values->Lookup("client_id"))->Data(),
		dynamic_cast<String^>(localSettings->Values->Lookup("client_secret"))->Data(),
		dynamic_cast<String^>(localSettings->Values->Lookup("access_token"))->Data());

	Mastodon::InstanceAnonymous{}.status(id)
		.then([this](const Mastodon::Status& status) {
		Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,
			ref new Windows::UI::Core::DispatchedHandler([this, status]()
		{
			tootpresenter->DataContext = ref new Toot(status);
			writer->AnswerTo = status.id;
			const auto& answerTag = U("@") + status._account.username;
			writer->Text = ref new Platform::String(answerTag.data());
		}));
	});

	Mastodon::InstanceAnonymous{}.status_context(id)
		.then([this](const Mastodon::Context& context) {
			auto ancestors = ref new Platform::Collections::Vector<Toot^>();
			for (const auto& a : context.ancestors)
			{
				ancestors->Append(ref new Toot{ a });
			}
			auto descendants = ref new Platform::Collections::Vector<Toot^>();
			for (const auto& d : context.descendants)
			{
				descendants->Append(ref new Toot{ d });
			}
			Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,
				ref new Windows::UI::Core::DispatchedHandler([this, ancestors, descendants]()
			{
				ancestorslist->DataContext = ancestors;
				descendantslist->DataContext = descendants;
			}));
	});
}


void client::FocusedToot::tootviewer_OnImagePressed(client::tootviewer^ c, default::int32 Id)
{
	Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(Profile::typeid), Id);
}


void client::FocusedToot::ancestorslist_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto toot = dynamic_cast<Toot^>(e->ClickedItem);
	Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(FocusedToot::typeid), toot->Id);
}
