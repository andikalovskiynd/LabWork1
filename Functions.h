/**
 * @file Functions.h
 * @brief Main functions needed to work with BMP image, rotate and blur it.
*/

/*
Andikalovskiy Nikita Dmitrievich
24.B-82mm
st131335@student.spbu.ru
LabWork 1 UPDATED
*/

#include <cstring>
#include <iostream>
#include "BMPheaders.h"

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

/**
 * @brief Read info about BMP image.
 * @param filename Name of file that we want to get information from.
 * @param info Object of BMPinfo structure that represents information about the file.
 * @return True, if image info was successfully read, false otherwise.
*/
bool readBMPinfo(const char* filename, BMPinfo& info);

/**
 * @brief Load the image to array.
 * @param filename Name of the file with image.
 * @param imgsize Size of the image.
 * @param info Information about the image.
 * @return Array which represents the image data.
*/
uint8_t* load(const char* filename, size_t& imgsize, BMPinfo& info);

/**
 * @brief Save BMPimage.
 * @param filename Name of new image.
 * @param imgdata Data of new image.
 * @param imgsize Size of new image.
 * @param info Information about the file.
*/
void save(const char* filename, const uint8_t* imgdata, size_t imgsize, BMPinfo& info);

/**
 * @brief Rotate the image 90 degrees clockwise.
 * @param imgdata Data of rotating image.
 * @param info Info about rotating image.
 * @param imgsize Size of rotating image.
*/
void rotateforward(uint8_t*& imgdata, BMPinfo& info, size_t& imgsize);

/**
 * @brief Rotate the image 90 degrees counterclockwise.
 * @param imgdata Data of rotating image.
 * @param info Info about rotating image.
 * @param imgsize Size of rotating image.
*/
void rotatebackwards(uint8_t*& imgdata, BMPinfo& info, size_t& imgsize);

/**
 * @brief Makes partial blurring of the image.
 * This function should be called by 1 thread.
 * @param imgdata Pointer on original image data.
 * @param info Struct object with information about the image.
 * @param start_y Starter string which thread will act on.
 * @param end_y Final string (not included).
 * @param stringSize Byte size of one string.
 * @param kernel Gauss matrix kernel.
 * @param halfKernel Half of size of the kernel.
*/
void blurSegment(uint8_t* imgdata, uint8_t* temporaryArray, const BMPinfo& info, int start_y, int end_y, size_t stringSize, const float kernel[5][5], int halfKernel);

/**
 * @brief Using blurSegment and threads blur whole image.
 * @param imgdata Pointer on original image data.
 * @param info Struct object with information about the image.
*/
void blur(uint8_t*& imgdata, BMPinfo& info);

/**
 * @brief Sequential old blur function.
 * @param imgdata Pointer on original image data.
 * @param info Struct object with information about the image.
*/
void oldBlur(uint8_t*& imgdata, BMPinfo& info);

#endif
