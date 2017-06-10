//
// TootViewer.xaml.h
// Declaration of the TootViewer class
//

#pragma once

#include "tootviewer.g.h"
#include "toot.h"

namespace client
{
	ref class TootViewer;
	public delegate void ImagePressed(TootViewer^ c, Account^ account);


	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class TootViewer sealed
	{
		Toot^ _toot;
	public:
		TootViewer();

		event ImagePressed^ OnImagePressed;

		property Toot^ Status
		{
			Toot^ get()
			{
				return static_cast<Toot^>(DataContext);
			}

			void set(Toot^ v)
			{
				DataContext = v;
			}
		}
	private:
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};

}
