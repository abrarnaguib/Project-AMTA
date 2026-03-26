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
static void RenderNotifications (App &app); // Notification pop-up rendering
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
static const ImVec4 COL_DIM_BG_CARD { 0.14f, 0.14f, 0.18f, 1.00f }; // grey
static const ImVec4 COL_BRIGHT_BG_CARD { 0.18f, 0.22f, 0.30f, 1.00f }; //blue-gray


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

// notification circle
static void customNotificationCircle(ImVec2 pos, int count) {
    ImGuiStyle& style = ImGui::GetStyle();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    float radius = 9.0f;
    ImVec2 center = ImVec2(pos.x + style.FramePadding.x + ImGui::CalcTextSize("  Notifications  ").x + radius, pos.y + ImGui::GetFrameHeight() * 0.5f);
    draw_list->AddCircleFilled(center, radius, ImGui::GetColorU32(COL_DANGER));
    std::string countStr = std::to_string(count);
    ImVec2 textSize = ImGui::CalcTextSize(countStr.c_str());
    draw_list->AddText(ImVec2(center.x - textSize.x * 0.5f, center.y - textSize.y * 0.5f), IM_COL32(0, 0, 0, 255), countStr.c_str());
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

static ImVec4 NotifColour(NotificationType t) {
    switch (t) {
        case NotificationType::ORDER_PLACED:    
            return COL_ACCENT;
        case NotificationType::ORDER_ACCEPTED:  
            return COL_SUCCESS;
        case NotificationType::ORDER_REJECTED:  
            return COL_DANGER;
        case NotificationType::ORDER_COMPLETED: 
            return COL_WARN;
        case NotificationType::MESSAGE: 
            return COL_ACCENT;
    }
    return COL_MUTED;
}

static ImVec4 OrderColour(OrderStatus s) {
    switch (s) {
        case OrderStatus::ACCEPTED: 
            return COL_SUCCESS;
        case OrderStatus::PENDING:
            return COL_WARN;
        case OrderStatus::REJECTED:
            return COL_DANGER;
        case OrderStatus::COMPLETED: 
            return COL_ACCENT;
    }
    return COL_MUTED;
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
        
        int unread = app.UnreadNotificationCount();
        std::string notifLabel = "  Notifications  ";
        if (unread > 0) {
            notifLabel += "   "; // empty space for the badge
        }
        
        std::string lhsText = roleStr + usernameStr + "    " + notifLabel + "    " + buttonName + "    Logout  ";
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(lhsText.c_str()).x - ImGui::CalcTextSize("      ").x);

        ImGui::TextColored(COL_ACCENT, "%s", roleStr.c_str());
        ImGui::SameLine();

        ImGui::TextColored(COL_SUCCESS, "%s", usernameStr.c_str());
        ImGui::SameLine();

        PushAccentButton(); 
        ImVec2 btnPos = ImGui::GetCursorScreenPos(); // for notification pop-up alignment
        if (ImGui::Button(notifLabel.c_str())) {
            ImGui::OpenPopup("NotificationsPopup");
        }
        ImGui::PopStyleColor(3);

        if (unread > 0) {
            customNotificationCircle(btnPos, unread);
        }

        // Align the popup right underneath the right edge of the button
        ImGuiStyle& style = ImGui::GetStyle();
        float btnWidth = ImGui::CalcTextSize(notifLabel.c_str()).x + style.FramePadding.x * 2.0f;
        ImGuiIO &io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(btnPos.x + btnWidth, btnPos.y + ImGui::GetFrameHeight() + 5.0f), ImGuiCond_Appearing, ImVec2(1.0f, 0.0f));
        ImGui::SetNextWindowSizeConstraints(ImVec2(io.DisplaySize.x * 0.375f, io.DisplaySize.y * 0.4f), ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.8f));
        
        if (ImGui::BeginPopup("NotificationsPopup")) {
            RenderNotifications(app);
            ImGui::EndPopup();
        }

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
    ImGui::InputText("##login_password", password, sizeof(password), ImGuiInputTextFlags_Password);

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
    ImGui::InputText("##reg_password", password, sizeof(password), ImGuiInputTextFlags_Password);
    ImGui::Spacing();

    ImGui::Text("Confirm Password");
    ImGui::InputText("##reg_confirm", confirm, sizeof(confirm), ImGuiInputTextFlags_Password);
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
    else {
        ImGui::PopStyleColor();
    }

    // product catalogue
    SectionLabel(COL_ACCENT, "Your Products");

    static int editingProductId = -1;
    static float editPrice = 0;
    static int editStock = 0;

    if (d->GetProducts().empty()) {
        ImGui::TextColored(COL_MUTED, "   No Products Lister Yet.");
    }

    for (const auto& p : d->GetProducts()) {
        ImGui::PushID(p.GetProductId());
        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_DIM_BG_CARD);
        ImGui::BeginChild("##ordercard", {0, 0}, ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders);

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

        // Per-product review listing (dealer view)
        const auto& revs = p.GetReviews();
        if (!revs.empty()) {
            ImGui::Spacing();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8.0f);
            std::string treeLabel = "  Reviews (" + std::to_string(revs.size()) + ")##drev" + std::to_string(p.GetProductId());
            if (ImGui::TreeNode(treeLabel.c_str())) {
                ImGui::Spacing();
                for (const auto& rev : revs) {
                    std::string revStars;
                    for (int i = 1; i <= 5; i++)
                        revStars += (i <= rev.GetRating() ? "★" : "☆");
                    ImVec4 revStarCol = (rev.GetRating() >= 4) ? COL_SUCCESS : (rev.GetRating() >= 3 ? COL_WARN : COL_DANGER);
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12.0f);
                    ImGui::TextColored(revStarCol, "%s", revStars.c_str());
                    if (!rev.GetComment().empty()) {
                        ImGui::SameLine();
                        ImGui::TextColored(COL_MUTED, u8"\u2014");
                        ImGui::SameLine();
                        ImGui::TextWrapped("%s", rev.GetComment().c_str());
                    }
                }
                ImGui::Spacing();
                ImGui::TreePop();
            }
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

    for (const auto& o : d->GetOrders()) {
        ImGui::PushID(o.GetOrderId());
        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_DIM_BG_CARD);
        ImGui::BeginChild("##productcard", {0, 0}, ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders);

        ImVec4 oStatus = OrderColour(o.GetStatus());
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
        else if (o.GetStatus() == OrderStatus::COMPLETED) {
            ImGui::TextColored(COL_SUCCESS, "Order Received by Recipient");
        }
        else if (o.GetStatus() == OrderStatus::ACCEPTED) {
            ImGui::TextColored(COL_WARN, "Waiting for Recipient's Response");
        }
        else {
            ImGui::TextColored(COL_DANGER, "Order Rejected");
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

    // Review modal state (static so it persists across frames)
    static int reviewOrderId   = -1;
    static int reviewProductId = -1;
    static int reviewRating    = 5;
    static char reviewComment[256] = "";
    static bool openReviewModal = false;

    // Order history cards
    // Use auto-height (0) so the card can expand for the Complete button row
    for (const auto& o : r->GetOrderHistory()) {
        ImGui::PushID(o.GetOrderId());
        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_DIM_BG_CARD);
        ImGui::BeginChild("##orderhist", {0, 0}, ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders);

        ImVec4 oStatus = OrderColour(o.GetStatus());

        ImGui::Spacing();
        ImGui::Text("   Order #%d   |   Product ID: %d   |   Qty: %d", o.GetOrderId(), o.GetProductId(), o.GetQuantity());
        ImGui::SameLine(500);
        ImGui::TextColored(oStatus, "[%s]", o.GetStatusStr().c_str());
        ImGui::Spacing();

        if (o.GetStatus() == OrderStatus::ACCEPTED) {
            ImGui::TextColored(oStatus, "   Order Accepted. Please press the  ");
            ImGui::SameLine();
            PushAccentButton();
            if (ImGui::SmallButton("  Complete  ")) {
                app.CompleteOrder(o.GetOrderId());
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            ImGui::TextColored(oStatus, "  button to receive the order");
        }
        else if (o.GetStatus() == OrderStatus::PENDING) {
            ImGui::TextColored(oStatus, "   Order Pending for Dealer's Approval");
        }
        else if (o.GetStatus() == OrderStatus::REJECTED) {
            ImGui::TextColored(oStatus, "   Order Rejected");
        }
        else {
            // COMPLETED branch — check whether this order has already been reviewed
            bool alreadyReviewed = !r->CanReviewOrder(o.GetOrderId());
            if (alreadyReviewed) {
                ImGui::TextColored(COL_MUTED, "   Order Received  |  ✓ Review Submitted");
            }
            else {
                ImGui::TextColored(oStatus, "   Order Received  ");
                ImGui::SameLine();
                PushWarnButton();
                if (ImGui::SmallButton("  Leave Review  ")) {
                    reviewOrderId = o.GetOrderId();
                    reviewProductId = o.GetProductId();
                    reviewRating = 5;
                    reviewComment[0] = '\0';
                    openReviewModal = true;
                }
                ImGui::PopStyleColor(3);
            }
        }

        ImGui::Spacing();
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::Spacing();
        ImGui::PopID();
    }

    // Trigger the popup (must be called outside the child loop)
    if (openReviewModal) {
        ImGui::OpenPopup("Leave a Review##modal");
        openReviewModal = false;
    }

    // Review modal popup
    ImGui::SetNextWindowSize({460, 0}, ImGuiCond_Always); // fixed width, auto height
    if (ImGui::BeginPopupModal("Leave a Review##modal", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

        SectionLabel(COL_ACCENT, "Rate this product");

        // Star rating slider
        ImGui::Text("Rating  (1 – 5)");
        ImGui::SameLine();
        ImGui::TextColored(COL_MUTED, "— optional");
        ImGui::PushItemWidth(220);
        ImGui::SliderInt("##rev_rating", &reviewRating, 1, 5);
        ImGui::PopItemWidth();

        // Visual star row next to the slider
        ImGui::SameLine();
        std::string stars;
        for (int i = 1; i <= 5; i++) {
              stars += (i <= reviewRating ? "★" : "☆");
        }
        ImVec4 starCol = (reviewRating >= 4) ? COL_SUCCESS : (reviewRating >= 3 ? COL_WARN : COL_DANGER);
        ImGui::TextColored(starCol, "%s", stars.c_str());

        ImGui::Spacing();

        // Comment box
        ImGui::Text("Comment");
        ImGui::SameLine();
        ImGui::TextColored(COL_MUTED, "— optional");
        ImGui::InputTextMultiline("##rev_comment", reviewComment, sizeof(reviewComment), {440, 80});

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Submit
        PushSuccessButton();
        if (ImGui::Button("  Submit Review  ", {140, 32})) {
            app.SubmitReview(reviewOrderId, reviewProductId, reviewRating, std::string(reviewComment));
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();

        // Skip — closes without submitting, does NOT mark as reviewed
        PushDangerButton();
        if (ImGui::Button("  Skip  ", {100, 32})) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor(3);

        ImGui::Spacing();
        ImGui::EndPopup();
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
    for (const auto &p : app.GetDatabase().GetAllProducts()) {
        // apply filters — show product if filter matches name OR category (fixed from original &&)
        if (!filter.empty()) {
            std::string nameLower = p.GetName();
            std::string catLower = p.GetCategory();
            std::string fLower = filter;
            for (char &c : nameLower) c = (char)tolower(c);
            for (char &c : catLower) c = (char)tolower(c);
            for (char &c : fLower) c = (char)tolower(c);
            // skip if filter doesn't match either name or category
            if (nameLower.find(fLower) == std::string::npos &&
                catLower.find(fLower) == std::string::npos) continue;
        }
        if (p.GetPrice() < minPrice || p.GetPrice() > maxPrice) continue;

        shown++;
        const auto& reviews = p.GetReviews();
        bool hasReviews = !reviews.empty();

        ImGui::PushID(p.GetProductId());
        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_DIM_BG_CARD);

        // Use auto-height when there are reviews to display, fixed height otherwise
        float cardH = hasReviews ? 0.0f : 75.0f;
        ImGui::BeginChild("##plist", {0, cardH}, ImGuiChildFlags_Borders | (hasReviews ? ImGuiChildFlags_AutoResizeY : 0));

        // Product header row
        ImGui::Spacing();
        ImGui::TextColored(COL_ACCENT, "  [%d] %s", p.GetProductId(), p.GetName().c_str());
        ImGui::SameLine(500);

        // Star rating summary
        float avg = p.GetAvgRating();
        ImVec4 rCol;
        if (avg >= 4.0f) {
          rCol = COL_SUCCESS;
        }
        else if (avg >= 2.5f) {
          rCol = COL_WARN;
        }
        else {
          rCol = COL_DANGER;
        }

        if (hasReviews) {
            ImGui::TextColored(rCol, "★ %.1f  (%d review%s)", avg, (int)reviews.size(), reviews.size() == 1 ? "" : "s");
        }
        else {
            ImGui::TextColored(COL_MUTED, "No reviews yet");
        }

        ImGui::Text("   Category: %-20s   Price: %.2f BDT   Stock: %d",
                    p.GetCategory().c_str(), p.GetPrice(), p.GetStock());

        // Order button / out-of-stock / login nudge
        if (isRetailer) {
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
        else if (!state.isLoggedIn) {
            ImGui::Spacing();
            ImGui::TextColored(COL_MUTED, "  Login as a retailer to place orders");
        }

        // Review list (collapsible)
        if (hasReviews) {
            ImGui::Spacing();
            // Indent the tree node slightly
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8.0f);
            std::string treeLabel = "  Reviews (" + std::to_string(reviews.size()) + ")##tree" + std::to_string(p.GetProductId());
            if (ImGui::TreeNode(treeLabel.c_str())) {
                ImGui::Spacing();
                for (const auto& rev : reviews) {
                    // Build star string for this review
                    std::string revStars;
                    for (int i = 1; i <= 5; i++) {
                        revStars += (i <= rev.GetRating() ? "★" : "☆");
                    }
                    ImVec4 revStarCol = (rev.GetRating() >= 4) ? COL_SUCCESS : (rev.GetRating() >= 3 ? COL_WARN : COL_DANGER);

                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12.0f);
                    ImGui::TextColored(revStarCol, "%s", revStars.c_str());

                    if (!rev.GetComment().empty()) {
                        ImGui::SameLine();
                        ImGui::TextColored(COL_MUTED, "—");
                        ImGui::SameLine();
                        ImGui::TextWrapped("%s", rev.GetComment().c_str());
                    }
                }
                ImGui::Spacing();
                ImGui::TreePop();
            }
        }

        ImGui::Spacing();
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

// notification of a user
static void RenderNotifications (App &app) {
    AppState &state = app.GetState();

    if (!state.isLoggedIn) {
        ImGui::TextColored(COL_DANGER, "You must be logged in to view notifications.");
        return;
    }

    // summary card
    SectionLabel(COL_ACCENT, "Notifications");

    // app wrapper already filtered and newest-first
    std::vector<const Notification*> mine = app.GetNotificationsForUser();

    // Mark all as read button
    int unread = app.UnreadNotificationCount();
    if (unread > 0) {
        PushAccentButton();
        if (ImGui::Button("  Mark all as read  ")) {
            for (const auto *n : mine)
                if (!n->IsRead())
                    app.MarkNotificationRead(n->GetNotificationId());
        }
        ImGui::PopStyleColor(3);
        ImGui::Spacing();
    }

    if (mine.empty()) {
        ImGui::TextColored(COL_MUTED, "  No notifications yet.");
    }

    ImGui::BeginChild("##notif_scroll", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    // Render newest -> oldest
    for (int i = 0; i < (int)mine.size(); ++i) {
        const Notification *n = mine[i];
        ImGui::PushID(n->GetNotificationId());

        // Unread notifications get a brighter card
        ImVec4 cardBg = n->IsRead() ? COL_DIM_BG_CARD : COL_BRIGHT_BG_CARD;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, cardBg);
        ImGui::BeginChild("##ncard", ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders);

        ImVec4 badgeCol = NotifColour(n->GetType());
        ImGui::TextColored(badgeCol, "[%s]", NotificationTypeToString(n->GetType()).c_str());
        ImGui::SameLine();

        // Unread dot (shows '?' currently; have to fix)
        if (!n->IsRead()) {
            ImGui::TextColored(COL_WARN, "● ");
        }
        ImGui::SameLine();

        ImGui::TextWrapped("%s", n->GetMessage().c_str());
        ImGui::Spacing();

        if (!n->IsRead()) {
            PushAccentButton();
            if (ImGui::SmallButton("  Mark as Read  "))
                app.MarkNotificationRead(n->GetNotificationId());
            ImGui::PopStyleColor(3);
        } else {
            ImGui::TextColored(COL_MUTED, "  Read");
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::Spacing();
        ImGui::PopID();
    }
    ImGui::EndChild();
}


static void RenderPlaceOrderPage (App &app) {
    AppState &state = app.GetState();


}