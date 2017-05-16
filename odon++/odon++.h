#pragma once


#include <string>
#include <memory>
#include <optional>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/json.h>

namespace Mastodon
{
	struct Account
	{
		utility::string_t acct;
		utility::string_t avatar;
		utility::string_t created_at;
		utility::string_t display_name;
		size_t followers_count;
		size_t following_count;
		utility::string_t header;
		size_t id;
		bool locked;
		utility::string_t note;
		size_t statuses_count;
		utility::string_t url;
		utility::string_t username;

		Account() = default;

		Account(const web::json::object& object)
		{
			acct = object.at(U("acct")).as_string();
			avatar = object.at(U("avatar")).as_string();
			created_at = object.at(U("created_at")).as_string();
			display_name = object.at(U("display_name")).as_string();
			followers_count = object.at(U("followers_count")).as_integer();
			following_count = object.at(U("following_count")).as_integer();
			header = object.at(U("header")).as_string();
			id = object.at(U("id")).as_integer();
			locked = object.at(U("locked")).as_bool();
			note = object.at(U("note")).as_string();
			statuses_count = object.at(U("statuses_count")).as_integer();
			url = object.at(U("url")).as_string();
			username = object.at(U("username")).as_string();
		}
	};

	enum class attachement_type
	{
		image,
		video,
		gifv
	};

	struct Attachment
	{
		int id;
		attachement_type type;
		utility::string_t url;
		std::optional<utility::string_t> remote_url;
		utility::string_t preview_url;
		std::optional<utility::string_t> text_url;

		Attachment() = default;

		Attachment(const web::json::object& object)
		{
			id = object.at(U("id")).as_integer();
			type = [](const auto& tp)
			{
				if (tp == U("image")) return attachement_type::image;
				if (tp == U("video")) return attachement_type::video;
				if (tp == U("gifv")) return attachement_type::gifv;
				throw;
			}(object.at(U("type")).as_string());
			url = object.at(U("url")).as_string();
			if (!object.at(U("remote_url")).is_null())
				remote_url = object.at(U("remote_url")).as_string();
			preview_url = object.at(U("preview_url")).as_string();
			if (!object.at(U("text_url")).is_null())
				text_url = object.at(U("text_url")).as_string();
		}
	};

	enum class visibility_level
	{
		public_level,
		unlisted_level,
		private_level,
		direct_level
	};

	struct Status
	{
		Account _account;
		size_t id;
		std::optional<utility::string_t> in_reply_to_id;
		utility::string_t created_at;
		std::optional<utility::string_t> in_reply_to_account_id;
		bool sensitive;
		std::optional<utility::string_t> spoiler_text;
		visibility_level visibility;
		utility::string_t application;
		std::vector<Attachment> media_attachments;
		std::vector<utility::string_t> mentions;
		std::vector<utility::string_t> tags;
		utility::string_t uri;
		utility::string_t content;
		utility::string_t url;
		size_t reblogs_count;
		size_t favourites_count;
		std::shared_ptr<Status> reblog;
		bool favourited;
		bool reblogged;

