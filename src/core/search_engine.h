#pragma once
#include "product.h" 
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <stdexcept>

struct SearchFilters {
    double minPrice = 0.0;
    double maxPrice = 1e9;
    float minRating = 0.0f;
};
struct SearchResult {
    const Product *product = nullptr;
    int matchScore = 0;
    float avgRating = 0.0f;
};

// exception class for search related errors
class SearchException: public std::runtime_error {
public:
    explicit SearchException(const std::string &msg): std::runtime_error(msg) {

    }
};

// throws error when Search() is called before Rebuild()
class SearchNotBuiltException: public SearchException {
public:
    SearchNotBuiltException(): SearchException("SearchEngine::Search() called before Rebuild(). Call Rebuild(products) at startup and after every catalogue change.") {

    }
};

// throws error when SearchFilters value is logically incorrect
class SearchFilterException: public SearchException {
public:
    explicit SearchFilterException(const std::string &msg): SearchException("Invalid search filter: " + msg) {

    }
};

class InvertedIndex {
public:

    // clears and rebuilds the index from the given product catalogus
    void Build(const std::vector<Product> &products);
    
    // returns the product ID whose name or category contains this token 
    const std::vector<int> &Lookup(const std::string &token) const;

    // splits text into lowercase alphanumeric tokens
    static std::vector<std::string> Tokenize(const std::string &text);

private:
    std::map<std::string, std::vector<int>> m_index;
    static const std::vector<int> s_empty;
};

class SearchEngine {
public:
    
    // rebuilds the InvertedIndex
    void Rebuild(const std::vector<Product> &products);
    
    // returns ranked list of SearchResults
    std::vector<SearchResult> Search(const std::string &query, const SearchFilters &filters, const std::vector<Product> &products) const;
private:
    InvertedIndex m_index;
    bool m_isBuilt = false;
};
