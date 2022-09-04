//
// Created by tg on 2022/9/3.
//

#ifndef ANTI_ANDROID_ANTI_FRIDA_H
#define ANTI_ANDROID_ANTI_FRIDA_H
#include <string>

class AntiFrida {
public:
    void check();

private:
    uint64_t frida_find_library_base(std::string library_name, std::string *library_path);
    uint64_t frida_find_library_space_base(uint64_t base, uint32_t page_size);
};


#endif //ANTI_ANDROID_ANTI_FRIDA_H
