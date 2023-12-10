//
// Created by Kris on 24/04/2022.
//
#include "QPrinter.hpp"

#ifndef ESPSTREAMER_PRINTFQPRINTER_HPP
#define ESPSTREAMER_PRINTFQPRINTER_HPP

class PrintfQPrinter : public QPrinter {
public:
    void print(std::string string) override;
};

#endif //ESPSTREAMER_PRINTFQPRINTER_HPP
