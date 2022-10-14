//
// Created by tg on 2022/10/15.
//

#ifndef ANTI_ANDROID_ANTI_EMULATOR_H
#define ANTI_ANDROID_ANTI_EMULATOR_H

#include <string>

// https://bbs.pediy.com/thread-255672.htm
class AntiEmulator {
public:
    std::string check();

private:
    bool check_of_file(std::string file_name);

    bool dir(std::string dir_name);

    std::string check_of_prop(std::string cmd);
};


#endif //ANTI_ANDROID_ANTI_EMULATOR_H
