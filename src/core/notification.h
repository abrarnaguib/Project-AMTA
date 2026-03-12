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

struct Notification
{
    int notificationId;
    int recipientUserId;
    NotificationType type;
    int orderId;
    std::string message;
    bool isRead;

    Notification(int nid, int recipientId, NotificationType t,
                 int oid, const std::string &msg)
        : notificationId(nid), recipientUserId(recipientId),
          type(t), orderId(oid), message(msg), isRead(false) {}

        std::string Serialize() const;
    static Notification Deserialize(const std::string &line);
};