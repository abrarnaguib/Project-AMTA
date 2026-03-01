#include "admin.h"
#include <sstream>
#include <algorithm>

Admin::Admin(int userId, const std::string& username, const std::string& passwordHash) : User(userId, username, passwordHash, UserRole::ADMIN) {}


// Debugger Helper
std::string Admin::GetDashboardInfo() const {
    std::ostringstream oss;
    oss << "=== Admin Dashboard ===\n" << "Username         : " << m_username << "\n" << "Approved Dealers : " << m_approvedDealers.size() << "\n" 
        << "Banned Products  : " << m_bannedProducts.size();
    return oss.str();
}

std::string Admin::Serialize() const {
    return User::Serialize();   // Admin has no extra fields for now
}

void Admin::ApproveDealer(int dealerId) {
    if (dealerId <= 0) throw ValidationException("Invalid dealer ID.");
    m_approvedDealers.push_back(dealerId);
}

void Admin::RemoveUser(int userId) {
    if (userId <= 0) throw ValidationException("Invalid user ID.");
    m_removedUsers.push_back(userId);
}

void Admin::BanProduct(int productId) {
    if (productId <= 0) throw ValidationException("Invalid product ID.");
    m_bannedProducts.push_back(productId);
}

