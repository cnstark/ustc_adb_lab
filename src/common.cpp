//
// Created by Yuhao Wang on 2019/12/16.
//

#include "common.hpp"

namespace adb {

    int BCB::get_page_id() const {
        return page_id;
    }

    int BCB::get_frame_id() const {
        return frame_id;
    }

    bool BCB::is_dirty() const {
        return dirty;
    }

    void BCB::set_dirty() {
        dirty = true;
    }

    void BCB::unset_dirty() {
        dirty = false;
    }

    LRU::LRU() {
        lru_list = new std::list<int>();
        lru_map = new std::unordered_map<int, std::list<int>::iterator>();
    }

    LRU::~LRU() {
        free(lru_list);
        free(lru_map);
    }

    int LRU::get_victim() {
        int victim = lru_list->back();
        lru_list->pop_back();
        return victim;
    }

    void LRU::push(int id) {
        lru_list->push_front(id);
        (*lru_map)[id] = lru_list->begin();
    }

    void LRU::update(int id) {
        auto iter = (*lru_map)[id];
        lru_list->erase(iter);
        push(id);
    }

    Frame::sptr generate_random_frame() {
        auto frame = std::make_shared<Frame>();
        for (unsigned int i = 0; i < FRAME_SIZE - 1; i++) {
            frame->field[i] = 'a' + arc4random_uniform(26);
        }
        frame->field[FRAME_SIZE - 1] = 0;
        return frame;
    }
}
