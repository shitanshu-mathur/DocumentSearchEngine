# DocumentSearchEngine

## Team member 1 (Team Lead)
(Toshni Nandani: 2510010971: toshninandani3@gmail.com)

## Team member 2
(Mohd Rehan Saifi: 2510012282: rehansaifi8191@gmail.com)

## Team member 3
(Shitanshu Mathur: 2510010855: Shitumathur20@gmail.com)

## Team member 4
(Vartika Yadav: 2510010855: vartikayadav0982@gmail.com)
# Document Search Engine with Advanced Keyword Processing and Relevance Ranking

A robust, beginner-friendly, high-performance terminal-based **Document Search Engine** built in C++17 using fundamental **Data Structures and Algorithms (DSA)**, **Object-Oriented Programming (OOP)** principles, and basic **Information Retrieval (IR)** concepts.

Designed specifically for college Project-Based Learning (PBL) demonstrations and technical viva examinations. Completely offline with zero external search libraries, databases, or AI/ML APIs.

---

## 1. Project Overview

Modern search engines (such as Google) do not scan millions of documents from start to finish on every search query. Doing so would take minutes or hours. Instead, they scan all documents **once in advance** to construct an in-memory **Inverted Index**.

This application implements that core architecture, upgraded with an advanced keyword-processing, positional verification, and multi-factor **Relevance Ranking System** across 10 complete Computer Science textbooks:
1. It reads local `.txt` files line-by-line using standard C++ file streams (`std::ifstream`).
2. It assigns each file a unique integer Document ID (0 to 9).
3. It cleans, normalizes (case folding, punctuation trimming, technical term preservation), stems, and tokenizes the text while tracking exact sequential word positions.
4. It isolates common stop words so they do not artificially dominate search results while preserving positional continuity for phrase queries.
5. It performs deterministic, rule-based pronoun resolution (coreference resolution) to associate pronouns (such as `it` and `they`) with clear preceding technical entities, tracking contextual references separately from literal word occurrences.
6. It builds an in-memory **Positional Inverted Index** mapping each unique normalized root word to documents, total occurrences, distinct line numbers, and token positions.
7. It supports single-keyword search, Boolean `AND` / `OR` search, and exact phrase matching in double quotes (`"..."`).
8. It scores and ranks matching documents using a transparent, multi-factor relevance formula combining Normalized Term Frequency, Keyword Coverage, Exact Phrase Match, Title Match, Keyword Proximity, and Contextual References.
9. It provides an interactive console interface with ranked result snippets, a **Paginated Index Viewer** (20 terms/page with N/P/J/S/Q navigation), document inspection, and clean menu controls.

---

## 2. Folder Structure

