#include "gui.h"
#include "../core/app.h"
#include "imgui.h"

namespace GUI
{
    void Render(App& app)
    {
        ImGui::Begin("Control Panel");

        AppState& state = app.GetState();

        ImGui::Text("Counter: %d", state.counter);

        if (ImGui::Button("Increment"))
        {
            state.counter++;
        }
        if (ImGui::Button("Decrement"))
        {
            state.counter--;
        }
        if (ImGui::Button("Multiply")) 
        {
            state.counter *= 2;
        }
        if (ImGui::Button("Divide")) 
        {
            state.counter /= 2;
        }
        ImGui::End();
    }
}
