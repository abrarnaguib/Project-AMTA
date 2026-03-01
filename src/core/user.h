#pragma once
#include <string>
#include <stdexcept>

//  Roles
enum class UserRole {
    RETAILER,
    DEALER,
    ADMIN
};


//  User Class : Abstract Base Class
class User {
protected:
    int m_userId;
    std::string m_username;
    std::string m_passwordHash;
    UserRole m_role;

private:
    static void ValidateUsername(const std::string& username);

public:
    User(int userId, const std::string& username, const std::string& passwordHash, UserRole role);

    // Derived class must inherit 
    // Debugging Helper
    virtual std::string GetDashboardInfo() const = 0;
    
    // Concrete shared behaviour
    bool CheckPassword(const std::string& plainPassword) const;
    void SetPassword(const std::string& plainPassword);

    // Getters
    int GetUserId() const { 
        return m_userId; 
    }

    std::string GetUsername() const {  
        return m_username; 
    }

    // Pure virtual getter
    virtual UserRole GetRole() const = 0;

    // Serialization helper for database
    virtual std::string Serialize() const;

    // Password encryption
    static std::string HashPassword(const std::string& plain);

    virtual ~User() = default;

};


// Helper Functions (data type converter)
std::string RoleToString(UserRole role);
UserRole StringToRole(const std::string& str);


// Exceptions
class AuthException : public std::runtime_error {
public:
    explicit AuthException(const std::string& msg) : std::runtime_error(msg) {}
};

class ValidationException : public std::runtime_error {
public:
    explicit ValidationException(const std::string& msg) : std::runtime_error(msg) {}
};