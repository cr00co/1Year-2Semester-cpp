#pragma once
#include "Product.hpp"
#include <nlohmann/json.hpp>
#include <memory>

class ProductFactory {
public:
    static std::unique_ptr<Product> create(const nlohmann::json& j) {
        auto p = std::make_unique<Product>();
        p->id              = j.at("id").get<int>();
        p->name            = j.at("name").get<std::string>();
        p->category        = j.at("category").get<std::string>();
        p->price           = j.at("price").get<double>();
        p->value           = j.at("value").get<int>();
        p->expiration_date = j.at("expiration_date").get<std::string>();
        return p;
    }
};
