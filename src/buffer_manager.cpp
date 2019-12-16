//
// Created by Yuhao Wang on 2019/11/26.
//

#include "buffer_manager.hpp"
#include <cstring>
#include <iostream>
#include <memory>

namespace adb {
    BufferManager::BufferManager() {
        dsm = new DataStorageManager();
        lru_list = new list<int>();
        free_frames_num = DEF_BUF_SIZE;
        hit_count = 0;
    }

    BufferManager::~BufferManager() {
        clean_buffer();
        std::cout << "Total IO count: " << get_io_count() << std::endl;
        std::cout << "Total hit count: " << get_hit_count() << std::endl;
        delete dsm;
        delete lru_list;

    }

    int BufferManager::fix_page(bool is_write, int page_id) {
        BCB *bcb = get_bcb(page_id);
        std::cout << "    op: " << (is_write?"w":"r");
        // buffer中不存在该page
        if (bcb == nullptr) {
            if (dsm->is_page_exist(page_id)) {
                std::cout << ", page_id: " << page_id;
                int frame_id;
                // buffer已满
                if (free_frames_num == 0) {
                    frame_id = select_victim();
                } else {
                    frame_id = DEF_BUF_SIZE - free_frames_num;
                    free_frames_num--;
                    std::cout << ", frame_id: " << frame_id
                              << ", not hit" << ", buffer not full" << std::endl;
                }
                BCB *new_bcb = new BCB(page_id, frame_id);
                insert_bcb(new_bcb);
                lru_list->push_front(frame_id);
                set_page_id(frame_id, page_id);
                // 如果是写操作，则不读取page
                if (!is_write) {
                    dsm->read_page(page_id, buffer + frame_id);
                    std::cout << "    IO count: " << get_io_count() << std::endl;
                }
                return frame_id;
            } else {
                return -1; // page_id 不存在
            }
        } else {
            int frame_id = bcb->frame_id;
            update_lru(frame_id);
            std::cout << ", page_id: " << page_id << ", frame_id: " << frame_id << ", hit!" << std::endl;
            inc_hit_count();
            std::cout << "    hit count: " << get_hit_count() << std::endl;
            return frame_id;
        }
    }

    void BufferManager::fix_new_page(const Frame::sptr &frame) {
        int frame_id;
        // buffer已满
        if (free_frames_num == 0) {
            frame_id = select_victim();
        } else {
            frame_id = DEF_BUF_SIZE - free_frames_num;
            free_frames_num--;
        }
        memcpy((buffer + frame_id)->field, frame->field, FRAME_SIZE);
        int page_id = dsm->create_new_page(buffer + frame_id);
        BCB *new_bcb = new BCB(page_id, frame_id);
        insert_bcb(new_bcb);
        lru_list->push_front(frame_id);
        set_page_id(frame_id, page_id);
    }

    int BufferManager::get_free_frames_num() {
        return free_frames_num;
    }

    int BufferManager::select_victim() {
        if (!lru_list->empty()) {
            int frame_id = lru_list->back();
            int victim_page_id = get_page_id(frame_id);

            std::cout << ", frame_id: " << frame_id
                      << ", not hit" << ", victim_page_id: " << victim_page_id;

            int hash = get_page_id_hash(victim_page_id);
            BCB **p = page_to_frame + hash;
            if ((*p)->page_id == victim_page_id) {
                if ((*p)->dirty) {
                    dsm->write_page(victim_page_id, buffer + frame_id);
                    std::cout << ", dirty" << std::endl;
                    std::cout << "    IO count: " << get_io_count();
                }
                auto t = (*p)->next;
                free((*p));
                (*p) = t;
            } else {
                BCB *pre = *p;
                BCB *now = pre->next;
                while (now != nullptr) {
                    if (now->page_id == victim_page_id) {
                        if (now->dirty) {
                            dsm->write_page(victim_page_id, buffer + frame_id);
                            std::cout << ", dirty" << std::endl;
                            std::cout << "    IO count: " << get_io_count();
                        }
                        pre->next = now->next;
                        free(now);
                        break;
                    }
                    pre = now;
                    now = now->next;
                }
            }
            lru_list->pop_back();
            std::cout << std::endl;
            return frame_id;
        }
        return 0;
    }

    int BufferManager::get_page_id_hash(int page_id) {
        return page_id % DEF_BUF_SIZE;
    }

    void BufferManager::set_dirty(int frame_id) {
        int page_id = get_page_id(frame_id);
        BCB *bcb = get_bcb(page_id);
        bcb->dirty = true;
    }

    void BufferManager::unset_dirty(int frame_id) {
        int page_id = get_page_id(frame_id);
        BCB *bcb = get_bcb(page_id);
        bcb->dirty = false;
    }

    void BufferManager::clean_buffer() {
        std::cout << "Cleaning buffer" << std::endl;
        for (BCB *bcb : page_to_frame) {
            auto p = bcb;
            while (p != nullptr) {
                if (p->dirty) {
                    dsm->write_page(p->page_id, buffer + p->frame_id);
                    std::cout << "    IO count: " << get_io_count() << std::endl;
                }
                p = p->next;
            }
        }
    }

    BCB *BufferManager::get_bcb(int page_id) {
        int hash = get_page_id_hash(page_id);
        for (BCB *p = page_to_frame[hash]; p != nullptr; p = p->next) {
            if (p->page_id == page_id) {
                return p;
            }
        }
        return nullptr;
    }

    void BufferManager::insert_bcb(BCB *bcb) {
        int hash = get_page_id_hash(bcb->page_id);
        BCB **end = &page_to_frame[hash];
        if (*end == nullptr) {
            *end = bcb;
            return;
        }
        BCB *a = *end;
        while (a->next != nullptr) {
            a = a->next;
        }
        a->next = bcb;
    }

    int BufferManager::get_page_id(int frame_id) {
        return frame_to_page[frame_id];
    }

    void BufferManager::set_page_id(int frame_id, int page_id) {
        frame_to_page[frame_id] = page_id;
    }

    Frame::sptr BufferManager::read_page(int page_id) {
        int frame_id = fix_page(false, page_id);
        auto frame = std::make_shared<Frame>();
        memcpy(frame->field, (buffer + frame_id)->field, FRAME_SIZE);
        return frame;
    }

    void BufferManager::write_page(int page_id, const Frame::sptr &frame) {
        int frame_id = fix_page(true, page_id);
        memcpy((buffer + frame_id)->field, frame->field, FRAME_SIZE);
        set_dirty(frame_id);
    }

    int BufferManager::get_io_count() {
        return dsm->get_io_count();
    }

    void BufferManager::update_lru(int frame_id) {
        for (auto i = lru_list->begin(); i != lru_list->end(); i++) {
            if (*i == frame_id) {
                lru_list->erase(i);
                break;
            }
        }
        lru_list->push_front(frame_id);
    }

    void BufferManager::inc_hit_count() {
        hit_count++;
    }

    int BufferManager::get_hit_count() {
        return hit_count;
    }
}