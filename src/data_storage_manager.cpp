//
// Created by Yuhao Wang on 2019/11/26.
//

#include "data_storage_manager.hpp"
#include <iostream>

#define FIRST_PAGE_SIZE 200 * 1024 //200kb，为第一个page的大小，存放page数与索引

// 首部存放信息的page不计入page_id，page_id从0开始
namespace adb {
    DataStorageManager::DataStorageManager() {
        db_file = nullptr;
        open_file();
        pages_num = read_pages_num();
        io_count = 0;
    }

    DataStorageManager::~DataStorageManager() {
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
        int page_id = pages_num + 1;
        unsigned int page_pointer = (page_id - 1) * sizeof(Frame) + FIRST_PAGE_SIZE;
        unsigned int page_pointer_offset = page_id * sizeof(unsigned int);
        seek(page_pointer_offset);
        fwrite(&page_pointer, sizeof(unsigned int), 1, db_file);
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
        pages_num++;
        seek(0);
        fwrite(&pages_num, sizeof(int), 1, db_file);
    }

    int DataStorageManager::get_pages_num() {
        return pages_num;
    }

    int DataStorageManager::read_pages_num() {
        int num;
        seek(0);
        fread(&num, sizeof(int), 1, db_file);
        return num;
    }

    unsigned int DataStorageManager::get_page_pointer(int page_id) {
        unsigned int page_pointer_offset = page_id * sizeof(unsigned int);
        unsigned int page_pointer;
        seek(page_pointer_offset);
        int a = fread(&page_pointer, sizeof(unsigned int), 1, db_file);
        return page_pointer;
    }

    void DataStorageManager::set_use(int index, int use_bit) {

    }

    int DataStorageManager::get_use(int index) {
        return 0;
    }

    bool DataStorageManager::is_page_exist(int page_id) {
        return page_id > 0 && page_id <= pages_num;
    }

    void DataStorageManager::inc_io_count() {
        io_count++;
    }

    int DataStorageManager::get_io_count() {
        return io_count;
    }
}
