#include "TextProcessor.h"
#include <sstream>
#include <cctype>
#include <algorithm>

std::string TextProcessor::toLowerCase(const std::string& text) {
    std::string result = text;
    for (char& c : result) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return result;
}

std::string TextProcessor::cleanToken(const std::string& rawToken) {
    if (rawToken.empty()) {
        return "";
    }

    std::string s = toLowerCase(rawToken);
    size_t start = 0;
    while (start < s.size() && (std::ispunct(static_cast<unsigned char>(s[start])) ||
                                std::isspace(static_cast<unsigned char>(s[start])))) {
        start++;
    }

    if (start >= s.size()) {
        return "";
    }

    size_t end = s.size() - 1;
    while (end > start && (std::ispunct(static_cast<unsigned char>(s[end])) ||
                           std::isspace(static_cast<unsigned char>(s[end])))) {
        end--;
    }

    return s.substr(start, end - start + 1);
}

std::vector<std::string> TextProcessor::tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    if (text.empty()) {
        return tokens;
    }

    std::stringstream ss(text);
    std::string rawWord;

    while (ss >> rawWord) {
        std::string cleaned = cleanToken(rawWord);
        if (!cleaned.empty()) {
            tokens.push_back(cleaned);
        }
    }

    return tokens;
}
