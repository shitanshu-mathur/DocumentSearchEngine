#include "SearchEngine.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <set>
#include <cmath>
#include <sstream>

SearchEngine::SearchEngine() : isInitialized(false) {}

std::string SearchEngine::trim(const std::string& str) {
    size_t first = 0;
    while (first < str.size() && std::isspace(static_cast<unsigned char>(str[first]))) {
        first++;
    }
    if (first >= str.size()) {
        return "";
    }
    size_t last = str.size() - 1;
    while (last > first && std::isspace(static_cast<unsigned char>(str[last]))) {
        last--;
    }
    return str.substr(first, last - first + 1);
}

bool SearchEngine::initialize(const std::vector<std::string>& filepaths) {
    std::cout << "\n==================================================\n";
    std::cout << "           DOCUMENT LOADING & INDEXING            \n";
    std::cout << "==================================================\n";

    reader.clear();
    index.clear();

    int loadedCount = reader.loadDocuments(filepaths);

    if (loadedCount == 0) {
        std::cerr << "\n[ERROR] No documents were successfully loaded!\n"
                  << "Please make sure the 'documents/' folder exists and contains valid .txt files.\n";
        isInitialized = false;
        return false;
    }

    std::cout << "\nBuilding Inverted Index from loaded documents...\n";
    for (const auto& doc : reader.getDocuments()) {
        index.addDocument(doc.id, doc.lines, doc.tokens, doc.tokenLines);
    }

    isInitialized = true;
    std::cout << "[SUCCESS] Indexed " << loadedCount << " document(s) with "
              << index.getVocabularySize() << " unique words in vocabulary.\n";

    exportIndexToFile("output/inverted_index.txt");
    std::cout << "[SUCCESS] Inverted Index exported to: \"output/inverted_index.txt\"\n";
    std::cout << "==================================================\n\n";
    return true;
}

