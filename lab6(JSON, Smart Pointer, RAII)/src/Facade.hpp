#pragma once
#include "Product.hpp"
#include "ProductFactory.hpp"
#include "Processor.hpp"
#include "FileGuard.hpp"
#include "TimerGuard.hpp"
#include "Logger.hpp"
#include <nlohmann/json.hpp>
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <iomanip>

// Делает 4 шага: загрузка, фильтрация, сохранение, вывод
class Facade {
public:
    explicit Facade(const std::string& inputPath, const std::string& outputPath)
        : m_inputPath(inputPath)
        , m_outputPath(outputPath)
    {}

    std::string execute() {
        TimerGuard timer("Facade::execute");

        auto products = loadProducts();
        Logger::info("Loaded " + std::to_string(products.size()) + " products");

        std::string today = getToday();
        Logger::info("Today: " + today);

        auto result = Processor::run(products, today);
        Logger::info("After filter: " + std::to_string(result.size()) + " products");

        saveJson(result);

        return buildGuiText(result);
    }

private:
    std::string m_inputPath;
    std::string m_outputPath;

    std::vector<std::shared_ptr<Product>> loadProducts() { 
        FileGuard file(m_inputPath);

        nlohmann::json j;
        file.stream() >> j;

        std::vector<std::shared_ptr<Product>> products; // 🤓☝️ pointer хранит products в векторе
        for (const auto& item : j) {
            products.push_back(ProductFactory::create(item)); // Создается product
        }
        return products;
    }

    void saveJson(const std::vector<std::shared_ptr<Product>>& products) {
        FileGuard file(m_outputPath, std::ios::out | std::ios::trunc);

        nlohmann::json arr = nlohmann::json::array();
        for (const auto& p : products) {
            arr.push_back({
                {"id",              p->id},
                {"name",            p->name},
                {"category",        p->category},
                {"price",           p->price},
                {"value",           p->value},
                {"expiration_date", p->expiration_date}
            });
        }

        file.stream() << arr.dump(2);
        Logger::info("Saved " + std::to_string(products.size()) + " products to " + m_outputPath);
    }

    static size_t utf8ExtraBytes(const std::string& str) {
        size_t extra = 0;
        for (unsigned char c : str) {
            if ((c & 0xC0) == 0x80) {
                extra++;
            }
        }
        return extra;
    }

    std::string buildGuiText(const std::vector<std::shared_ptr<Product>>& products) {
        if (products.empty()) {
            return "Нет свежих товаров.";
        }

        std::ostringstream oss;
        oss << std::left
            << std::setw(6)  << "ID"
            << std::setw(40) << "Name"
            << std::setw(10) << "Price"
            << "Expiration Date\n";
        oss << std::string(80, '-') << "\n";

        for (const auto& p : products) {
            oss << std::left
                << std::setw(6)  << p->id
                << std::setw(40 + utf8ExtraBytes(p->name)) << p->name
                << std::setw(10) << std::fixed << std::setprecision(2) << p->price
                << p->expiration_date << "\n";
        }

        return oss.str();
    }

    static std::string getToday() {
        return "2026-04-17";
    }
};
