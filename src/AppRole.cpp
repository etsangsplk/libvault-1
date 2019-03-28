#include <nlohmann/json.hpp>
#include "VaultClient.h"

AppRole::AppRole(std::string role_id, std::string secret_id) :
  role_id_(role_id), secret_id_(secret_id) {}

std::string AppRole::getUrl(VaultClient* vaultClient, std::string path) {
  return vaultClient->getUrl("/v1/auth/approle", path);
}

std::string AppRole::authenticate(VaultClient* vaultClient) {
  nlohmann::json j;
  j = nlohmann::json::object();
  j["role_id"] = role_id_;
  j["secret_id"] = secret_id_;

  auto response = vaultClient->getHttpClient().post(getUrl(vaultClient, "/login"), vaultClient->getToken(), j.dump());
  return nlohmann::json::parse(response)["auth"]["client_token"];
}