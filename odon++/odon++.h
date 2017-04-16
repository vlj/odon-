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
		std::vector<utility::string_t> media_attachments;
		std::vector<utility::string_t> mentions;
		std::vector<utility::string_t> tags;
		utility::string_t uri;
		utility::string_t content;
		utility::string_t url;
		size_t reblogs_count;
		size_t favourites_count;
		std::optional<utility::string_t> reblog;
		utility::string_t favorited;
		utility::string_t reblogged;

		Status(const web::json::value& v) : _account(v.at(U("account")).as_object())
		{
			id = v.at(U("id")).as_integer();
			//in_reply_to_id = v.at(U("in_reply_to_id")).as_string();
			//in_reply_to_account_id = v.at(U("in_reply_to_account_id")).as_string();
			created_at = v.at(U("created_at")).as_string();
			sensitive = v.at(U("sensitive")).as_bool();
			//spoiler_text = v.at(U("spoiler_text")).as_string();
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
			//application = v.at(U("application")).as_string();
			uri = v.at(U("uri")).as_string();
			content = v.at(U("content")).as_string();
			url = v.at(U("url")).as_string();
			reblogs_count = v.at(U("reblogs_count")).as_integer();
			favourites_count = v.at(U("favourites_count")).as_integer();
			//reblog = v.at(U("reblog")).as_string();
			//favorited = v.at(U("favorited")).as_string();
			//reblogged = v.at(U("reblogged")).as_string();
		}
	};

	struct InstanceConnexion
	{
		const std::string base_url{ "https://oc.todon.fr" };
		std::string api_base_url;
		const utility::string_t client_id;
		const utility::string_t client_secret;
		const utility::string_t access_token;
		size_t debug_requests;
		size_t ratelimit_method;

	private:
		auto __api_request(web::uri_builder uri)
		{
			web::http::client::http_client client(U("https://oc.todon.fr"));
			uri.append_query(U("access_token"), access_token);
			return client.request(web::http::methods::GET, uri.to_string())
				// Handle response headers arriving.
				.then([=](const web::http::http_response& response)
				{
					const auto& status = response.status_code();
					printf("Received response status code:%u\n", response.status_code());

					return response.extract_json();
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

		/**
		Fetch statuses, most recent ones first.Timeline can be home, mentions, local,
		public, or tag / hashtag.See the following functions documentation for what those do.

		The default timeline is the "home" timeline.

		Returns a list of toot dicts.
		*/
		auto timeline(const utility::string_t& timeline, size_t max_id, size_t since_id)
		{
			// Build request URI and start the request.
			web::uri_builder builder(U("/api/v1/timelines/"));
			builder.append_path(timeline);

			if (timeline == U("local"))
			{
				builder.append_query(U("local"), U("True"));
			}

			return __api_request(builder)
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

		/**
		Fetch the authenticated users home timeline (i.e. followed users and self).

		Returns a list of toot dicts.
		*/
		auto timeline_home(size_t max_id, size_t since_id)
		{
			return timeline(U("home"), max_id, since_id);
		}

		/**
		Fetches the authenticated users mentions.

		Returns a list of toot dicts.
		*/
		auto timeline_mentions(size_t max_id, size_t since_id)
		{
			return timeline(U("mentions"), max_id, since_id);
		}

		/**
		Fetches the local / instance-wide timeline.

		Returns a list of toot dicts.
		*/
		auto timeline_local(size_t max_id, size_t since_id)
		{
			return timeline(U("local"), max_id, since_id);
		}

		/**
		Fetches the public / visible-network timeline.

		Returns a list of toot dicts.
		*/
		auto timeline_public(size_t max_id, size_t since_id)
		{
			return timeline(U("public"), max_id, since_id);
		}

		/**
		Fetch a timeline of toots with a given hashtag.

		Returns a list of toot dicts.
		*/
		auto timeline_hashtag(const utility::string_t& hashtag, size_t max_id, size_t since_id)
		{
			return timeline(U("tag/") + hashtag, max_id, since_id);
		}

		auto status()
		{

		}

		auto status_context()
		{

		}

		auto status_reblogged_by()
		{

		}

		auto status_favourited_by()
		{

		}

		auto notifications()
		{

		}

		auto account()
		{

		}

		auto account_verify_credentials()
		{

		}

		auto account_unfollow()
		{

		}

		auto account_block()
		{

		}

		auto account_unblock()
		{

		}

		auto account_mute()
		{

		}

		auto account_unmute()
		{

		}

		auto account_search(const utility::string_t& search_term)
		{
			auto&& uri = web::uri_builder(U("/api/v1/search"));
			uri.append_query(U("q"), search_term);
			return __api_request(uri)
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
