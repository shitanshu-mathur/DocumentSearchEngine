#include "PronounResolver.h"
#include "TextProcessor.h"
#include <sstream>
#include <algorithm>

namespace {

void recordContextualMatch(const std::string& term,
                           int docId,
                           int lineNum,
                           std::map<std::string, std::vector<ContextualMatch>>& contextualIndex) {
    if (term.empty()) {
        return;
    }

    std::vector<ContextualMatch>& matches = contextualIndex[term];
    for (auto& match : matches) {
        if (match.docId == docId) {
            match.referenceCount++;
            if (match.lineNumbers.empty() || match.lineNumbers.back() != lineNum) {
                match.lineNumbers.push_back(lineNum);
            }
            return;
        }
    }

    ContextualMatch newMatch;
    newMatch.docId = docId;
    newMatch.referenceCount = 1;
    newMatch.lineNumbers.push_back(lineNum);
    matches.push_back(newMatch);
}

bool isSingularVerb(const std::string& word) {
    return (word == "is" || word == "was" || word == "has" ||
            word == "represents" || word == "provides" || word == "stores" ||
            word == "defines" || word == "serves");
}

bool isPluralVerb(const std::string& word) {
    return (word == "are" || word == "were" || word == "have" ||
            word == "represent" || word == "provide" || word == "store" ||
            word == "define" || word == "serve");
}

}

void PronounResolver::resolveDocument(int docId,
                                      const std::vector<std::string>& lines,
                                      std::map<std::string, std::vector<ContextualMatch>>& contextualIndex) {
    std::string candidateSingular = "";
    std::vector<std::string> candidatePlural;

    for (size_t lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
        int lineNum = static_cast<int>(lineIdx + 1);
        const std::string& lineText = lines[lineIdx];

        if (lineText.empty()) {
            candidateSingular.clear();
            candidatePlural.clear();
            continue;
        }

        std::vector<std::string> sentences;
        std::string currentSentence;
        for (char ch : lineText) {
            currentSentence += ch;
            if (ch == '.' || ch == '!' || ch == '?') {
                sentences.push_back(currentSentence);
                currentSentence.clear();
            }
        }
        if (!currentSentence.empty()) {
            sentences.push_back(currentSentence);
        }

        for (const auto& sentence : sentences) {
            std::vector<std::string> tokens = TextProcessor::tokenize(sentence);
            if (tokens.empty()) {
                continue;
            }

            const std::string& firstWord = tokens[0];

            if (TextProcessor::isPronoun(firstWord)) {
                if (TextProcessor::isSingularPronoun(firstWord)) {
                    if (!candidateSingular.empty() && candidatePlural.empty()) {
                        recordContextualMatch(candidateSingular, docId, lineNum, contextualIndex);
                    }
                } else if (TextProcessor::isPluralPronoun(firstWord)) {
                    if (candidateSingular.empty() && candidatePlural.size() >= 2) {
                        for (const auto& entity : candidatePlural) {
                            recordContextualMatch(entity, docId, lineNum, contextualIndex);
                        }
                    }
                }
                candidateSingular.clear();
                candidatePlural.clear();
            } else {
                candidateSingular.clear();
                candidatePlural.clear();
                if (tokens.size() >= 4 && tokens[1] == "and" && isPluralVerb(tokens[3])) {
                    if (!TextProcessor::isStopWord(tokens[0]) && !TextProcessor::isStopWord(tokens[2])) {
                        std::string entity1 = TextProcessor::stemWord(tokens[0]);
                        std::string entity2 = TextProcessor::stemWord(tokens[2]);
                        if (!entity1.empty() && !entity2.empty() && entity1 != entity2) {
                            candidatePlural.push_back(entity1);
                            candidatePlural.push_back(entity2);
                        }
                    }
                }
                else if (tokens.size() >= 2 && isSingularVerb(tokens[1])) {
                    if (!TextProcessor::isStopWord(tokens[0]) && !TextProcessor::isPronoun(tokens[0])) {
                        std::string entity = TextProcessor::stemWord(tokens[0]);
                        if (!entity.empty()) {
                            candidateSingular = entity;
                        }
                    }
                }
            }
        }
    }
}