		Status(const web::json::value& v) : _account(v.at(U("account")).as_object())
		{
			id = v.at(U("id")).as_integer();
			uri = v.at(U("uri")).as_string();
			url = v.at(U("url")).as_string();
			//in_reply_to_id = v.at(U("in_reply_to_id")).as_string();
			//in_reply_to_account_id = v.at(U("in_reply_to_account_id")).as_string();
//			sensitive = v.at(U("sensitive")).as_bool();
			//application = v.at(U("application")).as_string();
			if (v.has_field(U("reblog")) && !v.at(U("reblog")).is_null())
			{
				reblog = std::make_shared<Status>(v.at(U("reblog")));
			}
			content = v.at(U("content")).as_string();
			created_at = v.at(U("created_at")).as_string();
			reblogs_count = v.at(U("reblogs_count")).as_integer();
			favourites_count = v.at(U("favourites_count")).as_integer();
			reblogged = (!v.has_field(U("reblogged")) || v.at(U("reblogged")).is_null()) ?
				false :
				v.at(U("reblogged")).as_bool();
			favourited = (!v.has_field(U("favourited")) || v.at(U("favourited")).is_null()) ?
				false :
				v.at(U("favourited")).as_bool();
			const auto& spoiler = v.at(U("spoiler_text"));
			if (!spoiler.is_null())
				spoiler_text = spoiler.as_string();
			visibility = [](const auto& v)
			{
				if (v == U("public"))
					return visibility_level::public_level;
				if (v == U("private"))
					return visibility_level::private_level;
				if (v == U("unlisted"))
					return visibility_level::unlisted_level;
				if (v == U("direct"))
					return visibility_level::direct_level;
				throw;
			}(v.at(U("visibility")).as_string());
			media_attachments = [](const auto& v) {
				std::vector<Attachment> result;
				for (const auto& att : v)
				{
					result.emplace_back(att.as_object());
				}
				return result;
			} (v.at(U("media_attachments")).as_array());
		}

		Status() = default;
		Status(const Status& in) = default;
	};

	struct Context
	{
		std::vector<Status> ancestors;
		std::vector<Status> descendants;

		Context(const web::json::value& v)
		{
			for (const auto& a : v.at(U("ancestors")).as_array())
			{
				ancestors.emplace_back(a);
			}

			for (const auto& d : v.at(U("descendants")).as_array())
			{
				descendants.emplace_back(d);
			}
		}

		Context() = default;
	};

	struct Relationship
	{
		size_t id;
		bool following;
		bool followed_by;
		bool blocking;
		bool muting;
		bool requested;

		Relationship() = default;

		Relationship(const web::json::value& v)
		{
			id = v.at(U("id")).as_integer();
			following = v.at(U("following")).as_bool();
			followed_by = v.at(U("followed_by")).as_bool();
			blocking = v.at(U("blocking")).as_bool();
			muting = v.at(U("muting")).as_bool();
			requested = v.at(U("requested")).as_bool();
		}
	};

	enum class NotificationType
	{
		mention,
		reblog,
		favourite,
		follow,
	};

	struct Notifications
	{
		size_t id;
		NotificationType type;
		utility::string_t created_at;
		Account account;
		std::optional<Status> status;

		Notifications(const web::json::value& v) : account(v.at(U("account")).as_object())
		{
			id = v.at(U("id")).as_integer();
			type = [](const auto& t) {
				if (t == U("mention")) return NotificationType::mention;
				if (t == U("reblog")) return NotificationType::reblog;
				if (t == U("favourite")) return NotificationType::favourite;
				if (t == U("follow")) return NotificationType::follow;
				throw;
			} (v.at(U("type")).as_string());
			if (v.has_field(U("status")) && !v.at(U("status")).is_null())
				status = Status{ v.at(U("status")) };
			created_at = v.at(U("created_at")).as_string();
		}
	};

	struct InstanceAnonymous
	{
	protected:
		auto __api_request(web::uri_builder uri, const web::http::method& method) const
		{
			web::http::client::http_client client(base_url);
			return client.request(method, uri.to_string())
				// Handle response headers arriving.
				.then([=](const web::http::http_response& response)
				{
					const auto& status = response.status_code();
					printf("Received response status code:%u\n", response.status_code());

					return response.extract_json();
				});
		}

		/**
		Fetch statuses, most recent ones first.Timeline can be home, mentions, local,
		public, or tag / hashtag.See the following functions documentation for what those do.

		The default timeline is the "home" timeline.

		Returns a list of toot dicts.
		*/
		template<typename URIFixFunction>
		auto timeline(const utility::string_t& timeline, URIFixFunction&& f) const
		{
			// Build request URI and start the request.
			auto&& builder = web::uri_builder(U("/api/v1/timelines/"));
			builder.append_path(timeline);

			return __api_request(f(std::move(builder)), web::http::methods::GET)
				.then([=](const web::json::value& v)
				{
					const auto& json_array = v.as_array();
					auto&& result = std::vector<Status>{};
					for (const auto& json_v : json_array)
					{
						result.emplace_back(Status{ json_v });
					}
					return result;
				});
		}


