//
// Login.xaml.h
// Declaration of the Login class
//

#pragma once

#include "Login.g.h"

namespace client
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Login sealed
	{
	public:
		Login();

	private:
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
