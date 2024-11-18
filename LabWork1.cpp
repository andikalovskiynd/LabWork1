/*
Andikalovskiy Nikita Dmitrievich
24.B-82mm
st131335@student.spbu.ru
LabWork 1
*/

// Code is still not working but trying to fix that ASAP

#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <algorithm>

template <typename T>
T clamp(T value, T low, T high)
{
    if (value < low)
    {
        return low;
    }
    else if (value > high)
    {
        return high;
    }
    else
    {
        return value;
    }
}

struct BMPfilehead 
{
    uint16_t filetype;
    uint32_t filesize;
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t offset;
};

struct BMPinfo 
{
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitperpixel;
    uint32_t compression;
    uint32_t imagesize;
};

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

uint8_t* load(const char* filename, size_t& imgsize, BMPinfo& info) // opening file and loading it's data
{
    if (!readBMPinfo(filename, info))
    {
        return nullptr;
    }

    if (info.bitperpixel != 24)
    {
        std::cerr << "LOAD: Err: Only 24 bit BMP files supported!" << std::endl;
        return nullptr;
    }

    if (info.compression != 0)
    {
        std::cerr << "LOAD: Err: Only uncompressed BMP files supported!" << std::endl;
        return nullptr;
    }

    std::ifstream infile(filename, std::ios::binary);
    if (!infile)
    {
        std::cerr << "LOAD: Err: Cannot open file!" << std::endl;
        return nullptr;
    }

    size_t pixelOffset = 14 + info.size;
    infile.seekg(pixelOffset, std::ios::beg);

    size_t stringsize = (info.width * (info.bitperpixel / 8) +3 ) & ~3;
    imgsize = stringsize * std::abs(info.height);
   
    BMPfilehead filehead;
    std::cout << "Image size: " << imgsize << std::endl;
    std::cout << "Expected file size: " << filehead.offset + imgsize << std::endl;

    uint8_t* imgdata = new uint8_t[imgsize];

    infile.read(reinterpret_cast<char*>(imgdata), imgsize);

    if (!infile)
    {
        std::cerr << "LOAD: Err: Failed to read BMP pixel data!" << std::endl;
        delete[] imgdata;
        return nullptr;
    }

    std::cout << "Image loaded successfully!" << std::endl;
    return imgdata;
}

void save(const char* filename, const uint8_t* imgdata, size_t imgsize, BMPinfo& info)
{
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
    if (!outfile)
    {
        std::cerr << "SAVE: err: Couldn't open file!!" << std::endl;
        return;
    }

    outfile.write(reinterpret_cast<const char*>(&filehead), sizeof(filehead));
    
    std::cout << "Image size: " << imgsize << std::endl;
    std::cout << "Expected file size: " << filehead.offset + imgsize << std::endl;

    info.imagesize = imgsize;

    outfile.write(reinterpret_cast<const char*>(&info), sizeof (info));
    
    outfile.write(reinterpret_cast<const char*>(imgdata), imgsize);
    
    if (!outfile)
    {
        std::cerr << "SAVE: err: Failed to write to the file!" << std::endl;
    }
    
    else 
    {
        std::cout << "SAVE: Image saved successfully to " << filename << std::endl;
    }

    outfile.close();
    
}

void rotateforward(uint8_t* imgdata, BMPinfo& info, size_t& imgsize)
{
    size_t stringSize = (info.width * (info.bitperpixel / 8) + 3 ) & ~3;
    size_t rotatedStringSize = (info.height * (info.bitperpixel / 8) + 3) & ~3;
    size_t rotatedSize = rotatedStringSize * info.width;

    if (rotatedSize == 0)
    {
        std::cerr << "RFW: err: Memory fail" << std::endl;
        return;
    }

    uint8_t* rotated = new uint8_t[rotatedSize];
    if (!rotated)
    {
        std::cerr << "RFW: err: Memory failed!";
        return;
    }

    for (int y = 0; y < info.height; ++y)
    {
        for (int x = 0; x < info.width; ++x)
        {
            size_t oldPixel = y * stringSize + x * (info.bitperpixel / 8);
            size_t newPixel = (info.width - 1 - x) * rotatedStringSize + y * (info.bitperpixel / 8);
            std::memcpy(&rotated[newPixel], &imgdata[oldPixel], info.bitperpixel / 8);
        }
    }

    delete[] imgdata;
    imgdata = rotated;
    
    std::swap(info.width, info.height);
    imgsize = rotatedStringSize * info.width;

    BMPfilehead filehead;
    filehead.offset = sizeof(BMPinfo) + sizeof(BMPfilehead);
    std::cout << "String size: " << stringSize << std::endl;
    std::cout << "Image size: " << imgsize << std::endl;
    std::cout << "Expected file size: " << filehead.offset + imgsize << std::endl;

    std::cout << "RFW: Forward rotation completed successfully" << std::endl;
}

void rotatebackwards(uint8_t* imgdata, BMPinfo& info, size_t& imgsize)
{
    size_t stringSize = (info.width * (info.bitperpixel / 8) + 3 ) & ~3;
    size_t rotatedStringSize = (info.height * (info.bitperpixel / 8) + 3) & ~3;
    size_t rotatedSize = rotatedStringSize * info.width;

    if (rotatedSize == 0)
    {
        std::cerr << "RBW: err: Memory fail" << std::endl;
        return;
    }

    uint8_t* rotated = new uint8_t[rotatedSize];
    if (!rotated)
    {
        std::cerr << "RBW: err: Memory failed!";
        return;
    }

    for (int y = 0; y < info.height; ++y)
    {
        for (int x = 0; x < info.width; ++x)
        {
            size_t oldPixel = y * stringSize + x * (info.bitperpixel / 8);
            size_t newPixel = x * rotatedStringSize + (info.height - y - 1) * (info.bitperpixel / 8);

            std::memcpy(&rotated[newPixel], &imgdata[oldPixel], info.bitperpixel / 8);
        }
    }

    delete[] imgdata;
    imgdata = rotated;
    
    std::swap(info.width, info.height);
    imgsize = rotatedStringSize * info.width;

    size_t newStringSize = (info.width * (info.bitperpixel / 8) + 3) & ~3;

    BMPfilehead filehead;
    filehead.offset = sizeof(BMPinfo) + sizeof(BMPfilehead);
    std::cout << "String size: " << newStringSize << std::endl;
    std::cout << "Image size: " << imgsize << std::endl;
    std::cout << "Expected file size: " << filehead.offset + imgsize << std::endl;

    std::cout << "RBW: Backward rotation completed successfully" << std::endl;
}

void blur(uint8_t* imgdata, BMPinfo& info)
{
    
}

int main()
{
    const char* filename = "/Users/cyrep/Documents/blackbuck.bmp";
    BMPinfo info;
    size_t imgsize = 0;
    uint8_t* imgdata = load(filename, imgsize, info);
    
    if(imgdata)
    {
        std::cout << "Image is successfully loaded. Size of it is " << imgsize << " bytes." << std::endl;
        
        rotateforward(imgdata, info, imgsize);
        save("/Users/cyrep/Documents/rotated1.bmp", imgdata, imgsize, info);
        
        rotatebackwards(imgdata, info, imgsize);
        save("/Users/cyrep/Documents/rotated1and2.bmp", imgdata, imgsize, info);

        delete[] imgdata;
    }

    else
    {
        std::cout << "An error occured while loading image." << std::endl;
    }
    
    return 0;
}
