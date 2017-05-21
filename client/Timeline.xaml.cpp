//
// Timeline.xaml.cpp
// Implémentation de la classe Timeline
//

#include "pch.h"
#include "Timeline.xaml.h"
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

// Pour plus d'informations sur le modèle d'élément Page vierge, consultez la page https://go.microsoft.com/fwlink/?LinkId=234238

Timeline::Timeline()
{
	InitializeComponent();
}

void client::Timeline::ListView_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto toot = dynamic_cast<Toot^>(e->ClickedItem);
	Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(FocusedToot::typeid), toot->Id);
}


void client::Timeline::tootviewer_OnImagePressed(client::tootviewer^ c, Account^ acc)
{
	Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(Profile::typeid), acc);
}


void client::Timeline::mentionsection_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto notification = dynamic_cast<Notification^>(e->ClickedItem);
	Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(FocusedToot::typeid), notification->toot->Id);
}
