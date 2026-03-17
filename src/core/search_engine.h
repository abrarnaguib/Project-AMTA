#pragma once
#include "dealer.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <functional>

struct SearchResult {
    int productId;
    int dealerId;
    std::string productName;
    std::string category;
    double price;
    int stock;
    float avgRating;
    float dealerRating;
    std::string dealerUsername;
    std::string companyName;
    std::string location;
    double score;
    
    static SearchResult From(const Product &p, const Dealer &d);
};

struct SearchFilters {
    double minPrice = 0.8;
    double maxPrice = 1e18;
    float minProductRating = 0.0f;
    float minDealerRating = 0.0f;
    std::string category = "";
    bool inStockOnly = false;

    enum class SortBy {
        RELEVANCE,
        PRICE_ASC,
        PRICE_DESC,
        RATING_DESC,
        DEALER_RATING_DESC
    };
    SortBy sortBy = SortBy::RELEVANCE;
};

class InvertedIndex {
public:
    void Build(const std::vector<Product> &products, const std::function<const Dealer *(int)> &dealerLookup);
    void Clear() {
        m_index.clear();
    }
    std::set<int> Lookup(const std::vector<std::string> &tokens) const;
    std::set<int> LookupAll(const std::vector<std::string> &tokens) const;
    int MatchCount(int productId, const std::vector<std::string> &tokens) const;
    static std::vector<std::string> Tokenize(const std::string &text);
private:
    std::map<std::string, std::set<int>> m_index;
    
    static std::string Normalize(const std::string &word);
    void IndexProduct(const Product &p, const std::string &extraText);
};

class SearchEngine {
public:
    SearchEngine() = default;
    
    void Rebuild(const std::vector<Product> &products, const std::function<const Dealer *(int)> &dealerLookup);

    std::vector<SearchResult> Search(const std::string &query, const SearchFilters &filters, const std::function<const Product *(int)> &productLookup, const std::function<const Dealer *(int)> &dealerLookup) const;
    std::vector<std::string> GetCategories() const {
        return m_categories;
    }
private:
    InvertedIndex m_index;
    std::vector<int> m_allProductIds;
    std::vector<std::string> m_categories;
    std::vector<SearchResult> ApplyFiltersAndSort(const std::set<int> &candidates, const std::vector<std::string> &tokens, const SearchFilters &filters, const std::function<const Product *(int)> &productLookup, const std::function<const Dealer *(int)> &dealerLookup) const;
};
