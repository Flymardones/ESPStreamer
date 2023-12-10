#ifndef ESPSTREAMER_ESPPRINT_H
#define ESPSTREAMER_ESPPRINT_H

#include <map>
#include <deque>
#include "QPrinter.hpp"
#include "ICETask.hpp"
#include "ICEMutex.hpp"
#include "freertos/ringbuf.h"

enum printerType : int {
    debug = 0,
    controlInput = 1,
    controlOutput = 2,
};

struct stringOutput {
    printerType type;
    std::string message;
};

class QPrintTask : public ICETask {
private:
    std::multimap<printerType, QPrinter*>* printers;
    std::deque<stringOutput> printStrings;
    ICEMutex mutex;

public:
    QPrintTask(std::multimap<printerType, QPrinter*>* currentPrinters, stringOutput output);
    void print(stringOutput output, bool block);
    void runTask(void* unused) override;
};

class QPrint {
private:
    static std::multimap<printerType, QPrinter*> registeredPrinters;
    static QPrintTask* printTask;

public:
    static void registerPrinter(QPrinter* printer, printerType type = debug);
    static void print(const std::string& string, printerType type = debug, bool blocking = false);
    static void println(const std::string& string, printerType type = debug, bool blocking = false);
};



#endif //ESPSTREAMER_ESPPRINT_H
