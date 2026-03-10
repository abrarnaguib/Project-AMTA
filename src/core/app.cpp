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
    return false;
}

bool App::AcceptOrder(int orderId)
{
    return false;
}

bool App::RejectOrder(int orderId)
{
    return false;
}