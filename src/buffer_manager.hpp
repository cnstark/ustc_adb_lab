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

        /**
         * 外部接口，读取一个page
         */
        Frame::sptr read_page(int page_id);

        /**
         * 外部接口，写入一个page
         */
        void write_page(int page_id, const Frame::sptr &frame);

        /**
         * 将page读入buffer中并返回frame_id
         * 若目标page已存在于buffer中，则直接返回，并更新lru
         * 若目标page不存在于buffer中，则：
         *     若buffer未满，则将page读入未使用的frame，并更新lru
         *     若buffer已满，则调用select_victim找出victim并回收，再将page读入该frame，并更新lru
         */
        int fix_page(int page_id);

        /**
         * 与函数fix_page(int page_id)功能相同
         * 区别为当操作为写时，若目标page不存在于buffer中，只找到相应的frame空间，不将page读入其中
         */
        int fix_page(bool is_write, int page_id);

        /**
         * 创建一个新page，写入buffer中
         * 并将其设置为dirty
         */
        void fix_new_page(const Frame::sptr &frame);

        /**
         * 获取空闲frame的个数
         */
        int get_free_frames_num();

        /**
         * 获取IO计数
         */
        int get_io_count();

        /**
         * 获取buffer命中次数
         */
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

        /**
         * hash函数，计算公式为 page_id % DEF_BUF_SIZE
         */
        static int hash_func(int page_id);

        /**
         * 根据lru策略选择一个需要替换的frame
         * 若该frame为dirty，则调用write_page将frame写入磁盘
         * 删除该frame的hash信息（两个hash表）
         */
        int select_victim();

        /**
         * 清空buffer（析构函数中调用）
         * 将buffer中所有dirty的frame写回磁盘
         */
        void clean_buffer();

        /**
         * 在hash表中查找，根据frame_id获取page_id
         */
        int get_page_id(int frame_id);

        /**
         * 设置hash表，将page_id与frame_id关联
         */
        void set_page_id(int frame_id, int page_id);

        /**
         * 插入新的BCB
         */
        void insert_bcb(int page_id, int frame_id);

        /**
         * 获取指定page的BCB
         */
        BCB *get_bcb(int page_id);

        /**
         * 将frame设置为dirty
         */
        void set_dirty(int frame_id);

        /**
         * 取消frame的dirty状态
         * @param frame_id
         */
        void unset_dirty(int frame_id);

        /**
         * 增加命中计数
         */
        void inc_hit_count();
    };
}

#endif //ADB_PROJECT_BUFFER_MANAGER_HPP
