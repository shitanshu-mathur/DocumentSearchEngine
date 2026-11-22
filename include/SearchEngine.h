#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include "DocumentReader.h"
#include "TextProcessor.h"
#include "InvertedIndex.h"
#include "DocumentViewer.h"
#include <string>
#include <vector>

struct SearchResult {
    int docId = -1;
    std::string filename;
    double relevanceScore = 0.0;
    int literalFrequency = 0;
    int contextualReferences = 0;
    std::vector<int> matchingLines;
    std::string snippet;
};

class SearchEngine {
private:
    DocumentReader reader;
    InvertedIndex index;
    bool isInitialized;

    static std::string trim(const std::string& str);
    int countExactPhraseInDoc(int docId,
                              const std::vector<std::string>& phraseTokens,
                              std::vector<int>& phraseMatchingLines) const;
    double calculateProximityScore(int docId, const std::vector<std::string>& terms) const;
    bool checkTitleMatch(int docId, const std::vector<std::string>& terms) const;
    void handleSearchResults(const std::string& query,
                             const std::vector<SearchResult>& results,
                             bool showPrecedenceNotice = false) const;

public:
    SearchEngine();

    bool initialize(const std::vector<std::string>& filepaths);
    void search(const std::string& query) const;
    void exportResultsToJson(const std::string& query,
                             const std::vector<SearchResult>& results,
                             const std::string& outputPath = "output/search_results.json") const;
    void displayIndexPaginated() const;
    void displayIndex() const;
    void exportIndexToFile(const std::string& outputPath) const;
    void displayDocuments() const;
    void run();
};

#endif
