﻿//
// Timeline.xaml.cpp
// Implémentation de la classe Timeline
//

#include "pch.h"
#include "Timeline.xaml.h"

using namespace client;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// Pour plus d'informations sur le modèle d'élément Page vierge, consultez la page https://go.microsoft.com/fwlink/?LinkId=234238

Timeline::Timeline()
{
	InitializeComponent();
	_tootscol = ref new Platform::Collections::Vector<Toot^>();
	_tootscol->Append(ref new Toot("test"));
	_tootscol->Append(ref new Toot("test"));
	_tootscol->Append(ref new Toot("test"));
}


void client::Timeline::paneOpened_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->Pane->IsPaneOpen = !this->Pane->IsPaneOpen;
}