int SearchEngine::countExactPhraseInDoc(int docId,
                                       const std::vector<std::string>& phraseTokens,
                                       std::vector<int>& phraseMatchingLines) const {
    const Document* doc = reader.getDocument(docId);
    if (!doc || phraseTokens.empty() || doc->tokens.size() < phraseTokens.size()) {
        return 0;
    }

    std::vector<std::string> cleanPhrase;
    for (const auto& pt : phraseTokens) {
        std::string cp = TextProcessor::cleanToken(pt);
        if (!cp.empty()) {
            cleanPhrase.push_back(cp);
        }
    }

    if (cleanPhrase.empty() || doc->tokens.size() < cleanPhrase.size()) {
        return 0;
    }

    int occurrenceCount = 0;
    size_t pLen = cleanPhrase.size();
    size_t docLen = doc->tokens.size();

    for (size_t i = 0; i + pLen <= docLen; ++i) {
        bool match = true;
        for (size_t j = 0; j < pLen; ++j) {
            if (doc->tokens[i + j] != cleanPhrase[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            occurrenceCount++;
            int lineNum = (i < doc->tokenLines.size()) ? doc->tokenLines[i] : 1;
            if (phraseMatchingLines.empty() || phraseMatchingLines.back() != lineNum) {
                if (std::find(phraseMatchingLines.begin(), phraseMatchingLines.end(), lineNum) == phraseMatchingLines.end()) {
                    phraseMatchingLines.push_back(lineNum);
                }
            }
        }
    }

    return occurrenceCount;
}

double SearchEngine::calculateProximityScore(int docId, const std::vector<std::string>& terms) const {
    if (terms.size() < 2) {
        return 0.0;
    }

    std::vector<std::vector<int>> termPositions;
    for (const auto& term : terms) {
        std::vector<DocumentMatch> matches = index.getMatches(term);
        for (const auto& m : matches) {
            if (m.docId == docId && !m.positions.empty()) {
                termPositions.push_back(m.positions);
                break;
            }
        }
    }

    if (termPositions.size() < 2) {
        return 0.0;
    }

    int minDistance = 9999999;
    for (size_t i = 0; i < termPositions.size(); ++i) {
        for (size_t j = i + 1; j < termPositions.size(); ++j) {
            const auto& posA = termPositions[i];
            const auto& posB = termPositions[j];

            for (int pA : posA) {
                auto it = std::lower_bound(posB.begin(), posB.end(), pA);
                if (it != posB.end()) {
                    int dist = std::abs(*it - pA);
                    if (dist < minDistance) {
                        minDistance = dist;
                    }
                }
                if (it != posB.begin()) {
                    int dist = std::abs(*(it - 1) - pA);
                    if (dist < minDistance) {
                        minDistance = dist;
                    }
                }
            }
        }
    }

    if (minDistance <= 0 || minDistance >= 9999999) {
        return 0.0;
    }

    return 1.0 / static_cast<double>(minDistance);
}

bool SearchEngine::checkTitleMatch(int docId, const std::vector<std::string>& terms) const {
    std::string filename = reader.getFilename(docId);
    std::vector<std::string> titleTokens = TextProcessor::tokenize(filename);

    for (const auto& t : terms) {
        std::string stemT = TextProcessor::stemWord(TextProcessor::cleanToken(t));
        for (const auto& titleWord : titleTokens) {
            std::string stemTitle = TextProcessor::stemWord(TextProcessor::cleanToken(titleWord));
            if (!stemT.empty() && stemT == stemTitle) {
                return true;
            }
        }
    }
    return false;
}

void SearchEngine::search(const std::string& rawQuery) const {
    if (!isInitialized) {
        std::cerr << "\n[Error] Search engine has not been initialized with documents yet.\n\n";
        return;
    }

    std::string trimmedQuery = trim(rawQuery);

    if (trimmedQuery.empty()) {
        std::cout << "\n[Notice] Empty search query! Please enter a valid word or phrase.\n\n";
        return;
    }
    bool isPhraseQuery = false;
    std::string phraseContent;
    if (trimmedQuery.front() == '"' && trimmedQuery.back() == '"' && trimmedQuery.size() >= 2) {
        isPhraseQuery = true;
        phraseContent = trimmedQuery.substr(1, trimmedQuery.size() - 2);
    } else if (trimmedQuery.find('"') != std::string::npos) {
        size_t firstQuote = trimmedQuery.find('"');
        size_t lastQuote = trimmedQuery.rfind('"');
        if (firstQuote != lastQuote) {
            isPhraseQuery = true;
            phraseContent = trimmedQuery.substr(firstQuote + 1, lastQuote - firstQuote - 1);
        }
    }

    if (isPhraseQuery) {
        std::vector<std::string> phraseTokens = TextProcessor::tokenize(phraseContent);
        if (phraseTokens.empty()) {
            std::cout << "\n========================================\n";
            std::cout << "SEARCH RESULTS\n";
            std::cout << "========================================\n\n";
            std::cout << "Query: " << trimmedQuery << "\n\n";
            std::cout << "No results found.\n\n";
            std::cout << "========================================\n\n";
            return;
        }

        std::vector<SearchResult> results;
        for (const auto& doc : reader.getDocuments()) {
            std::vector<int> matchingLines;
            int phraseOccurrences = countExactPhraseInDoc(doc.id, phraseTokens, matchingLines);
            if (phraseOccurrences > 0) {
                SearchResult r;
                r.docId = doc.id;
                r.filename = reader.getFilename(doc.id);
                r.matchingLines = matchingLines;

                r.literalFrequency = phraseOccurrences;

                r.contextualReferences = 0;
                for (const auto& pt : phraseTokens) {
                    if (!TextProcessor::isStopWord(pt)) {
                        for (const auto& cm : index.getContextualMatches(pt)) {
                            if (cm.docId == doc.id) {
                                r.contextualReferences += cm.referenceCount;
                            }
                        }
                    }
                }

                int totalDocWords = index.getDocumentWordCount(doc.id);
                double normTF = (totalDocWords > 0) ? (static_cast<double>(r.literalFrequency) / totalDocWords) : 0.0;
                double keywordCoverage = 1.0;
                double exactPhraseBonus = (phraseTokens.size() >= 2) ? 1.0 : 0.0;
                double titleMatchBonus = checkTitleMatch(doc.id, phraseTokens) ? 1.0 : 0.0;
                double proximity = 1.0; 
                double contextualBonus = std::min(2.0, r.contextualReferences * 0.5);

                r.relevanceScore = 40.0 * normTF + 20.0 * keywordCoverage +
                                   20.0 * exactPhraseBonus + 10.0 * titleMatchBonus +
                                   10.0 * proximity + contextualBonus;

                if (!matchingLines.empty()) {
                    r.snippet = trim(reader.getLine(doc.id, matchingLines.front()));
                }
                results.push_back(r);
            }
        }

        std::sort(results.begin(), results.end(), [](const SearchResult& a, const SearchResult& b) {
            if (std::abs(a.relevanceScore - b.relevanceScore) > 1e-6) {
                return a.relevanceScore > b.relevanceScore;
            }
            return a.filename < b.filename;
        });

        handleSearchResults(trimmedQuery, results, false);
        return;
    }

    std::stringstream ss(trimmedQuery);
    std::string rawToken;
    std::vector<std::string> queryTokens;
    while (ss >> rawToken) {
        queryTokens.push_back(rawToken);
    }

    if (queryTokens.empty()) {
        std::cout << "\n========================================\n";
        std::cout << "SEARCH RESULTS\n";
        std::cout << "========================================\n\n";
        std::cout << "Query: " << trimmedQuery << "\n\n";
        std::cout << "No results found.\n\n";
        std::cout << "========================================\n\n";
        return;
    }

    auto isOp = [](const std::string& s) {
        std::string l = TextProcessor::toLowerCase(s);
        return (l == "and" || l == "or");
    };
    if (isOp(queryTokens.front()) || isOp(queryTokens.back())) {
        std::cout << "\n========================================\n";
        std::cout << "SEARCH RESULTS\n";
        std::cout << "========================================\n\n";
        std::cout << "Query: " << trimmedQuery << "\n\n";
        std::cout << "[Notice] Incomplete or invalid Boolean expression (missing term before or after operator).\n";
        std::cout << "Please provide valid search terms for all operators.\n\n";
        std::cout << "No results found.\n\n";
        std::cout << "========================================\n\n";
        return;
    }

    for (size_t i = 0; i + 1 < queryTokens.size(); ++i) {
        if (isOp(queryTokens[i]) && isOp(queryTokens[i + 1])) {
            std::cout << "\n========================================\n";
            std::cout << "SEARCH RESULTS\n";
            std::cout << "========================================\n\n";
            std::cout << "Query: " << trimmedQuery << "\n\n";
            std::cout << "[Notice] Incomplete or invalid Boolean expression (consecutive operators \"" 
                      << queryTokens[i] << " " << queryTokens[i + 1] << "\").\n";
            std::cout << "Please provide valid search terms between operators.\n\n";
            std::cout << "No results found.\n\n";
            std::cout << "========================================\n\n";
            return;
        }
    }

    std::vector<std::vector<std::string>> orGroups;
    std::vector<std::string> currentAndGroup;

    bool hasAnd = false;
    bool hasOr = false;

    for (const auto& tokenStr : queryTokens) {
        std::string lowerT = TextProcessor::toLowerCase(tokenStr);
        if (lowerT == "or") {
            hasOr = true;
            if (!currentAndGroup.empty()) {
                orGroups.push_back(currentAndGroup);
                currentAndGroup.clear();
            }
        } else if (lowerT == "and") {
            hasAnd = true;
        } else {
            std::vector<std::string> subTokens = TextProcessor::tokenize(tokenStr);
            for (const auto& st : subTokens) {
                currentAndGroup.push_back(st);
            }
        }
    }
    if (!currentAndGroup.empty()) {
        orGroups.push_back(currentAndGroup);
    }

    if (orGroups.empty()) {
        std::cout << "\n========================================\n";
        std::cout << "SEARCH RESULTS\n";
        std::cout << "========================================\n\n";
        std::cout << "Query: " << trimmedQuery << "\n\n";
        std::cout << "No results found.\n\n";
        std::cout << "========================================\n\n";
        return;
    }

    std::vector<std::string> searchableTerms;
    std::set<std::string> seenSearchable;
    for (const auto& grp : orGroups) {
        for (const auto& t : grp) {
            if (!TextProcessor::isStopWord(t)) {
                if (seenSearchable.insert(t).second) {
                    searchableTerms.push_back(t);
                }
            }
        }
    }

    if (searchableTerms.empty()) {
        std::cout << "\n========================================\n";
        std::cout << "SEARCH RESULTS\n";
        std::cout << "========================================\n\n";
        std::cout << "Query: " << trimmedQuery << "\n\n";
        std::cout << "[Notice] Query contains only common stop words.\n";
        std::cout << "Stop words do not independently create search results.\n\n";
        std::cout << "No results found.\n\n";
        std::cout << "========================================\n\n";
        return;
    }

    std::set<int> candidateDocIds;

    for (const auto& grp : orGroups) {
        std::vector<std::string> grpSearchable;
        for (const auto& t : grp) {
            if (!TextProcessor::isStopWord(t)) {
                grpSearchable.push_back(t);
            }
        }
        if (grpSearchable.empty()) {
            continue;
        }

        std::set<int> grpDocs;
        bool firstTerm = true;
        for (const auto& term : grpSearchable) {
            std::set<int> termDocs;
            for (int id : index.getDocumentIds(term)) {
                termDocs.insert(id);
            }
            for (const auto& cm : index.getContextualMatches(term)) {
                termDocs.insert(cm.docId);
            }

            if (firstTerm) {
                grpDocs = termDocs;
                firstTerm = false;
            } else {
                std::set<int> intersection;
                for (int id : grpDocs) {
                    if (termDocs.find(id) != termDocs.end()) {
                        intersection.insert(id);
                    }
                }
                grpDocs = intersection;
                if (grpDocs.empty()) {
                    break;
                }
            }
        }
        for (int id : grpDocs) {
            candidateDocIds.insert(id);
        }
    }

    std::vector<SearchResult> results;

    for (int docId : candidateDocIds) {
        SearchResult r;
        r.docId = docId;
        r.filename = reader.getFilename(docId);

        int totalDocWords = index.getDocumentWordCount(docId);
        int literalFreq = 0;
        int distinctFound = 0;
        std::set<int> allLines;

        for (const auto& term : searchableTerms) {
            std::vector<DocumentMatch> matches = index.getMatches(term);
            bool termInDoc = false;
            for (const auto& m : matches) {
                if (m.docId == docId) {
                    literalFreq += m.totalOccurrences;
                    termInDoc = true;
                    for (int ln : m.lineNumbers) {
                        allLines.insert(ln);
                    }
                    break;
                }
            }
            if (termInDoc) {
                distinctFound++;
            }
        }

        r.literalFrequency = literalFreq;

        // Contextual references
        r.contextualReferences = 0;
        for (const auto& term : searchableTerms) {
            for (const auto& cm : index.getContextualMatches(term)) {
                if (cm.docId == docId) {
                    r.contextualReferences += cm.referenceCount;
                    for (int ln : cm.lineNumbers) {
                        allLines.insert(ln);
                    }
                }
            }
        }

        for (int ln : allLines) {
            r.matchingLines.push_back(ln);
        }
        double normTF = (totalDocWords > 0) ? (static_cast<double>(literalFreq) / totalDocWords) : 0.0;
        double keywordCoverage = (!searchableTerms.empty()) ? (static_cast<double>(distinctFound) / searchableTerms.size()) : 0.0;

        double exactPhraseBonus = 0.0;
        if (searchableTerms.size() >= 2 && !hasOr) {
            std::vector<int> dummyLines;
            if (countExactPhraseInDoc(docId, searchableTerms, dummyLines) > 0) {
                exactPhraseBonus = 1.0;
            }
        }

        double titleMatchBonus = checkTitleMatch(docId, searchableTerms) ? 1.0 : 0.0;
        double proximity = calculateProximityScore(docId, searchableTerms);
        double contextualBonus = std::min(2.0, r.contextualReferences * 0.5);

        r.relevanceScore = 40.0 * normTF + 20.0 * keywordCoverage +
                           20.0 * exactPhraseBonus + 10.0 * titleMatchBonus +
                           10.0 * proximity + contextualBonus;

        if (!r.matchingLines.empty()) {
            r.snippet = trim(reader.getLine(docId, r.matchingLines.front()));
        }

        results.push_back(r);
    }

    std::sort(results.begin(), results.end(), [](const SearchResult& a, const SearchResult& b) {
        if (std::abs(a.relevanceScore - b.relevanceScore) > 1e-6) {
            return a.relevanceScore > b.relevanceScore;
        }
        return a.filename < b.filename;
    });

    handleSearchResults(trimmedQuery, results, hasAnd && hasOr);
}

void SearchEngine::handleSearchResults(const std::string& query,
                                      const std::vector<SearchResult>& results,
                                      bool showPrecedenceNotice) const {
    exportResultsToJson(query, results);

    if (results.empty()) {
        std::cout << "\n========================================\n";
        std::cout << "SEARCH RESULTS\n";
        std::cout << "========================================\n\n";
        std::cout << "Query: " << query << "\n\n";
        std::cout << "No results found.\n\n";
        std::cout << "========================================\n\n";
        return;
    }

    while (true) {
        std::cout << "\n========================================\n";
        std::cout << "SEARCH RESULTS\n";
        std::cout << "========================================\n\n";
        std::cout << "Query: " << query << "\n";
        if (showPrecedenceNotice) {
            std::cout << "[Notice] Evaluating query with Boolean AND-before-OR precedence.\n";
        }
        std::cout << "\n";

        for (size_t i = 0; i < results.size(); ++i) {
            const auto& r = results[i];
            std::cout << "Rank " << (i + 1) << ": " << r.filename << "\n";
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "Relevance Score: " << r.relevanceScore << "\n";
            std::cout << "Literal Keyword Frequency: " << r.literalFrequency << "\n";
            std::cout << "Contextual References: " << r.contextualReferences << "\n";
            std::cout << "Matching Lines: ";
            size_t maxLines = 10;
            for (size_t l = 0; l < r.matchingLines.size() && l < maxLines; ++l) {
                std::cout << r.matchingLines[l];
                if (l + 1 < r.matchingLines.size() && l + 1 < maxLines) {
                    std::cout << ", ";
                }
            }
            if (r.matchingLines.size() > maxLines) {
                std::cout << ", ... (" << r.matchingLines.size() << " total)";
            }
            std::cout << "\n";
            std::cout << "Snippet: " << r.snippet << "\n\n";
        }
        std::cout << "========================================\n";

        std::cout << "Enter the rank number to open a document (0 to return): ";
        std::string input;
        if (!std::getline(std::cin, input)) {
            break;
        }

        std::string choice = trim(input);
        if (choice == "0" || choice == "q" || choice == "Q" || choice == "return" || choice == "exit") {
            std::cout << "\nReturning to Main Menu...\n\n";
            break;
        }

        if (choice.empty()) {
            std::cout << "\n[Notice] Input was empty. Enter a rank number (1-" 
                      << results.size() << ") or 0 to return.\n";
            continue;
        }

        bool isNumber = true;
        for (char c : choice) {
            if (!std::isdigit(static_cast<unsigned char>(c))) {
                isNumber = false;
                break;
            }
        }

        if (!isNumber) {
            std::cout << "\n[Invalid Selection] \"" << choice 
                      << "\" is not a valid number. Please enter a rank number (1-" 
                      << results.size() << ") or 0 to return.\n";
            continue;
        }

        try {
            int rank = std::stoi(choice);
            if (rank >= 1 && rank <= static_cast<int>(results.size())) {
                const SearchResult& selected = results[rank - 1];
                std::cout << "\nOpening Rank " << rank << " (" << selected.filename << ")...\n";

                std::vector<std::string> queryTokens = TextProcessor::tokenize(query);
                std::vector<std::string> highlightKeywords;
                for (const auto& token : queryTokens) {
                    if (!TextProcessor::isStopWord(token)) {
                        highlightKeywords.push_back(token);
                    }
                }

                DocumentViewer::viewDocument(selected.filename, selected.matchingLines, highlightKeywords);
            } else {
                std::cout << "\n[Invalid Rank] Rank number " << rank 
                          << " is out of range. Please enter a rank number between 1 and " 
                          << results.size() << " (or 0 to return).\n";
            }
        } catch (...) {
            std::cout << "\n[Invalid Selection] Number too large. Please enter a rank between 1 and " 
                      << results.size() << " (or 0 to return).\n";
        }
    }
}

void SearchEngine::exportResultsToJson(const std::string& query,
                                      const std::vector<SearchResult>& results,
                                      const std::string& outputPath) const {
    std::ofstream outFile(outputPath);
    if (!outFile.is_open()) {
        std::cerr << "[WARNING] Could not open \"" << outputPath << "\" to export search results JSON.\n";
        return;
    }

    auto escapeJson = [](const std::string& str) -> std::string {
        std::string out;
        for (char c : str) {
            if (c == '"') out += "\\\"";
            else if (c == '\\') out += "\\\\";
            else if (c == '\b') out += "\\b";
            else if (c == '\f') out += "\\f";
            else if (c == '\n') out += "\\n";
            else if (c == '\r') out += "\\r";
            else if (c == '\t') out += "\\t";
            else if (static_cast<unsigned char>(c) < 0x20) {
                char buf[8];
                std::snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(c));
                out += buf;
            } else {
                out += c;
            }
        }
        return out;
    };

    outFile << "{\n";
    outFile << "  \"query\": \"" << escapeJson(query) << "\",\n";
    outFile << "  \"totalResults\": " << results.size() << ",\n";
    outFile << "  \"results\": [\n";

    for (size_t i = 0; i < results.size(); ++i) {
        const auto& r = results[i];
        outFile << "    {\n";
        outFile << "      \"rank\": " << (i + 1) << ",\n";
        outFile << "      \"docId\": " << r.docId << ",\n";
        outFile << "      \"filename\": \"" << escapeJson(r.filename) << "\",\n";
        outFile << "      \"relevanceScore\": " << std::fixed << std::setprecision(2) << r.relevanceScore << ",\n";
        outFile << "      \"literalFrequency\": " << r.literalFrequency << ",\n";
        outFile << "      \"contextualReferences\": " << r.contextualReferences << ",\n";
        outFile << "      \"snippet\": \"" << escapeJson(r.snippet) << "\",\n";
        outFile << "      \"matchingLines\": [";
        for (size_t l = 0; l < r.matchingLines.size(); ++l) {
            outFile << r.matchingLines[l];
            if (l + 1 < r.matchingLines.size()) {
                outFile << ", ";
            }
        }
        outFile << "]\n";
        outFile << "    }";
        if (i + 1 < results.size()) {
            outFile << ",";
        }
        outFile << "\n";
    }

    outFile << "  ]\n";
    outFile << "}\n";
    outFile.close();
}

void SearchEngine::displayIndexPaginated() const {
    if (!isInitialized) {
        std::cerr << "\n[Error] Search engine has not been initialized with documents yet.\n\n";
        return;
    }

    const auto& indexMap = index.getIndexMap();
    if (indexMap.empty()) {
        std::cout << "\nThe inverted index is currently empty.\n\n";
        return;
    }

    std::vector<std::string> vocabulary;
    vocabulary.reserve(indexMap.size());
    for (const auto& entry : indexMap) {
        vocabulary.push_back(entry.first);
    }

    const size_t pageSize = 20;
    size_t totalTerms = vocabulary.size();
    size_t totalPages = (totalTerms + pageSize - 1) / pageSize;
    size_t currentPage = 1;

    std::cout << "\n================================================================================\n";
    std::cout << "              INVERTED INDEX VIEWER (Paginated Red-Black Tree)                  \n";
    std::cout << "================================================================================\n";

    while (true) {
        size_t startIdx = (currentPage - 1) * pageSize;
        size_t endIdx = std::min(startIdx + pageSize, totalTerms);

        std::cout << "\n--------------------------------------------------------------------------------\n";
        std::cout << "PAGE " << currentPage << " of " << totalPages 
                  << "  |  Showing terms " << (startIdx + 1) << " - " << endIdx 
                  << " of " << totalTerms << " total unique words\n";
        std::cout << "--------------------------------------------------------------------------------\n";

        for (size_t i = startIdx; i < endIdx; ++i) {
            const std::string& word = vocabulary[i];
            const auto& matches = indexMap.at(word);

            std::cout << std::setw(5) << (i + 1) << ". [" << word << "] -> in " 
                      << matches.size() << " document(s):\n";

            for (const auto& match : matches) {
                std::cout << "       - " << reader.getFilename(match.docId) 
                          << " (Doc ID: " << match.docId << "): "
                          << match.totalOccurrences << " occurrences, "
                          << match.lineNumbers.size() << " line(s) [Lines: ";

                size_t maxLines = 5;
                for (size_t l = 0; l < match.lineNumbers.size() && l < maxLines; ++l) {
                    std::cout << match.lineNumbers[l];
                    if (l + 1 < match.lineNumbers.size() && l + 1 < maxLines) {
                        std::cout << ", ";
                    }
                }
                if (match.lineNumbers.size() > maxLines) {
                    std::cout << ", ...";
                }
                std::cout << "]\n";
            }
        }

        std::cout << "--------------------------------------------------------------------------------\n";
        std::cout << "Controls: [N] Next Page | [P] Previous Page | [J] Jump to Page | [S] Search Term | [Q] Return\n";
        std::cout << "Enter command: ";

        std::string command;
        if (!std::getline(std::cin, command)) {
            std::cout << "\nReturning to Main Menu...\n\n";
            return;
        }

        command = trim(command);
        if (command.empty()) {
            if (currentPage < totalPages) {
                currentPage++;
            } else {
                std::cout << "\n[Notice] Already at the last page.\n";
            }
            continue;
        }

        std::string cmdLower = TextProcessor::toLowerCase(command);

        if (cmdLower == "q" || cmdLower == "quit" || cmdLower == "exit" || cmdLower == "menu") {
            std::cout << "\nReturning to Main Menu...\n\n";
            return;
        } else if (cmdLower == "n" || cmdLower == "next") {
            if (currentPage < totalPages) {
                currentPage++;
            } else {
                std::cout << "\n[Notice] Already on the last page (" << totalPages << ").\n";
            }
        } else if (cmdLower == "p" || cmdLower == "prev" || cmdLower == "previous") {
            if (currentPage > 1) {
                currentPage--;
            } else {
                std::cout << "\n[Notice] Already on the first page.\n";
            }
        } else if (cmdLower == "j" || cmdLower == "jump") {
            std::cout << "Enter page number (1-" << totalPages << "): ";
            std::string pageInput;
            if (std::getline(std::cin, pageInput)) {
                try {
                    int p = std::stoi(trim(pageInput));
                    if (p >= 1 && p <= static_cast<int>(totalPages)) {
                        currentPage = static_cast<size_t>(p);
                    } else {
                        std::cout << "\n[Warning] Invalid page number! Must be between 1 and " << totalPages << ".\n";
                    }
                } catch (...) {
                    std::cout << "\n[Warning] Please enter a valid numerical page number.\n";
                }
            }
        } else if (cmdLower == "s" || cmdLower == "search" || cmdLower == "find") {
            std::cout << "Enter word to find in index: ";
            std::string termInput;
            if (std::getline(std::cin, termInput)) {
                std::string cleanTerm = TextProcessor::stemWord(TextProcessor::cleanToken(termInput));
                if (cleanTerm.empty()) {
                    std::cout << "\n[Warning] Invalid or empty search word.\n";
                } else {
                    auto it = std::lower_bound(vocabulary.begin(), vocabulary.end(), cleanTerm);
                    if (it != vocabulary.end() && *it == cleanTerm) {
                        size_t targetIdx = std::distance(vocabulary.begin(), it);
                        currentPage = (targetIdx / pageSize) + 1;
                        std::cout << "\n[Found] Word \"" << cleanTerm << "\" is located on Page " 
                                  << currentPage << " (Item " << (targetIdx + 1) << ")!\n";
                    } else {
                        std::cout << "\n[Not Found] Word \"" << cleanTerm << "\" does not exist in vocabulary.\n";
                    }
                }
            }
        } else {
            std::cout << "\n[Unrecognized Command] Please enter N, P, J, S, or Q.\n";
        }
    }
}

void SearchEngine::displayIndex() const {
    displayIndexPaginated();
}

void SearchEngine::exportIndexToFile(const std::string& outputPath) const {
    if (!isInitialized) {
        std::cerr << "[Error] Search engine has not been initialized with documents yet.\n";
        return;
    }

    std::ofstream outFile(outputPath);
    if (!outFile.is_open()) {
        std::cerr << "[WARNING] Could not open file to export index: " << outputPath << "\n";
        return;
    }

    const auto& indexMap = index.getIndexMap();
    outFile << "==================================================\n";
    outFile << "           COMPLETE INVERTED INDEX                \n";
    outFile << " (Alphabetical Order - Red-Black Tree Map)        \n";
    outFile << "==================================================\n";
    outFile << "Total unique words indexed: " << indexMap.size() << "\n\n";

    for (const auto& entry : indexMap) {
        const std::string& word = entry.first;
        const std::vector<DocumentMatch>& matches = entry.second;

        outFile << word << " -> ";
        for (size_t i = 0; i < matches.size(); ++i) {
            const DocumentMatch& match = matches[i];
            outFile << reader.getFilename(match.docId) 
                    << " [Occurrences: " << match.totalOccurrences << ", Lines: ";
            for (size_t j = 0; j < match.lineNumbers.size(); ++j) {
                outFile << match.lineNumbers[j];
                if (j + 1 < match.lineNumbers.size()) {
                    outFile << ", ";
                }
            }
            outFile << "]";
            if (i + 1 < matches.size()) {
                outFile << ", ";
            }
        }
        outFile << "\n";
    }
    outFile << "==================================================\n";
    outFile.close();
}

void SearchEngine::displayDocuments() const {
    const auto& docs = reader.getDocuments();

    std::cout << "\n================================================================================\n";
    std::cout << "                               LOADED DOCUMENTS                                 \n";
    std::cout << "================================================================================\n";

    if (docs.empty()) {
        std::cout << "No documents are currently loaded.\n";
    } else {
        std::cout << std::left << std::setw(8) << "ID"
                  << std::setw(30) << "Filepath"
                  << std::setw(15) << "Lines"
                  << std::setw(15) << "Words (Tokens)" << "\n";
        std::cout << "--------------------------------------------------------------------------------\n";

        for (const auto& doc : docs) {
            std::cout << std::left << std::setw(8) << doc.id
                      << std::setw(30) << doc.filepath
                      << std::setw(15) << doc.lines.size()
                      << std::setw(15) << doc.tokens.size() << "\n";
        }
        std::cout << "--------------------------------------------------------------------------------\n";
        std::cout << "Total loaded documents: " << docs.size() << "\n";
    }
    std::cout << "================================================================================\n\n";
}

void SearchEngine::run() {
    std::cout << "\n==================================================\n";
    std::cout << "              DOCUMENT SEARCH ENGINE              \n";
    std::cout << "==================================================\n";

    std::string input;
    while (true) {
        std::cout << "--------------------------------------------------\n";
        std::cout << "MAIN MENU:\n";
        std::cout << "  1. Search Documents (Single or Multi-Word)\n";
        std::cout << "  2. Display Inverted Index (Paginated Viewer)\n";
        std::cout << "  3. List Loaded Documents\n";
        std::cout << "  4. Exit Application\n";
        std::cout << "--------------------------------------------------\n";
        std::cout << "Enter your choice (1-4): ";

        if (!std::getline(std::cin, input)) {
            std::cout << "\nExiting application. Goodbye!\n";
            break;
        }

        std::string choice = trim(input);

        if (choice == "4" || choice == "0" || choice == "exit" || choice == "EXIT" || choice == "quit" || choice == "QUIT") {
            std::cout << "\nThank you for using Document Search Engine. Goodbye!\n\n";
            break;
        }

        if (choice.empty()) {
            std::cout << "\n[Notice] Input was empty. Please select an option (1-4).\n\n";
            continue;
        }

        if (choice == "1") {
            std::cout << "\nEnter word or phrase to search: ";
            std::string query;
            if (std::getline(std::cin, query)) {
                search(query);
            }
        } else if (choice == "2") {
            displayIndexPaginated();
        } else if (choice == "3") {
            displayDocuments();
        } else {
            std::cout << "\n[Invalid Choice] \"" << choice 
                      << "\" is not a recognized option. Please enter a number from 1 to 4.\n\n";
        }
    }
}
