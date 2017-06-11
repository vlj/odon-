//
// App.xaml.cpp
//

#include "pch.h"
#include "App.xaml.h"
#include "MainPage.xaml.h"
#include "TootListModelView.h"

using namespace client;

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

App::App()
{
    InitializeComponent();
    EnteredBackground += ref new EnteredBackgroundEventHandler(this, &App::OnEnteredBackground);
	LeavingBackground += ref new LeavingBackgroundEventHandler(this, &App::OnLeavingBackground);
}

void App::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e)
{
    auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);

    if (rootFrame == nullptr)
    {
        rootFrame = ref new Frame();

        rootFrame->NavigationFailed += ref new Windows::UI::Xaml::Navigation::NavigationFailedEventHandler(this, &App::OnNavigationFailed);

        if (e->PreviousExecutionState == ApplicationExecutionState::Terminated)
        {
        }

        if (e->PrelaunchActivated == false)
        {
            if (rootFrame->Content == nullptr)
            {
                rootFrame->Navigate(TypeName(MainPage::typeid), e->Arguments);
            }
            Window::Current->Content = rootFrame;
            Window::Current->Activate();
        }
    }
    else
    {
        if (e->PrelaunchActivated == false)
        {
            if (rootFrame->Content == nullptr)
            {
                rootFrame->Navigate(TypeName(MainPage::typeid), e->Arguments);
            }
            Window::Current->Activate();
        }
    }

	Windows::ApplicationModel::Background::BackgroundExecutionManager::RequestAccessAsync();
	auto backgroundtaskbuilder = ref new Windows::ApplicationModel::Background::BackgroundTaskBuilder();
	auto backgroundsync = ref new Windows::ApplicationModel::Background::TimeTrigger(15, false);
	backgroundtaskbuilder->SetTrigger(backgroundsync);
	auto registered = backgroundtaskbuilder->Register();
}

void App::OnEnteredBackground(Object^ sender, EnteredBackgroundEventArgs^ e)
{
	auto modelView = static_cast<TootListModelView^>(Application::Current->Resources->Lookup("tootlist"));
	modelView->SuspendTimer();
}

void App::OnLeavingBackground(Object^ sender, LeavingBackgroundEventArgs^ e)
{
	auto modelView = static_cast<TootListModelView^>(Application::Current->Resources->Lookup("tootlist"));
	modelView->SetTimer();
}

void App::OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e)
{
    throw ref new FailureException("Failed to load Page " + e->SourcePageType.Name);
}

void client::App::OnBackgroundActivated(Windows::ApplicationModel::Activation::BackgroundActivatedEventArgs ^ args)
{
	auto modelView = static_cast<TootListModelView^>(Application::Current->Resources->Lookup("tootlist"));
	modelView->refresh();
}
