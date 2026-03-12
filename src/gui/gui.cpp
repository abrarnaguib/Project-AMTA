#include "gui.h"
#include "../core/app.h"
#include "imgui.h"
#include<string>
#include<iostream>



// forward declaration for each and every page / panel
static void RenderHomePage (App &app);
static void RenderLoginPage (App &app);
static void RenderRegisterPage (App &app);
static void RenderDashBoard (App &app);
static void RenderProductList (App &app);
static void RenderPlaceOrderPage (App &app);
static void RenderUtilBar (App &app); // Render the top utility bar (account info, login logout buttons etc.)
static void RenderStatusBar(App &app);  // Render the bottom action status bar (wrong input, wrong password etc.)


// colour palette 
// color vectors
static const ImVec4 COL_ACCENT { 0.20f, 0.60f, 1.00f, 1.00f }; // blue
static const ImVec4 COL_SUCCESS { 0.18f, 0.80f, 0.44f, 1.00f }; // green
static const ImVec4 COL_DANGER { 0.90f, 0.25f, 0.25f, 1.00f }; // red
static const ImVec4 COL_MUTED   { 0.60f, 0.60f, 0.60f, 1.00f }; // dark grey
static const ImVec4 COL_WARN    { 1.00f, 0.75f, 0.10f, 1.00f }; // yellow
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


namespace GUI {
    void Render(App& app) {
        // window styles
        ImGui::StyleColorsDark();
        ImGuiStyle &style = ImGui::GetStyle();

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
        ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize(lhsText.c_str()).x - ImGui::CalcTextSize("     ").x);
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
        // std::string roleStr = (state.currentUser->GetRole() == UserRole::DEALER ? "[dealer]" : "[retailer]");
        std::string roleStr = "Dealer";
        // std::string usernameStr = state.currentUser->GetUsername();
        std::string usernameStr = "Talal";
        std::string lhsText = roleStr + usernameStr + "    DashBoard    Logout  ";
        ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize(lhsText.c_str()).x - ImGui::CalcTextSize("     ").x);

        ImGui::TextColored(COL_ACCENT, "%s", roleStr.c_str());
        ImGui::SameLine();

        ImGui::TextColored(COL_SUCCESS, "%s", usernameStr.c_str());
        ImGui::SameLine();

        PushAccentButton();
        if (ImGui::Button("  DashBoard  ")) {
            state.currentPage = AppState::Page::DASHBOARD;
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



}


static void RenderLoginPage (App &app) {
    AppState &state = app.GetState();
    
    
    ImGui::Spacing();
    ImGui::TextColored(COL_ACCENT, "Login to your account");
    ImGui::Spacing();
    ImGui::Separator();

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
    

}


static void RenderRegisterPage (App &app) {
    AppState &state = app.GetState();

    ImGui::Spacing();
    ImGui::TextColored(COL_ACCENT, "Create a new account");
    ImGui::Spacing();
    ImGui::Separator();

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

    ImGui::TextColored(COL_MUTED, "Already Have an Account?");
    ImGui::SameLine();
    ImGui::TextColored(COL_ACCENT, "Login");
    if (ImGui::IsItemClicked()) {
        state.currentPage = AppState::Page::LOGIN;
    }

}


static void RenderDashBoard (App &app) {
    AppState &state = app.GetState();
    ImGui::TextColored(COL_DANGER, "DashBoard Page");
    ImGui::SameLine();
    ImGui::Button("ssss");

}


static void RenderProductList (App &app) {
    AppState &state = app.GetState();


}


static void RenderPlaceOrderPage (App &app) {
    AppState &state = app.GetState();


}
