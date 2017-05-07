//
// tootviewer.xaml.h
// Declaration of the tootviewer class
//

#pragma once

#include "tootviewer.g.h"
#include "toot.h"

namespace client
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class tootviewer sealed
	{
		Toot^ _toot;
	public:
		tootviewer();

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
	};

}
