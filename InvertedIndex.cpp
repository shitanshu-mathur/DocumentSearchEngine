#include "InvertedIndex.h"
#include "TextProcessor.h"
#include <map>

void InvertedIndex::addDocument(int docId,
                                const std::vector<std::string>& documentLines,
                                const std::vector<std::string>& tokens,
                                const std::vector<int>& tokenLines) {
    if (docId >= static_cast<int>(docTotalWords.size())) {
        docTotalWords.resize(docId + 1, 0);
    }
    docTotalWords[docId] = static_cast<int>(tokens.size());

    struct WordStats {
        int totalOccurrences = 0;
        std::vector<int> lineNumbers;
        std::vector<int> positions;
    };
    std::map<std::string, WordStats> docWordStats;

    for (size_t i = 0; i < tokens.size(); ++i) {
        const std::string& token = tokens[i];
        int lineNum = (i < tokenLines.size()) ? tokenLines[i] : 1;
        int pos = static_cast<int>(i);

        if (TextProcessor::isStopWord(token)) {
            continue;
        }

        std::string root = TextProcessor::stemWord(token);
        if (root.empty()) {
            continue;
        }

        WordStats& stats = docWordStats[root];
        stats.totalOccurrences++;
        stats.positions.push_back(pos);
        if (stats.lineNumbers.empty() || stats.lineNumbers.back() != lineNum) {
            stats.lineNumbers.push_back(lineNum);
        }
    }

    for (const auto& entry : docWordStats) {
        const std::string& root = entry.first;
        const WordStats& stats = entry.second;

        DocumentMatch match;
        match.docId = docId;
        match.totalOccurrences = stats.totalOccurrences;
        match.lineNumbers = stats.lineNumbers;
        match.positions = stats.positions;

        index[root].push_back(match);
    }

    PronounResolver::resolveDocument(docId, documentLines, contextualIndex);
}

void InvertedIndex::addDocument(int docId, const std::vector<std::string>& documentLines) {
    std::vector<std::string> tokens;
    std::vector<int> tokenLines;

    for (size_t i = 0; i < documentLines.size(); ++i) {
        int lineNum = static_cast<int>(i + 1);
        std::vector<std::string> lineTokens = TextProcessor::tokenize(documentLines[i]);
        for (const auto& t : lineTokens) {
            tokens.push_back(t);
            tokenLines.push_back(lineNum);
        }
    }

    addDocument(docId, documentLines, tokens, tokenLines);
}

std::vector<DocumentMatch> InvertedIndex::getMatches(const std::string& word) const {
    std::string cleaned = TextProcessor::cleanToken(word);
    std::string root = TextProcessor::stemWord(cleaned);
    auto it = index.find(root);
    if (it != index.end()) {
        return it->second;
    }
    if (root != cleaned) {
        auto itClean = index.find(cleaned);
        if (itClean != index.end()) {
            return itClean->second;
        }
    }
    return {};
}

std::vector<ContextualMatch> InvertedIndex::getContextualMatches(const std::string& word) const {
    std::string cleaned = TextProcessor::cleanToken(word);
    std::string root = TextProcessor::stemWord(cleaned);
    auto it = contextualIndex.find(root);
    if (it != contextualIndex.end()) {
        return it->second;
    }
    return {};
}

std::vector<int> InvertedIndex::getDocumentIds(const std::string& word) const {
    std::vector<DocumentMatch> matches = getMatches(word);
    std::vector<int> docIds;
    for (const auto& match : matches) {
        docIds.push_back(match.docId);
    }
    return docIds;
}

bool InvertedIndex::hasWord(const std::string& word) const {
    std::string cleaned = TextProcessor::cleanToken(word);
    std::string root = TextProcessor::stemWord(cleaned);
    return index.find(root) != index.end() || index.find(cleaned) != index.end();
}

size_t InvertedIndex::getVocabularySize() const {
    return index.size();
}

const std::map<std::string, std::vector<DocumentMatch>>& InvertedIndex::getIndexMap() const {
    return index;
}

const std::map<std::string, std::vector<ContextualMatch>>& InvertedIndex::getContextualIndexMap() const {
    return contextualIndex;
}

int InvertedIndex::getDocumentWordCount(int docId) const {
    if (docId >= 0 && docId < static_cast<int>(docTotalWords.size())) {
        return docTotalWords[docId];
    }
    return 0;
}

void InvertedIndex::clear() {
    index.clear();
    contextualIndex.clear();
    docTotalWords.clear();
}
