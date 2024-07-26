#include <iostream>
#include <string>
#include <vector>
#include <fstream>

//  { bfSize = bfOffBits + sizeof(RGBtriple)}
class BitmapFileHeader;
class BitmapInfoHeader;
class Pallette;
class RGBtriple;
class PixArray;
class Bitmap;



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

        friend std::ostream& operator<< (std::ostream &os, const Pallette& pal) 
        {
            return os << pal.palletteArray;
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

        friend std::ostream& operator<< (std::ostream &os, const BitmapInfoHeader& bi) 
        {
            os.write((char*)&bi.biSize, sizeof(bi.biSize));
            os.write((char*)&bi.biWidth, sizeof(bi.biWidth));
            os.write((char*)&bi.biHeight, sizeof(bi.biHeight));
            os.write((char*)&bi.biPlanes, sizeof(bi.biPlanes));
            os.write((char*)&bi.biBitCount, sizeof(bi.biBitCount));
            os.write((char*)&bi.biCompression, sizeof(bi.biCompression));
            os.write((char*)&bi.biSizeImage, sizeof(bi.biSizeImage));
            os.write((char*)&bi.biXPelsPerMeter, sizeof(bi.biXPelsPerMeter));
            os.write((char*)&bi.biYPelsPerMeter, sizeof(bi.biYPelsPerMeter));
            os.write((char*)&bi.biClrUsed, sizeof(bi.biClrUsed));
            os.write((char*)&bi.biClrImportant, sizeof(bi.biClrImportant));
            return os ;
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
        void SetbfSize(BitmapInfoHeader& bi); 

        friend std::ostream& operator<< (std::ostream &os, const BitmapFileHeader& bf) 
        {
            os.write((char*)&bf.bfType, sizeof(bf.bfType));
            os.write((char*)&bf.bfSize, sizeof(bf.bfSize));
            os.write((char*)&bf.bfReserved1, sizeof(bf.bfReserved1));
            os.write((char*)&bf.bfReserved2, sizeof(bf.bfReserved2));
            os.write((char*)&bf.bfOffBits, sizeof(bf.bfOffBits));
            
            return os;
        }
    };

    class RGBtriple
    {
        private: 
        char rgbBlue;
        char rgbGreen;
        char rgbRed;
        public: 
        RGBtriple() { rgbBlue = 0; rgbGreen = 0; rgbRed = 0; } ; 
        void SetRGBtriple(char red, char green, char blue)
        {
            rgbBlue  = blue;
            rgbGreen = green;
            rgbRed   = red;
        }
        friend std::ostream& operator<< (std::ostream &os, const RGBtriple& rgb) 
        {
            return os << rgb.rgbBlue << rgb.rgbGreen << rgb.rgbRed;
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
        
        RGBtriple GetConcretePixel(int i)
        {
            return pixelArray.at(i);
        }

        friend std::ostream& operator<< (std::ostream &os, PixArray& pixels) 
        {
            for(int i = 0; i<pixels.pixelArray.size();i++)
            {
                os << pixels.GetConcretePixel(i);
            }
            return os;
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

    void SetBitmap(char red, char green, char blue)
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
        }
        catch(char const* errorline)
        {
            std::cout << errorline << std::endl;
            imageout.close();
        } 
    }
};

int main()
{
    std::cout << "Start" << std::endl;
    Bitmap image;

    image.SetBitmap(1, 1, 1);
    image.SaveAsBMP();
    
    
    return 0;
}

void Bitmap::BitmapFileHeader::SetbfSize(BitmapInfoHeader &bi)
{
    bfSize = bfOffBits + sizeof(RGBtriple) * bi.GetbiHeight() * bi.GetbiWidth();
}
