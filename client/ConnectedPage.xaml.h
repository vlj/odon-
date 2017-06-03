//
// ConnectedPage.xaml.h
// Déclaration de la classe ConnectedPage
//

#pragma once

#include "ConnectedPage.g.h"

namespace client
{
	/// <summary>
	/// Une page vide peut être utilisée seule ou constituer une page de destination au sein d'un frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ConnectedPage sealed
	{
	public:
		ConnectedPage();

		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	private:
		concurrency::task<void> login();
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
