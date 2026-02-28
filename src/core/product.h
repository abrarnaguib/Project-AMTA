#pragma once
#include <string>
#include <vector>
#include <stdexcept>

class ProductException : public std::runtime_error {
public:
    explicit ProductException(const std::string& msg) : std::runtime_error(msg) {}
};

struct Review {
    int         reviewerId;
    std::string comment;
    int         rating;

    std::string Serialize() const;
    static Review Deserialize(const std::string& line);
};


class Product {
public:
    Product(int productId, int dealerId, const std::string& name,
            const std::string& category, double price, int stock);

    int         GetProductId()  const { return m_productId; }
    int         GetDealerId()   const { return m_dealerId; }
    std::string GetName()       const { return m_name; }
    std::string GetCategory()   const { return m_category; }
    double      GetPrice()      const { return m_price; }
    int         GetStock()      const { return m_stock; }
    float       GetAvgRating()  const;

    void SetName(const std::string& name);
    void SetCategory(const std::string& category);
    void SetPrice(double price);
    void SetStock(int stock);

    void UpdateStock(int delta);
    void DeductStock(int quantity);

    void AddReview(const Review& review);
    const std::vector<Review>& GetReviews() const { return m_reviews; }

    std::string Serialize()   const;
    static Product Deserialize(const std::string& line);

    std::string ToString() const;

private:
    int         m_productId;
    int         m_dealerId;
    std::string m_name;
    std::string m_category;
    double      m_price;
    int         m_stock;
    std::vector<Review> m_reviews;

    static void ValidatePrice(double price);
    static void ValidateStock(int stock);
    static void ValidateName(const std::string& name);
};