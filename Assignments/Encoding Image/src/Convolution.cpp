#include <iostream>

#include "Convolution.h"

// Default constructor 
Convolution::Convolution() {
}

// Parametrized constructor for custom kernel and other parameters
Convolution::Convolution(double** customKernel, int kh, int kw, int stride_val, bool pad){
    this->kh = kh;
    this->kw = kw;
    this->stride = stride_val;
    this->pad = pad;

    this->customKernel = new double*[this->kh];
    for (int k = 0; k < this->kh; k++) {
        this->customKernel[k] = new double[this->kw];
    }

    for (int i = 0; i < kh; i++) {
        for (int j = 0; j < kw; j++) {
            this->customKernel[i][j] = customKernel[i][j];
        }
    }


}

// Destructor
Convolution::~Convolution() {
    if (customKernel != nullptr) {
        for (int i = 0; i < kh; i++) {
            delete[] customKernel[i];
        }
        delete[] customKernel;
    }
}

// Copy constructor
Convolution::Convolution(const Convolution &other){
    this->kh = other.kh;
    this->kw = other.kw;
    this->stride = other.stride;
    this->pad = other.pad;

    customKernel = new double*[kh];
    for (int k = 0; k < kh; k++) {
        customKernel[k] = new double[kw];
    }

    for (int i = 0; i < this->kh; i++) {
        for (int j = 0; j < this->kw; j++) {
            this->customKernel[i][j] = other.customKernel[i][j] ;
        }
    }
}

// Copy assignment operator
Convolution& Convolution::operator=(const Convolution &other) {
    if (this == &other) {
        return *this;
    }

    for (int i = 0; i < kh; ++i) {
        delete[] customKernel[i];
    }
    delete[] customKernel;

    this->kh = other.kh;
    this->kw = other.kw;
    customKernel = new double*[kh];
    for (int i = 0; i < kh; ++i) {
        customKernel[i] = new double[kw];
        for (int j = 0; j < kw; j++) {
            customKernel[i][j] = other.customKernel[i][j];
        }
    }

    return *this;
}


// Convolve Function: Responsible for convolving the input image with a kernel and return the convolved image.
ImageMatrix Convolution::convolve(const ImageMatrix& input_image) const {
    int height = ((input_image.get_height() - kh + 2 * pad) / stride) + 1;
    int width = ((input_image.get_width() - kw + 2 * pad) / stride) + 1;

    ImageMatrix convolvedMatrix(height, width);

    //0 Padding
    if (pad) {
        ImageMatrix paddedMatrix(input_image.get_height() + 2, input_image.get_width() + 2);

        for (int i = 0; i < input_image.get_height(); i++) {
            for (int j = 0; j < input_image.get_width(); j++) {
                paddedMatrix.get_data()[i + 1][j + 1] = input_image.get_data(i, j);
            }
        }

        int row = 0;
        int column = 0;

        int convolvedRow = 0;
        int convolvedColumn = 0;

        while (row < paddedMatrix.get_height() - kh + 1) {
            while (column < paddedMatrix.get_width() - kw + 1) {
                double value = 0;

                for (int kRow = 0; kRow < kh; kRow++) {
                    for (int kColumn = 0; kColumn < kw; kColumn++) {
                        value += paddedMatrix.get_data()[kRow + row][kColumn + column] * customKernel[kRow][kColumn];
                    }
                }
                convolvedMatrix.get_data()[convolvedRow][convolvedColumn] = value;
                convolvedColumn += 1;
                column += stride;
            }

            convolvedRow += 1;
            convolvedColumn = 0;
            column = 0;
            row += stride;
        }

    } else{
        int row = 0;
        int column = 0;

        int convolvedRow = 0;
        int convolvedColumn = 0;

        while (row < input_image.get_height() - kh + 1) {
            while (column < input_image.get_width() - kw + 1) {
                double value = 0;

                for (int kRow = 0; kRow < kh; kRow++) {
                    for (int kColumn = 0; kColumn < kw; kColumn++) {
                        value += input_image.get_data()[kRow + row][kColumn + column] * customKernel[kRow][kColumn];
                    }
                }
                convolvedMatrix.get_data()[convolvedRow][convolvedColumn] = value;
                convolvedColumn += 1;
                column += stride;
            }

            convolvedRow += 1;
            convolvedColumn = 0;
            column = 0;
            row += stride;
        }
    }
    return convolvedMatrix;
}
