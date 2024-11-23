/*
Andikalovskiy Nikita Dmitrievich
24.B-82mm
st131335@student.spbu.ru
LabWork 1
*/

#include <cstdint>
#include <iostream>

#ifndef BMPHEADERS_H
#define BMPHEADERS_H

#pragma pack(1)
struct BMPfilehead {
  uint16_t filetype;
  uint32_t filesize;
  uint16_t reserved1;
  uint16_t reserved2;
  uint32_t offset;
};
#pragma pack()

struct BMPinfo {
  uint32_t size;
  int32_t width;
  int32_t height;
  uint16_t planes;
  uint16_t bitperpixel;
  uint32_t compression;
  uint32_t imagesize;
  int32_t x_pixels_per_meter;
  int32_t y_pixels_per_meter;
  uint32_t colors_used;
  uint32_t colors_important;
};

#endif 