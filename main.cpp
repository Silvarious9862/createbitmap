#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>

class BitmapFileHeader;
class BitmapInfoHeader;
class Pallette;
class RGBtriple;
class PixArray;
class Bitmap;

double FindPerceivedLightness(__UINT8_TYPE__ red, __UINT8_TYPE__ green, __UINT8_TYPE__ blue);
inline double LumiToLstar(double luminance);
inline double ColorLinear(double color);
template<typename T>
void PrintMatrix(std::vector<double> const& matrix, int row_size);



class Bitmap
{
    private:
    class Pallette
    {
        private: 
        char palletteArray[1024];

        public: 
        Pallette()
        {
            for(int i = 0; i<1024;i++) 
            {
                palletteArray[i] = 0;
            }
        }

        friend std::ostream& operator<< (std::ostream &outstream, const Pallette& pal) 
        {
            return outstream << pal.palletteArray;
        }
    };

    class BitmapInfoHeader
    {
        private: 
        __UINT32_TYPE__ biSize;
        __LONG32 biWidth, biHeight;
        __UINT16_TYPE__ biPlanes, biBitCount; 
        __UINT32_TYPE__ biCompression, biSizeImage;
        __LONG32 biXPelsPerMeter, biYPelsPerMeter;
        __UINT32_TYPE__ biClrUsed, biClrImportant;
        
        public: 
        BitmapInfoHeader()
        {
            biSize          = 40;
            biWidth         = 200;
            biHeight        = 200;
            biPlanes        = 0x1;   // ploskost kartinki always 1   
            biBitCount      = 0x18;  // default bitcount = 24   
            biCompression   = 0x0;   // default compress = 0   
            biSizeImage     = 0x0;   // no compression = always 0
            biXPelsPerMeter = 0x0;
            biYPelsPerMeter = 0x0;
            biClrUsed       = 0x0;
            biClrImportant  = 0x0;
        }
        __LONG32 GetbiWidth() { return biWidth; };
        __LONG32 GetbiHeight() { return biHeight; };
        void SetbiBitCount(int bitCount = 24) { biBitCount = bitCount; };

        friend std::ostream& operator<< (std::ostream &outstream, const BitmapInfoHeader& bi) 
        {
            outstream.write((char*)&bi.biSize, sizeof(bi.biSize));
            outstream.write((char*)&bi.biWidth, sizeof(bi.biWidth));
            outstream.write((char*)&bi.biHeight, sizeof(bi.biHeight));
            outstream.write((char*)&bi.biPlanes, sizeof(bi.biPlanes));
            outstream.write((char*)&bi.biBitCount, sizeof(bi.biBitCount));
            outstream.write((char*)&bi.biCompression, sizeof(bi.biCompression));
            outstream.write((char*)&bi.biSizeImage, sizeof(bi.biSizeImage));
            outstream.write((char*)&bi.biXPelsPerMeter, sizeof(bi.biXPelsPerMeter));
            outstream.write((char*)&bi.biYPelsPerMeter, sizeof(bi.biYPelsPerMeter));
            outstream.write((char*)&bi.biClrUsed, sizeof(bi.biClrUsed));
            outstream.write((char*)&bi.biClrImportant, sizeof(bi.biClrImportant));
            return outstream ;
        }
        friend std::istream& operator>> (std::istream &instream, BitmapInfoHeader& bi) 
        {
            instream.read((char*)&bi.biSize, sizeof(bi.biSize));
            instream.read((char*)&bi.biWidth, sizeof(bi.biWidth));
            instream.read((char*)&bi.biHeight, sizeof(bi.biHeight));
            instream.read((char*)&bi.biPlanes, sizeof(bi.biPlanes));
            instream.read((char*)&bi.biBitCount, sizeof(bi.biBitCount));
            instream.read((char*)&bi.biCompression, sizeof(bi.biCompression));
            instream.read((char*)&bi.biSizeImage, sizeof(bi.biSizeImage));
            instream.read((char*)&bi.biXPelsPerMeter, sizeof(bi.biXPelsPerMeter));
            instream.read((char*)&bi.biYPelsPerMeter, sizeof(bi.biYPelsPerMeter));
            instream.read((char*)&bi.biClrUsed, sizeof(bi.biClrUsed));
            instream.read((char*)&bi.biClrImportant, sizeof(bi.biClrImportant));
            return instream ;
        }
    };
    
    

    class BitmapFileHeader 
    {
        private: 
        __UINT8_TYPE__ bfType[2];
        __UINT32_TYPE__ bfSize; 
        __UINT16_TYPE__ bfReserved1, bfReserved2;
        __UINT32_TYPE__ bfOffBits;

