//
// Search.xaml.cpp
// Implementation of the Search class
//

#include "pch.h"
#include "Search.xaml.h"
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

Search::Search()
{
	InitializeComponent();
}

void client::Search::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
	getSearchResults(dynamic_cast<String^>(e->Parameter)->Data());
}

void client::Search::displaySearch_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto acc = dynamic_cast<Account^>(e->ClickedItem);
	Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(Profile::typeid), acc);
}

concurrency::task<void> client::Search::getSearchResults(const std::wstring& SearchTerm)
{
	try
	{
		const auto& results = co_await Util::getInstance().account_search(SearchTerm);
		auto list = ref new Platform::Collections::Vector<Account^>();
		for (const auto& acc : results)
		{
			list->Append(ref new Account(acc));
		}
		displaySearch->DataContext = list;
	}
	catch (...)
	{
		ErrorText->Visibility = Windows::UI::Xaml::Visibility::Visible;
	}

}
