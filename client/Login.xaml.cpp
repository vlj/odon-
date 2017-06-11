//
// Login.xaml.cpp
// Implementation of the Login class
//

#include "pch.h"
#include "Login.xaml.h"
#include "ConnectedPage.xaml.h"
#include "Timeline.xaml.h"

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

Login::Login()
{
	InitializeComponent();
}

void client::Login::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	InstanceTokenRing->IsActive = true;
	const auto& instance_url = std::wstring{ InstanceUrl->Text->Data() };
	Mastodon::TokenRetrieval::create_app(U("odon++client"), instance_url)
		.then([this, instance_url](const std::tuple<utility::string_t, utility::string_t>& id_secret)
	{
		auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
		localSettings->Insert("client_id", PropertyValue::CreateString(ref new String(std::get<0>(id_secret).c_str())));
		localSettings->Insert("client_secret", PropertyValue::CreateString(ref new String(std::get<1>(id_secret).c_str())));
		localSettings->Insert("instance_url", PropertyValue::CreateString(ref new String(instance_url.c_str())));
		this->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,
			ref new Windows::UI::Core::DispatchedHandler([this, id_secret]() {
			this->Frame->Navigate(Interop::TypeName(ConnectedPage::typeid), nullptr);
		}));
	});

}

