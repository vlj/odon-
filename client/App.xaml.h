//
// App.xaml.h
// Déclaration de la classe App.
//

#pragma once

#include "App.g.h"
#include "odon++.h"
#include "toot.h"


namespace client
{
	/// <summary>
	/// Fournit un comportement spécifique à l'application afin de compléter la classe Application par défaut.
	/// </summary>
	ref class App sealed
	{
	protected:
		virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e) override;

	internal:
		App();
	public:
		void OnBackgroundActivated(Windows::ApplicationModel::Activation::BackgroundActivatedEventArgs^ args) override;
	private:
		void OnEnteredBackground(Platform::Object^ sender, Windows::ApplicationModel::EnteredBackgroundEventArgs^ e);
		void OnLeavingBackground(Platform::Object^ sender, Windows::ApplicationModel::LeavingBackgroundEventArgs^ e);
		void OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e);
	private:
		std::unique_ptr<Mastodon::InstanceConnexion> mInstanceConnexion;
		
	};
}
