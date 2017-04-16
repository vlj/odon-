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
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Timeline sealed
	{
	private:
		Platform::Collections::Vector<Toot^>^ _tootscol;

	public:
		Timeline();

		property Windows::Foundation::Collections::IVector<Toot^>^ tootscol
		{
			Windows::Foundation::Collections::IVector<Toot^>^ get()
			{
				return _tootscol;
			}
		}
	private:
		void paneOpened_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
