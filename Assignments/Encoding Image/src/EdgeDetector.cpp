// EdgeDetector.cpp

#include "EdgeDetector.h"
#include <cmath>

#include "EdgeDetector.h"
#include <cmath>
#include <iostream>

// Default constructor
EdgeDetector::EdgeDetector() {
    const double* kernelDataX[3] = {
            new double[3]{-1.0, 0.0, 1.0},
            new double[3]{-2.0, 0.0, 2.0},
            new double[3]{-1.0, 0.0, 1.0}
    };

    const double* kernelDataY[3] = {
            new double[3]{-1.0, -2.0, -1.0},
            new double[3]{0.0, 0.0, 0.0},
            new double[3]{1.0, 2.0, 1.0}
    };

    ImageMatrix kernelX(kernelDataX, 3, 3);
    ImageMatrix kernelY(kernelDataY, 3, 3);

    this->gX = new double*[3];
    for (int k = 0; k < 3; k++) {
        this->gX[k] = new double[3];
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            this->gX[i][j] = kernelX.get_data(i,j);
        }
    }

    this->gY = new double*[3];
    for (int k = 0; k < 3; k++) {
        this->gY[k] = new double[3];
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            this->gY[i][j] = kernelY.get_data(i,j);
        }
    }

}

// Destructor
EdgeDetector::~EdgeDetector() {

}



// Detect Edges using the given algorithm
std::vector<std::pair<int, int>> EdgeDetector::detectEdges(const ImageMatrix& input_image) {
    Convolution convolutionX(gX, 3, 3, 1, true);
    ImageMatrix imageX = convolutionX.convolve(input_image);

    Convolution convolutionY(gY, 3, 3, 1, true);
    ImageMatrix imageY = convolutionY.convolve(input_image);

    ImageMatrix gradientMatrix = ImageMatrix(imageX.get_height(), imageX.get_width());

    for(int i = 0; i < imageX.get_height(); i++) {
        for (int j = 0; j < imageX.get_width(); j++) {
            gradientMatrix.get_data()[i][j] = std::sqrt(std::pow(imageX.get_data(i,j),2) + std::pow(imageY.get_data(i,j), 2));
        }
    }

    double average = 0;

    for(int i = 0; i < gradientMatrix.get_height(); i++) {
        for(int j = 0; j < gradientMatrix.get_width(); j++) {
            average += gradientMatrix.get_data(i,j);
        }
    }

    average /= gradientMatrix.get_height() * gradientMatrix.get_width();

    std::vector<std::pair<int, int>> edgePixels;

    for(int i = 0; i < gradientMatrix.get_height(); i++) {
        for(int j = 0; j < gradientMatrix.get_width(); j++) {
            if(gradientMatrix.get_data(i,j) > average) {
                edgePixels.push_back({i,j});
            }
        }
    }

    return edgePixels;
}

