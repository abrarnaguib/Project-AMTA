#include "gui.h"
#include "../core/app.h"
#include "imgui.h"
#include<string>



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
    ImGui::TextColored(COL_ACCENT, "Login Page");

    // for temp testing
    if (ImGui::Button("test")) {
        // state.isLoggedIn = true;
        state.currentPage = AppState::Page::DASHBOARD;
        
    }
    

}


static void RenderRegisterPage (App &app) {
    AppState &state = app.GetState();
    ImGui::TextColored(COL_DANGER, "Register Page");

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
