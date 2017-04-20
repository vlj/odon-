﻿//
// MainPage.xaml.cpp
// Implémentation de la classe MainPage.
//

#include "pch.h"
#include "MainPage.xaml.h"
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

// Pour plus d'informations sur le modèle d'élément Page vierge, consultez la page https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();
	auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
	if (localSettings->Values->Lookup("client_id") == nullptr)
		contentFrame->Navigate(Windows::UI::Xaml::Interop::TypeName(Login::typeid), nullptr);
	if (localSettings->Values->Lookup("access_token") == nullptr)
		contentFrame->Navigate(Windows::UI::Xaml::Interop::TypeName(ConnectedPage::typeid), nullptr);
	else
		contentFrame->Navigate(Windows::UI::Xaml::Interop::TypeName(Timeline::typeid), nullptr);
}

void client::MainPage::paneOpened_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Pane->IsPaneOpen = !this->Pane->IsPaneOpen;
}

void client::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	/*	auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
	auto&& instance = Mastodon::InstanceConnexion(dynamic_cast<String^>(localSettings->Values->Lookup("client_id"))->Data(),
	dynamic_cast<String^>(localSettings->Values->Lookup("client_secret"))->Data(),
	dynamic_cast<String^>(localSettings->Values->Lookup("access_token"))->Data());

	instance.account_search(SearchBox->Text->Data())
	.then([this](const std::vector<Mastodon::Account>& results)
	{
	auto list = ref new Platform::Collections::Vector<Account^>();
	for (const auto& acc : results)
	{
	list->Append(ref new Account(
	ref new String(acc.username.c_str()),
	ref new String(acc.avatar.c_str()),
	acc.id
	));
	}
	Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low, ref new Windows::UI::Core::DispatchedHandler([this, list]()
	{
	this->displaySearch->DataContext = list;
	FlyoutBase::ShowAttachedFlyout(this->hub);
	}));
	});*/
}

