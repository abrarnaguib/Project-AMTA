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

        ImGui::End();
    }
}
