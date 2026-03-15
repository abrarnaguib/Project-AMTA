#include "database.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#ifdef _WIN32
#include <direct.h> // For _mkdir on Windows
#else
#include <sys/stat.h> // For mkdir on macOS/Linux
#endif

// file path helpers
std::string Database::UsersFile() const {
    return m_dataDir + "/users.tsv";
}
std::string Database::ProductsFile() const {
    return m_dataDir + "/products.tsv";
} 
std::string Database::OrdersFile() const {
    return m_dataDir + "/orders.tsv";
}
std::string Database::NotificationsFile() const {
     return m_dataDir + "/notifications.tsv"; 
}


void Database::EnsureDataDir() const {
    #ifdef _WIN32
        _mkdir(m_dataDir.c_str()); // For Windows
    #else 
        mkdir(m_dataDir.c_str(), 0755); // For macOS/Linux
    #endif
}

// constructor
Database::Database(const std::string &dataDir): m_dataDir(dataDir) {
    EnsureDataDir();
}

// load all three files to memory
void Database::LoadAll() {
    try {
        LoadUsers();
    }
    catch (...) {

    }
    try {
        LoadProducts();
    }
    catch (...) {

    }
    try {
        LoadOrders();
    }
    catch (...) {

    }
    try { 
        LoadNotifications();
    } 
    catch (...) {

    }
}

// saves back in .tsv format
void Database::SaveAll() const {
    EnsureDataDir();
    SaveUsers();
    SaveProducts();
    SaveOrders();
    SaveNotifications();
}

// reads from the .tsv file format
// reads users.tsv on startup and loads all users into memory
void Database::LoadUsers() {
    std::ifstream f(UsersFile());
    if (!f) {
        return;
    }
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty()) {
            continue;
        }
        std::istringstream iss(line);
        std::string tok;
        int id;
        std::string uname, hash, roleStr;
        std::getline(iss, tok, '\t');
        id = std::stoi(tok);
        std::getline(iss, uname, '\t');
        std::getline(iss, hash, '\t');
        std::getline(iss, roleStr, '\t');
        UserRole role = StringToRole(roleStr);

        if (role == UserRole::DEALER) {
            std::string company, loc;
            std::getline(iss, company, '\t');
            std::getline(iss, loc);
            auto d = std::make_unique<Dealer> (id, uname, hash, company, loc);
            if (id >= m_nextUserId) {
                m_nextUserId = id + 1;
            }
            m_users.push_back(std::move(d));
        }
        else if (role == UserRole::RETAILER) {
            std::string shop, loc;
            std::getline(iss, shop, '\t');
            std::getline(iss, loc);
            auto r = std::make_unique<Retailer> (id, uname, hash, shop, loc);
            if (id >= m_nextUserId) {
                m_nextUserId = id + 1;
            }
            m_users.push_back(std::move(r));
        }
        else if (role == UserRole::ADMIN) {
            auto a = std::make_unique<Admin> (id, uname, hash);
            if (id >= m_nextUserId) {
                m_nextUserId = id + 1;
            }
            m_users.push_back(std::move(a));
        }
    }
}

// reads products.tsv on startup and loads all products into memory
void Database::LoadProducts() {
    std::ifstream f(ProductsFile());
    if (!f) {
        return;
    }
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty()) {
            continue;
        }
        try {
            m_products.push_back(Product::Deserialize(line));
        }
        catch (const std::exception &e) {
            std::cerr << "[Database] skipping corrupt product record: " << e.what() << '\n';
        }
    }
    for (const auto &p: m_products) {
        if (p.GetProductId() >= m_nextProductId) {
            m_nextProductId = p.GetProductId() + 1;
        }
    }

    // Re-populate dealer's available products in retailer availavkkle product window
    for (const auto& p : m_products) {
        Dealer* d = GetDealer(p.GetDealerId());
        if (d) d->AddProduct(p);
    }
}

// reads orders.tsv on startup and loads all orders into memory
void Database::LoadOrders() {
    std::ifstream f(OrdersFile());
    if (!f) {
        return;
    }
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty()) {
            continue;
        }
        try {
            m_orders.push_back(Order::Deserialize(line));
        }
        catch (const std::exception &e) {
            std::cerr << "[Database] skipping corrupt order record: " << e.what() << '\n';
        }
    }
    for (const auto &o: m_orders) {
        if (o.GetOrderId() >= m_nextOrderId) {
            m_nextOrderId = o.GetOrderId() + 1;
        }
    }

    // Re-populate dealer's pending orders and send the order to his retailer's order history
    for (const auto& o : m_orders) {
        Dealer* d = GetDealer(o.GetDealerId());
        Retailer* r = GetRetailer(o.GetRetailerId());
        if (d && o.GetStatus() != OrderStatus::COMPLETED) {
            d->AddIncomingOrder(o);
        }
        if (r) {
            r->AddOrderToHistory(o);
        }
    }
}

