/*
Andikalovskiy Nikita Dmitrievich
24.B-82mm
st131335@student.spbu.ru
LabWork 1
*/

#include <iostream>
#include "BMPheaders.h"
#include "Functions.h"

int main() {
  const char* filename = "image.bmp";
  BMPinfo info;
  size_t imgsize = 0;
  uint8_t* imgdata = load(filename, imgsize, info);

  if (imgdata) {
    std::cout << "Image is successfully loaded. Size of it is " << imgsize << " bytes." << std::endl;

    rotateforward(imgdata, info, imgsize);
    save("rotated1.bmp", imgdata, imgsize, info);

    rotatebackwards(imgdata, info, imgsize);
    save("rotated1and2.bmp", imgdata, imgsize, info);

    blur(imgdata, info);
    save("rotatedAndBlurred.bmp", imgdata, imgsize, info);
  }

  else {
    std::cout << "An error occured while loading image." << std::endl;
  }

  return 0;
}
