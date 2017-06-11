//
// TootWriter.xaml.cpp
// Implementation of the TootWriter class
//

#include "pch.h"
#include "TootWriter.xaml.h"
#include "TootListModelView.h"

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

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

TootWriter::TootWriter()
{
	InitializeComponent();
}

concurrency::task<void> client::TootWriter::SendStatus()
{
	auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
	auto access_token = dynamic_cast<String^>(localSettings->Values->Lookup("access_token"));
	auto instance_url = dynamic_cast<String^>(localSettings->Values->Lookup("instance_url"));

	auto client = ref new Windows::Web::Http::HttpClient();
	auto uri = ref new Windows::Foundation::Uri(instance_url + "/api/v1/media");

	auto media_ids = std::vector<int>();
	for (const auto& media : medias)
	{
		auto bufferOp = media;
		auto httpBuffer = ref new Windows::Web::Http::HttpBufferContent(bufferOp.get());
		auto upload_pic = ref new Windows::Web::Http::HttpMultipartFormDataContent();
		upload_pic->Add(httpBuffer, "file", "somefile.jpg");
		upload_pic->Add(ref new Windows::Web::Http::HttpStringContent(access_token), "access_token");
		auto response = co_await client->PostAsync(uri, upload_pic);
		auto str = co_await response->Content->ReadAsStringAsync();
		auto jsonValue = Windows::Data::Json::JsonObject::Parse(str);
		const auto& n = static_cast<int>(jsonValue->GetNamedNumber(ref new Platform::String(U("id"))));
		media_ids.push_back(n);
	}
	medias.clear();
	const auto& answer_to = (_answerTo == 0) ? std::optional<int>{} : std::make_optional(_answerTo);
	const auto& spoiler_text = SpoilerText->Text->IsEmpty() ?
		std::make_optional<utility::string_t>() :
		SpoilerText->Text->Data();

	co_await Util::getInstance().status_post(NewToot->Text->Data(),
		Mastodon::visibility_level::public_level,
		media_ids,
		answer_to,
		spoiler_text, sensitive->IsOn);

	Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,
		ref new Windows::UI::Core::DispatchedHandler([this]() {
			SpoilerText->IsReadOnly = false;
			NewToot->IsReadOnly = false;
			NewToot->Text = ref new Platform::String();
			mediasToSend->Items->Clear();
			auto modelView = static_cast<TootListModelView^>(Application::Current->Resources->Lookup("tootlist"));
			//modelView->refresh();
	}));
}

void TootWriter::AppBarButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	NewToot->IsReadOnly = true;
	SpoilerText->IsReadOnly = true;
	SendStatus();
}


void client::TootWriter::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Windows::Storage::Pickers::FileOpenPicker^ openPicker = ref new Windows::Storage::Pickers::FileOpenPicker();
	openPicker->ViewMode = Windows::Storage::Pickers::PickerViewMode::Thumbnail;
	openPicker->SuggestedStartLocation = Windows::Storage::Pickers::PickerLocationId::PicturesLibrary;
	openPicker->FileTypeFilter->Append(".jpg");
	openPicker->FileTypeFilter->Append(".jpeg");
	openPicker->FileTypeFilter->Append(".png");

	const auto& get_file = Concurrency::create_task(openPicker->PickSingleFileAsync());

	auto&& to_append = get_file
		.then([](Windows::Storage::StorageFile^ file)
		{
			if (file == nullptr)
				throw std::exception();
			return Windows::Storage::FileIO::ReadBufferAsync(file);
		});
	medias.push_back(to_append);
	get_file.then([](Windows::Storage::StorageFile^ file)
	{
		return file->OpenAsync(Windows::Storage::FileAccessMode::Read);
	})
	.then([](Windows::Storage::Streams::IRandomAccessStream^ stream) {
		return Windows::Graphics::Imaging::BitmapDecoder::CreateAsync(stream);
	})
	.then([](Windows::Graphics::Imaging::BitmapDecoder^ decoder) {
		return decoder->GetSoftwareBitmapAsync();
	})
	.then([this](Windows::Graphics::Imaging::SoftwareBitmap^ bitmap) {
		if (bitmap->BitmapPixelFormat != Windows::Graphics::Imaging::BitmapPixelFormat::Bgra8 ||
			bitmap->BitmapAlphaMode == Windows::Graphics::Imaging::BitmapAlphaMode::Straight)
		{
			bitmap = Windows::Graphics::Imaging::SoftwareBitmap::Convert(bitmap,
				Windows::Graphics::Imaging::BitmapPixelFormat::Bgra8,
				Windows::Graphics::Imaging::BitmapAlphaMode::Premultiplied);
		}
		auto src = ref new Windows::UI::Xaml::Media::Imaging::SoftwareBitmapSource();
		src->SetBitmapAsync(bitmap);
		auto img = ref new Windows::UI::Xaml::Controls::Image();
		img->Width = 40;
		img->Height = 40;
		img->Source = src;
		Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,
			ref new Windows::UI::Core::DispatchedHandler([this, img]() {
			mediasToSend->Items->Append(img);
		}));

	});
}
