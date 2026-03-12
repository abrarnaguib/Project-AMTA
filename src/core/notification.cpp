#include "notification.h"

std::string Notification::Serialize() const
{
    std::ostringstream oss;
    oss << notificationId << "\t"
        << recipientUserId << "\t"
        << NotificationTypeToString(type) << "\t"
        << orderId << "\t"
        << (isRead ? "1" : "0") << "\t"
        << message;
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
    n.isRead = read;
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
    throw NotificationException("Unknown notification type: " + str);
}