```
DocumentSearchEngine/
|-- main.cpp                     # Application entry point; initializes engine and starts menu loop
|-- test_suite.cpp               # Automated unit test suite verifying all IR and DSA components
|-- include/                     # Header files (Class declarations)
|   |-- DocumentReader.h         # Declares Document struct (lines, tokens, tokenLines) and DocumentReader
|   |-- DocumentViewer.h         # Declares interactive paginated document viewer and safe loader
|   |-- TextProcessor.h          # Declares normalization, stemming, stop words, and pronoun utilities
|   |-- PronounResolver.h        # Declares deterministic rule-based coreference resolution module
|   |-- InvertedIndex.h          # Declares positional InvertedIndex and match structures
|   \-- SearchEngine.h           # Declares SearchEngine coordinator, parser, scorer, and CLI
|-- src/                         # Source files (Class implementations)
|   |-- DocumentReader.cpp       # File handling, line splitting, document token storage
|   |-- DocumentViewer.cpp       # Paginated document display, N/P/Q navigation, safe file reading
|   |-- TextProcessor.cpp        # Case conversion, punctuation trimming, conservative stemming
|   |-- PronounResolver.cpp      # Sentence boundary parsing, antecedent tracking, pronoun mapping
|   |-- InvertedIndex.cpp        # Positional inverted index building and occurrence counting
|   \-- SearchEngine.cpp        # Query parsing, Boolean AND/OR, phrase matching, proximity, scoring
|-- documents/                   # Text document repository (Exactly 10 source textbooks)
|   |-- algorithms.txt           # 50,812 words - Algorithms & Computational Complexity
|   |-- computer_networks.txt    # 51,020 words - Computer Networks & Communications
|   |-- cpp.txt                  # 50,985 words - C++ Systems Programming & Architecture
|   |-- database_systems.txt     # 50,954 words - Database Systems & Relational Theory
|   |-- data_structures.txt      # 51,084 words - Data Structures & Memory Organization
|   |-- dsa.txt                  # 51,027 words - Data Structures & Algorithms Unified
|   |-- oop.txt                  # 50,928 words - Object-Oriented Software Engineering & Design
|   |-- operating_systems.txt    # 50,991 words - Operating Systems Principles & Internals
|   |-- python.txt               # 51,100 words - Python Language Architecture & Engineering
|   \-- web_technologies.txt     # 50,811 words - Modern Web Technologies & Full-Stack Architecture
|-- output/                      # Generated application output (outside documents folder)
|   |-- inverted_index.txt       # Exported complete Inverted Index file on disk (with occurrences and lines)
|   \-- search_results.json      # Live JSON feed of ranked results for VS Code extension integration
|-- vscode-extension/            # Native VS Code extension for true mouse-clickable document opening
|   |-- package.json             # Extension manifest and contributions (TreeView and Webview)
|   |-- extension.js             # Extension activation, file watcher, and single-click editor opener
|   |-- resources/
|   |   \-- search-icon.svg      # Activity Bar icon for Document Search panel
|   \-- README.md                # Extension documentation and usage guide
|-- install_extension.bat        # 1-Click batch installer for the VS Code extension
|-- install_extension.ps1        # 1-Click PowerShell installer for the VS Code extension
|-- .vscode/                     # VS Code workspace settings and build tasks
|   |-- extensions.json          # Workspace extension recommendations
|   \-- tasks.json               # One-click build and execution tasks (Ctrl+Shift+B)
|-- README.md                    # Project documentation and viva guide
\-- .gitignore                   # Git ignore file for compiled binaries and archives
```

---

## 3. The 10 Source Documents (>50,000 Words Each)

The `documents/` folder contains exactly 10 comprehensive textbooks. Each document exceeds the 50,000-word requirement and features 25 structured chapters with theoretical foundations, architectural mechanics, source code walkthroughs, real-world case studies, and practical exercises:

| # | File | Subject Area & Content Scope | Word Count | File Size |
| :-: | :--- | :--- | :-: | :-: |
| 1 | **`documents/algorithms.txt`** | Design paradigms, asymptotic complexity, graph algorithms, dynamic programming | 50,812 | ~421 KB |
| 2 | **`documents/computer_networks.txt`** | OSI/TCP models, routing protocols, TCP/UDP, DNS, HTTP, socket mechanics | 51,020 | ~421 KB |
| 3 | **`documents/cpp.txt`** | Syntax, pointers, classes, vtables, templates, STL, memory models | 50,985 | ~421 KB |
| 4 | **`documents/database_systems.txt`** | Relational algebra, SQL, normalization (1NF-BCNF), ACID, transactions, B+ trees | 50,954 | ~420 KB |
| 5 | **`documents/data_structures.txt`** | Arrays, linked lists, stacks, queues, hash tables, heaps, balanced BSTs, tries | 51,084 | ~420 KB |
| 6 | **`documents/dsa.txt`** | Unified data structures & algorithmic analysis with implementation tradeoffs | 51,027 | ~420 KB |
| 7 | **`documents/oop.txt`** | Encapsulation, inheritance, polymorphism, abstraction, SOLID design principles | 50,928 | ~424 KB |
| 8 | **`documents/operating_systems.txt`** | Processes, threads, CPU scheduling, synchronization, deadlocks, paging, file systems | 50,991 | ~423 KB |
| 9 | **`documents/python.txt`** | Dynamic typing, OOP, iterators, generators, decorators, memory management | 51,100 | ~422 KB |
| 10 | **`documents/web_technologies.txt`** | HTML5, CSS layout engines, JavaScript V8, HTTP/REST APIs, web security | 50,811 | ~421 KB |
| | **TOTAL** | **Comprehensive Academic Corpus** | **509,712 words** | **~4.2 MB** |

---

