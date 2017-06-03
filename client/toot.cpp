#include "pch.h"
#include <regex>
#include "emojimap.h"


Windows::UI::Xaml::Controls::RichTextBlock ^ client::Util::convertParagraph(const utility::string_t & str)
{
	static const auto& parse_impl = [](Windows::Data::Xml::Dom::XmlNodeList^ nodes, auto container, const auto& parse_ref) -> void
	{
		for (auto node : nodes)
		{
			auto ndname = node->NodeName;
			if (ndname == "a")
			{
				auto a = ref new Windows::UI::Xaml::Documents::Hyperlink();
				parse_ref(node->ChildNodes, a->Inlines, parse_ref);
				auto url = node->Attributes->GetNamedItem("href");
				a->NavigateUri = ref new Windows::Foundation::Uri(url->InnerText);
				container->Append(a);
				continue;
			}
			if (ndname == "br")
			{
				auto br = ref new Windows::UI::Xaml::Documents::LineBreak();
				container->Append(br);
				continue;
			}
			if (ndname == "span")
			{
				parse_ref(node->ChildNodes, container, parse_ref);
				continue;
			}
			if (ndname == "#text")
			{
				auto run = ref new Windows::UI::Xaml::Documents::Run();
				run->Text = emojify(node->InnerText->Data());
				container->Append(run);
				continue;
			}
		}
		return;
	};

	auto ctrl = ref new Windows::UI::Xaml::Controls::RichTextBlock();
	ctrl->IsTextSelectionEnabled = false;
	auto doc = ref new Windows::Data::Xml::Dom::XmlDocument();
	auto&& escaped = std::regex_replace(str, std::wregex(L"(&(?![a-z]+;))"), LR"(&amp;)");
	// Fix some wrong html
	escaped = std::regex_replace(escaped, std::wregex(L"(<br>)"), LR"(<br/>)");
	auto content = ref new Platform::String(U("<div>")) +
		ref new Platform::String(escaped.c_str()) +
		ref new Platform::String(U("</div>"));
	auto buf = Windows::Security::Cryptography::CryptographicBuffer::ConvertStringToBinary(content, Windows::Security::Cryptography::BinaryStringEncoding::Utf8);
	doc->LoadXmlFromBuffer(buf);
	auto elements = doc->ChildNodes->Item(0)->ChildNodes;
	for (unsigned int i = 0; i < elements->Length; ++i)
	{
		auto tmp = elements->Item(i);
		auto nd = tmp->NodeName;
		if (nd != "p" && nd != "#text")
			throw;
		const auto& parse = [&](Windows::Data::Xml::Dom::XmlNodeList^ _nodes, Windows::UI::Xaml::Documents::InlineCollection^ _container)
		{
			parse_impl(_nodes, _container, parse_impl);
		};
		auto paragraph = ref new Windows::UI::Xaml::Documents::Paragraph();
		parse(tmp->ChildNodes, paragraph->Inlines);
		ctrl->Blocks->Append(paragraph);
	}
	return ctrl;
}


Platform::String ^ client::Util::emojify(const utility::string_t & str)
{
	auto&& result = replace(utility::string_t{ str });
	return ref new Platform::String(result.data());
}

Mastodon::InstanceConnexion client::Util::getInstance()
{
	auto localSettings = Windows::Storage::ApplicationData::Current->LocalSettings;

	if (localSettings->Values->Lookup("client_id") == nullptr ||
		localSettings->Values->Lookup("client_secret") == nullptr ||
		localSettings->Values->Lookup("access_token") == nullptr)
		throw ref new Platform::FailureException("Not logged properly");

	return Mastodon::InstanceConnexion{dynamic_cast<Platform::String^>(localSettings->Values->Lookup("access_token"))->Data()};
}
