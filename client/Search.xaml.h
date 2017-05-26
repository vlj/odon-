//
// Search.xaml.h
// Declaration of the Search class
//

#pragma once

#include "Search.g.h"

namespace client
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Search sealed
	{
	public:
		Search();
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	private:
		void displaySearch_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		concurrency::task<void> getSearchResults(const std::wstring& searchTerm);
	};
}
