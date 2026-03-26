#pragma once
#include "database.h"
#include <string>
#include <vector>

struct AppState
{   
    // Session
    bool isLoggedIn = false;
    User *currentUser = nullptr;

    // UI page control
    enum class Page
    {
        HOME,
        LOGIN,
        REGISTER,
        DASHBOARD,
        PRODUCT_LIST,
        PLACE_ORDER,
    };
    Page currentPage = Page::HOME;
    
    // Feedback messages shown in GUI
    std::string infoMessage;
    std::string errorMessage;

    void ClearMessages()
    {
        infoMessage.clear();
        errorMessage.clear();
    }
};

class App
{
private:
    Database m_db;
    AppState m_state;
    
public:
    App();

    void Update(); // called every frame by main loop

    // Auth operations (called by GUI)
    bool Login(const std::string &username, const std::string &password);
    bool Register(const std::string &username, const std::string &password, const std::string &role, const std::string &extraName, const std::string &location);
    void Logout();

    // Product operations
    bool AddProduct(const std::string &name, const std::string &category, double price, int stock);
    bool DeleteProduct(int productId);
    bool UpdateProduct(int productId, double newPrice, int newStock);

    // Order operations
    bool PlaceOrder(int productId, int quantity);
    bool AcceptOrder(int orderId);
    bool RejectOrder(int orderId);
    bool CompleteOrder(int orderId);
    bool SubmitReview(int orderId, int productId, int rating, const std::string& comment);

    // Notification actions
    int UnreadNotificationCount() const;
    void MarkNotificationRead(int notificationId);
    bool SendMessage(int recipientId, const std::string &msg);   // for future messaging
    std::vector<const Notification*> GetNotificationsForUser() const; // for current logged-in user
    
    // updated: Search actions
    std::vector<SearchResult> SearchProducts (const std::string &query, const SearchFilters &filters) const;

    // Accessors
    AppState &GetState() { return m_state; }
    Database &GetDatabase() { return m_db; }
};