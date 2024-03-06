#include <iostream>
#include "ImageProcessor.h"

ImageProcessor::ImageProcessor() {
}

ImageProcessor::~ImageProcessor() {

}


std::string ImageProcessor::decodeHiddenMessage(const ImageMatrix &img) {
    ImageSharpening imageSharpening;
    ImageMatrix sharpenedImage = imageSharpening.sharpen(img, 2);

    EdgeDetector edgeDetector;
    edgePixels = edgeDetector.detectEdges(sharpenedImage);

    DecodeMessage decodeMessage;
    std::string hiddenMessage = decodeMessage.decodeFromImage(sharpenedImage, edgePixels);

    return hiddenMessage;
}

ImageMatrix ImageProcessor::encodeHiddenMessage(const ImageMatrix &img, const std::string &message) {
    ImageSharpening imageSharpening;
    ImageMatrix sharpenedImage = imageSharpening.sharpen(img, 2);

    EdgeDetector edgeDetector;
    edgePixels = edgeDetector.detectEdges(sharpenedImage);

    EncodeMessage encodeMessage;
    ImageMatrix embeddedImage = encodeMessage.encodeMessageToImage(img, message, edgePixels);

    return embeddedImage;
}
