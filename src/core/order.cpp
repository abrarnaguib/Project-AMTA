#include "order.h"
#include <sstream>

std::string OrderStatusToString(OrderStatus status)
{
    switch (status)
    {
    case OrderStatus::PENDING:
        return "PENDING";
    case OrderStatus::ACCEPTED:
        return "ACCEPTED";
    case OrderStatus::REJECTED:
        return "REJECTED";
    case OrderStatus::COMPLETED:
        return "COMPLETED";
    }
    return "UNKNOWN";
}

OrderStatus StringToOrderStatus(const std::string &str)
{
    if (str == "PENDING")
        return OrderStatus::PENDING;
    if (str == "ACCEPTED")
        return OrderStatus::ACCEPTED;
    if (str == "REJECTED")
        return OrderStatus::REJECTED;
    if (str == "COMPLETED")
        return OrderStatus::COMPLETED;
    throw OrderException("Unknown order status: " + str);
}

void Order::ValidateQuantity(int qty)
{
    if (qty <= 0)
        throw OrderException("Order quantity must be at least 1.");
    if (qty > 10000)
        throw OrderException("Order quantity exceeds maximum (10000).");
}

Order::Order(int orderId, int retailerId, int dealerId,
             int productId, int quantity)
    : m_orderId(orderId), m_retailerId(retailerId), m_dealerId(dealerId),
      m_productId(productId), m_quantity(quantity), m_status(OrderStatus::PENDING)
{
    ValidateQuantity(quantity);
    if (retailerId <= 0)
        throw OrderException("Invalid retailer ID.");
    if (dealerId <= 0)
        throw OrderException("Invalid dealer ID.");
    if (productId <= 0)
        throw OrderException("Invalid product ID.");
}

void Order::Accept()
{
    if (m_status != OrderStatus::PENDING)
        throw OrderException("Only PENDING orders can be accepted.");
    m_status = OrderStatus::ACCEPTED;
}

void Order::Reject()
{
    if (m_status != OrderStatus::PENDING)
        throw OrderException("Only PENDING orders can be rejected.");
    m_status = OrderStatus::REJECTED;
}

void Order::Complete()
{
    if (m_status != OrderStatus::ACCEPTED)
        throw OrderException("Only ACCEPTED orders can be marked complete.");
    m_status = OrderStatus::COMPLETED;
}

std::string Order::Serialize() const
{
    std::ostringstream oss;
    oss << m_orderId << "\t" << m_retailerId << "\t"
        << m_dealerId << "\t" << m_productId << "\t"
        << m_quantity << "\t" << OrderStatusToString(m_status);
    return oss.str();
}

Order Order::Deserialize(const std::string &line)
{
    std::istringstream iss(line);
    std::string token;
    int oid, rid, did, pid, qty;
    std::string statusStr;

    std::getline(iss, token, '\t');
    oid = std::stoi(token);
    std::getline(iss, token, '\t');
    rid = std::stoi(token);
    std::getline(iss, token, '\t');
    did = std::stoi(token);
    std::getline(iss, token, '\t');
    pid = std::stoi(token);
    std::getline(iss, token, '\t');
    qty = std::stoi(token);
    std::getline(iss, statusStr);

    Order o(oid, rid, did, pid, qty);
    o.m_status = StringToOrderStatus(statusStr);
    return o;
}

std::string Order::ToString() const
{
    std::ostringstream oss;
    oss << "Order[" << m_orderId << "]"
        << " Product:" << m_productId
        << " Qty:" << m_quantity
        << " Status:" << OrderStatusToString(m_status);
    return oss.str();
}