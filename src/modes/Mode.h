#ifndef ONITAMA_MODE_H
#define ONITAMA_MODE_H

#include <iostream>
#include <array>
#include <vector>

struct Mode {
    std::vector<std::string> cards = {"", "", "", "", ""};
    int depth = 1;
    bool printReceivedPackets{false};
    
    virtual void run() = 0;
};


#endif //ONITAMA_MODE_H
