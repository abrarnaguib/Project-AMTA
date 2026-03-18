#include "search_engine.h"
#include <algorithm>
#include <cctype>
#include <unordered_map>

const std::vector<int> InvertedIndex::s_empty;

std::vector<std::string> InvertedIndex::Tokenize(const std::string &text) {
    std::vector<std::string> tokens;
    std::string current;
    for (unsigned char ch: text) {
        if (std::isalnum(ch)) {
            current += static_cast<char> (std::tolower(ch));
        }
        else {
            if (current.size() > 1) {
                tokens.push_back(current);
            }
            current.clear();
        }
    }
    if (current.size() > 1) {
        tokens.push_back(current);
    }
    return tokens;
}
void InvertedIndex::Build(const std::vector<Product> &products) {
    for (const auto &p: products) {
        if (p.GetProductId() <= 0) {
            throw SearchException("Cannot built search index: product " + p.GetName() + " has an invalid ID (" + std::to_string(p.GetProductId()) + "). All product IDs must be > 0.");
        }
    }
    m_index.clear();
    for (const auto &p: products) {
        int id = p.GetProductId();
        auto addTokens = [&](const std::string &text) {
            for (const auto &tok: Tokenize(text)) {
                auto &ids = m_index[tok];
                if (std::find(ids.begin(), ids.end(), id) == ids.end()) {
                    ids.push_back(id);
                }
            }
        };
        addTokens(p.GetName());
        addTokens(p.GetCategory());
    }
}

const std::vector<int> &InvertedIndex::Lookup(const std::string &token) const {
    auto it = m_index.find(token);
    if (it == m_index.end()) {
        return s_empty;
    }
    else {
        return it->second;
    }
}

void SearchEngine::Rebuild(const std::vector<Product> &products) {
    m_isBuilt = false;
    m_index.Build(products);
    m_isBuilt = true;
}