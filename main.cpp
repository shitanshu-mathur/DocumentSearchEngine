#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>

using namespace std;

// Store each word with its documents and positions
using InvertedIndex =
    unordered_map<string, unordered_map<string, vector<int>>>;

// Convert text into lowercase words
vector<string> getWords(string text) {
    vector<string> words;
    string word = "";

    for (char ch : text) {
        unsigned char c = static_cast<unsigned char>(ch);

        if (isalnum(c)) {
            word += static_cast<char>(tolower(c));
        } else {
            if (word != "") {
                words.push_back(word);
                word = "";
            }
        }
    }

    if (word != "") {
        words.push_back(word);
    }

    return words;
}

// Read a file and store its words in the index
void createIndex(string filename, InvertedIndex &index) {
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Unable to open " << filename << endl;
        return;
    }

    string line;
    int pos = 0;

    while (getline(file, line)) {
        vector<string> words = getWords(line);

        for (string word : words) {
            index[word][filename].push_back(pos);
            pos++;
        }
    }

    file.close();
}

// Display all words, documents, frequencies and positions
void showIndex(const InvertedIndex &index) {
    cout << "\n========== INVERTED INDEX ==========\n";

    for (const auto &word : index) {
        cout << "\nWord: " << word.first << endl;

        for (const auto &doc : word.second) {
            cout << "Document: " << doc.first << endl;
            cout << "Term Frequency: "
                 << doc.second.size() << endl;

            cout << "Positions: ";

            for (int p : doc.second) {
                cout << p << " ";
            }

            cout << endl;
        }
    }
}

// Search for a word entered by the user
void searchWord(const InvertedIndex &index, string query) {
    vector<string> words = getWords(query);

    if (words.size() != 1) {
        cout << "Please enter only one word." << endl;
        return;
    }

    string word = words[0];

    cout << "\n========== SEARCH RESULTS ==========\n";
    cout << "Word: " << word << endl;

    auto it = index.find(word);

    if (it == index.end()) {
        cout << "Word not found in any document." << endl;
        return;
    }

    for (const auto &doc : it->second) {
        cout << "\nDocument: " << doc.first << endl;
        cout << "Term Frequency: "
             << doc.second.size() << endl;

        cout << "Positions: ";

        for (int p : doc.second) {
            cout << p << " ";
        }

        cout << endl;
    }
}

int main() {
    vector<string> files = {
        "documents/dsa.txt",
        "documents/cpp.txt",
        "documents/python.txt"
    };

    InvertedIndex index;

    // Create the index for all documents
    for (string file : files) {
        createIndex(file, index);
    }

    // Show the complete index
    showIndex(index);

    // Take a word from the user
    string query;

    cout << "\nEnter a word to search: ";
    cin >> query;

    // Display the matching documents
    searchWord(index, query);

    return 0;
}