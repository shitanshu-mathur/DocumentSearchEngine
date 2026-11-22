#include "DocumentViewer.h"
#include <fstream>
#include <cctype>
#include <algorithm>
#include <cstdlib>

#ifndef _WIN32
#include <unistd.h>
#endif

std::string DocumentViewer::trim(const std::string& str) {
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

bool DocumentViewer::isAnsiSupported() {
    const char* noColor = std::getenv("NO_COLOR");
    if (noColor && noColor[0] != '\0') {
        return false;
    }
    const char* term = std::getenv("TERM");
    if (term && std::string(term) == "dumb") {
        return false;
    }

#ifdef _WIN32
    return true;
#else
    return isatty(fileno(stdout));
#endif
}

std::string DocumentViewer::highlightLineText(const std::string& line, 
                                             const std::vector<std::string>& keywords,
                                             bool enableColor) {
    if (!enableColor) {
        return "[MATCH] " + line;
    }

    const std::string ANSI_YELLOW_BG = "\033[43;30m";
    const std::string ANSI_KEYWORD   = "\033[1;31;43m";
    const std::string ANSI_RESET     = "\033[0m";

    if (keywords.empty()) {
        return ANSI_YELLOW_BG + line + ANSI_RESET;
    }

    std::string lowerLine = line;
    for (char& c : lowerLine) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }

    std::vector<bool> isKeywordChar(line.size(), false);

    for (const auto& rawKw : keywords) {
        std::string kw = rawKw;
        for (char& c : kw) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        if (kw.empty()) continue;

        size_t pos = 0;
        while ((pos = lowerLine.find(kw, pos)) != std::string::npos) {
            for (size_t k = 0; k < kw.size(); ++k) {
                isKeywordChar[pos + k] = true;
            }
            pos += kw.size();
        }
    }

    std::string result;
    bool inKeyword = false;

    result += ANSI_YELLOW_BG;

    for (size_t i = 0; i < line.size(); ++i) {
        if (isKeywordChar[i]) {
            if (!inKeyword) {
                result += ANSI_KEYWORD;
                inKeyword = true;
            }
        } else {
            if (inKeyword) {
                result += ANSI_YELLOW_BG;
                inKeyword = false;
            }
        }
        result += line[i];
    }

    result += ANSI_RESET;
    return result;
}

std::string DocumentViewer::formatMatchingLine(const std::string& line,
                                              const std::vector<std::string>& keywords,
                                              bool enableColor) {
    return highlightLineText(line, keywords, enableColor);
}

bool DocumentViewer::loadLines(const std::string& filepath, 
                              std::vector<std::string>& outLines, 
                              std::string& outErrorMessage) {
    outLines.clear();
    outErrorMessage.clear();

    std::ifstream file(filepath);
    if (!file.is_open()) {
        outErrorMessage = "Could not open file: \"" + filepath + "\". File is missing, unreadable, or access was denied.";
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        outLines.push_back(line);
    }
    file.close();

    return true;
}

bool DocumentViewer::viewDocument(const std::string& filepath, 
                                 const std::vector<int>& matchingLines,
                                 const std::vector<std::string>& highlightKeywords,
                                 size_t linesPerPage,
                                 std::istream& in, 
                                 std::ostream& out,
                                 bool forceColor) {
    std::vector<std::string> lines;
    std::string errorMsg;

    if (!loadLines(filepath, lines, errorMsg)) {
        out << "\n[ERROR] " << errorMsg << "\n\n";
        return false;
    }

    if (lines.empty()) {
        out << "\n[NOTICE] Document \"" << filepath << "\" is empty (0 lines of content).\n\n";
        return true;
    }

    if (linesPerPage == 0) {
        linesPerPage = DEFAULT_LINES_PER_PAGE;
    }

    size_t totalLines = lines.size();
    size_t totalPages = (totalLines + linesPerPage - 1) / linesPerPage;
    if (totalPages == 0) totalPages = 1;
    size_t currentPage = 1;

    // Fast lookup set for matching lines (1-based)
    std::set<int> matchSet(matchingLines.begin(), matchingLines.end());
    bool enableColor = forceColor || isAnsiSupported();

    while (true) {
        size_t startLine = (currentPage - 1) * linesPerPage;
        size_t endLine = std::min(startLine + linesPerPage, totalLines);

        // Count matching lines appearing on this specific page
        size_t matchesOnThisPage = 0;
        for (size_t i = startLine; i < endLine; ++i) {
            int lineNum = static_cast<int>(i + 1);
            if (matchSet.find(lineNum) != matchSet.end()) {
                matchesOnThisPage++;
            }
        }

        out << "\n================================================================================\n";
        out << "DOCUMENT VIEWER: " << filepath << "\n";
        out << "Page " << currentPage << " of " << totalPages 
            << " (Lines " << (startLine + 1) << " - " << endLine 
            << " of " << totalLines << ")";
        if (!matchingLines.empty()) {
            out << " | Matching Lines on this page: " << matchesOnThisPage 
                << " (Total: " << matchingLines.size() << ")";
        }
        out << "\n================================================================================\n";

        for (size_t i = startLine; i < endLine; ++i) {
            int lineNum = static_cast<int>(i + 1);
            if (matchSet.find(lineNum) != matchSet.end()) {
                // Entire matching line highlighted with high-contrast yellow background
                out << highlightLineText(lines[i], highlightKeywords, enableColor) << "\n";
            } else {
                // Non-matching lines printed normally
                out << lines[i] << "\n";
            }
        }

        out << "--------------------------------------------------------------------------------\n";
        out << "NAVIGATION: [N] Next page | [P] Previous page | [Q] Close document & return\n";
        out << "Enter command (N/P/Q): ";

        std::string cmd;
        if (!std::getline(in, cmd)) {
            break;
        }

        std::string trimmedCmd = trim(cmd);
        if (trimmedCmd.empty()) {
            continue;
        }

        if (trimmedCmd == "q" || trimmedCmd == "Q") {
            out << "\nClosing document viewer. Returning to search results...\n\n";
            break;
        } else if (trimmedCmd == "n" || trimmedCmd == "N") {
            if (currentPage < totalPages) {
                currentPage++;
            } else {
                out << "\n[Notice] Already on the last page (Page " << totalPages 
                    << " of " << totalPages << ").\n";
            }
        } else if (trimmedCmd == "p" || trimmedCmd == "P") {
            if (currentPage > 1) {
                currentPage--;
            } else {
                out << "\n[Notice] Already on the first page (Page 1 of " << totalPages << ").\n";
            }
        } else {
            out << "\n[Invalid Command] \"" << trimmedCmd 
                << "\" is not recognized. Please enter N for next page, P for previous page, or Q to close.\n";
        }
    }

    return true;
}

bool DocumentViewer::viewDocument(const std::string& filepath, 
                                 size_t linesPerPage,
                                 std::istream& in, 
                                 std::ostream& out) {
    return viewDocument(filepath, {}, {}, linesPerPage, in, out);
}
