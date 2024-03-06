#include <iostream>
#include "EncodeMessage.h"
#include "bitset"


// Default Constructor
EncodeMessage::EncodeMessage() = default;

// Destructor
EncodeMessage::~EncodeMessage() = default;

// Function to encode a message into an image matrix
ImageMatrix EncodeMessage::encodeMessageToImage(const ImageMatrix &img, const std::string &message, const std::vector<std::pair<int, int>>& positions) {

    std::string transformedMessage = characterTransformation(message);
    std::string shiftedMessage = rightCircularShifting(transformedMessage);
    std::string binaryString = createBinaryString(shiftedMessage);

    ImageMatrix embeddedImage = embedding(img, binaryString, positions);
    return embeddedImage;
}


bool EncodeMessage::isPrime(int number) {
    if (number <= 1) {
        return false;
    }

    for (int i = 2; i * i <= number; i++) {
        if (number % i == 0) {
            return false;
        }
    }
    return true;
}


int EncodeMessage::findFibonacci(int index) {
    int first = 0;
    int second = 1;
    int result = 0;

    if (index == 0) {
        return first;
    } else if (index == 1) {
        return second;
    }

    for (int i = 2; i <= index; i++) {
        result = first + second;
        first = second;
        second = result;
    }
    return result;
}

std::string EncodeMessage::toBinary(int asciiNum){
    std::string binaryNum;

    while (asciiNum > 0) {
        int bit = asciiNum % 2;
        binaryNum = std::to_string(bit) + binaryNum;
        asciiNum /= 2;
    }
    return binaryNum;
}

std::string EncodeMessage::characterTransformation(std::string message) {
    std::string transformedMessage;

    //fibonacci
    for (int i = 0; i < message.length(); i++) {
        if(isPrime(i)) {
            int newAsciiNum = static_cast<int>(message[i]) + findFibonacci(i);

            if (newAsciiNum <= 32) {
                newAsciiNum += 33;
            } else if (newAsciiNum > 126) {
                newAsciiNum = 126;
            }

            transformedMessage += static_cast<char>(newAsciiNum);
        } else {
            transformedMessage += message[i];
        }
    }
    return transformedMessage;
}

std::string EncodeMessage::rightCircularShifting(std::string transformedMessage) {
    std::string shiftedMessage;

    int shiftAmount = transformedMessage.length() / 2;
    int index = transformedMessage.length() - shiftAmount;

    for (int i = 0; i < transformedMessage.length(); i++) {
        shiftedMessage.append(1,transformedMessage[index]);

        if(index == transformedMessage.length() - 1) {
            index = 0;
        } else {
            index++;
        }
    }
    return shiftedMessage;
}

std::string EncodeMessage::createBinaryString(const std::string& shiftedMessage) {
    std::string binaryString;

    for (char i : shiftedMessage) {
        int asciiNum = static_cast<int>(i);

        std::string binaryNum = toBinary(asciiNum);

        while (binaryNum.length() < 7) {
            binaryNum = "0" + binaryNum;
        }

        binaryString += binaryNum;
    }
    return binaryString;
}

ImageMatrix EncodeMessage::embedding(const ImageMatrix &img, std::string binaryString, const std::vector<std::pair<int, int>>& positions) {
    int embeddingNum = positions.size() < binaryString.length() ? positions.size() : binaryString.length();
    ImageMatrix embeddedImage = img;

    for (int i = 0; i < embeddingNum; i++) {
        int x = positions[i].first;
        int y = positions[i].second;

        if (x>=0&& x<embeddedImage.get_height() && y>=0 && y<embeddedImage.get_width()) {
            if(i < binaryString.length()) {
                if (static_cast<int>(embeddedImage.get_data(x, y)) % 2 == 0 && binaryString[i] == '1') {
                    embeddedImage.get_data()[x][y] = embeddedImage.get_data(x, y) + 1;
                } else if (static_cast<int>(embeddedImage.get_data(x, y)) % 2 != 0 && binaryString[i] == '0') {
                    embeddedImage.get_data()[x][y] = embeddedImage.get_data(x, y) - 1;
                }
            }
        }

    }
    return embeddedImage;
}
