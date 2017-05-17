//
// MainPage.xaml.cpp
// Implémentation de la classe MainPage.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "Login.xaml.h"
#include "ConnectedPage.xaml.h"
#include "Timeline.xaml.h"
#include "Search.xaml.h"
#include "TootListModelView.h"
#include "Settings.xaml.h"

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
	else if (localSettings->Values->Lookup("access_token") == nullptr)
		contentFrame->Navigate(Windows::UI::Xaml::Interop::TypeName(ConnectedPage::typeid), nullptr);
	else
		contentFrame->Navigate(Windows::UI::Xaml::Interop::TypeName(Timeline::typeid), nullptr);

	Windows::UI::Core::SystemNavigationManager::GetForCurrentView()->BackRequested +=
		ref new Windows::Foundation::EventHandler<Windows::UI::Core::BackRequestedEventArgs^>(this, &MainPage::onBack);
	Windows::UI::Core::SystemNavigationManager::GetForCurrentView()->AppViewBackButtonVisibility =
		Windows::UI::Core::AppViewBackButtonVisibility::Visible;
}

void client::MainPage::paneOpened_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Pane->IsPaneOpen = !this->Pane->IsPaneOpen;
}

void client::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	contentFrame->Navigate(Windows::UI::Xaml::Interop::TypeName(Search::typeid), SearchBox->Text);
}

void client::MainPage::AppBarButton_Click_2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	contentFrame->Navigate(Windows::UI::Xaml::Interop::TypeName(Settings::typeid), nullptr);
}

void client::MainPage::onBack(Platform::Object ^ sender, Windows::UI::Core::BackRequestedEventArgs ^ e)
{
	if (contentFrame->CanGoBack)
	{
		e->Handled = true;
		contentFrame->GoBack();
	}
}


void client::MainPage::AppBarButton_Click_3(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto modelView = static_cast<TootListModelView^>(Application::Current->Resources->Lookup("tootlist"));
	modelView->refresh();
}
