﻿//
// Profile.xaml.cpp
// Implementation of the Profile class
//

#include "pch.h"
#include "Profile.xaml.h"

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

Profile::Profile()
{
	InitializeComponent();
}

void client::Profile::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	const auto& profile_id = (int)(e->Parameter);

	auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;
	Mastodon::InstanceConnexion{
		dynamic_cast<String^>(localSettings->Values->Lookup("client_id"))->Data(),
		dynamic_cast<String^>(localSettings->Values->Lookup("client_secret"))->Data(),
		dynamic_cast<String^>(localSettings->Values->Lookup("access_token"))->Data()
	}.statuses(profile_id)
		.then([this](const std::vector<Mastodon::Status>& v) {
			auto list = ref new Platform::Collections::Vector<Toot^>();
			for (const auto& toot : v) {
				list->Append(ref new Toot(toot));
			}
			Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,
				ref new Windows::UI::Core::DispatchedHandler([this, list]() {
				ProfileToot->DataContext = list;
			}));
		});
}