## 4. Text Processing Pipeline

### 4.1 Text Normalization & Internal Punctuation Splitting
- Converts characters to lowercase (`std::tolower`) so `Stack`, `STACK`, and `stack` are treated identically.
- Treats non-alphanumeric internal punctuation as token separators (e.g., `stack,queue` -> `stack`, `queue`; `stack.queue` -> `stack`, `queue`; `(stack), queue!` -> `stack`, `queue`).
- Preserves programming terms: `c++`, `c#`, `oop`, `dsa`, `html`, `sql`. Surrounding punctuation such as `(C++)` or `C++,` is cleanly parsed as `c++`.
- The exact same normalization and tokenization pipeline is shared between document indexing and query processing, ensuring consistency.
- Original document text and casing are untouched for snippets; normalization applies only to internal search tokens.

### 4.2 Conservative Rule-Based Stemming
To associate common grammatical variations with their base root, the engine employs a conservative, explainable stemmer:
- **Suffix `ies`**: Converts to `y` if root length >= 3 (e.g., `studies` -> `study`, `queries` -> `query`, `libraries` -> `library`).
- **Suffix `ing`**: Removes `ing` if root length >= 3 (e.g., `playing` -> `play`, `connecting` -> `connect`, `studying` -> `study`).
  - **Doubled Consonants**: Drops doubled consonant before `ing` (e.g., `running` -> `run`, `stopping` -> `stop`, `swimming` -> `swim`, `programming` -> `program`), while preserving doubled `s`, `l`, and `f` (e.g., `passing` -> `pass`, `calling` -> `call`).
- **Suffix `ed`**: Removes `ed` if root length >= 3 (e.g., `played` -> `play`, `connected` -> `connect`, `stopped` -> `stop`). Suffix `ied` maps to `study`. Short words (`bed`, `red`, `led`) are never stemmed.
- **Suffix `es`**: Strips `es` after `s`, `sh`, `ch`, `x`, `z` (e.g., `boxes` -> `box`, `classes` -> `class`, `processes` -> `process`, `branches` -> `branch`).
- **Suffix `s`**: Removes trailing `s` if root length >= 3 (e.g., `stacks` -> `stack`, `plays` -> `play`, `connects` -> `connect`).
- **Protected Words**: Words ending in `ss` (`class`, `process`, `business`, `access`), `is` (`analysis`, `basis`, `crisis`), and `us` (`status`, `focus`, `radius`) are protected from truncation.
- Words with length <= 3 are kept unchanged.

### 4.3 Stop-Word Handling
- Standard stop words include: `the`, `is`, `am`, `are`, `was`, `were`, `a`, `an`, `of`, `in`, `on`, `at`, `to`, `for`, `from`, `and`, `or`, `but`, `with`, `as`, `by`, `be`, `been`, `being`, `that`, `this`, `these`, `those`, `it`, `its`, `they`, `them`, `their`, `he`, `she`, `which`, `who`, etc.
- Technical terms (`c++`, `c#`, `oop`, `dsa`, `html`, `sql`) are **never** treated as stop words.
- Stop words do not independently create index entries or contribute to relevance scores.
- Sequential token positions are preserved so that stop words inside phrases (e.g., `"stack and queue"`) do not distort relative token distances.
- Searching for a stop word alone outputs an informative notice and returns no noisy results.

### 4.4 Basic Rule-Based Pronoun Resolution
- **Pronouns Handled**: Singular (`it`, `its`, `this`, `that`, `he`, `him`, `his`, `she`, `her`) and Plural (`they`, `them`, `their`, `theirs`, `these`, `those`).
- **Mechanism**:
  - Sentences are parsed for subject definitions:
    - *Singular Pattern*: `X is/was/has/represents ...` (e.g., `"Stack is an important data structure."` -> Candidate singular: `stack`).
    - *Plural Pattern*: `X and Y are/were/represent ...` (e.g., `"Stack and Queue are data structures."` -> Candidate plural: `{stack, queue}`).
  - When the subsequent sentence opens with an agreement-matching pronoun:
    - `"It is useful for memory management."` -> Singular pronoun `it` associates with `stack`.
    - `"They are useful in programming."` -> Plural pronoun `they` associates with `stack` and `queue`.
  - Contextual references are recorded in a separate index (`contextualIndex`) and **never** counted as literal word occurrences.
  - Ambiguous or complex sentences without clear antecedent patterns are skipped without making speculative associations.

