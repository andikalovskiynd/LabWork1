// COMPLETELY NOT FINAL VERSION!!!!!1!!!1!!
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
    int filetype;
    int filesize;
    int offset;
    int reserved1;
    int reserved2;
};

class BMPinfo // class for info header
{
public:
    int size;
    int width;
    int height;
    int planes;
    int bitperpixel;
    int compression;
    int imagesize;
};

const char* filename = "image.bmp";

size_t findmemorysize(const BMPinfo& info) // function that finds how much memory is needed to load img
{
    int byteperpixel = info.bitperpixel / 8;
    int stringsize = (byteperpixel * info.width + 3) & ~3;
    return stringsize * info.height;
}


int* load(const char* filename, size_t& imgsize, BMPinfo& info) // opening file and loading it's data
{
    BMPfilehead filehead;
    
    std::ifstream infile(filename, std::ios::binary); // reading file header and info header
    infile.read(reinterpret_cast<char*>(&filehead), sizeof(filehead));
    infile.read(reinterpret_cast<char*>(&info), sizeof(info));
    
    imgsize = findmemorysize(info); // counting how much memory we need for img
    
    infile.seekg(filehead.offset, std::ios::beg);
    int* imgdata = new int [imgsize]; // making array for imgdata
    infile.read(reinterpret_cast<char*>(imgdata), imgsize);
    
    infile.close();
    
    return imgdata;
}

void save(const char* filename, const int* imgdata, size_t imgsize, const BMPinfo& info)
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

void rotateforward(int* imgdata, BMPinfo& info, size_t imgsize)
{
    int width = info.width;
    int height = info.height;
    int byteperpixel = info.bitperpixel / 8;
    int stringsize = (width * byteperpixel + 3) & ~3;
    
    int* rotateddata = new int[width * height * byteperpixel];
    
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int original = (y * stringsize) + (x * byteperpixel);
            int rotated = (width - 1 -x) * stringsize + (y * byteperpixel);
            std::memcpy(&rotateddata[rotated], &imgdata[original], byteperpixel);
        }
    }
    
    std::swap(info.width, info.height);
    delete[] imgdata;
    imgdata = rotateddata;
}

void rotatebackwards(int* imgdata, BMPinfo& info, size_t imgsize)
{
    int width = info.width;
    int height = info.height;
    int byteperpixel = info.bitperpixel / 8;
    int stringsize = (width * byteperpixel + 3) & ~3;
    
    int* rotateddata = new int[width * height * byteperpixel];
    
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

void blur(int* imgdata, BMPinfo& info)
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
        for (int x = 1; y < width - 1; ++x)
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
                        
                        blurred += imgdata[res] * kernel[ky+1][kx+1];
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
    const char* filename = "img.bmp";
    size_t imgsize = 0;
    BMPinfo info;
    int* imgdata = load(filename, imgsize, info);
    
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
