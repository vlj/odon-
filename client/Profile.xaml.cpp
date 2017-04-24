//
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
	auto str = dynamic_cast<String^>(e->Parameter)->Data();
	wchar_t* strend = nullptr;
	const auto& profile_id = std::wcstol(str, &strend, 10);
	Mastodon::InstanceAnonymous{}.statuses(profile_id)
		.then([this](const std::vector<Mastodon::Status>& v) {
			auto list = ref new Platform::Collections::Vector<Toot^>();
			for (const auto& toot : v) {
				list->Append(ref new Toot(toot));
			}
			ProfileToot->DataContext = list;
		});
}