---

## 5. Inverted Index & Positional Storage

### 5.1 In-Memory Data Structures
- **Vocabulary Index**: `std::map<std::string, std::vector<DocumentMatch>>`
  - Self-balancing Red-Black Binary Search Tree providing $O(\log V)$ vocabulary lookup.
  - Postings structure:
    ```cpp
    struct DocumentMatch {
        int docId;                    // Document ID (0 to 9)
        int totalOccurrences = 0;     // Total literal frequency in document
        std::vector<int> lineNumbers; // Distinct line numbers where word occurs
        std::vector<int> positions;   // Sequential token positions in document (0-based)
    };
    ```
- **Contextual Index**: `std::map<std::string, std::vector<ContextualMatch>>`
  - Completely separate structure tracking pronoun associations:
    ```cpp
    struct ContextualMatch {
        int docId;
        int referenceCount = 0;
        std::vector<int> lineNumbers;
    };
    ```
- **Document Word Counts**: `std::vector<int> docTotalWords`
  - Stores the total normalized token count for each document **before** stop-word filtering. Used for fair length normalization.

### 5.2 Physical Export on Disk
- **Export Location**: `output/inverted_index.txt`
- Exported automatically upon application launch and stored outside the `documents/` folder.
- Contains the sorted vocabulary, document names, total occurrences, and line numbers.

---

## 6. Search Features & Query Modes

### 1. Single-Keyword Search
- Normalizes and stems the input keyword (e.g., `stacks` searches for root `stack`).
- Retrieves matching documents, calculates relevance scores, and displays literal frequency, contextual references, line numbers, and snippets.

### 2. Boolean Search: AND, OR, & Precedence Parsing
- **Boolean AND (`stack AND queue`)**:
  - Calculates the set intersection of document postings across all query terms.
  - Returns only documents containing **all** query keywords.
  - Default mode for multi-word queries without explicit operators (e.g., `stack queue`).
- **Boolean OR (`stack OR queue`)**:
  - Calculates the set union of document postings across all query terms.
  - Returns documents containing **at least one** query keyword.
- **AND-Before-OR Precedence (DNF Parsing)**:
  - In mixed expressions such as `stack AND queue OR python`, the engine parses the query in Disjunctive Normal Form (DNF): `(stack AND queue) OR (python)`.
  - AND-groups are intersected first, and the resulting sets are unioned, ensuring standard Boolean precedence.
- **Malformed Expression Validation**:
  - Queries with operators at boundaries (e.g., `stack AND`, `OR queue`) or consecutive operators (`stack AND OR queue`) are safely validated with clear, informative notices without crashes.
- Operators are case-insensitive (`AND`, `and`, `OR`, `or`).

### 3. Exact Phrase Search (`"..."`)
- Enclosing query terms in double quotes triggers exact phrase verification:
  - Example: `"data structure"` or `"stack and queue"`.
  - Verified by checking consecutive normalized token sequences in the document.
  - Returns only documents containing the exact phrase in consecutive order.
  - **Accurate Phrase Frequency**: Counts every actual phrase occurrence in a document, including multiple occurrences on the same line, while maintaining distinct matching line numbers.
  - Displays matching line numbers and contextual snippets.
  - Phrases with reverse ordering (e.g., `"structure data"`) return no results unless that exact sequence exists.

### 4. Keyword Proximity
- For multi-term queries, the engine evaluates the minimum distance between query term positions in each document using binary search (`std::lower_bound`).
- Bounded formula:
  $$\text{ProximityScore} = \frac{1.0}{d_{\min}}$$
  where $d_{\min}$ is the minimum token distance between different query terms ($d_{\min} \ge 1$).
  - Adjacent terms ($d_{\min} = 1$): Proximity = $1.0$.
  - 1 word apart ($d_{\min} = 2$): Proximity = $0.5$.
  - Single terms or terms not co-occurring: Proximity = $0.0$.

---

## 7. Document Length Normalization & Relevance Ranking

