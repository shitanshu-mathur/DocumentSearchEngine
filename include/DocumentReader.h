#ifndef DOCUMENT_READER_H
#define DOCUMENT_READER_H

#include <string>
#include <vector>
#include <map>

struct Document {
    int id;
    std::string filepath;
    std::string content;
    std::vector<std::string> lines;
    std::vector<std::string> tokens; 
    std::vector<int> tokenLines;         
};

class DocumentReader {
private:
    std::vector<Document> documents;         
    std::map<int, std::string> idToFilename; 

public:
    DocumentReader() = default;

    bool loadDocument(const std::string& filepath);
    int loadDocuments(const std::vector<std::string>& filepaths);
    const std::vector<Document>& getDocuments() const;
    const Document* getDocument(int docId) const;
    std::string getFilename(int docId) const;
    bool hasDocument(int docId) const;

    size_t getDocumentCount() const;
    std::string getLine(int docId, int lineNumber) const;
    const std::vector<std::string>& getDocumentLines(int docId) const;
    void clear();
};

#endif
