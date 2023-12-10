//
// Created by Kris on 24/04/2022.
//
#include <string>

#ifndef ESPSTREAMER_QPRINTER_HPP
#define ESPSTREAMER_QPRINTER_HPP

class QPrinter {
public:
    virtual void print(std::string string) = 0;
};

#endif //ESPSTREAMER_QPRINTER_HPP