	public:
		const utility::string_t base_url{ U("https://oc.todon.fr") };

		auto account_search(const utility::string_t& search_term) const
		{
			auto&& uri = web::uri_builder(U("/api/v1/search"));
			uri.append_query(U("q"), search_term);
			return __api_request(uri, web::http::methods::GET)
				.then([](const web::json::value& v)
			{
				auto&& result = std::vector<Account>{};
				for (const auto& json_v : v.at(U("accounts")).as_array())
				{
					result.emplace_back(Account{ json_v.as_object() });
				}
				return result;
			});
		}

		/**
		Fetches the public / visible-network timeline.

		Returns a list of toot dicts.
		*/
		auto timeline_public(const bool& local) const
		{
			return timeline(U("public"), [local](auto&& uri) {
				if (local) uri.append_query(U("local"), U("true"));
				return uri;
			});
		}

		/**
		Fetch a timeline of toots with a given hashtag.

		Returns a list of toot dicts.
		*/
		auto timeline_hashtag(const utility::string_t& hashtag, const bool& local) const
		{
			return timeline(U("tag/") + hashtag, [local](auto&& uri) {
				if (local) uri.append_query(U("local"), U("true"));
				return uri;
			});
		}

		auto status(const size_t& id) const
		{
			auto&& uri = web::uri_builder{ U("/api/v1/statuses/") + std::to_wstring(id) };
			return __api_request(uri, web::http::methods::GET)
				.then([](const web::json::value& v) {
				return Status{ v };
			});
		}

		auto status_context(const size_t& id) const
		{
			auto&& uri = web::uri_builder{ U("/api/v1/statuses/") + std::to_wstring(id) + U("/context")};
			return __api_request(uri, web::http::methods::GET)
				.then([](const web::json::value& v) {
				return Context{ v };
			});
		}
	};

	struct InstanceConnexion : public InstanceAnonymous
	{
		const utility::string_t client_id;
		const utility::string_t client_secret;
		const utility::string_t access_token;
		size_t debug_requests;
		size_t ratelimit_method;

	protected:
		auto __api_request(web::uri_builder uri, const web::http::method& method = web::http::methods::GET) const
		{
			uri.append_query(U("access_token"), access_token);
			return InstanceAnonymous::__api_request(uri, method);
		}

		auto __relationship_update(const size_t& id, const utility::string_t& key) const
		{
			web::uri_builder uri(U("/api/v1/accounts/"));
			uri.append_path(std::to_wstring(id));
			uri.append_path(key);
			return __api_request(uri, web::http::methods::POST)
				.then([](const web::json::value& v) { return Relationship{ v }; });
		}

		auto __status_interaction(const size_t& id, const utility::string_t& operation) const
		{
			auto&& uri = web::uri_builder{ U("/api/v1/statuses/") + std::to_wstring(id) + operation };
			return __api_request(uri, web::http::methods::POST)
				.then([](const web::json::value& v)
				{
					return Status{ v };
				});
		}
	public:
		/**
		Create a new API wrapper instance based on the given client_secret and client_id.If you
		give a client_id and it is not a file, you must also give a secret.

		You can also specify an access_token, directly or as a file(as written by log_in).

		odon++ can try to respect rate limits in several ways, controlled by ratelimit_method.
		"throw" makes functions throw a MastodonRatelimitError when the rate
		limit is hit. "wait" mode will, once the limit is hit, wait and retry the request as soon
		as the rate limit resets, until it succeeds. "pace" works like throw, but tries to wait in
		between calls so that the limit is generally not hit(How hard it tries to not hit the rate
		limit can be controlled by ratelimit_pacefactor).The default setting is "wait".Note that
		even in "wait" and "pace" mode, requests can still fail due to network or other problems!Also
		note that "pace" and "wait" are NOT thread safe.

		Specify api_base_url if you wish to talk to an instance other than the flagship one.
		If a file is given as client_id, read client ID and secret from that file.

		By default, a timeout of 300 seconds is used for all requests.If you wish to change this,
		pass the desired timeout(in seconds) as request_timeout.
		*/
		InstanceConnexion(const utility::string_t& _client_id, const utility::string_t& _client_secret) :
			client_id(_client_id), client_secret(_client_secret)
		{

		}

