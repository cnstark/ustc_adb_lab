#include <iostream>
#include "buffer_manager.hpp"
#include "instruction.hpp"

using namespace adb;
using namespace std;

void init_db(const BufferManager::sptr &bm) {
    for (unsigned int j = 1; j <= 50000; j++) {
        auto frame = generate_random_frame();
        bm->fix_new_page(frame);
    }
}

void run_test(const BufferManager::sptr &bm) {
    auto ins_vector = Instruction::read_instructions("./data-5w-50w-zipf.txt");
    int index = 0;
    for (auto &i : *ins_vector) {
        cout << index++ << ": " << i << endl;
        i.execute(bm);
    }
}

int main() {
    auto bm = make_shared<BufferManager>();
    run_test(bm);
    return 0;
}