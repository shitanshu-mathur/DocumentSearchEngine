#include "DocumentReader.h"
#include <iostream>
#include <fstream>
#include <sstream>

bool DocumentReader::loadDocument(const std::string& filepath) {
    // Attempt to open the file using standard C++ ifstream
    std::ifstream file(filepath);

    // Gracefully handle missing or unreadable files
    if (!file.is_open()) {
        std::cerr << "[WARNING] Could not open file: \"" << filepath 
                  << "\". File might be missing or unreadable. Skipping.\n";
        return false;
    }

    Document doc;
    doc.id = static_cast<int>(documents.size());
    doc.filepath = filepath;

    // Read the file line by line to preserve exact line numbers for word location tracking
    std::string line;
    while (std::getline(file, line)) {
        // Strip trailing \r in case of Windows CRLF line endings
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        doc.lines.push_back(line);
        doc.content += line + "\n";
    }
    file.close();

    // Check for empty files
    if (doc.content.empty()) {
        std::cout << "[INFO] File \"" << filepath << "\" is empty. Registered as Doc ID " 
                  << doc.id << " with no searchable words.\n";
    }

    documents.push_back(doc);
    idToFilename[doc.id] = filepath;

    std::cout << "[SUCCESS] Loaded \"" << filepath << "\" as Document ID: " << doc.id 
              << " (" << doc.lines.size() << " lines)\n";
    return true;
}

int DocumentReader::loadDocuments(const std::vector<std::string>& filepaths) {
    int successCount = 0;
    for (const auto& path : filepaths) {
        if (loadDocument(path)) {
            successCount++;
        }
    }
    return successCount;
}

const std::vector<Document>& DocumentReader::getDocuments() const {
    return documents;
}

std::string DocumentReader::getFilename(int docId) const {
    auto it = idToFilename.find(docId);
    if (it != idToFilename.end()) {
        return it->second;
    }
    return "Unknown_Document";
}

bool DocumentReader::hasDocument(int docId) const {
    return idToFilename.find(docId) != idToFilename.end();
}

size_t DocumentReader::getDocumentCount() const {
    return documents.size();
}

std::string DocumentReader::getLine(int docId, int lineNumber) const {
    for (const auto& doc : documents) {
        if (doc.id == docId) {
            if (lineNumber >= 1 && lineNumber <= static_cast<int>(doc.lines.size())) {
                return doc.lines[lineNumber - 1];
            }
            break;
        }
    }
    return "";
}

const std::vector<std::string>& DocumentReader::getDocumentLines(int docId) const {
    static const std::vector<std::string> emptyLines;
    for (const auto& doc : documents) {
        if (doc.id == docId) {
            return doc.lines;
        }
    }
    return emptyLines;
}
