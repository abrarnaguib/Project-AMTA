#include "gui.h"
#include "../core/app.h"
#include "imgui.h"
#include<string>
#include<iostream>
#include<math.h>



// forward declaration for each and every page / panel
static void RenderHomePage (App &app);
static void RenderLoginPage (App &app);
static void RenderRegisterPage (App &app);
static void RenderDashBoard (App &app);
static void RenderProductList (App &app);
static void RenderPlaceOrderPage (App &app);
static void RenderUtilBar (App &app); // Render the top utility bar (account info, login logout buttons etc.)
static void RenderStatusBar (App &app);  // Render the bottom action status bar (wrong input, wrong password etc.)
static void RenderDealerPanel (App &app); // Render dealer dashboard
static void RenderRetailerPanel (App &app); // Render retailer dashboard


// colour palette 
// color vectors
static const ImVec4 COL_ACCENT { 0.20f, 0.60f, 1.00f, 1.00f }; // blue
static const ImVec4 COL_SUCCESS { 0.18f, 0.80f, 0.44f, 1.00f }; // green
static const ImVec4 COL_DANGER { 0.90f, 0.25f, 0.25f, 1.00f }; // red
static const ImVec4 COL_MUTED { 0.60f, 0.60f, 0.60f, 1.00f }; // dark grey
static const ImVec4 COL_WARN { 1.00f, 0.75f, 0.10f, 1.00f }; // yellow
static const ImVec4 COL_BG_CARD { 0.14f, 0.14f, 0.18f, 1.00f }; // grey


