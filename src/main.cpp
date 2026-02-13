#include "app.hpp"

int main(int argc, char* argv[])
{
    Gin::App app;
    
    if (!app.initialize("Gin", 1280, 720))
    {
        return -1;
    }
    
    app.run();
    
    return 0;
}