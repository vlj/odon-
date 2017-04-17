//
// Timeline.xaml.cpp
// Implémentation de la classe Timeline
//

#include "pch.h"
#include "Timeline.xaml.h"
#include <regex>

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

Timeline::Timeline()
{
	InitializeComponent();
	this->timelinesection->DataContext = this;

	auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
	auto&& instance = Mastodon::InstanceConnexion(dynamic_cast<String^>(localSettings->Values->Lookup("client_id"))->Data(),
		dynamic_cast<String^>(localSettings->Values->Lookup("client_secret"))->Data(),
		dynamic_cast<String^>(localSettings->Values->Lookup("access_token"))->Data());

	instance.timeline_home(0, 0)
		.then([this](const std::vector<Mastodon::Status>& v)
		{
			_tootscol = ref new Platform::Collections::Vector<Toot^>();
			for (const auto& toot : v)
			{
				const auto& content =
					std::regex_replace(
						std::regex_replace(toot.content, std::wregex(L"(<p>)"), L""),
						std::wregex(L"(</p>)"), L"");
				Account^ acc = ref new Account(
					ref new String(toot._account.username.c_str()),
					ref new String(toot._account.avatar.c_str())
					);
				Toot^ t = ref new Toot(acc, ref new String(content.c_str()));
				_tootscol->Append(t);
			}
		});
}


void client::Timeline::paneOpened_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->Pane->IsPaneOpen = !this->Pane->IsPaneOpen;
}


void client::Timeline::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
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
					ref new String(acc.avatar.c_str())
				));
			}
			Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low, ref new Windows::UI::Core::DispatchedHandler([this, list]()
			{
				this->displaySearch->DataContext = list;
				FlyoutBase::ShowAttachedFlyout(this->hub);
			}));
		});
}
