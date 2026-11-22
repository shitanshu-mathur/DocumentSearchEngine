#include "TextProcessor.h"
#include <sstream>
#include <cctype>
#include <algorithm>
#include <unordered_set>

std::string TextProcessor::toLowerCase(const std::string& text) {
    std::string result = text;
    for (char& c : result) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return result;
}

bool TextProcessor::isSpecialTerm(const std::string& term) {
    static const std::unordered_set<std::string> specialTerms = {
        "c++", "c#", "oop", "dsa", "html", "sql"
    };
    return specialTerms.find(term) != specialTerms.end();
}

std::string TextProcessor::cleanToken(const std::string& rawToken) {
    if (rawToken.empty()) {
        return "";
    }

    std::string s = toLowerCase(rawToken);

    if (isSpecialTerm(s)) {
        return s;
    }

    if (s.find("c++") != std::string::npos) {
        return "c++";
    }
    if (s.find("c#") != std::string::npos) {
        return "c#";
    }

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

    std::string cleaned = s.substr(start, end - start + 1);

    if (isSpecialTerm(cleaned)) {
        return cleaned;
    }

    size_t internalPunct = 0;
    while (internalPunct < cleaned.size() && std::isalnum(static_cast<unsigned char>(cleaned[internalPunct]))) {
        internalPunct++;
    }
    if (internalPunct < cleaned.size()) {
        return cleaned.substr(0, internalPunct);
    }

    return cleaned;
}

std::vector<std::string> TextProcessor::tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    if (text.empty()) {
        return tokens;
    }

    size_t i = 0;
    size_t n = text.size();
    std::string curToken;

    while (i < n) {
        if ((text[i] == 'c' || text[i] == 'C') && (i + 2 < n) && text[i+1] == '+' && text[i+2] == '+') {
            bool validStart = (i == 0 || !std::isalnum(static_cast<unsigned char>(text[i - 1])));
            if (validStart) {
                if (!curToken.empty()) {
                    tokens.push_back(curToken);
                    curToken.clear();
                }
                tokens.push_back("c++");
                i += 3;
                continue;
            }
        }

        if ((text[i] == 'c' || text[i] == 'C') && (i + 1 < n) && text[i+1] == '#') {
            bool validStart = (i == 0 || !std::isalnum(static_cast<unsigned char>(text[i - 1])));
            if (validStart) {
                if (!curToken.empty()) {
                    tokens.push_back(curToken);
                    curToken.clear();
                }
                tokens.push_back("c#");
                i += 2;
                continue;
            }
        }

        char c = text[i];
        if (std::isalnum(static_cast<unsigned char>(c))) {
            curToken += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            i++;
        } else {

            if (!curToken.empty()) {
                tokens.push_back(curToken);
                curToken.clear();
            }
            i++;
        }
    }

    if (!curToken.empty()) {
        tokens.push_back(curToken);
    }

    return tokens;
}

