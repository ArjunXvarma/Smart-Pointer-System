#include <iostream>
#include <memory>
#include "../include/control_block.hpp"

void test_control_block() {
    using CB = separate_control_block<int, std::default_delete<int>>;

    CB* cb = new CB(new int(42), std::default_delete<int>());

    cb->add_strong();
    cb->release_strong();
    cb->release_strong();

    cb->add_weak();
    cb->release_weak();
}

int main() {
    test_control_block();
    std::cout << "Successfully deleted control block!" << std::endl;

    return 0;
}