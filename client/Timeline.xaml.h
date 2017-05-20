//
// Timeline.xaml.h
// Déclaration de la classe Timeline
//

#pragma once

#include "toot.h"
#include "Timeline.g.h"
#include "tootviewer.xaml.h"

namespace client
{
	/// <summary>
	/// Une page vide peut être utilisée seule ou constituer une page de destination au sein d'un frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Timeline sealed
	{
	public:
		Timeline();
	private:
		void ListView_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void tootviewer_OnImagePressed(client::tootviewer^ c, default::int32 Id);
		void mentionsection_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};
}
