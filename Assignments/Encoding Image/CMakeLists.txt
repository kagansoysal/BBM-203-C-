cmake_minimum_required(VERSION 3.26)
project(Assignment1)

set(CMAKE_CXX_STANDARD 11)

add_executable(Assignment1 main.cpp
        ImageProcessor.cpp
        ImageLoader.cpp
        ImageMatrix.cpp
        Convolution.cpp
        ImageSharpening.cpp
        EdgeDetector.cpp
        DecodeMessage.cpp
        EncodeMessage.cpp
        EncodeMessage.cpp)
