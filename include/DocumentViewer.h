#ifndef DOCUMENT_VIEWER_H
#define DOCUMENT_VIEWER_H

#include <string>
#include <vector>
#include <set>
#include <iostream>

class DocumentViewer {
private:
    static std::string trim(const std::string& str);
    static bool isAnsiSupported();
    static std::string highlightLineText(const std::string& line, 
                                         const std::vector<std::string>& keywords,
                                         bool enableColor);

public:
    static const size_t DEFAULT_LINES_PER_PAGE = 30;

    DocumentViewer() = default;

    static bool loadLines(const std::string& filepath, 
                          std::vector<std::string>& outLines, 
                          std::string& outErrorMessage);

    static bool viewDocument(const std::string& filepath,
                             const std::vector<int>& matchingLines,
                             const std::vector<std::string>& highlightKeywords = {},
                             size_t linesPerPage = DEFAULT_LINES_PER_PAGE,
                             std::istream& in = std::cin, 
                             std::ostream& out = std::cout,
                             bool forceColor = false);

    static bool viewDocument(const std::string& filepath,
                             size_t linesPerPage = DEFAULT_LINES_PER_PAGE,
                             std::istream& in = std::cin, 
                             std::ostream& out = std::cout);

    static std::string formatMatchingLine(const std::string& line,
                                          const std::vector<std::string>& keywords = {},
                                          bool enableColor = true);
};

#endif
