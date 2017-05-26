//
// Settings.xaml.cpp
// Implementation of the Settings class
//

#include "pch.h"
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

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

Settings::Settings()
{
	InitializeComponent();
}


void client::Settings::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;

	if (localSettings->Values->Lookup("client_id") == nullptr ||
		localSettings->Values->Lookup("client_secret") == nullptr ||
		localSettings->Values->Lookup("access_token") == nullptr)
		return;

	auto&& instance = Mastodon::InstanceConnexion(dynamic_cast<String^>(localSettings->Values->Lookup("access_token"))->Data());

	instance.update_account({ DisplayName->Text->Data() }, std::make_optional<utility::string_t>());
}


void client::Settings::Button_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}


void client::Settings::Button_Click_2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Windows::Storage::Pickers::FileOpenPicker^ openPicker = ref new Windows::Storage::Pickers::FileOpenPicker();
	openPicker->ViewMode = Windows::Storage::Pickers::PickerViewMode::Thumbnail;
	openPicker->SuggestedStartLocation = Windows::Storage::Pickers::PickerLocationId::PicturesLibrary;
	openPicker->FileTypeFilter->Append(".jpg");
	openPicker->FileTypeFilter->Append(".jpeg");
	openPicker->FileTypeFilter->Append(".png");

	Concurrency::create_task(openPicker->PickSingleFileAsync())
		.then([](Windows::Storage::StorageFile^ file)
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


void client::Settings::Button_Click_3(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}
