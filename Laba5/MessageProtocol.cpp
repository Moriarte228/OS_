#include "MessageProtocol.h"

const std::string CLIENT_SERVER_PIPE_BASE = R"(\\.\pipe\ClientServer_)";

std::string getPipeName(unsigned int id) {
    return CLIENT_SERVER_PIPE_BASE + std::to_string(id);
}
