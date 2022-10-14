//
// Created by tg on 2022/9/20.
//

#include "Utils.h"
#include <cstdio>
#include <iostream>
#include <array>
#include <memory>

std::string Utils::exec(const std::string& cmd) {
    std::array<char, 128> buffer{};
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {

    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}
