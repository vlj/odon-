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
	auto&& instance = Mastodon::InstanceConnexion(dynamic_cast<String^>(localSettings->Values->Lookup("client_id"))->Data(),
		dynamic_cast<String^>(localSettings->Values->Lookup("client_secret"))->Data(),
		dynamic_cast<String^>(localSettings->Values->Lookup("access_token"))->Data());
	NewToot->IsReadOnly = true;

	auto modelView = static_cast<TootListModelView^>(Application::Current->Resources->Lookup("tootlist"));
	const auto& answer_to = (_answerTo == 0) ? std::optional<int>{} : std::make_optional(_answerTo);
	instance.status_post(NewToot->Text->Data(),
		Mastodon::visibility_level::public_level,
		answer_to,
		std::optional<utility::string_t>{}, false)
		.then([this, modelView](const web::json::value&) {
		Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,
			ref new Windows::UI::Core::DispatchedHandler([this]() {
			NewToot->IsReadOnly = false;
			NewToot->Text = ref new Platform::String();
		}));
		modelView->refresh();
	});
}
