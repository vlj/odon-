//
// ConnectedPage.xaml.cpp
// Implémentation de la classe ConnectedPage
//

#include "pch.h"
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

// Pour plus d'informations sur le modèle d'élément Page vierge, consultez la page https://go.microsoft.com/fwlink/?LinkId=234238

ConnectedPage::ConnectedPage()
{
	InitializeComponent();
}

void client::ConnectedPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
}


void client::ConnectedPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	ring->IsActive = true;
	auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
	auto&& instance = Mastodon::InstanceConnexion(dynamic_cast<String^>(localSettings->Values->Lookup("client_id"))->Data(),
		dynamic_cast<String^>(localSettings->Values->Lookup("client_secret"))->Data());
		instance.log_in(username->Text->Data(), password->Text->Data())
			.then([this](const concurrency::task<utility::string_t>& tok_task)
			{
			try {
				String^ access_token = ref new String(tok_task.get().c_str());
				Windows::Storage::ApplicationData::Current->LocalSettings->Values->Insert("access_token", PropertyValue::CreateString(access_token));
				Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,
					ref new Windows::UI::Core::DispatchedHandler([this]()
					{
						this->Frame->Navigate(Interop::TypeName(Timeline::typeid), nullptr);
					})
				);
			}
			catch (...)
			{
				Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,
					ref new Windows::UI::Core::DispatchedHandler([this]()
					{
						ring->IsActive = false;
					})
				);
			}
		});
}
