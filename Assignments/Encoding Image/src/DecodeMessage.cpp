// DecodeMessage.cpp

#include "DecodeMessage.h"
#include <iostream>

// Default constructor
DecodeMessage::DecodeMessage() {
    // Nothing specific to initialize here
}

// Destructor
DecodeMessage::~DecodeMessage() {
    // Nothing specific to clean up
}


std::string DecodeMessage::decodeFromImage(const ImageMatrix& image, const std::vector<std::pair<int, int>>& edgePixels) {
    std::string binaryString;

    for(int i = 0; i < edgePixels.size(); i++) {
        int x = edgePixels[i].first;
        int y = edgePixels[i].second;

        binaryString.append(std::to_string(static_cast<int>(image.get_data(x,y))  & 1));
    }

    while (binaryString.length() % 7 != 0) {
        binaryString.insert(0, "0");
    }

    std::vector<std::string> binaryNumbers;

    for(int i = 0; i < binaryString.size(); i += 7) {
        binaryNumbers.push_back(binaryString.substr(i, 7));
    }

    std::string hiddenMessage;

    for(int i = 0; i < binaryNumbers.size(); i++) {
        int asciiNum = std::stoi(binaryNumbers[i], nullptr, 2);

        if (asciiNum <= 32) {
            asciiNum += 33;
        } else if (asciiNum == 127) {
            asciiNum = 126;
        }

        hiddenMessage.append(1, static_cast<char> (asciiNum));
    }

    return hiddenMessage;
}

