﻿//
// App.xaml.cpp
// Implémentation de la classe App.
//

#include "pch.h"
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

/// <summary>
/// Initialise l'objet d'application de singleton.  Il s'agit de la première ligne du code créé
/// à être exécutée. Elle correspond donc à l'équivalent logique de main() ou WinMain().
/// </summary>
App::App()
{
    InitializeComponent();
    EnteredBackground += ref new EnteredBackgroundEventHandler(this, &App::OnEnteredBackground);
	LeavingBackground += ref new LeavingBackgroundEventHandler(this, &App::OnLeavingBackground);
}

/// <summary>
/// Invoqué lorsque l'application est lancée normalement par l'utilisateur final.  D'autres points d'entrée
/// seront utilisés par exemple au moment du lancement de l'application pour l'ouverture d'un fichier spécifique.
/// </summary>
/// <param name="e">Détails concernant la requête et le processus de lancement.</param>
void App::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e)
{
    auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);

    // Ne répétez pas l'initialisation de l'application lorsque la fenêtre comporte déjà du contenu,
    // assurez-vous juste que la fenêtre est active
    if (rootFrame == nullptr)
    {
        // Crée un frame qui joue le rôle de contexte de navigation et l'associe à
        // une clé SuspensionManager
        rootFrame = ref new Frame();

        rootFrame->NavigationFailed += ref new Windows::UI::Xaml::Navigation::NavigationFailedEventHandler(this, &App::OnNavigationFailed);

        if (e->PreviousExecutionState == ApplicationExecutionState::Terminated)
        {
            // TODO: restaurez l'état de la session enregistrée uniquement quand cela est approprié, en planifiant
            // les étapes de lancement finales une fois la restauration terminée

        }

        if (e->PrelaunchActivated == false)
        {
            if (rootFrame->Content == nullptr)
            {
                // Quand la pile de navigation n'est pas restaurée, accédez à la première page,
                // puis configurez la nouvelle page en transmettant les informations requises en tant que
                // paramètre
                rootFrame->Navigate(TypeName(MainPage::typeid), e->Arguments);
            }
            // Placez le frame dans la fenêtre active
            Window::Current->Content = rootFrame;
            // Vérifiez que la fenêtre actuelle est active
            Window::Current->Activate();
        }
    }
    else
    {
        if (e->PrelaunchActivated == false)
        {
            if (rootFrame->Content == nullptr)
            {
                // Quand la pile de navigation n'est pas restaurée, accédez à la première page,
                // puis configurez la nouvelle page en transmettant les informations requises en tant que
                // paramètre
                rootFrame->Navigate(TypeName(MainPage::typeid), e->Arguments);
            }
            // Vérifiez que la fenêtre actuelle est active
            Window::Current->Activate();
        }
    }

	Windows::ApplicationModel::Background::BackgroundExecutionManager::RequestAccessAsync();
	auto backgroundtaskbuilder = ref new Windows::ApplicationModel::Background::BackgroundTaskBuilder();
	auto backgroundsync = ref new Windows::ApplicationModel::Background::TimeTrigger(15, false);
	backgroundtaskbuilder->SetTrigger(backgroundsync);
	auto registered = backgroundtaskbuilder->Register();
}

/// <summary>
/// Appelé lorsque l'exécution de l'application est suspendue.  L'état de l'application est enregistré
/// sans savoir si l'application pourra se fermer ou reprendre sans endommager
/// le contenu de la mémoire.
/// </summary>
/// <param name="sender">Source de la requête de suspension.</param>
/// <param name="e">Détails de la requête de suspension.</param>
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

/// <summary>
/// Appelé lorsque la navigation vers une page donnée échoue
/// </summary>
/// <param name="sender">Frame à l'origine de l'échec de navigation.</param>
/// <param name="e">Détails relatifs à l'échec de navigation</param>
void App::OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e)
{
    throw ref new FailureException("Failed to load Page " + e->SourcePageType.Name);
}

void client::App::OnBackgroundActivated(Windows::ApplicationModel::Activation::BackgroundActivatedEventArgs ^ args)
{
	auto modelView = static_cast<TootListModelView^>(Application::Current->Resources->Lookup("tootlist"));
	modelView->refresh();
}
