//
// Created by Yuhao Wang on 2019/12/16.
//

#ifndef ADB_PROJECT_INSTRUCTION_HPP
#define ADB_PROJECT_INSTRUCTION_HPP

#include "buffer_manager.hpp"
#include <cstring>
#include <iostream>
#include <vector>
#include <fstream>

namespace adb {
    class Instruction {
    public:
        typedef std::shared_ptr<std::vector<Instruction>> vector;

        explicit Instruction(const std::string &s) {
            int sp = s.find(',');
            std::string a = s.substr(0, sp);
            std::string b = s.substr(sp + 1, s.length());
            is_write = a == std::string("1");
            page_id = std::stoi(b);
        }

        Instruction(bool is_write, int page_id) : is_write(is_write), page_id(page_id) {};

        friend std::ostream &operator<<(std::ostream &os, const Instruction &i) {
            os << "Instruction(" << (i.is_write ? "write," : "read,") << i.page_id << ")";
            return os;
        }

        void execute(const BufferManager::sptr &bm) {
            if (is_write) {
                auto new_frame = generate_random_frame();
                bm->write_page(page_id, new_frame);
            } else {
                bm->read_page(page_id);
            }
        };

        static Instruction::vector read_instructions(const std::string &filename) {
            std::ifstream test_file(filename);
            assert(test_file.is_open());
            std::string line;
            auto ins_vector = std::make_shared<std::vector<Instruction>>();
            while (getline(test_file, line)) {
                ins_vector->emplace_back(line);
            }
            return ins_vector;
        }

    private:
        bool is_write;
        int page_id;
    };
}

#endif //ADB_PROJECT_INSTRUCTION_HPP
