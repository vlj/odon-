//
// TootWriter.xaml.h
// Declaration of the TootWriter class
//

#pragma once

#include "TootWriter.g.h"

namespace client
{

	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class TootWriter sealed
	{
		int _answerTo = 0;
		std::vector<Concurrency::task<Windows::Storage::Streams::IBuffer^> > medias;
	public:
		TootWriter();

		property int AnswerTo
		{
			int get()
			{
				return _answerTo;
			}

			void set(int v)
			{
				_answerTo = v;
			}
		}

		property Platform::String^ Text
		{
			void set(Platform::String^ v)
			{
				NewToot->Text = v;
			}
		}

		property Platform::String^ Spoiler
		{
			void set(Platform::String^ v)
			{
				SpoilerText->Text = v;
			}
		}

	private:
		void AppBarButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
