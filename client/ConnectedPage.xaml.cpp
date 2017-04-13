﻿//
// ConnectedPage.xaml.cpp
// Implémentation de la classe ConnectedPage
//

#include "pch.h"
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
		.then([this](const utility::string_t& tok)
		{
			Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,
				ref new Windows::UI::Core::DispatchedHandler([this, tok]()
				{
					Token->Text = ref new String(tok.c_str());
				})
			);
		});
}