### 7.1 Normalized Term Frequency (TF)
To prevent longer documents from dominating purely due to word volume:
$$\text{NormalizedTF}(D, Q) = \frac{\text{Total Literal Frequency of Matching Query Terms in } D}{\text{Total Number of Normalized Words in Document } D}$$
- Uses the total token count before stop-word removal.
- Includes division-by-zero protection (`totalWords > 0`).

### 7.2 Final Relevance Ranking Formula
Matching documents are evaluated using a transparent weighted scoring formula:

$$\text{Final Score}(D, Q) = 40 \times \text{NormalizedTF} + 20 \times \text{KeywordCoverage} + 20 \times \text{ExactPhraseMatch} + 10 \times \text{TitleMatch} + 10 \times \text{ProximityScore} + \text{ContextualBonus}$$

| Component | Weight | Range | Definition |
| :--- | :---: | :---: | :--- |
| **NormalizedTF** | 40 | $[0, 1]$ | Literal frequency of matched terms divided by total document word count |
| **KeywordCoverage** | 20 | $[0, 1]$ | Distinct query terms found in $D$ divided by total distinct query terms |
| **ExactPhraseMatch** | 20 | $\{0, 1\}$ | $1.0$ if the complete multi-token sequence occurs consecutively; strictly $0.0$ for single-word queries |
| **TitleMatch** | 10 | $\{0, 1\}$ | $1.0$ if at least one query term appears in document filename, else $0.0$ |
| **ProximityScore** | 10 | $[0, 1]$ | $1.0 / d_{\min}$ measuring compactness of query terms in $D$ |
| **ContextualBonus** | Bonus | $[0, 2]$ | Small bonus ($\min(2.0, \text{contextualCount} \times 0.5)$) from pronoun references |

### 7.3 Sorting & Deterministic Tie-Breaking
- Results are sorted in **descending order of Relevance Score**.
- If two documents receive identical relevance scores, ties are broken deterministically by **alphabetical filename order** (`a.filename < b.filename`).

---

## 8. Paginated Inverted Index Viewer (Option 2)

Because the vocabulary contains over 3,500 unique stemmed terms across 500,000 words, printing all terms at once would freeze the terminal. The engine includes a paginated viewer:
- Displays **20 terms per page**.
- Navigation controls:
  - `N`: Next page
  - `P`: Previous page
  - `J`: Jump to any specific page number
  - `S`: Instant alphabetical binary search for any term in the index
  - `Q`: Immediately and safely returns to the Main Menu.

---

## 9. Interactive Terminal Document Viewer with Complete Matching Line Highlighting

In addition to the VS Code native extension integration, the terminal search interface provides an instant, interactive document viewer:
- **Rank Selection**: When ranked search results appear, the user enters a rank number (e.g., `1` through `10`) to immediately open and inspect that document directly inside the console.
- **Complete Matching Line Highlighting**:
  - The **ENTIRE** text of every matching line is rendered with a high-contrast ANSI yellow background and dark text (`\033[43;30m`), making matching lines instantly recognizable at a glance.
  - Search query keywords occurring inside the line are highlighted with bold contrasting red text (`\033[1;31;43m`).
  - Every line terminates with an explicit ANSI reset escape code (`\033[0m`), guaranteeing that styling never bleeds into adjacent content.
  - Non-matching lines remain completely unhighlighted in their standard terminal formatting.
- **Exact Matching Line Tracking**:
  - The viewer leverages the exact 1-based line numbers (`r.matchingLines`) computed during query evaluation by `SearchEngine`.
  - Only lines containing actual keyword or phrase occurrences are highlighted.
- **Paginated Navigation & Line Indicators**:
  - Documents are presented in readable chunks (30 lines per page by default).
  - The page header reports exact context: `Page X of Y (Lines A - B of C) | Matching Lines on this page: Z (Total: T)`.
  - Interactive commands:
    - `N`: Advance to the next page.
    - `P`: Return to the previous page.
    - `Q`: Close the document viewer and return directly to the search results list, preserving the search state.
- **Safe I/O Fallback**:
  - Detects ANSI escape support automatically across Windows Terminal, PowerShell, CMD, and VS Code integrated terminal (`ENABLE_VIRTUAL_TERMINAL_PROCESSING`).
  - Gracefully falls back to a clean `[MATCH] ` prefix indicator if running in environments where ANSI colors are explicitly disabled via `NO_COLOR=1` or `TERM=dumb`.

