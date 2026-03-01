#pragma once
#include "user.h"

//  Derived Class from Abstract User
//  (stretch feature - partial implementation for now)
class Admin : public User {
private:
    std::vector<int> m_approvedDealers;
    std::vector<int> m_removedUsers;
    std::vector<int> m_bannedProducts;
public:
    Admin(int userId, const std::string& username, const std::string& passwordHash);

    UserRole GetRole() const override { return UserRole::ADMIN; }
    std::string GetDashboardInfo() const override;
    std::string Serialize() const override;

    // Admin actions
    void ApproveDealer(int dealerId);
    void RemoveUser(int userId);
    void BanProduct(int productId);

    const std::vector<int>& GetApprovedDealers() const {
        return m_approvedDealers; 
    }
    const std::vector<int>& GetBannedProducts() const { 
        return m_bannedProducts; 
    }

};