# Experiment Report

## Introduction
Original assignment was to make an aplication which will be able to perform different changes to BMP image. The most costly operation is Gaussian blur. So we will apply parallel methods to blur algorithm to check how will that affect the speed of completing the operations.

## Setup
We will extend the methods of blurring using two approaches:
    * **Original:** blur is being applied sequentily using 4 'for' cycles.
    * **Parallel:** original `blur` was split in two parts: `blurSegment` (for each thread) and `blur` which assigns threads. 

However, for the clarity of experiment, we will keep other functions (`rotateforward` and `rotatebackwards`).

## Reproducible experiment
To impartially compare two different approaches we will change main. Now it has 2 logic parts.
First is cycle of old `blur`s:
```cpp
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
```
And second is for new `blur` and `blurSegment` (inside the blur);
```cpp
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
        parallelTime += duration.count();

        delete[] parData;
    }
```

Finally, we have time counters and calculations that will allow us to compare the results:
```cpp
    std::cout << "\n Total time for " << REPETITIONS << " sequential runs is " << sequentTime << " ms" << std::endl;
    std::cout << "Average sequential time per run: " << static_cast<double>(sequentTime) / REPETITIONS << " ms" << std::endl;

    std::cout << "\n Total time for " << REPETITIONS << " parallel runs: " << parallelTime << " ms" << std::endl;
    std::cout << "Average parallel time per run: " << static_cast<double>(parallelTime) / REPETITIONS << " ms" << std::endl;

    std::cout << "\n Totally parallel is " << sequentTime - parallelTime << " ms faster than sequent." << std::endl;
    std::cout << "Averagely parallel is " << static_cast<double>(sequentTime) / REPETITIONS - static_cast<double>(parallelTime) / REPETITIONS<< " ms faster than sequent." << std::endl;
```

## Results
As a result of 15 cycles we have:
```Bash
 Total time for 15 sequential runs is 813 ms
Average sequential time per run: 54.2 ms

 Total time for 15 parallel runs: 206 ms
Average parallel time per run: 13.7333 ms

 Totally parallel is 607 ms faster than sequent.
Averagely parallel is 40.4667 ms faster than sequent.
```

50 cycles:
```Bash
 Total time for 50 sequential runs is 2638 ms
Average sequential time per run: 52.76 ms

 Total time for 50 parallel runs: 664 ms
Average parallel time per run: 13.28 ms

 Totally parallel is 1974 ms faster than sequent.
Averagely parallel is 39.48 ms faster than sequent.
```

100 cycles:
```Bash
 Total time for 100 sequential runs is 5235 ms
Average sequential time per run: 52.35 ms

 Total time for 100 parallel runs: 1321 ms
Average parallel time per run: 13.21 ms

 Totally parallel is 3914 ms faster than sequent.
Averagely parallel is 39.14 ms faster than sequent.
```

And even 1000! :
```Bash
 Total time for 1000 sequential runs is 52301 ms
Average sequential time per run: 52.301 ms

 Total time for 1000 parallel runs: 13600 ms
Average parallel time per run: 13.6 ms

 Totally parallel is 38701 ms faster than sequent.
Averagely parallel is 38.701 ms faster than sequent.
```

## Conclusion
Finally, it is noticeable that the time gain of the parallel approach is from **38.7** to **40,5**, which decisively proves that in this case the parallel approach gives a huge time gain.