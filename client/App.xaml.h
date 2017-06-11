//
// App.xaml.h
//

#pragma once

#include "App.g.h"

namespace client
{
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
	};
}
