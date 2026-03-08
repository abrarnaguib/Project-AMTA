#pragma once
#include "user.h"
#include "product.h"
#include "order.h"
#include <vector>
#include <memory>

class Dealer : public User
{

private:
    std::string m_companyName;
    std::string m_location;
    std::vector<Product> m_products;
    std::vector<Order> m_orders;
    std::vector<Review> m_reviews;

    static void ValidateCompanyName(const std::string &name);

public:
    Dealer(int userId, const std::string &username,
           const std::string &passwordHash,
           const std::string &companyName,
           const std::string &location);

    // Overrides
    UserRole GetRole() const override { return UserRole::DEALER; }
    std::string GetDashboardInfo() const override;
    std::string Serialize() const override;

    // Getters
    std::string GetCompanyName() const { return m_companyName; }
    std::string GetLocation() const { return m_location; }
    float GetRating() const;
    const std::vector<Product> &GetProducts() const { return m_products; }
    const std::vector<Order> &GetOrders() const { return m_orders; }
    const std::vector<Review> &GetReviews() const { return m_reviews; }

    // Setters
    void SetCompanyName(const std::string &name);
    void SetLocation(const std::string &loc);

    // Product Management
    void AddProduct(const Product &product);
    void RemoveProduct(int productId);
    void UpdateProduct(int productId, double newPrice, int newStock);
    Product *FindProduct(int productId);

    // Order Management
    void AddIncomingOrder(const Order &order);
    void RespondToOrder(int orderId, bool accept);

    // Review management
    void AddReview(int reviewerId, int rating, const std::string &comment);
};