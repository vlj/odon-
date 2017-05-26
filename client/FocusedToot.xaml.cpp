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
	auto toot = static_cast<Toot^>(e->Parameter);
	getConversation(toot->Id);
	tootpresenter->DataContext = toot;
	writer->AnswerTo = toot->Id;
	writer->Spoiler = toot->SpoilerText;
	writer->Text = U("@") + toot->Author->Username;
}


void client::FocusedToot::tootviewer_OnImagePressed(client::tootviewer^ c, Account^ acc)
{
	Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(Profile::typeid), acc);
}


void client::FocusedToot::ancestorslist_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto toot = dynamic_cast<Toot^>(e->ClickedItem);
	Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(FocusedToot::typeid), toot);
}

concurrency::task<void> client::FocusedToot::getConversation(const int& id)
{
	const auto& context = co_await Util::getInstance().status_context(id);
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
}