// reads notifications.tsv on startup and loads all notifications into memory
void Database::LoadNotifications() {
    std::ifstream f(NotificationsFile());
    if (!f) return;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty()) {
            continue;
        }
        try {
            m_notifications.push_back(Notification::Deserialize(line));
        } 
        catch (const std::exception& e) {
            std::cerr << "[Database] Skipping corrupt notification: " << e.what() << "\n";
        }
    }
    for (const auto& n : m_notifications) {
        if (n.GetNotificationId() >= m_nextNotificationId) {
            m_nextNotificationId = n.GetNotificationId() + 1;
        }
    }
}

// converts objects and write to the tsv file
// writes all in-memory users to users.tsv
void Database::SaveUsers() const {
    std::ofstream f(UsersFile());
    if (!f) {
        throw DatabaseException("Cannot open users file for writing.\n");
    }
    for (const auto &u: m_users) {
        f << u->Serialize() << '\n';
    }
}
// writes all in-memory products to products.tsv
void Database::SaveProducts() const {
    std::ofstream f(ProductsFile());
    if (!f) {
        throw DatabaseException("Cannot open products file for writing.\n");
    }
    for (const auto &p: m_products) {
        f << p.Serialize() << '\n';
    }
}
// writes all in-memory orders to orders.tsv
void Database::SaveOrders() const {
    std::ofstream f(OrdersFile());
    if (!f) {
        throw DatabaseException("Cannot open orders file for writing.\n");
    }
    for (const auto &o: m_orders) {
        f << o.Serialize() << '\n';
    }
}


// writes all in-memory notifications to notifications.tsv
void Database::SaveNotifications() const {
    std::ofstream f(NotificationsFile());
    if (!f) throw DatabaseException("Cannot open notifications file for writing.");
    for (const auto& n : m_notifications) {
        f << n.Serialize() << "\n";
    }
}

// ---User---
// returns true if username taken
bool Database::UsernameExists(const std::string &username) const {
    return FindUserByName(username) != nullptr;
}

// validation for username and checks password size is at least 6
Retailer* Database::RegisterRetailer(const std::string &username, const std::string &password, const std::string &shopName, const std::string &location) {
    if (UsernameExists(username)) {
        throw ValidationException("Username " + username + " is already taken.\n"); 
    }
    if (password.size() < 6) {
        throw ValidationException("Password must be at least 6 characters.\n");
    }
    auto r = std::make_unique<Retailer> (m_nextUserId++, username, User::HashPassword(password), shopName, location);
    Retailer* ptr = r.get();
    m_users.push_back(std::move(r));
    SaveAll();
    return ptr;
}
// same procedure
Dealer* Database::RegisterDealer(const std::string &username, const std::string &password, const std::string &companyName, const std::string &location) {
    if (UsernameExists(username)) {
        throw ValidationException("Username " + username + " is already taken.\n");
    }
    if (password.size() < 6) {
        throw ValidationException("Password must be at least 6 characters.\n");
    }
    auto d = std::make_unique<Dealer> (m_nextUserId++, username, User::HashPassword(password), companyName, location);
    Dealer* ptr = d.get();
    m_users.push_back(std::move(d));
    SaveAll();
    return ptr;
}

// finds user by username
User* Database::Login(const std::string &username, const std::string &password) {
    User* u = FindUserByName(username);
    if (!u) {
        throw AuthException("No account found with username " + username + ".\n");
    }
    if (!u->CheckPassword(password)) {
        throw AuthException("Incorrect password.\n");
    }
    return u;
}

// looks in the m_users to look for users
User* Database::FindUserById(int userId) const {
    for (const auto &u: m_users) 
        if (u->GetUserId() == userId) {
            return u.get();
        }
    return nullptr;
}
// same procedure
User* Database::FindUserByName(const std::string &username) const {
    for (const auto &u: m_users) {
        if (u->GetUsername() == username) {
            return u.get();
        }
    }
    return nullptr;
}

// confirms dealer using userId or user
Dealer* Database::GetDealer(int userId) const {
    return dynamic_cast<Dealer*> (FindUserById(userId));
}

Dealer* Database::GetDealer(User* user) const {
    return dynamic_cast<Dealer*> (user);
}

// same procedure
Retailer* Database::GetRetailer(int userId) const {
    return dynamic_cast<Retailer*>(FindUserById(userId));
}