---

## 10. Compilation & Execution Guide

### Requirements
- **OS**: Windows (Command Prompt or PowerShell)
- **Compiler**: MinGW `g++` (supporting C++17)

### Step 1: Open Terminal in Project Directory
```powershell
cd C:\Users\HP\OneDrive\Documents\DocumentSearchEngine
```

### Step 2: Compile with C++17 and Optimizations
```powershell
g++ -std=c++17 -O2 -Wall -Wextra -I include main.cpp src/DocumentReader.cpp src/TextProcessor.cpp src/PronounResolver.cpp src/InvertedIndex.cpp src/SearchEngine.cpp src/DocumentViewer.cpp -o main.exe
```

### Step 3: Run the Application
```powershell
.\main.exe
```

### Running the Automated Unit Test Suite (16 Test Modules)
```powershell
g++ -std=c++17 -O2 -Wall -Wextra -I include test_suite.cpp src/TextProcessor.cpp src/PronounResolver.cpp src/InvertedIndex.cpp src/DocumentReader.cpp src/SearchEngine.cpp src/DocumentViewer.cpp -o test_suite.exe
.\test_suite.exe
```

---

## 11. True Mouse-Clickable Ranked Results in Visual Studio Code

### Why a VS Code Extension?
In standard terminal environments (Windows Console, PowerShell, integrated VS Code terminal), text outputs cannot reliably trigger editor document opening with a bare **single left mouse click**:
- Terminal links require `Ctrl + Click` or `Alt + Click`.
- Terminal numbers require typing the number and pressing Enter.
- Console applications cannot directly command the host VS Code window to open an editor tab.

To solve this properly, this project provides a lightweight, native **VS Code Extension** (`document-search-engine-viewer`) that bridges the high-performance C++ backend with the rich VS Code frontend.

```
+------------------------------------+
|  C++17 Search Engine (main.exe)    |
|  - Reads & parses 10 textbooks     |
|  - Builds Inverted Index in RAM    |
|  - Calculates Relevance Rankings   |
|  - Writes output/search_results.json
+-----------------+------------------+
                  | (Live File System Watcher)
                  v
+-----------------+------------------+
|  VS Code Native Extension          |
|  - Auto-refreshes Webview & Tree   |
|  - Listens for SINGLE LEFT CLICK   |
|  - Opens .txt file in editor tab   |
+------------------------------------+
```

### Features of the VS Code Integration
1. **True Single Left Click**: Left-click any ranked document in the VS Code sidebar view or Webview to immediately open its original `.txt` file in an editor tab.
2. **Zero Keyboard Modifiers**: No `Ctrl + Click`, no typing rank numbers, no terminal navigation needed.
3. **Line Jumping**: Click individual line badges (`L567`, `L572`) to open the document and automatically highlight that exact matching line in the editor!
4. **Dual Interface**:
   - **Interactive Webview Panel**: Displays rich cards with rank badges, relevance scores, frequency counts, snippets, and a search box.
   - **Native TreeView**: Integrated directly into both the Activity Bar ("Document Search") and the standard VS Code Explorer sidebar.
5. **Real-Time Live Sync**: The extension watches `output/search_results.json`. The moment a search is run in the terminal, the VS Code views update instantaneously.

### 1-Click Extension Installation
1. Open PowerShell or Command Prompt in the project folder and run:
   ```powershell
   powershell -ExecutionPolicy Bypass -File .\install_extension.ps1
   ```
   *(or double-click `install_extension.bat` in Windows Explorer)*
2. In VS Code, reload the window:
   - Press `Ctrl + Shift + P`
   - Select **`Developer: Reload Window`**
3. You will now see the **Document Search** icon in the Activity Bar and the **Ranked Search Results** panel in the Explorer sidebar!

---

## 12. Sample Interactive Session

