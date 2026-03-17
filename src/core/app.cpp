#include "app.h"
#include <iostream>

App::App() : m_db("../../data")
{
    try
    {
        m_db.LoadAll();
    }
    catch (const std::exception &e)
    {
        std::cerr << "[App] Database load error: " << e.what() << "\n";
    }
}

void App::Update()
{
    // Per-frame logic
}

// Auth
bool App::Login(const std::string &username, const std::string &password)
{
    m_state.ClearMessages();
    try
    {
        m_state.currentUser = m_db.Login(username, password);
        m_state.isLoggedIn = true;
        m_state.currentPage = AppState::Page::DASHBOARD;
        m_state.infoMessage = "Welcome, " + username + "!";
        return true;
    }
    catch (const std::exception &e)
    {
        m_state.errorMessage = e.what();
        return false;
    }
}

bool App::Register(const std::string &username, const std::string &password,
                   const std::string &role, const std::string &extraName,
                   const std::string &location)
{
    m_state.ClearMessages();
    try
    {
        if (role == "RETAILER")
        {
            m_db.RegisterRetailer(username, password, extraName, location);
        }
        else if (role == "DEALER")
        {
            m_db.RegisterDealer(username, password, extraName, location);
        }
        else
        {
            m_state.errorMessage = "Invalid role selected.";
            return false;
        }
        m_state.infoMessage = "Registration successful! Please log in.";
        m_state.currentPage = AppState::Page::LOGIN;
        return true;
    }
    catch (const std::exception &e)
    {
        m_state.errorMessage = e.what();
        return false;
    }
}

void App::Logout()
{
    m_state.isLoggedIn = false;
    m_state.currentUser = nullptr;
    m_state.currentPage = AppState::Page::HOME;
    m_state.infoMessage = "Logged out.";
}

// Products
bool App::AddProduct(const std::string &name, const std::string &category,
                     double price, int stock)
{
    m_state.ClearMessages();
    if (!m_state.isLoggedIn || m_state.currentUser->GetRole() != UserRole::DEALER)
    {
        m_state.errorMessage = "Only logged-in dealers can add products.";
        return false;
    }
    try
    {
        m_db.AddProduct(m_state.currentUser->GetUserId(), name, category, price, stock);
        m_state.infoMessage = "Product '" + name + "' added.";
        return true;
    }
    catch (const std::exception &e)
    {
        m_state.errorMessage = e.what();
        return false;
    }
}

bool App::DeleteProduct(int productId)
{
    m_state.ClearMessages();
    try
    {
        m_db.DeleteProduct(productId);
        m_state.infoMessage = "Product deleted.";
        return true;
    }
    catch (const std::exception &e)
    {
        m_state.errorMessage = e.what();
        return false;
    }
}

bool App::UpdateProduct(int productId, double newPrice, int newStock)
{
    m_state.ClearMessages();
    if (!m_state.isLoggedIn || m_state.currentUser->GetRole() != UserRole::DEALER)
    {
        m_state.errorMessage = "Only dealers can update products.";
        return false;
    }
    try
    {
        Dealer *d = m_db.GetDealer(m_state.currentUser->GetUserId());
        if (!d)
        {
            m_state.errorMessage = "Dealer not found.";
            return false;
        }
        d->UpdateProduct(productId, newPrice, newStock);
        m_db.SaveAll();
        m_state.infoMessage = "Product updated.";
        return true;
    }
    catch (const std::exception &e)
    {
        m_state.errorMessage = e.what();
        return false;
    }
}

// Orders
bool App::PlaceOrder(int productId, int quantity)
{
    m_state.ClearMessages();
    if (!m_state.isLoggedIn || m_state.currentUser->GetRole() != UserRole::RETAILER)
    {
        m_state.errorMessage = "Only logged-in retailers can place orders.";
        return false;
    }
    try
    {
        Product *p = m_db.FindProduct(productId);
        if (!p)
        {
            m_state.errorMessage = "Product not found.";
            return false;
        }
        Order* o = m_db.PlaceOrder(m_state.currentUser->GetUserId(), p->GetDealerId(), productId, quantity);

        // Notifies delaer of new order that has been placed on their product
        m_db.AddNotification(
            p->GetDealerId(),
            NotificationType::ORDER_PLACED,
            o->GetOrderId(),
            "New order #" + std::to_string(o->GetOrderId()) + ": " +
            std::to_string(quantity) + "x '" + p->GetName() +
            "' from '" + m_state.currentUser->GetUsername() + "'."
        );

        m_state.infoMessage = "Order placed successfully!";
        return true;
    }
    catch (const std::exception &e)
    {
        m_state.errorMessage = e.what();
        return false;
    }
}

