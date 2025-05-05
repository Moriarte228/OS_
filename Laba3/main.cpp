#include "Data.h"
#include "ThreadController.h"


int main() {
    size_t size;
    std::cout << "Enter size of array: ";
    std::cin >> size;
    Data data(size);

    unsigned int markers;
    while (true) {
        std::cout << "Enter number of markers: ";
        std::cin >> markers;
        if (markers > ThreadController::gMAX_MARKERS()) {
            std::cout << "Too many markers, try again\n";
        }
        else
            break;
    }

    ThreadController threadController(markers, &data);

    threadController.StartEvent();
    threadController.CycleEvent();

    std::cout << "End of the program";
}
