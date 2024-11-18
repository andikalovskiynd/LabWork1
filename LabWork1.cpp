/*
Andikalovskiy Nikita Dmitrievich
24.B-82mm
st131335@student.spbu.ru
LabWork 1
*/

// The code still not working but i'm trying to fix that ASAP

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

class BMPfilehead // class for file header
{
public:
    uint16_t filetype;
    uint32_t filesize;
    uint32_t offset;
    uint32_t reserved1;
    uint32_t reserved2;
};

class BMPinfo // class for info header
{
public:
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitperpixel;
    uint32_t compression;
    uint32_t imagesize;
};

size_t findmemorysize(const BMPinfo& info) // function that finds how much memory is needed to load img
{
    int byteperpixel = info.bitperpixel / 8;
    int stringsize = (byteperpixel * info.width + 3) & ~3;
    return stringsize * info.height;
}


uint8_t* load(const char* filename, size_t& imgsize, BMPinfo& info) // opening file and loading it's data
{
    BMPfilehead filehead;
    
    std::ifstream infile(filename, std::ios::binary); // reading file header and info header
    
        if(!infile)
    {
        std::cerr << "cannot open file!" << std::endl;
        return nullptr;
    }

    infile.read(reinterpret_cast<char*>(&filehead), sizeof(filehead));

        if(filehead.filetype != 0x4D42)
        {
            std::cerr << "Not a BMP file" << std::endl;
            return nullptr;
        }

    infile.read(reinterpret_cast<char*>(&info), sizeof(info));
    
    std::cout << "width: " << info.width << std::endl;
    std::cout << "height: " << info.height << std::endl;
    std::cout << "bits per pixel: " << info.bitperpixel << std::endl;

    imgsize = findmemorysize(info); // counting how much memory we need for img

    if (imgsize <= 0 || imgsize > 1e8)
    {
        std::cerr << "Size is incorrect" << std::endl;
    }

    std::cout << "Img size is " << imgsize <<std::endl;

    infile.seekg(filehead.offset, std::ios::beg);

    uint8_t* imgdata = new uint8_t[imgsize]; // making array for imgdata
    infile.read(reinterpret_cast<char*>(imgdata), imgsize);
    
    infile.close();
    
    return imgdata;
}

void save(const char* filename, const uint8_t* imgdata, size_t imgsize, const BMPinfo& info)
{
    BMPfilehead filehead;
    filehead.filetype = 0xD42;
    filehead.reserved1 = 0;
    filehead.reserved2 = 0;
    
    filehead.offset = sizeof(BMPfilehead) + sizeof(BMPinfo);
    filehead.filesize = static_cast<int>(filehead.offset + imgsize);
    
    std::ofstream outfile(filename, std::ios::binary);
    outfile.write(reinterpret_cast<const char*>(&filehead), sizeof(filehead));
    
    BMPinfo newinfo = info;
    outfile.write(reinterpret_cast<const char*>(&newinfo), sizeof (newinfo));
    
    outfile.write(reinterpret_cast<const char*>(imgdata), imgsize);
    
    outfile.close();
    
}

void rotateforward(uint8_t* imgdata, BMPinfo& info, size_t imgsize)
{
    int width = info.width;
    int height = info.height;
    int byteperpixel = info.bitperpixel / 8;
    int stringsize = (width * byteperpixel + 3) & ~3;
    
    uint8_t* rotateddata = new uint8_t[height * stringsize];
    
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int original = (y * stringsize) + (x * byteperpixel);
            int rotated = (width - 1 -x) * stringsize + (y * byteperpixel);
            if(original < imgsize && rotated < imgsize)
            {
            std::memcpy(&rotateddata[rotated], &imgdata[original], byteperpixel);
            }
        }
    }
    
    std::swap(info.width, info.height);
    delete[] imgdata;
    imgdata = rotateddata;
}

void rotatebackwards(uint8_t* imgdata, BMPinfo& info, size_t imgsize)
{
    int width = info.width;
    int height = info.height;
    int byteperpixel = info.bitperpixel / 8;
    int stringsize = (width * byteperpixel + 3) & ~3;
    
    uint8_t* rotateddata = new uint8_t[height * stringsize];
    
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int original = (y * stringsize) + (x * byteperpixel);
            int rotated = (x * stringsize) + (height - 1 - y) * byteperpixel;
            std::memcpy(&rotateddata[rotated], &imgdata[original], byteperpixel);
        }
    }
    
    std::swap(info.width, info.height);
    delete[] imgdata;
    imgdata = rotateddata;
}

void blur(uint8_t* imgdata, BMPinfo& info)
{
    int width = info.width;
    int height = info.height;
    int byteperpixel = info.bitperpixel / 8;
    int stringsize = (width * byteperpixel + 3) & ~3;
    
    float kernel[3][3] =
    {
        {1 / 16.0f, 2 / 16.0f, 1 / 16.0f},
        {2 / 16.0f, 4 / 16.0f, 2 / 16.0f},
        {1 / 16.0f, 2 / 16.0f, 1 / 16.0f}
    };
    
    int* blurreddata = new int[height * stringsize];
    std::memcpy(blurreddata, imgdata, height * stringsize);
    
    for (int y = 1; y < height - 1; ++y)
    {
        for (int x = 1; x < width - 1; ++x)
        {
            for (int channel = 0; channel < byteperpixel; ++channel)
            {
                float blurred = 0.0f;
                
                for (int ky = -1; ky <= 1; ++ky)
                {
                    for (int kx =-1; kx <= 1; ++kx)
                    {
                        int pX = x + kx;
                        int pY = y + ky;
                        int res = pY * stringsize + pX * byteperpixel + channel;
                        
                        if (res >= 0 && res < height * stringsize)
                        {
                            blurred += imgdata[res] * kernel[ky+1][kx+1];
                        }
                    }
                }
                
                blurreddata[y * stringsize + x * byteperpixel + channel] = static_cast<int>(clamp(blurred, 0.0f, 255.0f));
            }
        }
    }
    
    std::memcpy(imgdata, blurreddata, height * stringsize);
    delete[] blurreddata;
}

int main()
{
    const char* filename = "/Users/cyrep/Documents/image.bmp";
    size_t imgsize = 0;
    BMPinfo info;
    uint8_t* imgdata = load(filename, imgsize, info);
    
    if(imgdata)
    {
        std::cout << "Image is successfully loaded. Size of it is" << imgsize << "bytes." << std::endl;
        
        rotateforward(imgdata, info, imgsize);
        save("rotated1.bmp", imgdata, imgsize, info);
        
        rotatebackwards(imgdata, info, imgsize);
        save("rotated1and2.bmp", imgdata, imgsize, info);
        
        blur(imgdata, info);
        save("blurred.bmp", imgdata, imgsize, info);
        delete[] imgdata;
    }
    else
    {
        std::cout << "An error occured while loading image." << std::endl;
    }
    
    return 0;
}
