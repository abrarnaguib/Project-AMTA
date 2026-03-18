#pragma once
#include"review.h"
#include <string>
#include <stdexcept>

// Operations
enum class OrderStatus {
    PENDING,
    ACCEPTED,
    REJECTED,
    COMPLETED
};

// Helper Functions (data type converter)
std::string OrderStatusToString(OrderStatus status);
OrderStatus StringToOrderStatus(const std::string &str);

class Order {
private:
    int m_orderId;
    int m_retailerId;
    int m_dealerId;
    int m_productId;
    int m_quantity;
    OrderStatus m_status;
    bool m_isReviewed;   // persisted via reviews.tsv, not orders.tsv
    Review m_review;

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
    bool IsReviewed() const { 
        return m_isReviewed; 
    }
    Review GetReview() const { 
        return m_review; 
    }

    // Setters
    void SetReviewed(bool val) { 
        m_isReviewed = val; 
    }
    void SetReview(Review r) { 
        m_review = r; 
    }

    // Helper for database (.tsv file based)
    std::string Serialize() const;
    static Order Deserialize(const std::string &line);

    // Debugging Helper
    std::string ToString() const;
};



// Exceptions
class OrderException : public std::runtime_error {
public:
    explicit OrderException(const std::string &msg) : std::runtime_error(msg) {}
};