		InstanceConnexion(const utility::string_t& _client_id, const utility::string_t& _client_secret,
			const utility::string_t& _access_token) :
			client_id(_client_id), client_secret(_client_secret), access_token(_access_token)
		{

		}

		/**
		Create a new app with given client_name and scopes(read, write, follow)

		Specify redirect_uris if you want users to be redirected to a certain page after authenticating.
		Specify to_file to persist your apps info to a file so you can use them in the constructor.
		Specify api_base_url if you want to register an app on an instance different from the flagship one.

		Presently, app registration is open by default, but this is not guaranteed to be the case for all
		future mastodon instances or even the flagship instance in the future.

		Returns client_id and client_secret.
		*/
		static
		auto create_app(const  utility::string_t& client_name)
		{
			web::http::client::http_client client(U("https://oc.todon.fr"));

			// Build request URI and start the request.
			web::uri_builder builder(U("/api/v1/apps"));
			builder.append_query(U("client_name"), client_name);
			builder.append_query(U("scopes"), U("read write follow"));
			builder.append_query(U("redirect_uris"), U("urn:ietf:wg:oauth:2.0:oob"));
			return client.request(web::http::methods::POST, builder.to_string())
				// Handle response headers arriving.
				.then([=](const web::http::http_response& response)
			{
				printf("Received response status code:%u\n", response.status_code());

				// Write response body into the file.
				return response.extract_json();
			})
				.then([=](web::json::value v)
			{
				return std::make_tuple(
					v[U("client_id")].as_string(),
					v[U("client_secret")].as_string());
			});
		}

		/**
		Log in and sets access_token to what was returned. Note that your
		username is the e-mail you use to log in into mastodon.

		Can persist access token to file, to be used in the constructor.

		Will throw a MastodonIllegalArgumentError if username / password
		are wrong, scopes are not valid or granted scopes differ from requested.

		Returns the access_token.
		*/
		auto log_in(const utility::string_t& username, const utility::string_t& password)
		{
			web::http::client::http_client client(U("https://oc.todon.fr"));
			// Build request URI and start the request.
			web::uri_builder builder(U("/oauth/token"));
			builder.append_query(U("username"), username);
			builder.append_query(U("password"), password);
			builder.append_query(U("client_id"), client_id);
			builder.append_query(U("client_secret"), client_secret);
			builder.append_query(U("scope"), U("read write follow"));
			builder.append_query(U("grant_type"), U("password"));
			return client.request(web::http::methods::POST, builder.to_string())
				// Handle response headers arriving.
				.then([=](const web::http::http_response& response)
				{
					printf("Received response status code:%u\n", response.status_code());

					// Write response body into the file.
					return response.extract_json();
				})
				.then([=](web::json::value v)
				{
					return v[U("access_token")].as_string();
				});
		}

		auto account(const size_t& id) const
		{
			auto&& uri = web::uri_builder{ U("/api/v1/accounts/") + std::to_wstring(id) };
			return __api_request(uri, web::http::methods::GET)
				.then([](const web::json::value& v) { return Account{ v.as_object() }; });
		}

		/**
		Fetch the authenticated users home timeline (i.e. followed users and self).

		Returns a list of toot dicts.
		*/
		auto timeline_home() const
		{
			return Mastodon::InstanceAnonymous::timeline(U("home"),
				[this](auto&& uri) {
					uri.append_query(U("access_token"), access_token);
					return uri; });
		}

