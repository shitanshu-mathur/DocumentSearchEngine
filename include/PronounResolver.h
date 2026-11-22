#ifndef PRONOUN_RESOLVER_H
#define PRONOUN_RESOLVER_H

#include <string>
#include <vector>
#include <map>

struct ContextualMatch {
    int docId;
    int referenceCount = 0;
    std::vector<int> lineNumbers;
};

class PronounResolver {
public:
    PronounResolver() = default;

    static void resolveDocument(int docId,
                                const std::vector<std::string>& lines,
                                std::map<std::string, std::vector<ContextualMatch>>& contextualIndex);
};

#endif