### 12.1 Startup & Indexing
```
==================================================
           DOCUMENT LOADING & INDEXING            
==================================================
[SUCCESS] Loaded "documents/algorithms.txt" as Document ID: 0 (3121 lines, 52101 tokens)
[SUCCESS] Loaded "documents/computer_networks.txt" as Document ID: 1 (3078 lines, 52352 tokens)
[SUCCESS] Loaded "documents/cpp.txt" as Document ID: 2 (3185 lines, 52138 tokens)
[SUCCESS] Loaded "documents/database_systems.txt" as Document ID: 3 (3061 lines, 51968 tokens)
[SUCCESS] Loaded "documents/data_structures.txt" as Document ID: 4 (3121 lines, 52279 tokens)
[SUCCESS] Loaded "documents/dsa.txt" as Document ID: 5 (3141 lines, 52222 tokens)
[SUCCESS] Loaded "documents/oop.txt" as Document ID: 6 (3120 lines, 51976 tokens)
[SUCCESS] Loaded "documents/operating_systems.txt" as Document ID: 7 (3101 lines, 52210 tokens)
[SUCCESS] Loaded "documents/python.txt" as Document ID: 8 (3200 lines, 52261 tokens)
[SUCCESS] Loaded "documents/web_technologies.txt" as Document ID: 9 (3061 lines, 52131 tokens)

Building Inverted Index from loaded documents...
[SUCCESS] Indexed 10 document(s) with 2985 unique words in vocabulary.
[SUCCESS] Inverted Index exported to: "output/inverted_index.txt"
==================================================
```

### 12.2 Single Keyword Ranked Search
```
Enter word or phrase to search: stack

========================================
SEARCH RESULTS
========================================

Query: stack

Rank 1: documents/data_structures.txt
Relevance Score: 20.07
Literal Keyword Frequency: 87
Contextual References: 0
Matching Lines: 567, 572, 574, 576, 579, 582, 593, 596, 599, 611, ... (60 total)
Snippet: CHAPTER 6: Stacks: Contiguous vs Linked Memory Representations

Rank 2: documents/web_technologies.txt
Relevance Score: 20.06
Literal Keyword Frequency: 77
Contextual References: 0
Matching Lines: 2, 8, 36, 120, 148, 232, 260, 344, 372, 456, ... (77 total)
Snippet: Modern Web Technologies and Full-Stack Architecture: The Complete Guide

Rank 3: documents/dsa.txt
Relevance Score: 20.04
Literal Keyword Frequency: 47
Contextual References: 0
Matching Lines: 138, 567, 572, 574, 576, 579, 586, 596, 599, 601, ... (39 total)
Snippet: The Operational Execution Pipeline for Space Complexity and Call Stack Overhead:

Rank 4: documents/cpp.txt
Relevance Score: 20.02
Literal Keyword Frequency: 24
Contextual References: 0
Matching Lines: 634, 681, 686, 688, 690, 693, 696, 710, 713, 725, ... (20 total)
Snippet: Case Study Title: Eliminating Object Copies in RPC Stacks

========================================
```

### 12.3 Exact Phrase Search
```
Enter word or phrase to search: "data structure"

========================================
SEARCH RESULTS
========================================

Query: "data structure"

Rank 1: documents/data_structures.txt
Relevance Score: 50.06
Literal Keyword Frequency: 77
Contextual References: 0
Matching Lines: 7, 12, 14, 16, 19, 22, 36, 49, 59, 70, ... (77 total)
Snippet: Fundamental Abstract Data Types (ADTs): Definitions and Specifications

Rank 2: documents/dsa.txt
Relevance Score: 50.06
Literal Keyword Frequency: 77
Contextual References: 0
Matching Lines: 7, 12, 14, 16, 19, 22, 36, 49, 59, 70, ... (77 total)
Snippet: Fundamental Abstract Data Types (ADTs): Definitions and Specifications

========================================
```

---

## 13. Computational Complexity Analysis

