/**
 * @file Functions.cpp
 * @brief Implementation of Functions.h functions.
*/

/*
Andikalovskiy Nikita Dmitrievich
24.B-82mm
st131335@student.spbu.ru
LabWork 1
*/

#include <algorithm>
#include <fstream>
#include <iostream>
#include <thread>

#include "Functions.h"

/**
 * @brief Template function that checks if value is in range.
 * @tparam T Value that is being checked. Should support standard operators.
 * @param value Value that we want to check.
 * @param low Lower boundary of checking.
 * @param high Higher boundary of checking.
 * @return Low if value is less than low, high if value is greater than high, value otherwise.
*/
template <typename T>
T clamp(T value, T low, T high) {
    if (value < low) {
        return low;
    } else if (value > high) {
        return high;
    } else {
        return value;
    }
}

bool readBMPinfo(const char* filename, BMPinfo& info) {
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "RBI: Error: Cannot open file " << filename << std::endl;
        return false;
    }

    infile.seekg(14, std::ios::beg);

    infile.read(reinterpret_cast<char*>(&info), sizeof(BMPinfo));
    if (!infile) {
        std::cerr << "RBI: Error: Failed to read BMP info header" << std::endl;
        return false;
    }

    return true;
}

uint8_t* load(const char* filename, size_t& imgsize, BMPinfo& info) {
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "LOAD: Err: Cannot open file!" << std::endl;
        return nullptr;
    }

    BMPfilehead filehead;

    infile.read(reinterpret_cast<char*>(&filehead), sizeof(BMPfilehead));

    filehead.offset = sizeof(BMPinfo) + sizeof(BMPfilehead);

    if (!readBMPinfo(filename, info)) {
        return nullptr;
    }

    if (filehead.filetype != 0x4D42) {
        std::cout << "Incorrect filetype!" << std::endl;
    }

    if (info.bitperpixel != 24) {
        std::cerr << "LOAD: Err: Only 24 bit BMP files supported!" << std::endl;
        return nullptr;
    }

    if (info.compression != 0) {
        std::cerr << "LOAD: Err: Only uncompressed BMP files supported!"
                  << std::endl;
        return nullptr;
    }

    size_t pixelOffset = 14 + info.size;
    infile.seekg(pixelOffset, std::ios::beg);

    size_t stringsize = (info.width * (info.bitperpixel / 8) + 3) & ~3;
    imgsize = stringsize * std::abs(info.height);

    uint8_t* imgdata = new uint8_t[imgsize];

    infile.read(reinterpret_cast<char*>(imgdata), imgsize);

    if (!infile) {
        std::cerr << "LOAD: Err: Failed to read BMP pixel data!" << std::endl;
        delete[] imgdata;
        return nullptr;
    }

    return imgdata;
}

void save(const char* filename, const uint8_t* imgdata, size_t imgsize, BMPinfo& info) {
    BMPfilehead filehead;
    filehead.filetype = 0x4D42;
    filehead.reserved1 = 0;
    filehead.reserved2 = 0;

    filehead.offset = sizeof(BMPfilehead) + sizeof(BMPinfo);
    filehead.filesize = filehead.offset + imgsize;

    std::cout << "File type: " << std::hex << filehead.filetype << std::endl;
    std::cout << "File size: " << filehead.filesize << std::endl;
    std::cout << "Offset: " << filehead.offset << std::endl;

    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "SAVE: err: Couldn't open file!!" << std::endl;
        return;
    }

    outfile.write(reinterpret_cast<const char*>(&filehead), sizeof(filehead));

    info.imagesize = imgsize;

    outfile.write(reinterpret_cast<const char*>(&info), sizeof(info));

    outfile.write(reinterpret_cast<const char*>(imgdata), imgsize);

    if (!outfile) {
        std::cerr << "SAVE: err: Failed to write to the file!" << std::endl;
    }

    outfile.close();
}

