#pragma once
#include "user.h"
#include "dealer.h"
#include "retailer.h"
#include "admin.h"
#include "product.h"
#include "order.h"
#include "notification.h"
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>

// custom exception class
class DatabaseException: public std::runtime_error {
public:
    explicit DatabaseException (const std::string &msg): std::runtime_error(msg) {

    }
};

// file-based persistence layer and saved as .tsv
class Database {
public:
    explicit Database(const std::string &dataDir = "../../data");
    
    // reads and writes files
    void LoadAll();
    void SaveAll() const;

    // user related operations
    Retailer* RegisterRetailer(const std::string &username, const std::string &password, const std::string &shopName, const std::string &location);
    Dealer* RegisterDealer(const std::string &username, const std::string &password, const std::string &companyName, const std::string &location);
    User* Login(const std::string &username, const std::string &password);
    User* FindUserById(int userId) const;
    User* FindUserByName(const std::string &username) const;
    Dealer* GetDealer(int userId) const;
    Dealer* GetDealer(User *user) const;
    Retailer* GetRetailer(int userId) const;
    Retailer* GetRetailer(User *user) const;

    // product related operations
    Product* AddProduct(int dealerId, const std::string &name, const std::string &category, double price, int stock);
    void DeleteProduct(int productId);
    Product* FindProduct(int productId) const;
    const std::vector<Product> &GetAllProducts() const {
        return m_products;
    }

    // order related operations
    Order* PlaceOrder(int retailerId, int dealerId, int productId, int quantity);
    void RespondToOrder(int orderId, int dealerId, bool accept);
    void CompleteOrder(int orderId, int retailerId);
    Order* FindOrder(int orderId) const;
    const std::vector<Order> &GetAllOrders() const  {
        return m_orders;
    }

    // notification related operations
    Notification* AddNotification(int recipientId, NotificationType type, int orderId, const std::string& msg);
    void MarkNotificationRead(int notificationId);
    const std::vector<Notification>& GetAllNotifications() const { return m_notifications; }

    // in the public section, under "product related operations"
    void SubmitReview(int retailerId, int orderId, int productId, int rating, const std::string& comment);
    std::string ReviewsFile() const;

    // in the private section
    void LoadReviews();
    void SaveReviews() const;

private:
    std::string m_dataDir;
    std::vector<std::unique_ptr<User>> m_users;
    std::vector<Product> m_products;
    std::vector<Order> m_orders;
    std::vector<Notification> m_notifications;

    // tracks increments
    int m_nextUserId = 1;
    int m_nextProductId = 1;
    int m_nextOrderId = 1;
    int m_nextNotificationId = 1;

    // helper functions: returns the file paths only
    std::string UsersFile() const; // deals with retailers, dealers, admins 
    std::string ProductsFile() const; // deals with all the product items
    std::string OrdersFile() const; // deals with all the placed orders
    std::string NotificationsFile() const; // deals with all the notifications

    void LoadUsers();
    void LoadProducts();
    void LoadOrders();
    void LoadNotifications();
    void SaveUsers() const;
    void SaveProducts() const;
    void SaveOrders() const;
    void SaveNotifications() const;

    bool UsernameExists(const std::string &username) const;
    void EnsureDataDir() const;
};