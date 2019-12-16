//
// Created by Yuhao Wang on 2019/11/26.
//

#ifndef ADB_PROJECT_DATA_STORAGE_MANAGER_HPP
#define ADB_PROJECT_DATA_STORAGE_MANAGER_HPP

#include <cstdio>
#include <string>
#include "common.hpp"

#define DB_FILE_PATH "./data.dbf"

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

        unsigned int get_page_pointer(int page_id);

        void set_use(int index, int use_bit);

        int get_use(int index);

        int get_io_count();

    private:
        FILE *db_file;
        int pages_num;

        int io_count;

        /***
         * 打开DB文件
         * @return
         */
        void open_file();

        int close_file();

        int seek(unsigned int offset);

        void inc_pages_num();

        int read_pages_num();

        void inc_io_count();
    };
}


#endif //ADB_PROJECT_DATA_STORAGE_MANAGER_HPP
