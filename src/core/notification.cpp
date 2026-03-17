#include "notification.h"

void Notification::ValidateId(int id, const std::string &fieldName)
{
    if (id <= 0) {
        throw NotificationException("Invalid " + fieldName + ".");
    }
}

void Notification::ValidateMessage(const std::string &msg)
{
    if (msg.empty()) {
        throw NotificationException("Notification message cannot be empty.");
    }
}

Notification::Notification(int nid, int recipientId, NotificationType t, int oid, const std::string &msg)
    : m_notificationId(nid), m_recipientUserId(recipientId), m_type(t), m_orderId(oid), m_message(msg), m_isRead(false)
{
    ValidateId(nid, "notification ID");
    ValidateId(recipientId, "recipient ID");
    if (oid != -1) ValidateId(oid, "order ID");   // -1 is valid (no order)
    ValidateMessage(msg);
}

std::string Notification::Serialize() const
{
    std::ostringstream oss;
    oss << m_notificationId << "\t" << m_recipientUserId << "\t" << NotificationTypeToString(m_type) << "\t"
        << m_orderId << "\t" << (m_isRead ? "1" : "0") << "\t" << m_message;
    return oss.str();
}

Notification Notification::Deserialize(const std::string &line)
{
    std::istringstream iss(line);
    std::string token;
    int nid, rid, oid;
    std::string typeStr, msg;
    bool read;

    std::getline(iss, token, '\t');
    nid = std::stoi(token);
    std::getline(iss, token, '\t');
    rid = std::stoi(token);
    std::getline(iss, typeStr, '\t');
    std::getline(iss, token, '\t');
    oid = std::stoi(token);
    std::getline(iss, token, '\t');
    read = (token == "1");
    std::getline(iss, msg);

    Notification n(nid, rid, StringToNotificationType(typeStr), oid, msg);
    n.m_isRead = read;
    return n;
}

std::string NotificationTypeToString(NotificationType type)
{
    switch (type)
    {
    case NotificationType::ORDER_PLACED:
        return "ORDER_PLACED";
    case NotificationType::ORDER_ACCEPTED:
        return "ORDER_ACCEPTED";
    case NotificationType::ORDER_REJECTED:
        return "ORDER_REJECTED";
    case NotificationType::ORDER_COMPLETED:
        return "ORDER_COMPLETED";
    case NotificationType::MESSAGE:
        return "MESSAGE";
    }
    return "UNKNOWN";
}

NotificationType StringToNotificationType(const std::string &str)
{
    if (str == "ORDER_PLACED")
        return NotificationType::ORDER_PLACED;
    if (str == "ORDER_ACCEPTED")
        return NotificationType::ORDER_ACCEPTED;
    if (str == "ORDER_REJECTED")
        return NotificationType::ORDER_REJECTED;
    if (str == "ORDER_COMPLETED")
        return NotificationType::ORDER_COMPLETED;
    if (str == "MESSAGE")
        return NotificationType::MESSAGE;
    throw NotificationException("Unknown notification type: " + str);
}