// Helper functions for pushing a styled button colour
// remember to pop after use ( ImGui::PopStyleColor(3) )
static void PushDangerButton() {
    ImGui::PushStyleColor(ImGuiCol_Button, {0.70f, 0.15f, 0.15f, 1.00f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.90f, 0.25f, 0.25f, 1.00f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, {0.50f, 0.10f, 0.10f, 1.00f});
} // red button

static void PushSuccessButton() {
    ImGui::PushStyleColor(ImGuiCol_Button, {0.15f, 0.55f, 0.30f, 1.00f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.18f, 0.75f, 0.40f, 1.00f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, {0.10f, 0.40f, 0.20f, 1.00f});
} // green button

static void PushAccentButton() {
    ImGui::PushStyleColor(ImGuiCol_Button, {0.15f, 0.45f, 0.90f, 1.00f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.25f, 0.60f, 1.00f, 1.00f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, {0.10f, 0.35f, 0.70f, 1.00f});
} // blue button

static void PushWarnButton() {
    ImGui::PushStyleColor(ImGuiCol_Button, {1.00f, 0.75f, 0.10f, 1.00f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {1.00f, 0.85f, 0.20f, 1.00f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, {0.80f, 0.68f, 0.5f, 1.00f});
} // yellow button


// custom drawings
// seperator line with a fixed width
static void customSeperator(float width) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImVec2 p1 = ImVec2(p.x, p.y);
    ImVec2 p2 = ImVec2(p.x + width, p.y);
    ImU32 separator_color = ImGui::GetColorU32(ImGuiCol_Separator);
    float thickness = 1.0f; 
    draw_list->AddLine(p1, p2, separator_color, thickness);
    ImGui::Dummy(ImVec2(width, ImGui::GetStyle().ItemSpacing.y)); 
}


// for summary card on top of a panel
static void SectionLabel(const ImVec4 &col, const char* label) {
    ImGui::Spacing();
    ImGui::TextColored(col, "%s", label);
    ImGui::Separator();
    ImGui::Spacing();
}

static void SectionLabelWidth(const ImVec4 &col, float width, const char* label) {
    ImGui::Spacing();
    ImGui::TextColored(col, "%s", label);
    customSeperator(width);
    ImGui::Spacing();
}

namespace GUI {
    void Render(App& app) {
        // window styles
        ImGui::StyleColorsDark();
        ImGuiStyle &style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.FrameRounding = 4.0f;
        style.GrabRounding = 4.0f;
        style.ItemSpacing = {10.0f, 8.0f};
        style.FramePadding = {8.0f, 5.0f};

        // full screen host window
        ImGuiIO &io = ImGui::GetIO();
        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::SetNextWindowBgAlpha(1.0f);

        // window poistion and size fixed        
        ImGui::Begin("##root", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | 
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus 
            | ImGuiWindowFlags_NoScrollbar);
        

        RenderUtilBar(app);

        // Main contents in child region (leaves room for other panels)
        float statusH = 55.0f;
        ImGui::BeginChild("##page", {0, io.DisplaySize.y - 2 * statusH}, 0);

        AppState& state = app.GetState();
        switch (state.currentPage) 
        {
        case AppState::Page::HOME:
            RenderHomePage(app);
            break;
        case AppState::Page::LOGIN:
            RenderLoginPage(app);
            break;
        case AppState::Page::REGISTER:
            RenderRegisterPage(app);
            break;
        case AppState::Page::DASHBOARD:
            RenderDashBoard(app);
            break;
            case AppState::Page::PRODUCT_LIST:
            RenderProductList(app);
            break;
        case AppState::Page::PLACE_ORDER:
            RenderPlaceOrderPage(app);
            break;
        default:
            break;
        }
        
        ImGui::EndChild();

        RenderStatusBar(app);


        ImGui::End();
    }
}


static void RenderUtilBar (App &app) {
    AppState &state = app.GetState();
    
    ImGui::TextColored(COL_ACCENT, "BabaTalal's Smart Marketplace");

    if (!state.isLoggedIn && state.currentPage != AppState::Page::DASHBOARD) {
        std::string lhsText = "  Login    Register  ";
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(lhsText.c_str()).x - ImGui::CalcTextSize("     ").x);
        PushAccentButton();
        if (ImGui::Button("  Login  ")) {
            state.currentPage = AppState::Page::LOGIN;
        }
        ImGui::PopStyleColor(3);
        ImGui::SameLine();

        PushSuccessButton();
        if (ImGui::Button("  Register  ")) {
            state.currentPage = AppState::Page::REGISTER;
        }
        ImGui::PopStyleColor(3);
    }
    else {
        std::string roleStr = (state.currentUser->GetRole() == UserRole::DEALER ? "[dealer]" : "[retailer]");
        std::string usernameStr = state.currentUser->GetUsername();
        std::string buttonName = (state.currentPage == AppState::Page::DASHBOARD ? "Home" : "DashBoard");
        std::string lhsText = roleStr + usernameStr + "    " + buttonName + "    Logout  ";
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(lhsText.c_str()).x - ImGui::CalcTextSize("      ").x);

        ImGui::TextColored(COL_ACCENT, "%s", roleStr.c_str());
        ImGui::SameLine();

        ImGui::TextColored(COL_SUCCESS, "%s", usernameStr.c_str());
        ImGui::SameLine();

        PushAccentButton();
        if (state.currentPage == AppState::Page::DASHBOARD) {
            if (ImGui::Button("  Home  ")) {
                state.currentPage = AppState::Page::HOME;
            }
        }
        else {
            if (ImGui::Button("  DashBoard  ")) {
                state.currentPage = AppState::Page::DASHBOARD;
            }
        }
        ImGui::PopStyleColor(3);
        ImGui::SameLine();

        PushDangerButton();
        if (ImGui::Button("  Logout  ")) {
            app.Logout();
        }
        ImGui::PopStyleColor(3);
    }
    
    // visual seperation
    ImGui::Separator();
    ImGui::Spacing();
}



static void RenderStatusBar(App &app) {
    AppState &state = app.GetState();

    // visual seperation
    ImGui::Separator();
    ImGui::Spacing();

    if (!state.infoMessage.empty()) {
        ImGui::TextColored(COL_SUCCESS, "  ✓  %s", state.infoMessage.c_str());
    }
    else if (!state.errorMessage.empty()) {
        ImGui::TextColored(COL_DANGER, "  ✗  %s", state.errorMessage.c_str());
    }
    else {
        ImGui::TextColored(COL_ACCENT, "Ready");
    }
}



static void RenderHomePage (App &app) {
    AppState &state = app.GetState();

    ImGui::Spacing();
    ImGui::Spacing();

    // app description
    ImGui::SetWindowFontScale(1.8f);
    ImGui::TextColored(COL_ACCENT, "  Welcome to Babatalal");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "  Bangladesh's lightweight B2B marketplace connecting\n  small retailers with importers and dealers");
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // stats
    int productCount = (int)app.GetDatabase().GetAllProducts().size();
    int orderCount = (int)app.GetDatabase().GetAllOrders().size();
    ImGui::Text("  Products Listed: ");
    ImGui::SameLine();
    ImGui::TextColored(COL_WARN, "%d", productCount);
    ImGui::SameLine(250);
    ImGui::Text("Total orders placed: ");
    ImGui::SameLine();
    ImGui::TextColored(COL_WARN, "%d", orderCount);
    ImGui::Spacing();
    ImGui::Spacing();

    PushAccentButton();
    if (ImGui::Button("  Browse Products  ", {200, 40})) {
        state.currentPage = AppState::Page::PRODUCT_LIST;
    }
    ImGui::PopStyleColor(3);

    if (!state.isLoggedIn) {
        ImGui::SameLine();
        PushSuccessButton();
        if (ImGui::Button("  Register Now  ", {200, 40})) {
            state.currentPage = AppState::Page::REGISTER;
        }
        ImGui::PopStyleColor(3);
    }
}


static void RenderLoginPage (App &app) {
    AppState &state = app.GetState();
    
    float cardWidth = ImGui::GetContentRegionAvail().x * .5f;
    float winWidth = ImGui::GetContentRegionAvail().x;
    // center the login card
    ImGui::SetCursorPosX((winWidth - cardWidth) * 0.5f);

    ImGui::BeginGroup();
    ImGui::PushItemWidth(cardWidth);
    
    SectionLabelWidth(COL_ACCENT, cardWidth, "Login to your account");

    static char username[64] = "";
    static char password[64] = "";

    ImGui::Text("Username");
    ImGui::InputText("##login_username", username, sizeof(username));
    ImGui::Spacing();

    ImGui::Text("Password");
    ImGui::InputText("##login_password", password, sizeof(password));

    PushAccentButton();
    bool loginPressed = ImGui::Button("  Login  ");
    ImGui::PopStyleColor(3);
    

    if (loginPressed || (ImGui::IsKeyPressed(ImGuiKey_Enter) && username[0] != '\0')) {
        bool exists = app.Login(username, password);
        if (exists) {
            username[0] = '\0';
            password[0] = '\0';
        } 
    }
    ImGui::Spacing();

    if (ImGui::Button("  Back To Home  ")) {
        state.currentPage = AppState::Page::HOME;
    }
    ImGui::Spacing();

    ImGui::TextColored(COL_MUTED, "Don't Have an Account?");
    ImGui::SameLine();
    ImGui::TextColored(COL_ACCENT, "Register");
    if (ImGui::IsItemClicked()) {
        state.currentPage = AppState::Page::REGISTER;
    }
    
    ImGui::PopItemWidth();
    ImGui::EndGroup();
}


static void RenderRegisterPage (App &app) {
    AppState &state = app.GetState();

    float cardWidth = ImGui::GetContentRegionAvail().x * .65f;
    float winWidth = ImGui::GetContentRegionAvail().x;
    // center the login card
    ImGui::SetCursorPosX((winWidth - cardWidth) * 0.5f);

    ImGui::BeginGroup();
    ImGui::PushItemWidth(cardWidth);

    SectionLabelWidth(COL_ACCENT, cardWidth, "Create a new account");

    static char username[64] = "";
    static char password[64] = "";
    static char confirm[64] = "";
    static char location[128] = "";
    static char extraName[128] = "";
    const char *roles[] = {"Retailer", "Dealer"};
    static int roleIdx = 0;

    ImGui::Text("Username");
    ImGui::InputText("##reg_username", username, sizeof(username));
    ImGui::Spacing();

    ImGui::Text("Password");
    ImGui::InputText("##reg_password", password, sizeof(password));
    ImGui::Spacing();

    ImGui::Text("Confirm Password");
    ImGui::InputText("##reg_confirm", confirm, sizeof(confirm));
    if (confirm[0] != '\0') {
        if (std::string(password) == std::string(confirm)) {
            ImGui::TextColored(COL_SUCCESS, "  ✓ Passwords match");
        }
        else {
            ImGui::TextColored(COL_DANGER, "  ✗ Passwords do not match");
        }
    }
    ImGui::Spacing();

    ImGui::Text("Account Type");
    ImGui::Combo("##reg_role", &roleIdx, roles, 2); // dropdown menu for choosing roles
    ImGui::Spacing();

    ImGui::Text(roleIdx == 0 ? "Shop Name" : "Company Name");
    ImGui::InputText("##reg_extraName", extraName, sizeof(extraName));
    ImGui::Spacing();

    ImGui::Text("Location / City");
    ImGui::InputText("##reg_location", location, sizeof(location));
    ImGui::Spacing();
    ImGui::Spacing();

    bool canRegister = ((std::string(password) == std::string(confirm)) && password[0] != '\0');
    if (!canRegister) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.4f);
        ImGui::Button("  Register  ");
        ImGui::PopStyleVar();
    }
    else {
        PushSuccessButton();
        if (ImGui::Button("  Register  ")) {
            const char *roleStrs[] = {"RETAILER", "DEALER"};
            bool registered = app.Register(username, password, roleStrs[roleIdx], extraName, location);
            if (registered) {
                username[0] = '\0';
                password[0] = '\0';
                confirm[0] = '\0';
                extraName[0] = '\0';
                location[0] = '\0';
            }
        }
        ImGui::PopStyleColor(3);
    }
    ImGui::Spacing();

    if (ImGui::Button("  Back To Home  ")) {
        state.currentPage = AppState::Page::HOME;
    }
    ImGui::Spacing();

    ImGui::TextColored(COL_MUTED, "Already Have an Account?");
    ImGui::SameLine();
    ImGui::TextColored(COL_ACCENT, "Login");
    if (ImGui::IsItemClicked()) {
        state.currentPage = AppState::Page::LOGIN;
    }

    ImGui::PopItemWidth();
    ImGui::EndGroup();
}


