//
// MainPage.xaml.cpp
// Implémentation de la classe MainPage.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "ConnectedPage.xaml.h"

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

// Pour plus d'informations sur le modèle d'élément Page vierge, consultez la page https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();
}


void client::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	InstanceTokenRing->IsActive = true;
	Mastodon::InstanceConnexion::create_app(U("odon++client"))
		.then([this](const std::tuple<utility::string_t, utility::string_t>& id_secret)
		{
			auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
			localSettings->Insert("client_id", PropertyValue::CreateString(ref new String(std::get<0>(id_secret).c_str())));
			localSettings->Insert("client_secret", PropertyValue::CreateString(ref new String(std::get<1>(id_secret).c_str())));
			this->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,
				ref new Windows::UI::Core::DispatchedHandler([this, id_secret]() {
					this->Frame->Navigate(Interop::TypeName(ConnectedPage::typeid), nullptr);
				}));
		});

}