        public: 
        BitmapFileHeader()
            {
                bfType[0]   = 0x42;    // BM
                bfType[1]   = 0x4D;
                bfSize      = 0x0000;
                bfReserved1 = 0x0000;  // always 0
                bfReserved2 = 0x0000;  // always 0
                bfOffBits   = 0x36;    // always 54
            }
        void SetbfSize(BitmapInfoHeader& bi)
        {
            bfSize = bfOffBits + sizeof(RGBtriple) * bi.GetbiHeight() * bi.GetbiWidth();
        } 

        
        bool CheckBM()
        {
            if (bfType[0] == 0x42 && bfType[1] == 0x4D) return true;
            else return false;
        }

        __UINT32_TYPE__ GetbfSize() { return bfSize; };
        __UINT32_TYPE__ GetbfOffBits() { return bfOffBits; };

        friend std::ostream& operator<< (std::ostream &outstream, const BitmapFileHeader& bf) 
        {
            outstream.write((char*)&bf.bfType, sizeof(bf.bfType));
            outstream.write((char*)&bf.bfSize, sizeof(bf.bfSize));
            outstream.write((char*)&bf.bfReserved1, sizeof(bf.bfReserved1));
            outstream.write((char*)&bf.bfReserved2, sizeof(bf.bfReserved2));
            outstream.write((char*)&bf.bfOffBits, sizeof(bf.bfOffBits));
            
            return outstream;
        }
        friend std::istream& operator>> (std::istream &instream, const BitmapFileHeader& bf) 
        {
            instream.read((char*)&bf.bfType, sizeof(bf.bfType));
            instream.read((char*)&bf.bfSize, sizeof(bf.bfSize));
            instream.read((char*)&bf.bfReserved1, sizeof(bf.bfReserved1));
            instream.read((char*)&bf.bfReserved2, sizeof(bf.bfReserved2));
            instream.read((char*)&bf.bfOffBits, sizeof(bf.bfOffBits));
            
            return instream;
        }
    };

    class RGBtriple
    {
        private: 
        __UINT8_TYPE__ rgbBlue;
        __UINT8_TYPE__ rgbGreen;
        __UINT8_TYPE__ rgbRed;

        public: 
        RGBtriple() { rgbBlue = 0; rgbGreen = 0; rgbRed = 0; } ; 
        void SetRGBtriple(__UINT8_TYPE__ red, __UINT8_TYPE__ green, __UINT8_TYPE__ blue)
        {
            rgbBlue  = blue;
            rgbGreen = green;
            rgbRed   = red;
        }

        void SetRGBblue(__UINT8_TYPE__ blue) { rgbBlue = blue; }
        void SetRGBgreen(__UINT8_TYPE__ green) { rgbGreen = green; }
        void SetRGBred(__UINT8_TYPE__ red) { rgbRed = red; }
        __UINT8_TYPE__ GetRgbBlue() const { return rgbBlue; }
        __UINT8_TYPE__ GetRgbGreen() const { return rgbGreen; }
        __UINT8_TYPE__ GetRgbRed() const { return rgbRed; }

        friend std::ostream& operator<< (std::ostream &outstream, const RGBtriple& rgb) 
        {
            return outstream << rgb.rgbBlue << rgb.rgbGreen << rgb.rgbRed;
        }
        friend std::istream& operator>> (std::istream &instream, const RGBtriple& rgb) 
        {
            instream.read((char*)rgb.rgbBlue, sizeof(rgb.rgbBlue));
            instream.read((char*)rgb.rgbGreen, sizeof(rgb.rgbGreen));
            instream.read((char*)rgb.rgbRed, sizeof(rgb.rgbRed));
            return instream;
        }
    };    

    class PixArray
    {
        private:
        std::vector<RGBtriple> pixelArray;

        public: 

        void SetArraySize(int i)
        {
            RGBtriple rgb_null;
            pixelArray.resize(i, rgb_null);
        }

        void SetMonoArray(RGBtriple triple)
        {
            for(int i=0; i<pixelArray.size();i++)
            {
                pixelArray.at(i) = triple;
            }
            pixelArray.shrink_to_fit();
        }

        void SetConcretePixel(int i, RGBtriple triple) 
        {
            pixelArray[i] = triple;
        }

        void SetConcretePixel(int i, char blue, char green, char red) 
        {
            pixelArray[i].SetRGBtriple(red, green, blue);
            RGBtriple triple = GetConcretePixel(i);
            pixelArray[i] = triple;
        }
        
        RGBtriple GetConcretePixel(int i)
        {
            return pixelArray.at(i);
        }

        friend std::ostream& operator<< (std::ostream &outstream, PixArray& pixels) 
        {
            for(int i = 0; i<pixels.pixelArray.size();i++)
            {
                outstream << pixels.GetConcretePixel(i);
            }
            return outstream;
        }
        friend std::istream& operator>> (std::istream &instream, PixArray& pixels) 
        {
            char tmpblue, tmpgreen, tmpred;
            for(int i = 0; i<pixels.pixelArray.size();i++)
            {
                instream.read((char*)&tmpblue, sizeof(tmpblue));
                pixels.pixelArray[i].SetRGBblue(tmpblue);
                instream.read((char*)&tmpgreen, sizeof(tmpgreen));
                pixels.pixelArray[i].SetRGBgreen(tmpgreen);
                instream.read((char*)&tmpred, sizeof(tmpred));
                pixels.pixelArray[i].SetRGBred(tmpred);
            }
            return instream;
        }
    };
    