static void RenderDashBoard (App &app) {
    AppState &state = app.GetState();
    if (!state.isLoggedIn) {
        ImGui::TextColored(COL_DANGER, "You must be logged in to view this page");
        return;
    }
    if (state.currentUser->GetRole() == UserRole::DEALER) {
        RenderDealerPanel(app);
    }
    else if (state.currentUser->GetRole() == UserRole::RETAILER) {
        RenderRetailerPanel(app);
    }
}


static void RenderDealerPanel(App &app) {
    AppState &state = app.GetState();
    Dealer *d = app.GetDatabase().GetDealer(app.GetState().currentUser);
    if (!d) return;

    // summary card
    SectionLabel(COL_ACCENT, "Dealer Dashboard");

    ImGui::TextColored(COL_MUTED, "Company  : ");
    ImGui::SameLine();
    ImGui::Text("%s", d->GetCompanyName().c_str());

    ImGui::TextColored(COL_MUTED, "Location : ");
    ImGui::SameLine();
    ImGui::Text("%s", d->GetLocation().c_str());
    
    ImGui::TextColored(COL_MUTED, "Rating : ");
    ImGui::SameLine();
    float r = d->GetRating();
    ImVec4 rCol;
    if (r >= 4.0) rCol = COL_SUCCESS;
    else if (r >= 2.5) rCol = COL_WARN;
    else rCol = COL_DANGER;
    ImGui::TextColored(rCol, "%.1f / 5.0", r);
    ImGui::Spacing();

    // add product form
    ImGui::PushStyleColor(ImGuiCol_Text, COL_SUCCESS);
    if (ImGui::CollapsingHeader("  + Add New Product", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::PopStyleColor();
        ImGui::Spacing();
        static char pName[128] = "";
        static char pCat[64] = "";
        static float pPrice = 0.0f;
        static int pStock = 0;

        float iw = 280.0f;
        ImGui::PushItemWidth(iw);

        ImGui::Text("Produt Name");
        ImGui::InputText("##pName", pName, sizeof(pName));
        ImGui::SameLine(iw + 30);
   
        ImGui::Text("Category");
        ImGui::SetCursorPosX(iw + 30);
        ImGui::InputText("##pCat", pCat, sizeof(pCat));
             
        ImGui::Text("Price");
        ImGui::InputFloat("##pPrice", &pPrice, 1.0f, 10.0f, "%.2f"); 
        if (pPrice < 0) pPrice = 0;
        ImGui::SameLine(iw + 30);

        ImGui::Text("Stock");
        ImGui::SetCursorPosX(iw + 30);
        ImGui::InputInt("##pStock", &pStock);
        if (pStock < 0) pStock = 0;

        ImGui::PopItemWidth();
        ImGui::Spacing();

        PushSuccessButton();
        if (ImGui::Button("  Add Product  ", {200, 32})) {
            if (pName[0] != '\0' && pCat[0] != '\0') {
                if (app.AddProduct(pName, pCat, pPrice, pStock)) {
                    pName[0] = '\0';
                    pCat[0] = '\0';
                }
            }
            else {
                app.GetState().errorMessage = "Name and category are required";
            }
        }
        ImGui::PopStyleColor(3);
        ImGui::Spacing();
    }

    // product catalogue
    SectionLabel(COL_ACCENT, "Your Products");

    static int editingProductId = -1;
    static float editPrice = 0;
    static int editStock = 0;

    if (d->GetProducts().empty()) {
        ImGui::TextColored(COL_MUTED, "   No Products Lister Yet.");
    }

    const int pw = 80;
    for (const auto& p : d->GetProducts()) {
        ImGui::PushID(p.GetProductId());
        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_CARD);
        ImGui::BeginChild("##ordercard", {0, pw}, ImGuiChildFlags_Borders);

        ImGui::TextColored(COL_ACCENT, "[%d] %s", p.GetProductId(), p.GetName().c_str());
        ImGui::SameLine(400);
        ImGui::TextColored(COL_MUTED, "Avg Rating: %.1f", p.GetAvgRating());
        ImGui::Text("   Category: %s   |   Price: %.2f   |   Stock: %d", p.GetCategory().c_str(), p.GetPrice() , p.GetStock());
        ImGui::Spacing();

        // for edit / delete button seletion
        if (editingProductId != p.GetProductId()) {
            PushAccentButton();
            if (ImGui::SmallButton("  Edit  ")) {
                editingProductId = p.GetProductId();
                editPrice = (float)p.GetPrice();
                editStock = p.GetStock();
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();

            PushDangerButton();
            if (ImGui::SmallButton("  Delete  " )) {
                app.DeleteProduct(p.GetProductId());
            }
            ImGui::PopStyleColor(3);
        }
        else { // for inline edit
            ImGui::PushItemWidth(120);
            ImGui::InputFloat("Price##e", &editPrice, 1.0f, 10.0f, "%.2f");
            ImGui::SameLine();
            ImGui::InputInt("Stock##e", &editStock);
            ImGui::PopItemWidth();
            if (editPrice < 0) editPrice = 0;
            if (editStock < 0) editStock = 0;
            ImGui::SameLine();

            PushSuccessButton();
            if (ImGui::SmallButton("  Save  ")) {
                app.UpdateProduct(editingProductId, (double)editPrice, editStock);
                editingProductId = -1;
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();

            PushDangerButton();
            if (ImGui::SmallButton("  Cancel  ")) {
                editingProductId = -1;
            }
            ImGui::PopStyleColor(3);
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::Spacing();
        ImGui::PopID();
    }

    // incoming orders
    SectionLabel(COL_ACCENT, "Incoming Orders");

    if (d->GetOrders().empty()) {
        ImGui::TextColored(COL_MUTED, "   No incoming orders remaining.");
        return;
    }

    const int ow = 70;
    for (const auto& o : d->GetOrders()) {
        ImGui::PushID(o.GetOrderId());
        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_CARD);
        ImGui::BeginChild("##ordercard", {0, ow}, ImGuiChildFlags_Borders);

        ImVec4 oStatus;
        switch (o.GetStatus())
        {
        case OrderStatus::ACCEPTED:
            oStatus = COL_SUCCESS;
            break;
        case OrderStatus::PENDING:
            oStatus = COL_WARN;
            break;
        case OrderStatus::REJECTED:
            oStatus = COL_DANGER;
            break;
        default:
            break;
        }
        ImGui::Text("   Order #%d   |   Product ID: %d   |   Qty: %d", o.GetOrderId(), o.GetProductId(), o.GetQuantity());
        ImGui::SameLine(500);
        ImGui::TextColored(oStatus, "[%s]", o.GetStatusStr().c_str());
        ImGui::Spacing();

        if (o.GetStatus() == OrderStatus::PENDING) {
            PushSuccessButton();
            if (ImGui::SmallButton("  Accept  ")) {
                app.AcceptOrder(o.GetOrderId());
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();

            PushDangerButton();
            if (ImGui::SmallButton("  Reject  ")) {
                app.RejectOrder(o.GetOrderId());
            }
            ImGui::PopStyleColor(3);
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::Spacing();
        ImGui::PopID();
    }
}


static void RenderRetailerPanel(App &app) {
    AppState &state = app.GetState();
    Retailer *r = app.GetDatabase().GetRetailer(app.GetState().currentUser);
    if (!r) return;
    
    // summary card
    SectionLabel(COL_ACCENT, "Retailer Dashboard");

    ImGui::TextColored(COL_MUTED, "Shop     : ");
    ImGui::SameLine();
    ImGui::Text("%s", r->GetShopName().c_str());

    ImGui::TextColored(COL_MUTED, "Location : ");
    ImGui::SameLine();
    ImGui::Text("%s", r->GetLocation().c_str());
    ImGui::Spacing();

    PushAccentButton();
    if (ImGui::Button("  Browse & Order Products ")) {
        state.currentPage = AppState::Page::PRODUCT_LIST;
    }
    ImGui::PopStyleColor(3);


    // order history of a retailer
    SectionLabel(COL_ACCENT, "Order History");

    if (r->GetOrderHistory().empty()) {
        ImGui::TextColored(COL_MUTED, "   No orders placed yet. Browse products to start ordering!");
        return;
    }

    const int w = 60;
    for (const auto& o : r->GetOrderHistory()) {
        ImGui::PushID(o.GetOrderId());
        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_CARD);
        ImGui::BeginChild("##orderhist", {0, w}, ImGuiChildFlags_Borders);

        ImVec4 oStatus;
        switch (o.GetStatus())
        {
        case OrderStatus::ACCEPTED:
            oStatus = COL_SUCCESS;
            break;
        case OrderStatus::PENDING:
            oStatus = COL_WARN;
            break;
        case OrderStatus::REJECTED:
            oStatus = COL_DANGER;
            break;
        default:
            break;
        }
        ImGui::Text("   Order #%d   |   Product ID: %d   |   Qty: %d", o.GetOrderId(), o.GetProductId(), o.GetQuantity());
        ImGui::SameLine(500);
        ImGui::TextColored(oStatus, "[%s]", o.GetStatusStr().c_str());

        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::Spacing();
        ImGui::PopID();
    }
}


// product list 
static void RenderProductList (App &app) {
    AppState &state = app.GetState();
    bool isRetailer = state.isLoggedIn && state.currentUser->GetRole() == UserRole::RETAILER;

    // summary card
    SectionLabel(COL_ACCENT, "Browse Products");

    // search bar 
    static char searchBuffer[128] = "";
    static double maxPrice = 99999.0f;
    static double minPrice = 0.0f;

    ImGui::PushItemWidth(300);
    ImGui::InputText("Search##srch", searchBuffer, sizeof(searchBuffer));
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushItemWidth(130);
    ImGui::InputDouble("Min Price##mn", &minPrice, 0, 0, "%.0f BDT");
    ImGui::SameLine();
    ImGui::InputDouble("Max Price##mx", &maxPrice, 0, 0, "%.0f BDT");
    ImGui::PopItemWidth();

    minPrice = fmax(0.0f, minPrice);
    maxPrice = fmax(minPrice, maxPrice);
    ImGui::Spacing();

    // Quantity selector (retailer only)
    static int orderQty = 1;
    if (isRetailer) {
        ImGui::PushItemWidth(120);
        ImGui::InputInt("Order Qty##oqty", &orderQty);
        if (orderQty < 1) orderQty = 1;
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::TextColored(COL_MUTED, "(applies to all Order buttons below)");
    }
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    std::string filter(searchBuffer);

    int shown = 0;
    const int pw = 75;
    for (const auto &p : app.GetDatabase().GetAllProducts()) {
        // apply filters
        if (!filter.empty()) {
            std::string nameLower = p.GetName();
            std::string catLower = p.GetCategory();
            std::string fLower = filter;
            for (char &c : nameLower) c = (char)tolower(c);
            for (char &c : catLower) c = (char)tolower(c);
            for (char &c : fLower) c = (char)tolower(c);
            if (nameLower.find(fLower) != std::string::npos && catLower.find(fLower) != std::string::npos) continue;
        }
        if (p.GetPrice() < minPrice || p.GetPrice() > maxPrice) continue;
        shown++;
        ImGui::PushID(p.GetProductId());
        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_CARD);
        ImGui::BeginChild("##plist", {0, pw}, ImGuiChildFlags_Borders);

        ImGui::TextColored(COL_ACCENT, "[%d] %s", p.GetProductId(), p.GetName().c_str());
        ImGui::SameLine(500);

        //star rating display
        float avg = p.GetAvgRating();
        ImVec4 rCol;
        if (avg >= 4.0f) rCol = COL_SUCCESS;
        else if (avg >= 2.5f) rCol = COL_WARN;
        else rCol = COL_DANGER;

        if (avg) ImGui::TextColored(rCol, "★ %.1f", avg);
        else ImGui::TextColored(COL_MUTED, "No reviews yet");

        ImGui::Text("   Category: %-20s   Price: %.2f BDT   Stock: %d", p.GetCategory().c_str(), p.GetPrice(), p.GetStock());

        if (!isRetailer) {
            ImGui::Spacing();
            if (p.GetStock() > 0) {
                PushAccentButton();
                std::string label = "  Order  ##" + std::to_string(p.GetProductId());
                if (ImGui::SmallButton(label.c_str())) {
                    app.PlaceOrder(p.GetProductId(), orderQty);
                }
                ImGui::PopStyleColor(3);
            }
            else {
                ImGui::TextColored(COL_DANGER, "  Out of Stock");
            }
        }
        else {
            ImGui::Spacing();
            ImGui::TextColored(COL_MUTED, "  Login as a retailer to place orders");
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::Spacing();
        ImGui::PopID();
    }

    if (!shown) ImGui::TextColored(COL_MUTED, "  No products match your search.");
    ImGui::Spacing();

    if (ImGui::Button("  Back to Home  ")) {
        state.currentPage = AppState::Page::HOME;
    }
}


static void RenderPlaceOrderPage (App &app) {
    AppState &state = app.GetState();


}
