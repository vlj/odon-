#include "pch.h"
#include <regex>

Windows::UI::Xaml::Controls::RichTextBlock ^ client::Toot::convertParagraph(const utility::string_t & str)
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
				run->Text = Windows::Data::Html::HtmlUtilities::ConvertToText(node->InnerText);
				container->Append(run);
				continue;
			}
		}
		return;
	};

	auto ctrl = ref new Windows::UI::Xaml::Controls::RichTextBlock();
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
		if (nd != "p")
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