void rotateforward(uint8_t*& imgdata, BMPinfo& info, size_t& imgsize) {
    size_t stringSize = (info.width * (info.bitperpixel / 8) + 3) & ~3;
    size_t rotatedStringSize = (info.height * (info.bitperpixel / 8) + 3) & ~3;
    size_t rotatedSize = rotatedStringSize * info.width;

    uint8_t* rotated = new uint8_t[rotatedSize]();
    if (!rotated) {
        std::cerr << "RFW: Error: Memory failed!" << std::endl;
        return;
    }

    for (int y = 0; y < info.height; ++y) {
        for (int x = 0; x < info.width; ++x) {
            size_t oldIndex = y * stringSize + x * (info.bitperpixel / 8);
            size_t newIndex =
                (info.width - 1 - x) * rotatedStringSize + y * (info.bitperpixel / 8);
            std::memcpy(&rotated[newIndex], &imgdata[oldIndex], info.bitperpixel / 8);
        }
    }

    delete[] imgdata;
    imgdata = rotated;

    std::swap(info.width, info.height);
    imgsize = rotatedSize;

}

void rotatebackwards(uint8_t*& imgdata, BMPinfo& info, size_t& imgsize) {
    size_t stringSize = (info.width * (info.bitperpixel / 8) + 3) & ~3;
    size_t rotatedStringSize = (info.height * (info.bitperpixel / 8) + 3) & ~3;
    size_t rotatedSize = rotatedStringSize * info.width;

    uint8_t* rotated = new uint8_t[rotatedSize]();
    if (!rotated) {
        std::cerr << "RBW: Error: Memory allocation failed!" << std::endl;
        return;
    }

    for (int y = 0; y < info.height; ++y) {
        for (int x = 0; x < info.width; ++x) {
            size_t oldIndex = y * stringSize + x * (info.bitperpixel / 8);
            size_t newIndex = x * rotatedStringSize +
                              (info.height - 1 - y) * (info.bitperpixel / 8);
            std::memcpy(&rotated[newIndex], &imgdata[oldIndex], info.bitperpixel / 8);
        }
    }

    delete[] imgdata;
    imgdata = rotated;

    std::swap(info.width, info.height);
    imgsize = rotatedSize;

}

void blurSegment(uint8_t* imgdata, uint8_t* temporaryArray, const BMPinfo& info, int start_y, int end_y, size_t stringSize, const float kernel[5][5], int halfKernel) {
    for (int y = start_y; y < end_y; ++y) {
        for (int x = 0; x < info.width; ++x) {
            float r = 0.0f;
            float g = 0.0f;
            float b = 0.0f;

            for (int _y = -halfKernel; _y <= halfKernel; ++_y) {
                for (int _x = -halfKernel; _x <= halfKernel; ++_x) {
                    // using clamp to be sure that we are still in borders of the image
                    int X = clamp(x + _x, 0, info.width - 1);
                    int Y = clamp(y + _y, 0, info.width - 1);

                    // index in imgdata array
                    size_t pixelIndex = Y * stringSize + X * (info.bitperpixel / 8);

                    // same operation as in full 'blur' earlier
                    float weight = kernel[_y + halfKernel][_x + halfKernel];

                    b += imgdata[pixelIndex + 0] * weight;
                    g += imgdata[pixelIndex + 1] * weight;
                    r += imgdata[pixelIndex + 2] * weight;
                }
            }

            size_t newPixelIndex = y * stringSize + x * (info.bitperpixel / 8);

            temporaryArray[newPixelIndex + 0] = static_cast<uint8_t>(clamp(b, 0.0f, 255.0f));
            temporaryArray[newPixelIndex + 1] = static_cast<uint8_t>(clamp(g, 0.0f, 255.0f));
            temporaryArray[newPixelIndex + 2] = static_cast<uint8_t>(clamp(r, 0.0f, 255.0f));
        }
    }
}

