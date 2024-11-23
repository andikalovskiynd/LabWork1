/*
Andikalovskiy Nikita Dmitrievich
24.B-82mm
st131335@student.spbu.ru
LabWork 1
*/

#include <algorithm>
#include <fstream>
#include <iostream>
#include "Functions.h"

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

uint8_t* load(const char* filename, size_t& imgsize, BMPinfo& info)  
{
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

  std::cout << "Image loaded successfully!" << std::endl;
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

  else {
    std::cout << "Image saved successfully to " << filename << std::endl;
  }

  outfile.close();
}

void rotateforward(uint8_t*& imgdata, BMPinfo& info, size_t& imgsize) {
  size_t stringSize = (info.width * (info.bitperpixel / 8) + 3) & ~3;
  size_t rotatedStringSize = (info.height * (info.bitperpixel / 8) + 3) & ~3;
  size_t rotatedSize = rotatedStringSize * info.width;

  uint8_t* rotated = new uint8_t[rotatedSize];
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
  imgsize = rotatedStringSize * info.width;

  std::cout << "RFW: Forward rotation completed successfully!" << std::endl;
}

void rotatebackwards(uint8_t*& imgdata, BMPinfo& info, size_t& imgsize) {
  size_t stringSize = (info.width * (info.bitperpixel / 8) + 3) & ~3;
  size_t rotatedStringSize = (info.height * (info.bitperpixel / 8) + 3) & ~3;
  size_t rotatedSize = rotatedStringSize * info.width;

  uint8_t* rotated = new uint8_t[rotatedSize];
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
  imgsize = rotatedStringSize * info.width;

  std::cout << "RBW: Backward rotation completed successfully!" << std::endl;
}

void blur(uint8_t*& imgdata, BMPinfo& info) {
  const int kernelsize = 5;
  const int halfKernel = kernelsize / 2;
  const float kernel[5][5] = {
      {1 / 273.0f, 4 / 273.0f, 7 / 273.0f, 4 / 273.0f, 1 / 273.0f},
      {4 / 273.0f, 16 / 273.0f, 26 / 273.0f, 16 / 273.0f, 4 / 273.0f},
      {7 / 273.0f, 26 / 273.0f, 41 / 273.0f, 26 / 273.0f, 7 / 273.0f},
      {4 / 273.0f, 16 / 273.0f, 26 / 273.0f, 16 / 273.0f, 4 / 273.0f},
      {1 / 273.0f, 4 / 273.0f, 7 / 273.0f, 4 / 273.0f, 1 / 273.0f}};

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

  std::cout << "Blurred successfully!" << std::endl;
}

