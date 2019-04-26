#include "server.hpp"

int main() {
    //auto const port = static_cast<unsigned short>(3388);
    server("0.0.0.0", 3388);
    return 0;
}
