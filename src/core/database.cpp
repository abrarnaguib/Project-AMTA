#include "database.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/stat.h>
#include <algorithm>

// file path helpers
std::string Database::UsersFile() const {
    return m_dataDir + "/users.tsv";
}
std::string Database::ProductFile() const {
    return m_dataDir + "/products.tsv";
} 
std::string Database::OrdersFile() const {
    return m_dataDir + "/orders.tsv";
}

void Database::EnsureDataDir() const {
    #ifdef _WIN32
        _mkdir(m_dataDir.c_str());
    #else 
        mkdir(m_dataDir.c_str(), 0755); // for linux permission
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
}

// saves back in .tsv format
void Database::SaveAll() const {
    EnsureDataDir();
    SaveUsers();
    SaveProducts();
    SaveOrders();
}

// reads from the .tsv file format
void Database::LoadUsers() {
    std::ifsteram f(UsersFile());
    if (!f) {
        return ;
    }
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty()) {
            continue ;
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

// converts a tsv line to product object, if failed, show's an warning
void Database::LoadProducts() {
    std::ifstream f(ProductsFile());
    if (!f) {
        return ;
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
}
// same working format
void Database::LoadOrders() {
    std::ifstream f(OrdersFile());
    if (!f) {
        return ;
    }
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty()) {
            continue ;
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
}

// converts objects and write to the tsv file
void Database::SaveUsers() const {
    std::ofstream f(UsersFile());
    if (!f) {
        throw DatabaseException("Cannot open users file for writing.\n");
    }
    for (const auto &u: m_users) {
        f << u->Serialize() << '\n';
    }
}
// same procedure
void Database::SaveProducts() const {
    std::ofstream f(ProductsFile());
    if (!f) {
        throw DatabaseException("Cannot open products file for writing.\n");
    }
    for (const auto &p: m_products) {
        f << p->Serialize() << '\n';
    }
}
// same procedure
void Database::SaveOrders() const {
    std::ofstream f(OrdersFile());
    if (!f) {
        throw DatabaseException("Cannot open orders file for writing.\n");
    }
    for (const auto &o: m_orders) {
        f << o->Serialize() << '\n';
    }
}

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

// confirms dealer using userId
Dealer* Database::GetDealer(int userId) const {
    return dynamic_cast<Dealer*> (FindUserById(userId));
}
// same procedure
Retailer* Database::GetRetailer(int userId) const {
    return dynamic_cast<Retailer*>(FindUserById(userId));
}

// verifies dealer and adds product to global product list as well as dealer's list
Product* Database::AddProduct(int dealerId, const std::string &name, const std::string &category, double price, int stock) {
    Dealer* d = GetDealer(dealerId);
    if (!d) {
        throw DatabaseException("Dealer ID " + std::to_string(dealerId) + " not found.\n");
    }
    m_products.push_back(m_nextProductId++, dealerId, name, category, price, stock);
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
    for (auto &p: const_case<std::vector<Product>&> (m_products)) {
        if (p.GetProductId() == productId) {
            return &p;
        }
    }
    return nullptr;
}
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
    m_orders.push_back(m_nextOrderId++, retailerId, dealerId, productId, quantity);
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
void Database::CompleteOrder(int orderId) {
    Order* o = FindOrder(orderId);
    if (!o) {
        throw OrderException("Order ID " + std::to_string(orderId) + " not found.\n");
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