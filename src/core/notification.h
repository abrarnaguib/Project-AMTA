#pragma once
#include <string>
#include <sstream>
#include <stdexcept>

// needed before class as otherwise will run in compilation error that "error: ‘NotificationException’ was not declared in this scope"
class NotificationException : public std::runtime_error
{
public:
    explicit NotificationException(const std::string &msg) : std::runtime_error(msg) {}
};

// Notification Types
enum class NotificationType
{
    ORDER_PLACED,
    ORDER_ACCEPTED,
    ORDER_REJECTED,
    ORDER_COMPLETED
};

std::string NotificationTypeToString(NotificationType type);
NotificationType StringToNotificationType(const std::string &str);

class Notification
{
private:
    int m_notificationId;
    int m_recipientUserId;
    NotificationType m_type;
    int m_orderId;
    std::string m_message;
    bool m_isRead;

    // Constraint Helpers
    static void ValidateId(int id, const std::string &fieldName);
    static void ValidateMessage(const std::string &msg);

public:
    Notification(int nid, int recipientId, NotificationType t, int oid, const std::string &msg);

    int GetNotificationId() const { 
        return m_notificationId; 
    }
    int GetRecipientUserId() const { 
        return m_recipientUserId; 
    }
    NotificationType GetType() const { 
        return m_type; 
    }
    int GetOrderId() const { 
        return m_orderId; 
    }
    std::string GetMessage() const { 
        return m_message; 
    }
    bool IsRead() const { 
        return m_isRead; 
    }

    void MarkAsRead() { 
        m_isRead = true; 
    }

    std::string Serialize() const;
    static Notification Deserialize(const std::string &line);
};