std::string TextProcessor::stemWord(const std::string& word) {
    if (word.empty()) {
        return "";
    }

    if (isSpecialTerm(word)) {
        return word;
    }

    static const std::unordered_set<std::string> protectedWords = {
        "class", "business", "process", "analysis", "address", "access",
        "series", "species", "status", "this", "basis", "crisis", "as",
        "is", "us", "yes", "plus", "bus", "gas", "loss", "pass", "mass",
        "across", "less", "always", "bias", "canvas", "chaos", "lens",
        "focus", "radius", "corpus", "data", "schema", "media", "string",
        "bring", "spring", "ring", "sing", "wing", "king"
    };

    if (protectedWords.find(word) != protectedWords.end()) {
        return word;
    }

    if (word.size() <= 3) {
        return word;
    }

    if (word.size() >= 5 && word.substr(word.size() - 3) == "ies") {
        std::string prefix = word.substr(0, word.size() - 3);
        if (prefix.size() >= 3) {
            return prefix + "y";
        }
    }

    if (word.size() >= 5 && word.substr(word.size() - 3) == "ing") {
        if (word == "studying") {
            return "study";
        }

        std::string prefix = word.substr(0, word.size() - 3);
        if (prefix.size() >= 3) {
            size_t pLen = prefix.size();
            if (pLen >= 3 && prefix[pLen - 1] == prefix[pLen - 2]) {
                char lastChar = prefix[pLen - 1];
                if (lastChar != 's' && lastChar != 'l' && lastChar != 'f') {
                    prefix.pop_back();
                }
            }
            return prefix;
        }
    }

    if (word.size() >= 5 && word.substr(word.size() - 3) == "ied") {
        std::string prefix = word.substr(0, word.size() - 3);
        if (prefix.size() >= 3) {
            return prefix + "y";
        }
    }

    if (word.size() >= 5 && word.substr(word.size() - 2) == "ed") {
        std::string prefix = word.substr(0, word.size() - 2);
        if (prefix.size() >= 3) {
  
            size_t pLen = prefix.size();
            if (pLen >= 3 && prefix[pLen - 1] == prefix[pLen - 2]) {
                char lastChar = prefix[pLen - 1];
                if (lastChar != 's' && lastChar != 'l' && lastChar != 'f') {
                    prefix.pop_back();
                }
            }
            return prefix;
        }
    }

    if (word.size() >= 5 && word.substr(word.size() - 2) == "es") {
        std::string stemCandidate = word.substr(0, word.size() - 2);
        size_t cLen = stemCandidate.size();
        if (cLen >= 3) {
            char endChar = stemCandidate.back();
            if (endChar == 'x' || endChar == 'z' || endChar == 's' ||
                (cLen >= 2 && stemCandidate.substr(cLen - 2) == "ch") ||
                (cLen >= 2 && stemCandidate.substr(cLen - 2) == "sh")) {
                return stemCandidate;
            }
        }
    }

    if (word.size() >= 4 && word.back() == 's') {
        if (word.size() >= 2) {
            std::string ending2 = word.substr(word.size() - 2);
            if (ending2 == "ss" || ending2 == "is" || ending2 == "us") {
                return word;
            }
        }
        std::string prefix = word.substr(0, word.size() - 1);
        if (prefix.size() >= 3) {
            return prefix;
        }
    }

    return word;
}

bool TextProcessor::isStopWord(const std::string& word) {
    if (isSpecialTerm(word)) {
        return false;
    }

    static const std::unordered_set<std::string> stopWords = {
        "the", "is", "am", "are", "was", "were", "a", "an", "of", "in",
        "on", "at", "to", "for", "from", "and", "or", "but", "with", "as",
        "by", "be", "been", "being", "that", "this", "these", "those",
        "it", "its", "they", "them", "their", "theirs", "he", "him", "his",
        "she", "her", "hers", "which", "who", "whom", "whose", "if", "then",
        "else", "when", "where", "why", "how", "all", "any", "both", "each",
        "few", "more", "most", "other", "some", "such", "no", "nor", "not",
        "only", "own", "same", "so", "than", "too", "very", "can", "will",
        "just", "should", "now"
    };

    return stopWords.find(word) != stopWords.end();
}

bool TextProcessor::isPronoun(const std::string& word) {
    static const std::unordered_set<std::string> pronouns = {
        "it", "its", "this", "that", "these", "those", "they", "them",
        "their", "theirs", "he", "him", "his", "she", "her", "hers"
    };
    return pronouns.find(word) != pronouns.end();
}

bool TextProcessor::isSingularPronoun(const std::string& word) {
    static const std::unordered_set<std::string> singularPronouns = {
        "it", "its", "this", "that", "he", "him", "his", "she", "her", "hers"
    };
    return singularPronouns.find(word) != singularPronouns.end();
}

bool TextProcessor::isPluralPronoun(const std::string& word) {
    static const std::unordered_set<std::string> pluralPronouns = {
        "they", "them", "their", "theirs", "these", "those"
    };
    return pluralPronouns.find(word) != pluralPronouns.end();
}
