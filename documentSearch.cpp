#include "SearchEngine.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cctype>

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
    std::cout << "\n========================================\n";
    std::cout << "      DOCUMENT LOADING & INDEXING       \n";
    std::cout << "========================================\n";

    // Step 1: Read all documents from disk
    int loadedCount = reader.loadDocuments(filepaths);

    if (loadedCount == 0) {
        std::cerr << "\n[ERROR] No documents were successfully loaded!\n"
                  << "Please make sure the 'documents/' folder exists and contains valid .txt files.\n";
        isInitialized = false;
        return false;
    }

    // Step 2: Clear any previous index
    index.clear();

    // Step 3: Populate the Inverted Index line-by-line to track exact word locations
    for (const auto& doc : reader.getDocuments()) {
        index.addDocument(doc.id, doc.lines);
    }

    isInitialized = true;
    std::cout << "\n[SUCCESS] Indexed " << loadedCount << " document(s) with "
              << index.getVocabularySize() << " unique words in vocabulary.\n";

    // Export the generated inverted index to a local text file for inspection & demonstration
    exportIndexToFile("inverted_index.txt");
    std::cout << "[SUCCESS] Inverted Index exported to file: \"inverted_index.txt\"\n";
    std::cout << "========================================\n\n";
    return true;
}

void SearchEngine::search(const std::string& rawQuery) const {
    if (!isInitialized) {
        std::cerr << "[Error] Search engine has not been initialized with documents yet.\n";
        return;
    }

    std::string trimmedQuery = trim(rawQuery);

    // Handle empty queries
    if (trimmedQuery.empty()) {
        std::cout << "\n[Error] Empty search query! Please enter a valid word.\n";
        return;
    }

    // Tokenize query using the same TextProcessor used for documents
    std::vector<std::string> queryTokens = TextProcessor::tokenize(trimmedQuery);

    // Handle queries consisting solely of punctuation/symbols
    if (queryTokens.empty()) {
        std::cout << "\nSearch Results for: \"" << trimmedQuery << "\"\n";
        std::cout << "[Notice] Query contains no valid alphanumeric search terms.\n";
        return;
    }

    // Single-keyword constraint handling
    if (queryTokens.size() > 1) {
        std::cout << "\n[Notice] Multiple words detected (\"" << trimmedQuery << "\").\n"
                  << "This version supports single-keyword search.\n"
                  << "Searching for the first keyword: \"" << queryTokens[0] << "\"...\n";
    }

    std::string keyword = queryTokens[0];

    // Search the Inverted Index for matches with line locations (O(log V) lookup)
    std::vector<DocumentMatch> matches = index.getMatches(keyword);

    std::cout << "\n========================================\n";
    std::cout << "Search Results for: " << keyword << "\n";
    std::cout << "========================================\n\n";

    if (matches.empty()) {
        std::cout << "No documents found for this word.\n";
    } else {
        for (size_t i = 0; i < matches.size(); ++i) {
            const DocumentMatch& match = matches[i];
            std::cout << (i + 1) << ". " << reader.getFilename(match.docId) << "\n";
            std::cout << "   - Total occurrences: " << match.lineNumbers.size() << " line(s)\n";
            std::cout << "   - Location: Line ";
            for (size_t j = 0; j < match.lineNumbers.size(); ++j) {
                std::cout << match.lineNumbers[j];
                if (j + 1 < match.lineNumbers.size()) {
                    std::cout << ", ";
                }
            }
            std::cout << "\n";
            std::cout << "   - Content snippet(s):\n";
            for (int lineNum : match.lineNumbers) {
                std::string lineContent = reader.getLine(match.docId, lineNum);
                std::cout << "     [Line " << lineNum << "] \"" << lineContent << "\"\n";
            }
            std::cout << "\n";
        }
        std::cout << "Total matching documents: " << matches.size() << "\n";
    }
    std::cout << "\n";
}

