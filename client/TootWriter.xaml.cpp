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

void TootWriter::AppBarButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
	auto access_token = dynamic_cast<String^>(localSettings->Values->Lookup("access_token"));

	NewToot->IsReadOnly = true;

	auto client = ref new Windows::Web::Http::HttpClient();
	auto uri = ref new Windows::Foundation::Uri("https://oc.todon.fr/api/v1/media");

	auto media_ids = std::make_shared<std::vector<int>>();
	const auto& callback =
		[access_token, client, uri, media_ids](const Concurrency::task<Windows::Storage::Streams::IBuffer^>& buffer_task)
	{
		return buffer_task.then([access_token, client, uri, media_ids](Windows::Storage::Streams::IBuffer^ bufferOp)
		{
			auto httpBuffer = ref new Windows::Web::Http::HttpBufferContent(bufferOp);
			auto upload_pic = ref new Windows::Web::Http::HttpMultipartFormDataContent();
			upload_pic->Add(httpBuffer, "file", "somefile.jpg");
			upload_pic->Add(ref new Windows::Web::Http::HttpStringContent(access_token), "access_token");
			return client->PostAsync(uri, upload_pic);
		}).then([](Windows::Web::Http::HttpResponseMessage^ response)
		{
			return response->Content->ReadAsStringAsync();
		})
		.then([=](Platform::String^ str)
		{
			auto jsonValue = Windows::Data::Json::JsonObject::Parse(str);
			auto n = jsonValue->GetNamedNumber(ref new Platform::String(U("id")));
			media_ids->push_back(n);
		});
	};

	auto modelView = static_cast<TootListModelView^>(Application::Current->Resources->Lookup("tootlist"));
	const auto& answer_to = (_answerTo == 0) ? std::optional<int>{} : std::make_optional(_answerTo);
	const auto& instance = Mastodon::InstanceConnexion(dynamic_cast<String^>(localSettings->Values->Lookup("client_id"))->Data(),
		dynamic_cast<String^>(localSettings->Values->Lookup("client_secret"))->Data(),
		dynamic_cast<String^>(localSettings->Values->Lookup("access_token"))->Data());

	const auto& continuation = [&]() {
		if (medias.empty())
		{
			return instance.status_post(NewToot->Text->Data(),
				Mastodon::visibility_level::public_level,
				std::vector<int>{},
				answer_to,
				std::optional<utility::string_t>{}, false);
		}

		auto&& chain = callback(medias[0]);
		for (int i = 1; i < medias.size(); i++)
		{
			chain = chain.then([=]() { return callback(medias[i]); });
		}
		return chain.then([=](const auto&) {
			return instance.status_post(NewToot->Text->Data(),
				Mastodon::visibility_level::public_level,
				*media_ids,
				answer_to,
				std::optional<utility::string_t>{}, false);
		});
	}();

	continuation.then([=](const Concurrency::task<web::json::value>& t) {
		try
		{
			t.get();
		}
		catch (...)
		{

		}
		Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,
			ref new Windows::UI::Core::DispatchedHandler([this, modelView]() {
			NewToot->IsReadOnly = false;
			NewToot->Text = ref new Platform::String();
			modelView->refresh();
		}));
	});
}


void client::TootWriter::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Windows::Storage::Pickers::FileOpenPicker^ openPicker = ref new Windows::Storage::Pickers::FileOpenPicker();
	openPicker->ViewMode = Windows::Storage::Pickers::PickerViewMode::Thumbnail;
	openPicker->SuggestedStartLocation = Windows::Storage::Pickers::PickerLocationId::PicturesLibrary;
	openPicker->FileTypeFilter->Append(".jpg");
	openPicker->FileTypeFilter->Append(".jpeg");
	openPicker->FileTypeFilter->Append(".png");

	// TODO: What happens if buffer is not ready when onclick ?

	auto&& to_append = Concurrency::create_task(openPicker->PickSingleFileAsync())
		.then([](Windows::Storage::StorageFile^ file)
		{
			if (file == nullptr)
				throw std::exception();
			return Windows::Storage::FileIO::ReadBufferAsync(file);
		});
	medias.push_back(to_append);
}
