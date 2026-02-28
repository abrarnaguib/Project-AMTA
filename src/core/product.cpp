#include "product.h"
#include <sstream>
#include <numeric>
#include <algorithm>
#include <iomanip>

std::string Review::Serialize() const
{
    std::ostringstream oss;
    std::string safe = comment;
    std::replace(safe.begin(), safe.end(), '|', ' ');
    oss << reviewerId << "|" << rating << "|" << safe;
    return oss.str();
}

Review Review::Deserialize(const std::string &line)
{
    std::istringstream iss(line);
    std::string token;
    Review r;
    std::getline(iss, token, '|');
    r.reviewerId = std::stoi(token);
    std::getline(iss, token, '|');
    r.rating = std::stoi(token);
    std::getline(iss, token);
    r.comment = token;
    return r;
}

void Product::ValidatePrice(double price)
{
    if (price < 0.0)
        throw ProductException("Price cannot be negative.");
}

void Product::ValidateStock(int stock)
{
    if (stock < 0)
        throw ProductException("Stock cannot be negative.");
}

void Product::ValidateName(const std::string &name)
{
    if (name.empty())
        throw ProductException("Product name cannot be empty.");
    if (name.size() > 120)
        throw ProductException("Product name too long (max 120 chars).");
}

Product::Product(int productId, int dealerId, const std::string &name,
                 const std::string &category, double price, int stock)
    : m_productId(productId), m_dealerId(dealerId),
      m_name(name), m_category(category), m_price(price), m_stock(stock)
{
    ValidateName(name);
    ValidatePrice(price);
    ValidateStock(stock);
}

void Product::SetName(const std::string &name)
{
    ValidateName(name);
    m_name = name;
}

void Product::SetCategory(const std::string &category)
{
    if (category.empty())
        throw ProductException("Category cannot be empty.");
    m_category = category;
}

void Product::SetPrice(double price)
{
    ValidatePrice(price);
    m_price = price;
}

void Product::SetStock(int stock)
{
    ValidateStock(stock);
    m_stock = stock;
}

void Product::UpdateStock(int delta)
{
    int newStock = m_stock + delta;
    if (newStock < 0)
        throw ProductException("Stock cannot go below zero.");
    m_stock = newStock;
}

void Product::DeductStock(int quantity)
{
    if (quantity <= 0)
        throw ProductException("Quantity to deduct must be positive.");
    if (quantity > m_stock)
        throw ProductException("Insufficient stock for product: " + m_name);
    m_stock -= quantity;
}

void Product::AddReview(const Review &review)
{
    if (review.rating < 1 || review.rating > 5)
        throw ProductException("Rating must be between 1 and 5.");
    m_reviews.push_back(review);
}

float Product::GetAvgRating() const
{
    if (m_reviews.empty())
        return 0.0f;
    float sum = 0;
    for (const auto &r : m_reviews)
        sum += r.rating;
    return sum / static_cast<float>(m_reviews.size());
}

std::string Product::Serialize() const
{
    std::ostringstream oss;
    oss << m_productId << "\t" << m_dealerId << "\t"
        << m_name << "\t" << m_category << "\t"
        << std::fixed << std::setprecision(2) << m_price << "\t"
        << m_stock;
    return oss.str();
}

Product Product::Deserialize(const std::string &line)
{
    std::istringstream iss(line);
    std::string token;
    int pid, did, stock;
    double price;
    std::string name, category;

    std::getline(iss, token, '\t');
    pid = std::stoi(token);
    std::getline(iss, token, '\t');
    did = std::stoi(token);
    std::getline(iss, name, '\t');
    std::getline(iss, category, '\t');
    std::getline(iss, token, '\t');
    price = std::stod(token);
    std::getline(iss, token);
    stock = std::stoi(token);

    return Product(pid, did, name, category, price, stock);
}

std::string Product::ToString() const
{
    std::ostringstream oss;
    oss << "[" << m_productId << "] " << m_name
        << " | Category: " << m_category
        << " | Price: " << std::fixed << std::setprecision(2) << m_price
        << " BDT | Stock: " << m_stock
        << " | Rating: " << std::setprecision(1) << GetAvgRating() << "/5";
    return oss.str();
}