bool App::AcceptOrder(int orderId)
{
    m_state.ClearMessages();
    if (!m_state.isLoggedIn || m_state.currentUser->GetRole() != UserRole::DEALER)
    {
        m_state.errorMessage = "Only dealers can accept orders.";
        return false;
    }
    try
    {
        Order* o = m_db.FindOrder(orderId);
        Product* p = m_db.FindProduct(o->GetProductId());
        std::string productName = p->GetName();

        m_db.RespondToOrder(orderId, m_state.currentUser->GetUserId(), true);

        // Notifies the retailer that their order has been accepted by the dealer
        m_db.AddNotification(
            o->GetRetailerId(),
            NotificationType::ORDER_ACCEPTED,
            orderId,
            "Your order #" + std::to_string(orderId) +
            " for '" + productName + "' was ACCEPTED by '" +
            m_state.currentUser->GetUsername() + "'."
        );

        m_state.infoMessage = "Order accepted.";
        return true;
    }
    catch (const std::exception &e)
    {
        m_state.errorMessage = e.what();
        return false;
    }
}

bool App::RejectOrder(int orderId)
{
    m_state.ClearMessages();
    if (!m_state.isLoggedIn || m_state.currentUser->GetRole() != UserRole::DEALER)
    {
        m_state.errorMessage = "Only dealers can reject orders.";
        return false;
    }
    try
    {
        Order* o = m_db.FindOrder(orderId);
        Product* p = m_db.FindProduct(o->GetProductId());
        std::string productName = p->GetName();

        m_db.RespondToOrder(orderId, m_state.currentUser->GetUserId(), false);
        
        // add the ordered amount to stock
        if (p) p->SetStock(p->GetStock() + o->GetQuantity());

        // Notifies the retailer that their order has been rejected by the dealer
        m_db.AddNotification(
            o->GetRetailerId(),
            NotificationType::ORDER_REJECTED,
            orderId,
            "Your order #" + std::to_string(orderId) +
            " for '" + productName + "' was REJECTED by '" +
            m_state.currentUser->GetUsername() + "'."
        );

        m_state.infoMessage = "Order rejected.";
        return true;
    }
    catch (const std::exception &e)
    {
        m_state.errorMessage = e.what();
        return false;
    }
}

bool App::CompleteOrder(int orderId)
{
    m_state.ClearMessages();
    if (!m_state.isLoggedIn || m_state.currentUser->GetRole() != UserRole::RETAILER)
    {
        m_state.errorMessage = "Only retailers can mark orders as complete.";
        return false;
    }
    try
    {
        Order* o = m_db.FindOrder(orderId);
        Product* p = m_db.FindProduct(o->GetProductId());
        std::string productName = p->GetName();

        m_db.CompleteOrder(orderId, m_state.currentUser->GetUserId());

        // Notifies the dealer that the retailer has marked the order as completed
        m_db.AddNotification(
            o->GetDealerId(),
            NotificationType::ORDER_COMPLETED,
            orderId,
            "Order #" + std::to_string(orderId) +
            " for '" + productName + "' marked COMPLETED by '" +
            m_state.currentUser->GetUsername() + "'."
        );

        m_state.infoMessage = "Order #" + std::to_string(orderId) + " marked as complete.";
        return true;
    }
    catch (const std::exception &e)
    {
        m_state.errorMessage = e.what();
        return false;
    }
}

// Returns number of unread notifications for current logged-in user
int App::UnreadNotificationCount() const {
    if (!m_state.isLoggedIn) return 0;
    int uid = m_state.currentUser->GetUserId();
    int count = 0;
    for (const auto& n : m_db.GetAllNotifications())
        if (n.GetRecipientUserId() == uid && !n.IsRead()) {
            ++count;
        }
    return count;
}

// Marks a specific notification as read by its ID
void App::MarkNotificationRead(int notificationId) {
    m_db.MarkNotificationRead(notificationId);
}

// Returns notifications for the currently logged-in user, newest first
std::vector<const Notification*> App::GetNotificationsForUser() const {
    if (!m_state.isLoggedIn) {
        return {};
    }
    return m_db.GetNotificationsForUser(m_state.currentUser->GetUserId());
}


// Sends a plain message notification to another user (no order attached)
bool App::SendMessage(int recipientId, const std::string &msg) {
    m_state.ClearMessages();
    try {
        m_db.SendMessage(recipientId, msg);
        m_state.infoMessage = "Message sent.";
        return true;
    }
    catch (const std::exception &e) {
        m_state.errorMessage = e.what();
        return false;
    }
}

// Review
bool App::SubmitReview(int orderId, int productId, int rating, const std::string& comment) {
    m_state.ClearMessages();
    if (!m_state.isLoggedIn || m_state.currentUser->GetRole() != UserRole::RETAILER) {
        m_state.errorMessage = "Only retailers can submit reviews.";
        return false;
    }
    try {
        m_db.SubmitReview(m_state.currentUser->GetUserId(), orderId, productId, rating, comment);
        m_state.infoMessage = "Review submitted!";
        return true;
    } catch (const std::exception& e) {
        m_state.errorMessage = e.what();
        return false;
    }
}
