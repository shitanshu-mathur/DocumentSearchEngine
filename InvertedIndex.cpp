#include "InvertedIndex.h"
#include "TextProcessor.h"
#include <map>

void InvertedIndex::addDocument(int docId, const std::vector<std::string>& documentLines) {
    // Map each unique word in this document to the line numbers where it occurs
    std::map<std::string, std::vector<int>> wordToLines;

    for (size_t i = 0; i < documentLines.size(); ++i) {
        int lineNum = static_cast<int>(i + 1); // 1-based line number
        std::vector<std::string> tokens = TextProcessor::tokenize(documentLines[i]);

        for (const std::string& word : tokens) {
            std::vector<int>& lines = wordToLines[word];
            // Avoid duplicate line numbers if a word appears multiple times on the same line
            if (lines.empty() || lines.back() != lineNum) {
                lines.push_back(lineNum);
            }
        }
    }

    // Insert into the inverted index
    for (const auto& entry : wordToLines) {
        const std::string& word = entry.first;
        const std::vector<int>& lines = entry.second;

        DocumentMatch match;
        match.docId = docId;
        match.lineNumbers = lines;

        index[word].push_back(match);
    }
}

std::vector<DocumentMatch> InvertedIndex::getMatches(const std::string& word) const {
    auto it = index.find(word);
    if (it != index.end()) {
        return it->second;
    }
    return {};
}

std::vector<int> InvertedIndex::getDocumentIds(const std::string& word) const {
    auto it = index.find(word);
    if (it == index.end()) {
        return {};
    }
    std::vector<int> docIds;
    for (const auto& match : it->second) {
        docIds.push_back(match.docId);
    }
    return docIds;
}

bool InvertedIndex::hasWord(const std::string& word) const {
    return index.find(word) != index.end();
}

size_t InvertedIndex::getVocabularySize() const {
    return index.size();
}

const std::map<std::string, std::vector<DocumentMatch>>& InvertedIndex::getIndexMap() const {
    return index;
}

void InvertedIndex::clear() {
    index.clear();
}