    BitmapInfoHeader bi;
    BitmapFileHeader bf;
    Pallette pal;
    RGBtriple rgb;
    PixArray pixels;
    std::fstream imageout;
    std::string filename;
    
    public:

    Bitmap(const char* filename = "tmp.bmp")
    {
        bf.SetbfSize(bi);
        pixels.SetArraySize(bi.GetbiHeight() * bi.GetbiWidth());
        this->filename = filename;
    }

    void SetMonoBitmap(char red, char green, char blue)
    {
        bi.SetbiBitCount(24);
        rgb.SetRGBtriple(red, green, blue);
        pixels.SetMonoArray(rgb);
    }

    void SaveAsBMP()
    {
        std::fstream imageout("out.bmp", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
        try
        {
            if (!imageout.is_open()) throw "Cannot create file";
            imageout << bf << bi << pixels;
            if (imageout.fail()) throw "Cannot print in file";
            imageout.close();
            std::cout << "Saved successfully" << std::endl;
        }
        catch(char const* errorline)
        {
            std::cout << errorline << std::endl;
            imageout.close();
        } 
    }

    int GetPixArrSize() { return bf.GetbfSize()-bf.GetbfOffBits(); }
    int GetbiHeight_public() { return bi.GetbiHeight(); };
    int GetbiWidth_public() { return bi.GetbiWidth(); };
    __INT8_TYPE__ GetPixelRed(int i)
    {
        rgb = pixels.GetConcretePixel(i);
        return rgb.GetRgbRed();
    }
    __INT8_TYPE__ GetPixelGreen(int i)
    {
        rgb = pixels.GetConcretePixel(i);
        return rgb.GetRgbGreen();
    }
    __INT8_TYPE__ GetPixelBlue(int i)
    {
        rgb = pixels.GetConcretePixel(i);
        return rgb.GetRgbBlue();
    }


    void ReadBMP(const char* filename)
    {
        this->filename = filename;
        std::fstream imagein(this->filename, std::ios_base::in | std::ios_base::binary);
        try {
            if (!imagein.is_open()) throw "Cannot open to read file";
            imagein >> bf;
            if (!this->bf.CheckBM()) throw "File is not a BMP or corrupted";
            pixels.SetArraySize(GetPixArrSize());
            imagein >> bi >> pixels;
            if (imagein.eofbit) throw "Readed successfully";
            imagein.close();
        }
        catch(char const* errorline)
        {
            std::cout << errorline << std::endl;
            imageout.close();
        } 
    }
};

double FindPerceivedLightness(__UINT8_TYPE__ red, __UINT8_TYPE__ green, __UINT8_TYPE__ blue)
{
    double colorRed = red / 255.0, colorGreen = green / 255.0, colorBlue = blue / 255.0;
    return LumiToLstar(ColorLinear(colorRed) * 0.2126 + ColorLinear(colorGreen) * 0.7152 + ColorLinear(colorBlue) * 0.0722);
}

inline double LumiToLstar(double luminance)
{
    if (luminance <= 216/24389) return luminance * 903.3;
    else return pow(luminance, (1.0/3.0)) * 116.0 - 16.0;
}

inline double ColorLinear(double color)
{
    if (color <= 0.4045) return color / 12.92;
    else return pow(((color+0.055)/1.055), 2.4);
}

void PrintMatrix(std::vector<double> const& matrix, int row_size)
{
    int row_number = 0;
    for (int i = matrix.size() - 1, j = 0; i > 0;)
    {
        std::cout << std::setw(4) << std::setprecision(2) << matrix[i] << " ";
        i -= 10; j += 10;
        if (j >= row_size) { std::cout << "  R" << row_number <<std::endl; j = 0; row_number++;}
    }
};

int main()
{
    std::cout << "START" << std::endl;
    Bitmap image;
    
    image.ReadBMP("samples/in.bmp");

    std::vector<double> matrix(1, 0);
    matrix.resize(image.GetPixArrSize()/3);
    __UINT8_TYPE__ red, green, blue;
    double lightness;
    int row, column;

    for (int i = 0; i<matrix.size();i++)
    {
        red = image.GetPixelRed(i);
        green = image.GetPixelGreen(i);
        blue = image.GetPixelBlue(i);
        lightness = FindPerceivedLightness(red, green, blue);
        matrix.at(i) = lightness;
    }
    PrintMatrix(matrix, image.GetbiWidth_public());

    std::cout << "EXIT" << std::endl;
    return 0;
}
