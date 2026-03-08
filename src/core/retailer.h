#pragma once
#include "user.h"
#include "order.h"
#include "product.h"
#include <vector>

class Retailer : public User
{
private:
    std::string m_shopName;
    std::string m_location;
    std::vector<Order> m_orderHistory;
    std::vector<int> m_reviewedOrderIds;

public:
    Retailer(int userId, const std::string &username, const std::string &passwordHash, const std::string &shopName, const std::string &location);

    std::string Serialize() const override;

    // Getters
    UserRole GetRole() const override
    {
        return UserRole::RETAILER;
    }
    std::string GetDashboardInfo() const override;

    std::string GetShopName() const
    {
        return m_shopName;
    }
    std::string GetLocation() const
    {
        return m_location;
    }

    // Setters
    void SetShopName(const std::string &name);
    void SetLocation(const std::string &loc);

    // Orders
    Order PlaceOrder(int orderId, int dealerId, int productId, int quantity);

    void AddOrderToHistory(const Order &order);
    const std::vector<Order> &GetOrderHistory() const
    {
        return m_orderHistory;
    }

    // Reviews
    bool CanReviewOrder(int orderId) const;
    void MarkOrderReviewed(int orderId);
};