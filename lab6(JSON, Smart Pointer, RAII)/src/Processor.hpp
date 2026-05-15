#pragma once
#include "Product.hpp"
#include "Logger.hpp"
#include <vector>
#include <memory>
#include <string>
#include <cctype>
#include <algorithm>

class Processor {
public:
    static std::vector<std::shared_ptr<Product>> // 🤓☝️ pointer хранит products в векторе
    run(const std::vector<std::shared_ptr<Product>>& products, const std::string& today)
    {
        std::vector<std::shared_ptr<Product>> result;

        for (const auto& p : products) {
            if (isFresh(p->expiration_date, today)) {
                auto modified       = std::make_shared<Product>(*p);
                modified->name      = uppercaseVowels(p->name);
                result.push_back(modified);
                Logger::debug("Kept: " + p->name + " -> " + modified->name);
            } else {
                Logger::warning("Expired, skipped: " + p->name + " (" + p->expiration_date + ")");
            }
        }

        return result;
    }

private:
    static bool isFresh(const std::string& expDate, const std::string& today) {
        return expDate > today;
    }

    static std::string uppercaseVowels(const std::string& name) {
        const std::string vowels = "aeiouAEIOUаеёиоуыэюяАЕЁИОУЫЭЮЯ";
        std::string result = name;
        for (size_t i = 0; i < result.length(); ) {
            unsigned char byte = static_cast<unsigned char>(result[i]);
            if (byte >= 0xC0) {
                if (i + 1 < result.length()) {
                    std::string twoBytes = result.substr(i, 2);
                    if (twoBytes == "а") { result.replace(i, 2, "А"); i += 2; }
                    else if (twoBytes == "е") { result.replace(i, 2, "Е"); i += 2; }
                    else if (twoBytes == "и") { result.replace(i, 2, "И"); i += 2; }
                    else if (twoBytes == "о") { result.replace(i, 2, "О"); i += 2; }
                    else if (twoBytes == "у") { result.replace(i, 2, "У"); i += 2; }
                    else if (twoBytes == "ы") { result.replace(i, 2, "Ы"); i += 2; }
                    else if (twoBytes == "э") { result.replace(i, 2, "Э"); i += 2; }
                    else if (twoBytes == "ю") { result.replace(i, 2, "Ю"); i += 2; }
                    else if (twoBytes == "я") { result.replace(i, 2, "Я"); i += 2; }
                    else if (twoBytes == "ё") { result.replace(i, 2, "Ё"); i += 2; }
                    else { i += 2; }
                } else { i++; }
            } else if (byte < 0x80) {
                if (vowels.find(result[i]) != std::string::npos) {
                    result[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[i])));
                }
                i++;
            } else {
                i++;
            }
        }
        return result;
    }
};
