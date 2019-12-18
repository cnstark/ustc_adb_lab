//
// Created by Yuhao Wang on 2019/11/27.
//

#ifndef ADB_PROJECT_COMMON_HPP
#define ADB_PROJECT_COMMON_HPP

#include <cstdlib>
#include <memory>

#define FRAME_SIZE 4096

namespace adb {
    struct BCB {
        BCB(int page_id, int frame_id) : page_id(page_id), frame_id(frame_id), dirty(false){};
        int page_id;
        int frame_id;
        bool dirty;
    };

    struct Frame {
        typedef std::shared_ptr<Frame> sptr;
        char field[FRAME_SIZE] = {};
    };

    Frame::sptr generate_random_frame();
}

#endif //ADB_PROJECT_COMMON_HPP
