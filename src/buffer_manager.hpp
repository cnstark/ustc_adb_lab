//
// Created by Yuhao Wang on 2019/11/26.
//

#ifndef ADB_PROJECT_BUFFER_MANAGER_HPP
#define ADB_PROJECT_BUFFER_MANAGER_HPP

#include "common.hpp"
#include "data_storage_manager.hpp"
#include <list>

#define DEF_BUF_SIZE 1024

using std::list;

namespace adb {
    class BufferManager {
    public:
        typedef std::shared_ptr<BufferManager> sptr;

        BufferManager();

        ~BufferManager();

        Frame::sptr read_page(int page_id);

        void write_page(int page_id, const Frame::sptr &frame);

        int fix_page(bool is_write, int page_id);

        void fix_new_page(const Frame::sptr &frame);

        void clean_buffer();

        int get_io_count();

        int get_hit_count();

    private:
        Frame buffer[DEF_BUF_SIZE]{};
        DataStorageManager *dsm;
        int free_frames_num;
        // Hash Table
        int frame_to_page[DEF_BUF_SIZE]{};
        list<BCB> page_to_frame[DEF_BUF_SIZE];
        // lru list
        LRU *lru;

        int hit_count;

        static int get_page_id_hash(int page_id);

        int get_free_frames_num();

        int get_page_id(int frame_id);

        void set_page_id(int frame_id, int page_id);

        void insert_bcb(int page_id, int frame_id);

        BCB *get_bcb(int page_id);

        int select_victim();

        void set_dirty(int frame_id);

        void unset_dirty(int frame_id);

        void inc_hit_count();
    };
}

#endif //ADB_PROJECT_BUFFER_MANAGER_HPP