void SearchEngine::displayIndex() const {
    if (!isInitialized) {
        std::cerr << "[Error] Search engine has not been initialized with documents yet.\n";
        return;
    }

    const auto& indexMap = index.getIndexMap();

    std::cout << "\n==================================================\n";
    std::cout << "           COMPLETE INVERTED INDEX                \n";
    std::cout << " (Alphabetical Order - Red-Black Tree Map)        \n";
    std::cout << "==================================================\n";

    if (indexMap.empty()) {
        std::cout << "The inverted index is currently empty.\n";
        std::cout << "==================================================\n\n";
        return;
    }

    std::cout << "Total unique words indexed: " << indexMap.size() << "\n\n";

    for (const auto& entry : indexMap) {
        const std::string& word = entry.first;
        const std::vector<DocumentMatch>& matches = entry.second;

        std::cout << word << " -> ";
        for (size_t i = 0; i < matches.size(); ++i) {
            const DocumentMatch& match = matches[i];
            std::cout << reader.getFilename(match.docId) << " (Line ";
            for (size_t j = 0; j < match.lineNumbers.size(); ++j) {
                std::cout << match.lineNumbers[j];
                if (j + 1 < match.lineNumbers.size()) {
                    std::cout << ", ";
                }
            }
            std::cout << ")";
            if (i + 1 < matches.size()) {
                std::cout << ", ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "==================================================\n\n";
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
            outFile << reader.getFilename(match.docId) << " (Line ";
            for (size_t j = 0; j < match.lineNumbers.size(); ++j) {
                outFile << match.lineNumbers[j];
                if (j + 1 < match.lineNumbers.size()) {
                    outFile << ", ";
                }
            }
            outFile << ")";
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

    std::cout << "\n========================================\n";
    std::cout << "            LOADED DOCUMENTS            \n";
    std::cout << "========================================\n";

    if (docs.empty()) {
        std::cout << "No documents are currently loaded.\n";
    } else {
        for (const auto& doc : docs) {
            std::cout << "Doc ID [" << doc.id << "] -> " << doc.filepath 
                      << " (" << doc.lines.size() << " lines, " << doc.content.size() << " characters)\n";
        }
        std::cout << "Total loaded documents: " << docs.size() << "\n";
    }
    std::cout << "========================================\n\n";
}

void SearchEngine::run() {
    std::cout << "\n==================================================\n";
    std::cout << "              DOCUMENT SEARCH ENGINE              \n";
    std::cout << "==================================================\n";

    std::string input;
    while (true) {
        std::cout << "--------------------------------------------------\n";
        std::cout << "MENU OPTIONS:\n";
        std::cout << "  1. Search for a keyword\n";
        std::cout << "  2. Display complete Inverted Index\n";
        std::cout << "  3. List loaded documents\n";
        std::cout << "  4. Exit application\n";
        std::cout << "--------------------------------------------------\n";
        std::cout << "Enter your choice (1-4) or enter a search word directly: ";

        if (!std::getline(std::cin, input)) {
            // EOF reached (e.g. piped input or Ctrl+D/Ctrl+Z)
            std::cout << "\nExiting application. Goodbye!\n";
            break;
        }

        std::string choice = trim(input);

        // Check for exit commands
        if (choice == "4" || choice == "0" || choice == "exit" || choice == "EXIT" || choice == "quit" || choice == "QUIT") {
            std::cout << "\nThank you for using Document Search Engine. Goodbye!\n";
            break;
        }

        if (choice.empty()) {
            std::cout << "[Notice] Input was empty. Please select an option or type a word.\n\n";
            continue;
        }

        if (choice == "1") {
            std::cout << "\nEnter a word to search: ";
            std::string query;
            if (std::getline(std::cin, query)) {
                search(query);
            }
        } else if (choice == "2") {
            displayIndex();
        } else if (choice == "3") {
            displayDocuments();
        } else {
            // User typed a search term directly!
            // Search it right away for maximum user convenience
            search(choice);
        }
    }
}
