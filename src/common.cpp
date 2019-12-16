//
// Created by Yuhao Wang on 2019/12/16.
//

#include "common.hpp"

namespace adb {
    Frame::sptr generate_random_frame() {
        auto frame = std::make_shared<Frame>();
        for (unsigned int i = 0; i < FRAME_SIZE - 1; i++) {
            frame->field[i] = 'a' + arc4random_uniform(26);
        }
        frame->field[FRAME_SIZE - 1] = 0;
        return frame;
    }
}
