//
// Created by Kris on 24/04/2022.
//

#include "include/QPrint.hpp"
#include "ICESleep.hpp"

std::multimap<printerType, QPrinter*> QPrint::registeredPrinters;
QPrintTask* QPrint::printTask = nullptr;

void QPrint::registerPrinter(QPrinter *printer, printerType type) {
    QPrint::registeredPrinters.insert(registeredPrinters.begin(), std::make_pair(type, printer));
}

void QPrint::print(const std::string& string, printerType type, bool blocking) {
    //Since we do not know how fast printers will return from print, it is likely best to run prints in a task
    //So we create a print task to take care of this while we return immediately and keep running.
    stringOutput output = {
            .type = type,
            .message = string
    };

    if (printTask == nullptr) {
        printTask = new QPrintTask(&registeredPrinters, output);
        printTask->setup(NULL, "print task", 4096, 0, 0);
        printTask->start();
    } else {
        printTask->print(output, blocking);
    }
}

void QPrint::println(const std::string& string, printerType type, bool blocking) {
    std::string outString = string + "\n";
    QPrint::print(outString, type, blocking);
}


QPrintTask::QPrintTask(std::multimap<printerType, QPrinter*>* currentPrinters, stringOutput output) {
    printers = currentPrinters;
    printStrings.push_back(output);
}

void QPrintTask::runTask(void* unused) {
    printerType type;
    std::string message;
    while(1) {
        if (!printStrings.empty()) {
            type = printStrings.front().type;
            message = printStrings.front().message;

            if (!mutex.lock(50)) {
                continue; //Just rerun loop. If we dont get mutex, then I guess we will just retry...
            }
            printStrings.pop_front();
            mutex.unlock();

            for (auto printer : *printers) {
                if (printer.first == type) {
                    printer.second->print(message);
                }
            }
        }
        ICESleep(5); //Sleep a little, print is not supposed to be priority ever..
    }
}

void QPrintTask::print(stringOutput output, bool block) {
    int waitTime = 0;
    if (block) {
        waitTime = 5000; //Lots of ticks to wait
    }
    
    if (mutex.lock(waitTime) != 0) { //If we cant get mutex immediately, we can yield once and if still nothing, we just drop..
        taskYIELD();
        if (mutex.lock(waitTime) != 0) {
            return;
        }
    }

    if (printStrings.size() < 15) { //Limit to 15 strings to make sure we have a somewhat limited ram usage..
        printStrings.push_back(output);
    } else if (block) {
        while (printStrings.size() < 15) {
            ICESleep(2);
        }
        printStrings.push_back(output);
    }

    mutex.unlock();
}
