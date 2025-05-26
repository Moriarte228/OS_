#include "RingBuffer.hpp"

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: Sender.exe <filename> <sender_id> <HANDLE>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    int senderId = std::atoi(argv[2]);

    RingBuffer buffer(filename.c_str(), 0, false);

    uintptr_t rawHandle = static_cast<uintptr_t>(std::strtoull(argv[3], nullptr, 10));
    MyEvent readyEvent;

    if (!readyEvent.createReinterpret(rawHandle)) {
        std::cerr << "[Sender #" << senderId << "] Failed to open ready event." << std::endl;
        return 1;
    }

    readyEvent.setEvent();
    std::cout << "[Sender #" << senderId << "] Ready." << std::endl;

    std::string command;
    while (true) {
        std::cout << "\n[Sender #" << senderId << "] Enter command (send/exit): ";
        std::getline(std::cin, command);

        if (command == "send") {
            Message msg;
            std::cout << "[Sender #" << senderId << "] Enter message (<="  <<
                      MAX_MESSAGE_LENGTH<<  " chars): ";
            std::string line;
            std::getline(std::cin, line);
            if (!msg.setText(line)) {
                std::cout << "Invalid string: more than " << MAX_MESSAGE_LENGTH <<
                          " length of message OR message is empty\n";
                continue;
            }

            if (buffer.writeMessage(msg)) {
                std::cout << "[Sender #" << senderId << "] Message sent." << std::endl;
            } else {
                std::cout << "[Sender #" << senderId << "] Buffer full. Waiting..." << std::endl;
                Sleep(1000);
            }
        } else if (command == "exit") {
            std::cout << "[Sender #" << senderId << "] Exiting." << std::endl;
            break;
        } else {
            std::cout << "[Sender #" << senderId << "] Unknown command." << std::endl;
        }
    }

    return 0;
}
