#include "RingBuffer.hpp"


int main(){
    std::string filename;
    int recordCount;
    std::cout << "Enter name of binary file: ";
    std::cin >> filename;
    std::cout << "Enter max count of records (capacity): ";
    std::cin >> recordCount;
    RingBuffer ringBuffer = RingBuffer(filename.c_str(), recordCount, true);
    if (!ringBuffer.isOpened()) {
        std::cerr << "Failed to create buffer: " << std::endl;
        return 1;
    }
    int senderCount;
    std::cout << "Enter number of Senders: ";
    std::cin >> senderCount;

    std::vector<MyEvent> readyEvents(senderCount);
    std::vector<MyProcess> senderProcesses(senderCount);

    for (int i = 0; i < senderCount; ++i) {
        bool okEvent = readyEvents[i].createInherit(TRUE, FALSE);

        if (!okEvent) {
            std::cerr << "Failed to create event: " << std::endl;
            return 1;
        }


        // Запуск Sender
        std::ostringstream cmd;
        cmd << "Sender.exe " << filename << " " << i << " " << readyEvents[i].getHandleUintptr_t();

        bool okSender = senderProcesses[i].createA(cmd, TRUE);
        if (!okSender) {
            std::cerr << "Failed to launch Sender process." << std::endl;
            return 1;
        }
    }

    // Wait signal "Ready" from all Senders
    std::cout << "Waiting for all Sender processes to signal readiness..." << std::endl;
    MyEvent::waitMultipleObjects(senderCount, readyEvents, TRUE);
    std::cout << "All Sender processes are ready." << std::endl;


    // Main cycle
    std::string command;
    while (true) {
        std::cout << "\n[Receiver] Enter command (read/exit)(r/e): ";
        std::cin >> command;

        if (command == "read" || command == "r") {
            Message msg;
            if (ringBuffer.readMessage(msg)) {
                std::cout << "[Receiver] Got message: " << msg.text << std::endl;
            } else {
                std::cout << "[Receiver] Buffer is empty. Waiting for message..." << std::endl;
                Sleep(1000); // Простое ожидание
            }
        } else if (command == "exit" || command == "e") {
            std::cout << "[Receiver] Exiting." << std::endl;
            break;
        } else {
            std::cout << "[Receiver] Unknown command." << std::endl;
        }
    }

    for (auto& sender: senderProcesses) {
        sender.terminate();
    }

    return 0;
}