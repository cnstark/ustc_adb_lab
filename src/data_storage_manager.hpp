//
// Created by Yuhao Wang on 2019/11/26.
//

#ifndef ADB_PROJECT_DATA_STORAGE_MANAGER_HPP
#define ADB_PROJECT_DATA_STORAGE_MANAGER_HPP

#include <cstdio>
#include <string>
#include "common.hpp"

#define DB_FILE_PATH "./data.dbf"

#define MAX_PAGE_NUM (50 * 1024)  //51200，一个文件存放的最大page数
#define FIRST_PAGE_SIZE (MAX_PAGE_NUM * 4) //200kb，为第一个page的大小，存放page数与索引

namespace adb {
    class DataStorageManager {
    public:
        DataStorageManager();

        ~DataStorageManager();

        bool is_page_exist(int page_id);

        int read_page(int page_id, Frame *frm);

        int write_page(int page_id, Frame *frm);

        int create_new_page(Frame *frm);

        int get_pages_num();

        int get_io_count();

    private:
        FILE *db_file;

        /**
         * 第一个page，存放page数与指针
         */
        int first_page[MAX_PAGE_NUM] = {0};

        int io_count;

        /***
         * 打开DB文件
         * @return
         */
        void open_file();

        int close_file();

        int seek(unsigned int offset);

        void read_first_page();

        void write_first_page();

        unsigned int get_page_pointer(int page_id);

        void set_page_pointer(int page_id, int pointer);

        void set_use(int page_id, bool is_use);

        bool get_use(int page_id);

        void inc_pages_num();

        void inc_io_count();
    };
}

#endif //ADB_PROJECT_DATA_STORAGE_MANAGER_HPP
