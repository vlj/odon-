#pragma once


#include <string>
#include <memory>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/json.h>

namespace Mastodon
{
	struct Toot
	{
		struct account
		{
			std::string acct;
			std::string avatar;
			std::string created_at;
			std::string display_name;
			size_t followers_count;
			size_t following_count;
			std::string header;
			size_t id;
			bool locked;
			std::string note;
			size_t statuses_count;
			std::string url;
			std::string username;

			account(const web::json::object& object)
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

		account _account;

		Toot(const web::json::value& v) : _account(v.at(U("account")).as_object())
		{

		}
	};

	struct InstanceConnexion
	{
		const std::string base_url{ "https://oc.todon.fr" };
		std::string api_base_url;
		const std::string client_id;
		const std::string client_secret;
		std::string access_token;
		size_t debug_requests;
		size_t ratelimit_method;

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
		InstanceConnexion(const std::string& _client_id, const std::string& _client_secret) :
			client_id(_client_id), client_secret(_client_secret)
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
			auto create_app(const std::string& client_name)
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
		auto log_in(const std::string& username, const std::string& password)
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
		auto timeline(const std::string& timeline, size_t max_id, size_t since_id, const std::string& access_token)
		{
			// Open stream to output file.
			web::http::client::http_client client(U("https://oc.todon.fr"));

			// Build request URI and start the request.
			web::uri_builder builder(U("/api/v1/timelines/") + timeline);
			//builder.append_query(U("timeline"), timeline);
			builder.append_query(U("access_token"), access_token);

			if (timeline == "local")
			{
				builder.append_query(U("local"), U("True"));
			}
			return client.request(web::http::methods::GET, builder.to_string())
				// Handle response headers arriving.
				.then([=](const web::http::http_response& response)
			{
				printf("Received response status code:%u\n", response.status_code());

				return response.extract_json();
			})
				.then([=](const web::json::value& v)
			{
				const auto& json_array = v.as_array();
				auto&& result = std::vector<Toot>{};
				for (const auto& json_v : json_array)
				{
					result.emplace_back(Toot{ json_v });
				}
				return result;
			});
		}

		/**
		Fetch the authenticated users home timeline (i.e. followed users and self).

		Returns a list of toot dicts.
		*/
		auto timeline_home(size_t max_id, size_t since_id, const std::string& access_token)
		{
			return timeline("home", max_id, since_id, access_token);
		}

		/**
		Fetches the authenticated users mentions.

		Returns a list of toot dicts.
		*/
		auto timeline_mentions(size_t max_id, size_t since_id, const std::string& access_token)
		{
			return timeline("mentions", max_id, since_id, access_token);
		}

		/**
		Fetches the local / instance-wide timeline.

		Returns a list of toot dicts.
		*/
		auto timeline_local(size_t max_id, size_t since_id, const std::string& access_token)
		{
			return timeline("local", max_id, since_id, access_token);
		}

		/**
		Fetches the public / visible-network timeline.

		Returns a list of toot dicts.
		*/
		auto timeline_public(size_t max_id, size_t since_id, const std::string& access_token)
		{
			return timeline("public", max_id, since_id, access_token);
		}

		/**
		Fetch a timeline of toots with a given hashtag.

		Returns a list of toot dicts.
		*/
		auto timeline_hashtag(const std::string& hashtag, size_t max_id, size_t since_id, const std::string& access_token)
		{
			return timeline("tag/" + hashtag, max_id, since_id, access_token);
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

		auto __api_request()
		{

		}
	};

}