void blur(uint8_t*& imgdata, BMPinfo& info) {
    const int kernelsize = 5;
    const int halfKernel = kernelsize / 2;
    const float kernel[5][5] = {
        {1 / 273.0f, 4 / 273.0f, 7 / 273.0f, 4 / 273.0f, 1 / 273.0f},
        {4 / 273.0f, 16 / 273.0f, 26 / 273.0f, 16 / 273.0f, 4 / 273.0f},
        {7 / 273.0f, 26 / 273.0f, 41 / 273.0f, 26 / 273.0f, 7 / 273.0f},
        {4 / 273.0f, 16 / 273.0f, 26 / 273.0f, 16 / 273.0f, 4 / 273.0f},
        {1 / 273.0f, 4 / 273.0f, 7 / 273.0f, 4 / 273.0f, 1 / 273.0f}
    };

    size_t stringSize = (info.width * (info.bitperpixel / 8) + 3) & ~3;
    uint8_t* temporaryArray = new uint8_t[stringSize * info.height];

    // Define how much threads we can use
    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) {
        num_threads = 1;
        std::cout << "DEBUG: Num threads is 1!" << std::endl;
    }

    std::vector<std::thread> threads;
    int rows_per_thread = info.height / num_threads;

    for (unsigned int i = 0; i < num_threads; ++i) {
        int start_y = i * rows_per_thread;
        int end_y;
        if (i == num_threads - 1) {
            end_y = info.height;
        } else {
            end_y = (i + 1) * rows_per_thread;
        }

        // Create new thread and add in vector
        threads.emplace_back(blurSegment, imgdata, temporaryArray, std::ref(info), start_y, end_y, stringSize, std::ref(kernel), std::ref(halfKernel));
    }

    for (std::thread& current_thread : threads) {
        current_thread.join();
    }

    std::memcpy(imgdata, temporaryArray, stringSize * info.height);
    delete[] temporaryArray;

}

void oldBlur(uint8_t*& imgdata, BMPinfo& info) {
    const int kernelsize = 5;
    const int halfKernel = kernelsize / 2;
    const float kernel[5][5] = {
        {1 / 273.0f, 4 / 273.0f, 7 / 273.0f, 4 / 273.0f, 1 / 273.0f},
        {4 / 273.0f, 16 / 273.0f, 26 / 273.0f, 16 / 273.0f, 4 / 273.0f},
        {7 / 273.0f, 26 / 273.0f, 41 / 273.0f, 26 / 273.0f, 7 / 273.0f},
        {4 / 273.0f, 16 / 273.0f, 26 / 273.0f, 16 / 273.0f, 4 / 273.0f},
        {1 / 273.0f, 4 / 273.0f, 7 / 273.0f, 4 / 273.0f, 1 / 273.0f}
    };

    size_t stringSize = (info.width * (info.bitperpixel / 8) + 3) & ~3;

    uint8_t* temporaryArray = new uint8_t[stringSize * info.height];

    for (int y = 0; y < info.height; ++y) {
        for (int x = 0; x < info.width; ++x) {
            float r = 0.0f;
            float g = 0.0f;
            float b = 0.0f;

            for (int ay = -halfKernel; ay <= halfKernel; ++ay) {
                for (int bx = -halfKernel; bx <= halfKernel; ++bx) {
                    int cx = clamp(x + bx, 0, info.width - 1);
                    int dy = clamp(y + ay, 0, info.height - 1);

                    size_t pixelIndex = dy * stringSize + cx * (info.bitperpixel / 8);
                    float weight = kernel[ay + halfKernel][bx + halfKernel];

                    b += imgdata[pixelIndex + 0] * weight;
                    g += imgdata[pixelIndex + 1] * weight;
                    r += imgdata[pixelIndex + 2] * weight;
                }
            }

            size_t newPixelIndex = y * stringSize + x * (info.bitperpixel / 8);
            temporaryArray[newPixelIndex + 0] =
                static_cast<uint8_t>(clamp(b, 0.0f, 255.0f));
            temporaryArray[newPixelIndex + 1] =
                static_cast<uint8_t>(clamp(g, 0.0f, 255.0f));
            temporaryArray[newPixelIndex + 2] =
                static_cast<uint8_t>(clamp(r, 0.0f, 255.0f));
        }
    }

    std::memcpy(imgdata, temporaryArray, stringSize * info.height);
    delete[] temporaryArray;

}