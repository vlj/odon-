//
// MainPage.xaml.h
// Déclaration de la classe MainPage.
//

#pragma once

#include "MainPage.g.h"

namespace client
{
	/// <summary>
	/// Une page vide peut être utilisée seule ou constituer une page de destination au sein d'un frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	private:
		void paneOpened_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void AppBarButton_Click_2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void onBack(Platform::Object^ sender, Windows::UI::Core::BackRequestedEventArgs^ e);
		void AppBarButton_Click_3(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
