#ifndef TEXT_PROCESSOR_H
#define TEXT_PROCESSOR_H

#include <string>
#include <vector>

class TextProcessor {
public:
    TextProcessor() = default;

    static std::string toLowerCase(const std::string& text);
    static bool isSpecialTerm(const std::string& term);
    static std::string cleanToken(const std::string& rawToken);
    static std::vector<std::string> tokenize(const std::string& text);

    static std::string stemWord(const std::string& word);

    static bool isStopWord(const std::string& word);
    static bool isPronoun(const std::string& word);
    static bool isSingularPronoun(const std::string& word);
    static bool isPluralPronoun(const std::string& word);
};

#endif
