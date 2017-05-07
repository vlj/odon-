﻿//
// tootviewer.xaml.cpp
// Implementation of the tootviewer class
//

#include "pch.h"
#include "tootviewer.xaml.h"

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

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

tootviewer::tootviewer()
{
	InitializeComponent();
}


void client::tootviewer::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto toot = dynamic_cast<Toot^>(DataContext);
	auto id = static_cast<int>(toot->Author->id);
	OnImagePressed(this, id);
}
