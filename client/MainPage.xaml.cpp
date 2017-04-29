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

void client::MainPage::AppBarButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
	auto&& instance = Mastodon::InstanceConnexion(dynamic_cast<String^>(localSettings->Values->Lookup("client_id"))->Data(),
		dynamic_cast<String^>(localSettings->Values->Lookup("client_secret"))->Data(),
		dynamic_cast<String^>(localSettings->Values->Lookup("access_token"))->Data());

	instance.status_post(NewToot->Text->Data(), false);
}


void client::MainPage::AppBarButton_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	FlyoutBase::ShowAttachedFlyout(contentFrame);
}


void client::MainPage::AppBarButton_Click_2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Windows::Storage::Pickers::FileOpenPicker^ openPicker = ref new Windows::Storage::Pickers::FileOpenPicker();
	openPicker->ViewMode = Windows::Storage::Pickers::PickerViewMode::Thumbnail;
	openPicker->SuggestedStartLocation = Windows::Storage::Pickers::PickerLocationId::PicturesLibrary;
	openPicker->FileTypeFilter->Append(".jpg");
	openPicker->FileTypeFilter->Append(".jpeg");
	openPicker->FileTypeFilter->Append(".png");

	Concurrency::create_task(openPicker->PickSingleFileAsync()).then([](Windows::Storage::StorageFile^ file)
		{
			if (file == nullptr)
				throw std::exception();
			return Windows::Storage::FileIO::ReadBufferAsync(file);
		}).then([](Concurrency::task<Windows::Storage::Streams::IBuffer^> bufferOp)
		{
			try
			{
				auto buffer = bufferOp.get();
				auto encoded_buffer = Windows::Security::Cryptography::CryptographicBuffer::EncodeToBase64String(buffer);
				auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;

				auto upload_pic = ref new Windows::Web::Http::HttpMultipartFormDataContent();
				upload_pic->Add(ref new Windows::Web::Http::HttpStringContent("data:image/png;base64," + encoded_buffer), "avatar");
				upload_pic->Add(ref new Windows::Web::Http::HttpStringContent(
					dynamic_cast<String^>(localSettings->Values->Lookup("access_token"))), "access_token");

				auto client = ref new Windows::Web::Http::HttpClient();
				auto request = ref new Windows::Web::Http::HttpRequestMessage(
					ref new Windows::Web::Http::HttpMethod("PATCH"),
					ref new Windows::Foundation::Uri("https://oc.todon.fr/api/v1/accounts/update_credentials"));
				request->Content = upload_pic;
				client->SendRequestAsync(request);
			}
			catch (...)
			{
			}
		});
}

void client::MainPage::onBack(Platform::Object ^ sender, Windows::UI::Core::BackRequestedEventArgs ^ e)
{
	if (contentFrame->CanGoBack)
	{
		e->Handled = true;
		contentFrame->GoBack();
	}
}
