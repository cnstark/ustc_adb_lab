//
// Created by Yuhao Wang on 2019/11/26.
//

#include "data_storage_manager.hpp"
#include <iostream>

// 首部存放信息的page不计入page_id，page_id从0开始
namespace adb {
    DataStorageManager::DataStorageManager() {
        db_file = nullptr;
        open_file();
        read_first_page();
        io_count = 0;
    }

    DataStorageManager::~DataStorageManager() {
        write_first_page();
        close_file();
    }

    void DataStorageManager::open_file() {
        db_file = fopen(DB_FILE_PATH, "rb+");
        if (db_file == nullptr) {
            db_file = fopen(DB_FILE_PATH, "wb+");
            char temp[FIRST_PAGE_SIZE] = {0};
            fwrite(&temp, FIRST_PAGE_SIZE * sizeof(char), 1, db_file);
            close_file();
            db_file = fopen(DB_FILE_PATH, "rb+");
            assert(db_file != nullptr);
        }
    }

    int DataStorageManager::close_file() {
        if (fclose(db_file) == -1) {
            return -1;
        } else {
            db_file = nullptr;
            return 0;
        }
    }

    int DataStorageManager::read_page(int page_id, Frame *frm) {
        if (!is_page_exist(page_id)) {
            return -1;
        }
        unsigned int page_pointer = get_page_pointer(page_id);
        seek(page_pointer);
        fread(frm->field, sizeof(char), FRAME_SIZE, db_file);
        inc_io_count();
        return 0;
    }

    int DataStorageManager::write_page(int page_id, Frame *frm) {
        if (!is_page_exist(page_id)) {
            return -1;
        }
        unsigned int page_pointer = get_page_pointer(page_id);
        seek(page_pointer);
        fwrite(frm->field, sizeof(char), FRAME_SIZE, db_file);
        inc_io_count();
        return 0;
    }

    int DataStorageManager::create_new_page(Frame *frm) {
        int page_id = get_pages_num() + 1;
        int page_pointer = (page_id - 1) * (int)sizeof(frm->field) + FIRST_PAGE_SIZE;
        set_page_pointer(page_id, page_pointer);
        seek(page_pointer);
        fwrite(frm->field, sizeof(char), FRAME_SIZE, db_file);
        inc_pages_num();
        inc_io_count();
        return page_id;
    }

    int DataStorageManager::seek(unsigned int offset) {
        return fseek(db_file, offset, SEEK_SET);
    }

    void DataStorageManager::inc_pages_num() {
        first_page[0]++;
    }

    int DataStorageManager::get_pages_num() {
        return first_page[0];
    }

    unsigned int DataStorageManager::get_page_pointer(int page_id) {
        return first_page[page_id] & 0x7fffffff;
    }

    void DataStorageManager::set_page_pointer(int page_id, int pointer) {
        first_page[page_id] = pointer;
    }

    void DataStorageManager::set_use(int page_id, bool is_use) {
        if (is_use) {
            first_page[page_id] = first_page[page_id] & 0x7fffffff;
        } else {
            first_page[page_id] = first_page[page_id] | 0x80000000;
        }
    }

    bool DataStorageManager::get_use(int page_id) {
        return !(first_page[page_id] & 0x80000000);
    }

    bool DataStorageManager::is_page_exist(int page_id) {
        return page_id > 0 && page_id <= first_page[0] && get_use(page_id);
    }

    void DataStorageManager::inc_io_count() {
        io_count++;
    }

    int DataStorageManager::get_io_count() {
        return io_count;
    }

    void DataStorageManager::read_first_page() {
        seek(0);
        fread(first_page, sizeof(int), MAX_PAGE_NUM, db_file);
    }

    void DataStorageManager::write_first_page() {
        seek(0);
        fwrite(first_page, sizeof(int), MAX_PAGE_NUM, db_file);
    }
}
