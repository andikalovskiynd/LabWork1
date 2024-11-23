/*
Andikalovskiy Nikita Dmitrievich
24.B-82mm
st131335@student.spbu.ru
LabWork 1
*/

#include <cstring>
#include <iostream>
#include "BMPheaders.h"

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

bool readBMPinfo(const char* filename, BMPinfo& info);
uint8_t* load(const char* filename, size_t& imgsize, BMPinfo& info);
void save(const char* filename, const uint8_t* imgdata, size_t imgsize, BMPinfo& info);
void rotateforward(uint8_t*& imgdata, BMPinfo& info, size_t& imgsize);
void rotatebackwards(uint8_t*& imgdata, BMPinfo& info, size_t& imgsize);
void blur(uint8_t*& imgdata, BMPinfo& info);

#endif
