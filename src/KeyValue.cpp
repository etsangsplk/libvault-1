#include <nlohmann/json.hpp>
#include "VaultClient.h"

KeyValue::KeyValue(VaultClient &client) :
  version_(KeyValue::Version::v2), client_(client), mount_("/secret") {}

KeyValue::KeyValue(VaultClient &client, std::string mount) :
  version_(KeyValue::Version::v2), client_(client), mount_(mount) {}

KeyValue::KeyValue(VaultClient &client, KeyValue::Version version) :
  version_(version), client_(client), mount_("/secret") {}

KeyValue::KeyValue(VaultClient &client,
		   std::string mount,
		   KeyValue::Version version) :
  version_(version), client_(client), mount_(mount) {}

// TODO: this should be an optional
std::string KeyValue::getUrl(std::string path) {
  switch (version_) {
  case KeyValue::Version::v1:
    return client_.getUrl("/v1" + mount_ + "/", path);
  case KeyValue::Version::v2:
    return client_.getUrl("/v1" + mount_ + "/data/", path);
  default:
    return "ERROR";
  }
}

std::experimental::optional<std::string> KeyValue::get(std::string path) {
  if (!client_.isAuthenticated()) {
    return std::experimental::nullopt;
  }

  auto response = client_.getHttpClient().get(getUrl(path), client_.getToken());
  return response ?
    std::experimental::optional<std::string>(response.value().body) :
    std::experimental::nullopt;
}

std::experimental::optional<std::string>
KeyValue::put(std::string path,
	      std::unordered_map<std::string, std::string> map) {
  if (!client_.isAuthenticated()) {
    return std::experimental::nullopt;
  }

  nlohmann::json j;
  j["data"] = nlohmann::json::object();
  std::for_each(map.begin(), map.end(),
		[&](std::pair<std::string, std::string> pair) {
    j["data"][pair.first] = pair.second;
  });

  auto response = client_.getHttpClient()
    .post(getUrl(path), client_.getToken(), j.dump());

  return response ?
    std::experimental::optional<std::string>(response.value().body) :
    std::experimental::nullopt;
}

std::experimental::optional<std::string> KeyValue::del(std::string path) {
  if (!client_.isAuthenticated()) {
    return std::experimental::nullopt;
  }

  auto response = client_.getHttpClient().del(getUrl(path), client_.getToken());
  return response ?
    std::experimental::optional<std::string>(response.value().body) :
    std::experimental::nullopt;
}
