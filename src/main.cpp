#include <iostream>
#include <fstream>
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

Instruction::vector read_instructions(const string &filename) {
    ifstream test_file(filename);
    assert(test_file.is_open());
    string line;
    auto ins_vector = make_shared<vector<Instruction>>();
    while (getline(test_file, line)) {
        ins_vector->emplace_back(line);
    }
    return ins_vector;
}

int main() {
    auto bm = make_shared<BufferManager>();
    auto ins_vector = read_instructions("./data-5w-50w-zipf.txt");

    int index = 0;
    for (auto & i : *ins_vector) {
        cout << index++ << ": " << i << endl;
        i .execute(bm);
    }
    return 0;
}