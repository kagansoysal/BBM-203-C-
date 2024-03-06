#include <iostream>
#include "ImageSharpening.h"
#include "ImageMatrix.h"

// Default constructor
ImageSharpening::ImageSharpening() {

}

ImageSharpening::~ImageSharpening(){

}

ImageMatrix ImageSharpening::sharpen(const ImageMatrix& input_image, double k) {
    const double* kernelData[3] = {
            new double[3]{1.0/9.0, 1.0/9.0, 1.0/9.0},
            new double[3]{1.0/9.0, 1.0/9.0, 1.0/9.0},
            new double[3]{1.0/9.0, 1.0/9.0, 1.0/9.0}
    };

    ImageMatrix kernel(kernelData, 3, 3);

    Convolution convolution(kernel.get_data(), 3, 3, 1, true);
    ImageMatrix convolvedMatrix = convolution.convolve(input_image);


    ImageMatrix firstMatrix = input_image - convolvedMatrix;
    ImageMatrix scalarMultiplied = firstMatrix * k;
    ImageMatrix sharpenedImage = input_image + scalarMultiplied;


    for (int i = 0; i < sharpenedImage.get_height(); i++) {
        for (int j = 0; j < sharpenedImage.get_width(); j++) {
            if (sharpenedImage.get_data(i,j) < 0) {
                sharpenedImage.get_data()[i][j] = 0;
            }else if (sharpenedImage.get_data(i, j) > 255) {
                sharpenedImage.get_data()[i][j] = 255;
            }
        }
    }
    return sharpenedImage;
}
