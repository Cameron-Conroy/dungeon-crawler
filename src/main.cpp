#include "Application.hpp"
#include <cstdlib>
#include <ctime>

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    Application app;
    app.run();

    return 0;
}
