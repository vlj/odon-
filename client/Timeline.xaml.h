//
// Timeline.xaml.h
// Déclaration de la classe Timeline
//

#pragma once

#include "toot.h"
#include "Timeline.g.h"

namespace client
{
	/// <summary>
	/// Une page vide peut être utilisée seule ou constituer une page de destination au sein d'un frame.
	/// </summary>
	[Windows::UI::Xaml::Data::Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Timeline sealed
	{
	private:
		Platform::Collections::Vector<Toot^>^ _tootscol;

	public:
		Timeline();

		property Windows::Foundation::Collections::IObservableVector<Toot^>^ tootscol
		{
			Windows::Foundation::Collections::IObservableVector<Toot^>^ get()
			{
				return _tootscol;
			}
		}
	private:
		void ListView_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};
}
