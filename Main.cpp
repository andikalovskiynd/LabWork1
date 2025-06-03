/**
 * @file Main.cpp
 * @brief Main file where the task is being completed.
*/

/*
Andikalovskiy Nikita Dmitrievich
24.B-82mm
st131335@student.spbu.ru
LabWork 1
*/

#include <iostream>
#include <chrono>
#include "BMPheaders.h"
#include "Functions.h"

int main() {
    const char* filename = "image.bmp";
    const int REPETITIONS = 15;

    std::cout << "Starting performance test with " << REPETITIONS << " repetitions." << std::endl;

    // TEST 1:
    long long sequentTime = 0;

    for (int i = 0; i < REPETITIONS; ++i) {
        BMPinfo seqInfo;
        size_t seqSize = 0;
        uint8_t* seqData = load(filename, seqSize, seqInfo);

        if (!seqData) {
            std::cerr << "Error loading image for sequential test repetition " << i + 1 << std::endl;
            continue;
        }
        auto start_time = std::chrono::high_resolution_clock::now();

        rotateforward(seqData, seqInfo, seqSize);
        rotatebackwards(seqData, seqInfo, seqSize);
        oldBlur(seqData, seqInfo);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        sequentTime += duration.count();

        delete[] seqData;
    }

    std::cout << "Total time for " << REPETITIONS << " sequential runs is " << sequentTime << " ms" << std::endl;
    std::cout << "Average sequential time per run: " << static_cast<double>(sequentTime) / REPETITIONS << " ms" << std::endl;

    long long parallelTime = 0;
    for (int i = 0; i < REPETITIONS; ++i) {
        BMPinfo parInfo;
        size_t parSize = 0;
        uint8_t* parData = load(filename, parSize, parInfo);

        if (!parData) {
            std::cerr << "Error loading image for sequential test repetition " << i + 1 << std::endl;
            continue;
        }
        auto start_time = std::chrono::high_resolution_clock::now();

        rotateforward(parData, parInfo, parSize);
        rotatebackwards(parData, parInfo, parSize);
        blur(parData, parInfo);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        sequentTime += duration.count();

        delete[] parData;
    }

    std::cout << "Total time for " << REPETITIONS << " parallel runs: " << parallelTime << " ms" << std::endl;
    std::cout << "Average parallel time per run: " << static_cast<double>(parallelTime) / REPETITIONS << " ms" << std::endl;

    std::cout << "Totally parallel is " << sequentTime - parallelTime << " ms faster than sequent." << std::endl;
    std::cout << "Averagely parallel is " << static_cast<double>(sequentTime) / REPETITIONS - static_cast<double>(parallelTime) / REPETITIONS<< " ms faster than sequent." << std::endl;
}
