#include "user.h"
#include <sstream>
#include <functional>


User::User(int userId, const std::string& username, const std::string& passwordHash, UserRole role) 
            : m_userId(userId), m_username(username), m_passwordHash(passwordHash), m_role(role) 
{
    ValidateUsername(username);
}


//  Serialization ('\t' delimiter for tsv files)
std::string User::Serialize() const {
    std::ostringstream oss;
    oss << m_userId << "\t" << m_username << "\t" << m_passwordHash << "\t" << RoleToString(m_role);
    return oss.str();
}

//  Password helpers
bool User::CheckPassword(const std::string& plainPassword) const {
    return m_passwordHash == HashPassword(plainPassword);
}

void User::SetPassword(const std::string& plainPassword) {
    if (plainPassword.size() < 6) {
        throw ValidationException("Password must be at least 6 characters.");
    }
    m_passwordHash = HashPassword(plainPassword);
}

//  Static helper methods
std::string User::HashPassword(const std::string& plain) {
    // simple hash
    std::size_t h = std::hash<std::string>{} (plain + "baba_talal_bustling_donkeys");
    std::ostringstream oss;
    oss << std::hex << h;
    return oss.str();
}


void User::ValidateUsername(const std::string& username) {
    if (username.empty()) {
        throw ValidationException("Username cannot be empty.");
    }
    if (username.size() < 3) {
        throw ValidationException("Username must be at least 3 characters.");
    }
    for (char c : username) {
        if (!std::isalnum(c) && c != '_') {
            throw ValidationException("Username may only contain letters, digits, or underscores.");
        }
    }
        
}


//  Helpers function implementation
std::string RoleToString(UserRole role) {
    switch (role) {
        case UserRole::RETAILER: return "RETAILER";
        case UserRole::DEALER:   return "DEALER";
        case UserRole::ADMIN:    return "ADMIN";
    }
    return "UNKNOWN";
}

UserRole StringToRole(const std::string& str) {
    if (str == "RETAILER") return UserRole::RETAILER;
    if (str == "DEALER")   return UserRole::DEALER;
    if (str == "ADMIN")    return UserRole::ADMIN;
    throw ValidationException("Unknown role string: " + str);
}