Retailer* Database::GetRetailer(User* user) const {
    return dynamic_cast<Retailer*>(user);
}

// ---Product---
// verifies dealer and adds product to global product list as well as dealer's list
Product* Database::AddProduct(int dealerId, const std::string &name, const std::string &category, double price, int stock) {
    Dealer* d = GetDealer(dealerId);
    if (!d) {
        throw DatabaseException("Dealer ID " + std::to_string(dealerId) + " not found.\n");
    }
    m_products.push_back({m_nextProductId++, dealerId, name, category, price, stock});
    Product* p = &m_products.back();
    d->AddProduct(*p);
    SaveAll();
    return p;
}
// finds the product, deletes from both dealer's list as well as global list
void Database::DeleteProduct(int productId) {
    auto it = std::find_if(m_products.begin(), m_products.end(), [productId] (const Product &p) {
        return p.GetProductId() == productId;
    });
    if (it == m_products.end()) {
        throw ProductException("Product ID " + std::to_string(productId) + " not found.\n");
    }
    Dealer* d = GetDealer(it->GetDealerId());
    if (d) {
        d->RemoveProduct(productId);
    }
    m_products.erase(it);
    SaveAll();
}
// looks for all product and returns the matching one
Product* Database::FindProduct(int productId) const {
    for (auto &p: const_cast<std::vector<Product>&> (m_products)) {
        if (p.GetProductId() == productId) {
            return &p;
        }
    }
    return nullptr;
}

// ---Order---
// verifies retailer, dealer and product. deducts if product exists
Order* Database::PlaceOrder(int retailerId, int dealerId, int productId, int quantity) {
    Retailer* r = GetRetailer(retailerId);
    if (!r) {
        throw DatabaseException("Retailer ID " + std::to_string(retailerId) + " not found.\n");
    }
    Dealer* d = GetDealer(dealerId);
    if (!d) {
        throw DatabaseException("Dealer ID " + std::to_string(dealerId) + " not found.\n");
    }
    Product* p = FindProduct(productId);
    if (!p) {
        throw DatabaseException("Product ID " + std::to_string(productId) + " not found.\n");
    }
    p->DeductStock(quantity);
    m_orders.push_back({m_nextOrderId++, retailerId, dealerId, productId, quantity});
    Order* o = &m_orders.back();
    d->AddIncomingOrder(*o);
    r->AddOrderToHistory(*o);
    SaveAll();
    return o;
}
// correct authorization of the dealer
void Database::RespondToOrder(int orderId, int dealerId, bool accept) {
    Order* o = FindOrder(orderId);
    if (!o) {
        throw OrderException("Order ID " + std::to_string(orderId) + " not found.\n");
    }
    if (o->GetDealerId() != dealerId) {
        throw AuthException("You are not authorized to respond to this order.\n");
    }
    if (accept) {
        o->Accept();
    }
    else {
        o->Reject();
    }
    SaveAll();
}
// marks the order as completed
void Database::CompleteOrder(int orderId, int retailerId) {
    Order* o = FindOrder(orderId);
    if (!o) {
        throw OrderException("Order ID " + std::to_string(orderId) + " not found.\n");
    }
    if (o->GetRetailerId() != retailerId){
        throw AuthException("You are not authorized to complete this order.");
    }
    o->Complete();
    SaveAll();
}

Order* Database::FindOrder(int orderId) const {
    for (auto &o: const_cast<std::vector<Order>&> (m_orders)) {
        if (o.GetOrderId() == orderId) {
            return &o;
        }
    }
    return nullptr;
}

// --- Notifaction ---
// creates notification and saves it in file while returning a pointer to it
Notification* Database::AddNotification(int recipientId, NotificationType type,
                                         int orderId, const std::string& msg) {
    if (!FindUserById(recipientId)) {
        throw DatabaseException("Recipient user ID " + std::to_string(recipientId) + " not found.\n");
    }
    if (!FindOrder(orderId)) {
        throw DatabaseException("Order ID " + std::to_string(orderId) + " not found.\n");
    }
    m_notifications.emplace_back(m_nextNotificationId++, recipientId, type, orderId, msg);
    Notification* n = &m_notifications.back();
    SaveNotifications();
    return n;
}

// finds a notification by ID, marks it as read, and saves to file.
void Database::MarkNotificationRead(int notificationId) {
    for (auto& n : m_notifications) {
        if (n.GetNotificationId() == notificationId) {
            n.MarkAsRead();
            SaveNotifications();
            return;
        }
    }
    throw NotificationException("Notification ID " + std::to_string(notificationId) + " not found.\n");
}