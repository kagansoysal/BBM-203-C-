#ifndef ENCODE_MESSAGE_H
#define ENCODE_MESSAGE_H

#include <string>
#include <vector>
#include "ImageMatrix.h"

class EncodeMessage {
public:
    EncodeMessage();
    ~EncodeMessage();

    ImageMatrix encodeMessageToImage(const ImageMatrix &img, const std::string &message, const std::vector<std::pair<int, int>>& positions);


private:
    // Any private helper functions or variables if necessary
    static bool isPrime(int number);

    static int findFibonacci(int index);

    static std::string toBinary(int asciiNum);

    static std::string characterTransformation(std::string message);

    static std::string rightCircularShifting(std::string transformedMessage);

    static std::string createBinaryString(const std::string& shiftedMessage);

    static ImageMatrix
    embedding(const ImageMatrix &img, std::string binaryString, const std::vector<std::pair<int, int>> &positions);
};

#endif // ENCODE_MESSAGE_H
