#pragma once
#include <string>
#include <vector>
#include <stdexcept>

class Product {
private:
    int m_productId;
    int m_dealerId;
    std::string m_name;
    std::string m_category;
    double m_price;
    int m_stock;
    std::vector<Review> m_reviews;  // Composition

    // Constraint Helpers
    static void ValidatePrice(double price);
    static void ValidateStock(int stock);
    static void ValidateName(const std::string& name);
public:
    Product(int productId, int dealerId, const std::string& name, const std::string& category, double price, int stock);
    
    // Getters
    int GetProductId() const { 
        return m_productId; 
    }
    int GetDealerId() const { 
        return m_dealerId; 
    }
    std::string GetName() const { 
        return m_name; 
    }
    std::string GetCategory() const { 
        return m_category; 
    }
    double GetPrice() const { 
        return m_price; 
    }
    int GetStock() const { 
        return m_stock; 
    }
    float GetAvgRating() const;

    // Setters
    void SetName(const std::string& name);
    void SetCategory(const std::string& category);
    void SetPrice(double price);
    void SetStock(int stock);

    // Stock Management
    void UpdateStock(int delta);
    void DeductStock(int quantity);

    // Review Management
    void AddReview(const Review& review);
    const std::vector<Review>& GetReviews() const { return m_reviews; }

    // Helper for database (.tsv file based)
    std::string Serialize()   const;
    static Product Deserialize(const std::string& line);

    // Debugging Helper
    std::string ToString() const;

};


struct Review {
    int reviewerId;
    std::string comment;
    int rating;

    std::string Serialize() const;
    static Review Deserialize(const std::string& line);
};

// Exceptions
class ProductException : public std::runtime_error {
public:
    explicit ProductException(const std::string& msg) : std::runtime_error(msg) {}
};