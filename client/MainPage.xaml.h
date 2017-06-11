//
// MainPage.xaml.h
//

#pragma once

#include "MainPage.g.h"

namespace client
{
	public ref class MainPage sealed
	{
	public:
		MainPage();

	private:
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void AppBarButton_Click_2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void onBack(Platform::Object^ sender, Windows::UI::Core::BackRequestedEventArgs^ e);
		void AppBarButton_Click_3(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void AppBarButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
