//
// Created by tvysh on 5/29/2025.
//

#ifndef OS_MESSAGEPROTOCOL_H
#define OS_MESSAGEPROTOCOL_H

#include "Employee.h"

constexpr const char* TYPE_READ = "R";
constexpr const char* TYPE_MODIFY = "M";
constexpr const char* TYPE_EXIT = "E";

constexpr const char* ERROR_OK = "0";
constexpr const char* ERROR_UNKNOWN = "1";
constexpr const char* ERROR_ID_NOT_FOUND = "2";
constexpr const char* ERROR_UNKNOWN_COMMAND = "3";
constexpr const char* ERROR_FAILED_MODIFY = "4";
constexpr const char* ERROR_FAILED_READ = "5";

const std::string CLIENT_SERVER_PIPE_BASE= R"(\\.\pipe\ClientServer_)";

std::string getPipeName(unsigned int id) {
    return CLIENT_SERVER_PIPE_BASE + std::to_string(id);
}

#pragma pack(push, 1)

struct ClientModifyMessage {
    double newHours;
    std::vector<char> newName;
};

#pragma pack(pop)

#endif //OS_MESSAGEPROTOCOL_H
