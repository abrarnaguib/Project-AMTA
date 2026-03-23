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

std::vector<SearchResult> SearchEngine::Search (const std::string &query, const SearchFilters &filters, const std::vector<Product> &products) const {
    if (!m_isBuilt) {
        throw SearchNotBuiltException();
    }
    if (filters.minPrice < 0.0) {
        throw SearchFilterException("minPrice cannot be negative (got " + std::to_string(filters.minPrice) + ").\n");
    }
    if (filters.maxPrice < 0.0) {
        throw SearchFilterException("maxPrice cannot be negative (got " + std::to_string(filters.maxPrice) + ").\n");
    }
    if (filters.minPrice > filters.maxPrice) {
        throw SearchFilterException("minPrice (" + std::to_string(filters.minPrice) + ") cannot exceed maxPrice (" + std::to_string(filters.maxPrice) + ").\n");
    }
    if (filters.minRating < 0.0f or filters.minRating > 5.0f) {
        throw SearchFilterException("minRating must be between 0 and 5 (got " + std::to_string(filters.minRating) + ").\n");
    }

    std::unordered_map<int, int> scoreMap;
    const std::vector<std::string> queryTokens = InvertedIndex::Tokenize(query);
    const bool queryIsBlank = queryTokens.empty();

    if (!queryIsBlank) {
        for (const auto &tok: queryTokens) {
            for (int id: m_index.Lookup(tok)) {
                ++scoreMap[id];
            }
        }
    }
    else {
        for (const auto &p: products) {
            scoreMap[p.GetProductId()] = 0;
        }
    }

    std::unordered_map<int, const Product *> idMap;
    idMap.reserve(products.size());
    for (const auto &p: products) {
        idMap[p.GetProductId()] = &p;
    }

    std::vector<SearchResult> results;
    results.reserve(scoreMap.size());
    
    for (const auto &[id, score]: scoreMap) {
        if (!queryIsBlank and score == 0) {
            continue;
        }
        auto it = idMap.find(id);
        if (it == idMap.end()) {
            continue;
        }

        const Product *p = it->second;

        if (p->GetPrice() < filters.minPrice or p->GetPrice() > filters.maxPrice) {
            continue;
        }
        
        float avg = p->GetAvgRating();
        if (avg < filters.minRating) {
            continue;
        }
        results.push_back({p, score, avg});
    }
    
    std::sort(results.begin(), results.end(), [] (const SearchResult &a, const SearchResult &b) {
        if (a.matchScore != b.matchScore) {
            return a.matchScore > b.matchScore;
        }
        return a.avgRating > b.avgRating;
    });
    return results;
}