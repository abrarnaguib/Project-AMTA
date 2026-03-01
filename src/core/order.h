#pragma once
#include <string>
#include <stdexcept>

// Operations
enum class OrderStatus {
    PENDING,
    ACCEPTED,
    REJECTED,
    COMPLETED
};

class Order {
private:
    int m_orderId;
    int m_retailerId;
    int m_dealerId;
    int m_productId;
    int m_quantity;
    OrderStatus m_status;

    // Constraint Helpers
    static void ValidateQuantity(int qty);

public:
    Order(int orderId, int retailerId, int dealerId, int productId, int quantity);

    // Order Operations
    void Accept();
    void Reject();
    void Complete();

    // Getters
    int GetOrderId() const { 
        return m_orderId; 
    }
    int GetRetailerId() const { 
        return m_retailerId; 
    }
    int GetDealerId() const { 
        return m_dealerId; 
    }
    int GetProductId() const { 
        return m_productId; 
    }
    int GetQuantity() const { 
        return m_quantity; 
    }
    OrderStatus GetStatus() const { 
        return m_status; 
    }
    std::string GetStatusStr() const { 
        return OrderStatusToString(m_status); 
    }

    // Helper for database (.tsv file based)
    std::string Serialize() const;
    static Order Deserialize(const std::string &line);

    // Debugging Helper
    std::string ToString() const;
};

// Helper Functions (data type converter)
std::string OrderStatusToString(OrderStatus status);
OrderStatus StringToOrderStatus(const std::string &str);

// Exceptions
class OrderException : public std::runtime_error {
public:
    explicit OrderException(const std::string &msg) : std::runtime_error(msg) {}
};