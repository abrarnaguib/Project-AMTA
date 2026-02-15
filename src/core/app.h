#pragma once

struct AppState
{
    int counter = 0;
};

class App
{
public:
    void Update();
    AppState& GetState();

private:
    AppState state;
};
