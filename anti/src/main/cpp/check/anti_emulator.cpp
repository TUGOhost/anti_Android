//
// Created by tg on 2022/10/15.
//

#include "anti_emulator.h"
#include "../mini_io/_mini_io.h"
#include <fcntl.h>
#include <vector>
#include <errno.h>
#include "sys/system_properties.h" // 可以自己实现一遍
#include "../utils/Utils.h"


std::string AntiEmulator::check() {
    std::string result = "security";
    std::vector<std::string> list_of_files =
            {
                    "/boot/bstmods/vboxguest.ko",
                    "/boot/bstmods/vboxsf.ko",
                    "/dev/qemu_pipe",
                    "/dev/socket/qemud",
                    "/dev/socket/windroyed-audio",
                    "/dev/socket/windroyed-camera",
                    "/dev/socket/windroyed-gps",
                    "/dev/socket/windroyed-sensors",
                    "/dev/vboxguest"
            };
    for (auto file: list_of_files) {
        if (check_of_file(file) || dir(file)) {
            result = "checked";
        }
    }

    std::vector<std::string> list_of_props = {
            "ro.redfinger.server.enable",
            "androVM.vbox_dpi",
            "androVM.vbox_graph_mode"
    };

    for (auto prop: list_of_props) {
        if (check_of_prop(prop).find("redfinger") != std::string::npos) {
            result = "checked";
        }
    }

    LOGE("result: %s", result.c_str());
    return result;
}

bool AntiEmulator::check_of_file(std::string file_name) {
    int fd = _open(file_name.c_str(), O_RDONLY);
    if (fd == errno) {
        return false;
    }
    return true;
}

bool AntiEmulator::dir(std::string dir_name) {
    int fd = _open(dir_name.c_str(), O_DIRECTORY);
    if (fd == errno) {
        return false;
    }
    return true;
}

std::string AntiEmulator::check_of_prop(std::string cmd) {
    char value[256];
    std::string result;
    __system_property_get(cmd.c_str(), value);

    result = value;

    memset(value, 0, sizeof value);

    return result;
}
