#ifndef INVERTED_INDEX_H
#define INVERTED_INDEX_H

#include "PronounResolver.h"
#include <string>
#include <vector>
#include <map>

struct DocumentMatch {
    int docId;                    
    int totalOccurrences = 0;   
    std::vector<int> lineNumbers; 
    std::vector<int> positions;   
};

class InvertedIndex {
private:
    std::map<std::string, std::vector<DocumentMatch>> index;
    std::map<std::string, std::vector<ContextualMatch>> contextualIndex;
    std::vector<int> docTotalWords; 

public:
    InvertedIndex() = default;

    void addDocument(int docId,
                     const std::vector<std::string>& documentLines,
                     const std::vector<std::string>& tokens,
                     const std::vector<int>& tokenLines);

    void addDocument(int docId, const std::vector<std::string>& documentLines);

    std::vector<DocumentMatch> getMatches(const std::string& word) const;
    std::vector<ContextualMatch> getContextualMatches(const std::string& word) const;
    std::vector<int> getDocumentIds(const std::string& word) const;
    bool hasWord(const std::string& word) const;
    size_t getVocabularySize() const;

    const std::map<std::string, std::vector<DocumentMatch>>& getIndexMap() const;
    const std::map<std::string, std::vector<ContextualMatch>>& getContextualIndexMap() const;

    int getDocumentWordCount(int docId) const;
    void clear();
};

#endif
