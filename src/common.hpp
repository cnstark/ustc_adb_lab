//
// Created by Yuhao Wang on 2019/11/27.
//

#ifndef ADB_PROJECT_COMMON_HPP
#define ADB_PROJECT_COMMON_HPP

#include <cstdlib>
#include <memory>
#include <list>
#include <unordered_map>

#define FRAME_SIZE 4096

namespace adb {
    struct Frame {
        typedef std::shared_ptr<Frame> sptr;
        char field[FRAME_SIZE] = {};
    };

    class BCB {
    public:
        BCB(int page_id, int frame_id) : page_id(page_id), frame_id(frame_id), dirty(false) {};

        int get_page_id() const;

        int get_frame_id() const;

        bool is_dirty() const;

        void set_dirty();

        void unset_dirty();

    private:
        int page_id;
        int frame_id;
        bool dirty;
    };

    class LRU {
    public:
        LRU();

        ~LRU();

        /**
         * 选择一个victim，将其从链表中移出
         */
        int get_victim();

        /**
         * 将frame放入头部
         */
        void push(int id);

        /**
         * 将frame标记为最新使用，从list中移动至头部
         */
        void update(int id);

    private:
        std::list<int> *lru_list;
        std::unordered_map<int, std::list<int>::iterator> *lru_map;
    };

    /**
     * 随机生成frame内容
     */
    Frame::sptr generate_random_frame();
}

#endif //ADB_PROJECT_COMMON_HPP