| Operation | Time Complexity | Space Complexity | Description |
| :--- | :---: | :---: | :--- |
| **Document Tokenization** | $O(N \cdot L)$ | $O(N \cdot L)$ | Reading $N=10$ documents of length $L \approx 50,000$ tokens |
| **Inverted Index Construction** | $O(N \cdot L \cdot \log V)$ | $O(V + \text{tokens})$ | Inserting into self-balancing Red-Black BST (`std::map`) |
| **Vocabulary Term Lookup** | $O(\log V)$ | $O(1)$ | Red-Black Tree search for normalized root term ($V \approx 3,563$) |
| **Boolean AND Intersection** | $O(K \cdot \log V + \sum |P_i|)$ | $O(\min |P_i|)$ | Intersecting postings lists of $K$ query terms |
| **Boolean OR Union** | $O(K \cdot \log V + \sum |P_i|)$ | $O(\sum |P_i|)$ | Union of postings lists of $K$ query terms |
| **Exact Phrase Verification** | $O(|P_0| \cdot m)$ | $O(1)$ | Checking $m$ consecutive token positions in candidate documents |
| **Keyword Proximity** | $O(|P_1| \cdot \log |P_2|)$ | $O(1)$ | Binary search for closest token position |
| **Relevance Ranking & Sort** | $O(C \log C)$ | $O(C)$ | Sorting candidate documents ($C \le 10$) |

---

## 14. Known Limitations & Design Boundaries

1. **Terminal Single-Click Limitation**: As standard Windows terminal emulators (cmd, powershell, xterm) do not permit applications to capture single left-click events to open external GUI editor windows, the true mouse-clickable document opening is implemented cleanly via the included native VS Code Extension.
2. **Rule-Based Coreference**: The pronoun resolver uses deterministic subject-verb patterns (e.g., `X is ...` followed by `It is ...`). Complex anaphora, cataphora, and distant multi-paragraph pronouns are intentionally omitted to avoid false associations.
3. **Conservative Stemming**: The stemmer targets common inflectional suffixes (`ing`, `ed`, `es`, `s`, `ies`) and doubled consonants. Irregular verbs (e.g., `go`/`went`, `sing`/`sang`) retain their surface forms.
4. **No External Storage**: Indexing is purely in-memory; no external database engine is used.
5. **Text Format**: Optimized for plain text (`.txt`) documents only.

---

## 15. Project Verification Checklist

- [x] Exactly 10 `.txt` source documents in `documents/` folder.
- [x] Every document contains >= 50,000 words (50,811 - 51,100 words verified).
- [x] True mouse-clickable ranked search results interface for VS Code via included native extension.
- [x] Single normal left mouse click immediately opens the document in a VS Code text editor tab.
- [x] Terminal Document Viewer allows entering rank number to open full document text immediately.
- [x] High-contrast yellow background (`\033[43;30m`) highlights the **ENTIRE** text of every matching line.
- [x] Contrasting bold red text (`\033[1;31;43m`) additionally emphasizes query keywords within matching lines.
- [x] ANSI style reset (`\033[0m`) strictly terminates every line preventing style bleed.
- [x] Non-matching lines remain completely unhighlighted with standard formatting.
- [x] Matching line numbers strictly correspond to actual Search Engine calculations (`r.matchingLines`).
- [x] Paginated document navigation (N/P/Q) preserves line numbering and highlighting across pages.
- [x] Live real-time JSON feed (`output/search_results.json`) generated by C++ search engine.
- [x] Interactive Webview View and native Tree View in VS Code Activity Bar and Explorer sidebar.
- [x] Text normalization pipeline preserves `c++`, `c#`, `oop`, `dsa`, `html`, `sql`.
- [x] Conservative rule-based stemmer handles common suffixes while protecting keywords.
- [x] Standard stop words isolated from keyword indexing while maintaining token position offsets.
- [x] Rule-based pronoun resolution maps `it` and `they` to singular and plural antecedents without inflating literal frequency.
- [x] Positional inverted index records token positions, line numbers, and frequencies.
- [x] Boolean `AND` / `OR` query parsing with set intersection, union, and AND-before-OR precedence.
- [x] Exact phrase search with double quotes (`"..."`) verifying consecutive positions.
- [x] Bounded keyword proximity calculation using binary search.
- [x] Normalized Term Frequency using total document word count.
- [x] Multi-factor relevance ranking formula with deterministic alphabetical tie-breaking.
- [x] Paginated Index Viewer with N, P, J, S, Q navigation and safe return.
- [x] Main menu numbering preserved (1 to 4).
- [x] Automated unit test suite (`test_suite.cpp`) passing 16 test modules (100%).
- [x] Compiles cleanly with `g++ -std=c++17 -O2 -Wall -Wextra` on Windows.
