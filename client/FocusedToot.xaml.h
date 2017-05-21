//
// FocusedToot.xaml.h
// Declaration of the FocusedToot class
//

#pragma once

#include "FocusedToot.g.h"
#include "TootWriter.xaml.h"

namespace client
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class FocusedToot sealed
	{
	public:
		FocusedToot();

		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e) override;
	private:
		void tootviewer_OnImagePressed(client::tootviewer^ c, Account^ acc);
		void ancestorslist_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};
}
