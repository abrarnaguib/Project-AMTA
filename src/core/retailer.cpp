#include "retailer.h"
#include <sstream>
#include <algorithm>

// constructor
Retailer::Retailer(int userId, const std::string &username,
                   const std::string &passwordHash,
                   const std::string &shopName,
                   const std::string &location)
    : User(userId, username, passwordHash, UserRole::RETAILER),
      m_shopName(shopName), m_location(location)
{
    if (shopName.empty())
        throw ValidationException("Shop name cannot be empty.");
    if (location.empty())
        throw ValidationException("Location cannot be empty.");
}

// Override
std::string Retailer::GetDashboardInfo() const
{
    std::ostringstream oss;
    oss << "=== Retailer Dashboard ===\n"
        << "Username  : " << m_username << "\n"
        << "Shop      : " << m_shopName << "\n"
        << "Location  : " << m_location << "\n"
        << "Orders    : " << m_orderHistory.size();
    return oss.str();
}

std::string Retailer::Serialize() const
{
    std::ostringstream oss;
    oss << User::Serialize() << "\t" << m_shopName << "\t" << m_location;
    return oss.str();
}

// Setter
void Retailer::SetShopName(const std::string &name)
{
    if (name.empty())
        throw ValidationException("Shop name cannot be empty.");
    m_shopName = name;
}

void Retailer::SetLocation(const std::string &loc)
{
    if (loc.empty())
        throw ValidationException("Location cannot be empty.");
    m_location = loc;
}

// Orders
Order Retailer::PlaceOrder(int orderId, int dealerId, int productId, int quantity)
{
    Order newOrder(orderId, m_userId, dealerId, productId, quantity);
    m_orderHistory.push_back(newOrder);
    return newOrder;
}

void Retailer::AddOrderToHistory(const Order &order)
{
    m_orderHistory.push_back(order);
}

void Retailer::RespondToOrder(int orderId, OrderStatus status)
{
    for (auto &o : m_orderHistory)
    {
        if (o.GetOrderId() == orderId) {
            if (status == OrderStatus::ACCEPTED)
                o.Accept();
            else if (status == OrderStatus::REJECTED)
                o.Reject();
            else if (status == OrderStatus::COMPLETED) 
                o.Complete();
            return;
        }
    }
    throw OrderException("Order ID " + std::to_string(orderId) + " not found.");
}

// Tracking Review
void Retailer::MarkOrderReviewed(int orderId)
{
    if (!CanReviewOrder(orderId))
        throw OrderException("Cannot review order " + std::to_string(orderId) +
                             " (not completed or already reviewed).");
    m_reviewedOrderIds.push_back(orderId);
}

bool Retailer::CanReviewOrder(int orderId) const
{
    bool completed = false;
    for (const auto &o : m_orderHistory)
        if (o.GetOrderId() == orderId && o.GetStatus() == OrderStatus::COMPLETED)
            completed = true;

    if (!completed)
        return false;

    return std::find(m_reviewedOrderIds.begin(), m_reviewedOrderIds.end(), orderId) == m_reviewedOrderIds.end();
}