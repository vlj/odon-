//
// App.xaml.h
// Déclaration de la classe App.
//

#pragma once

#include "odon++.h"
#include "toot.h"
#include "App.g.h"

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

	private:
		void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
		void OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e);

	private:
		std::unique_ptr<Mastodon::InstanceConnexion> mInstanceConnexion;
		
	};
}
