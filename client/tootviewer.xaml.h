//
// tootviewer.xaml.h
// Declaration of the tootviewer class
//

#pragma once

#include "tootviewer.g.h"
#include "toot.h"

namespace client
{
	ref class tootviewer;
	public delegate void ImagePressed(tootviewer^ c, int Id);


	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class tootviewer sealed
	{
		Toot^ _toot;
	public:
		tootviewer();

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