		auto update_account(const utility::string_t& display_name, const utility::string_t& image_as_base64) const
		{
			auto&& uri = web::uri_builder{ U("/api/v1/accounts/update_credentials") };
			uri.append_query(U("access_token"), access_token);
			web::http::client::http_client client(base_url);
			return client.request(web::http::methods::PATCH, uri.to_string(), U("data:image/png;base64,") + image_as_base64)
				.then([=](const web::http::http_response& response)
			{
				const auto& status = response.status_code();
				printf("Received response status code:%u\n", response.status_code());

				return;
			});
		}

		/**
		Fetches the authenticated users mentions.

		Returns a list of toot dicts.
		*/


		auto statuses(const size_t& id) const
		{
			auto&& uri = web::uri_builder{ U("/api/v1/accounts/") + std::to_wstring(id) + U("/statuses") };
			return __api_request(uri, web::http::methods::GET)
				.then([](const web::json::value& v) {
				auto&& result = std::vector<Status>{};
				for (const auto& s : v.as_array())
				{
					result.emplace_back(s);
				}
				return result;
			});
		}

		auto status_reblogged_by()
		{

		}

		auto status_favourited_by()
		{

		}

		auto status_post(const utility::string_t& content,
			const visibility_level& visibility,
			const std::vector<int>& media_ids,
			const std::optional<int> &answer_to,
			const std::optional<utility::string_t>& spoiler_text,
			const bool& sensitive) const
		{
			auto&& uri = web::uri_builder{ U("/api/v1/statuses") };
			uri.append_query(U("status"), content);
			uri.append_query(U("sensitive"), sensitive ? U("True") : U("False"));
			for (const auto& id : media_ids)
			{
				uri.append_query(U("media_ids[]"), id);
			}

			if (answer_to.has_value())
			{
				uri.append_query(U("in_reply_to_id"), answer_to.value());
			}
			if (spoiler_text.has_value())
			{
				uri.append_query(U("spoiler_text"), spoiler_text.value());
			}
			uri.append_query(U("visibility"), [](const auto& visibility) {
				switch (visibility)
				{
				case visibility_level::direct_level:
					return U("direct");
				case visibility_level::private_level:
					return U("private");
				case visibility_level::public_level:
					return U("public");
				case visibility_level::unlisted_level:
					return U("unlisted");
				}
				throw;
			} (visibility));
			return __api_request(uri, web::http::methods::POST);
		}

		auto notifications() const
		{
			auto&& uri = web::uri_builder{ U("/api/v1/notifications") };
			return __api_request(uri, web::http::methods::GET)
				.then([](const web::json::value& v) {
					auto&& result = std::vector<Notifications>{};
					for (const auto& relation : v.as_array())
					{
						result.emplace_back(relation);
					}
					return result;
				});
		}

		auto status_favourite(const size_t& id) const
		{
			return __status_interaction(id, U("/favourite"));
		}

		auto status_unfavourite(const size_t& id) const
		{
			return __status_interaction(id, U("/unfavourite"));
		}

		auto status_reblog(const size_t& id) const
		{
			return __status_interaction(id, U("/reblog"));
		}

		auto status_unreblog(const size_t& id) const
		{
			return __status_interaction(id, U("/unreblog"));
		}

		auto account_verify_credentials()
		{

		}

		auto account_follow(const size_t& id)
		{
			return __relationship_update(id, U("follow"));
		}

		auto account_unfollow(const size_t& id)
		{
			return __relationship_update(id, U("unfollow"));
		}

		auto account_block(const size_t& id)
		{
			return __relationship_update(id, U("block"));
		}

		auto account_unblock(const size_t& id)
		{
			return __relationship_update(id, U("unblock"));
		}

		auto account_mute(const size_t& id)
		{
			return __relationship_update(id, U("mute"));
		}

		auto account_unmute(const size_t& id)
		{
			return __relationship_update(id, U("unmute"));
		}

		auto follow_request_authorize()
		{

		}

		auto follow_request_reject()
		{

		}

		auto media_post()
		{

		}

	private:
		auto __datetime_to_epoch()
		{

		}
	};

}
