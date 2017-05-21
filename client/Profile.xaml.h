//
// Profile.xaml.h
// Declaration of the Profile class
//

#pragma once

#include "Profile.g.h"
#include "tootviewer.xaml.h"

namespace client
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Profile sealed
	{
	public:
		Profile();

		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

	private:
		void ProfileToot_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void tootviewer_OnImagePressed(client::tootviewer^ c, Account^ acc);
	};
}
