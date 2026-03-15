#include "dealer.h"
#include <sstream>
#include <algorithm>
#include <stdexcept>

Dealer::Dealer(int userId, const std::string &username,
               const std::string &passwordHash,
               const std::string &companyName,
               const std::string &location)
    : User(userId, username, passwordHash, UserRole::DEALER),
      m_companyName(companyName), m_location(location)
{
    ValidateCompanyName(companyName);
    if (location.empty())
        throw ValidationException("Dealer location cannot be empty.");
}

// Overrides
std::string Dealer::GetDashboardInfo() const
{
    std::ostringstream oss;
    oss << "=== Dealer Dashboard ===\n"
        << "Username : " << m_username << "\n"
        << "Company  : " << m_companyName << "\n"
        << "Location : " << m_location << "\n"
        << "Products : " << m_products.size() << "\n"
        << "Orders   : " << m_orders.size() << "\n"
        << "Rating   : " << GetRating() << "/5";
    return oss.str();
}

std::string Dealer::Serialize() const
{
    // Base fields + dealer-specific fields
    std::ostringstream oss;
    oss << User::Serialize() << "\t" << m_companyName << "\t" << m_location;
    return oss.str();
}

// setters
void Dealer::SetCompanyName(const std::string &name)
{
    ValidateCompanyName(name);
    m_companyName = name;
}

void Dealer::SetLocation(const std::string &loc)
{
    if (loc.empty())
        throw ValidationException("Location cannot be empty.");
    m_location = loc;
}

// Getters
float Dealer::GetRating() const
{
    if (m_reviews.empty())
        return 0.0f;
    float sum = 0;
    for (const auto &r : m_reviews)
        sum += r.rating;
    return sum / static_cast<float>(m_reviews.size());
}

// Product Management
void Dealer::AddProduct(const Product &product)
{
    // Prevent duplicate productId
    for (const auto &p : m_products)
        if (p.GetProductId() == product.GetProductId())
            throw ProductException("Product with this ID already exists in your catalogue.");
    m_products.push_back(product);
}

void Dealer::RemoveProduct(int productId)
{
    auto it = std::find_if(m_products.begin(), m_products.end(),
                           [productId](const Product &p)
                           { return p.GetProductId() == productId; });

    if (it == m_products.end())
        throw ProductException("Product ID " + std::to_string(productId) + " not found.");

    m_products.erase(it);
}

void Dealer::UpdateProduct(int productId, double newPrice, int newStock)
{
    Product *p = FindProduct(productId);
    if (!p)
        throw ProductException("Product ID " + std::to_string(productId) + " not found.");
    p->SetPrice(newPrice);
    p->SetStock(newStock);
}


// Order Management
void Dealer::AddIncomingOrder(const Order &order)
{
    m_orders.push_back(order);
}

void Dealer::RespondToOrder(int orderId, OrderStatus status)
{
    for (auto &o : m_orders)
    {
        if (o.GetOrderId() == orderId)
        {
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

// Review Management
void Dealer::AddReview(int reviewerId, int rating, const std::string &comment)
{
    if (rating < 1 || rating > 5)
        throw ValidationException("Rating must be between 1 and 5.");
    Review r;
    r.reviewerId = reviewerId;
    r.rating = rating;
    r.comment = comment;
    m_reviews.push_back(r);
}


// Helpers
void Dealer::ValidateCompanyName(const std::string &name)
{
    if (name.empty())
        throw ValidationException("Company name cannot be empty.");
    if (name.size() > 100)
        throw ValidationException("Company name too long (max 100 chars).");
}

Product *Dealer::FindProduct(int productId)
{
    for (auto &p : m_products)
        if (p.GetProductId() == productId)
            return &p;
    return nullptr;
}