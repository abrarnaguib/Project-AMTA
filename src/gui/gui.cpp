#include "gui.h"
#include "../core/app.h"
#include "imgui.h"



// forward declaration for each and every page / panel
static void RenderHomePage (App &app);
static void RenderLoginPage (App &app);
static void RenderRegisterPage (App &app);
static void RenderDashBoard (App &app);
static void RenderProductList (App &app);
static void RenderPlaceOrderPage (App &app);


namespace GUI
{
    void Render(App& app)
    {
        // window styles
        ImGui::StyleColorsDark();


        // full screen host window
        ImGuiIO &io = ImGui::GetIO();
        ImGui::SetWindowPos({0, 0});
        ImGui::SetWindowSize(io.DisplaySize);
        ImGui::SetNextWindowBgAlpha(1.0f);

        // window poistion and size fixed        
        ImGui::Begin("##root", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | 
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus 
            | ImGuiWindowFlags_NoScrollbar);
        
        // Main contents in child region (leaves room for other bars)
        float statusH = 55.0f;
        ImGui::BeginChild("##mainpage",{0, io.DisplaySize.y - 2 * statusH}, 0);

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
        ImGui::End();
    }
}
