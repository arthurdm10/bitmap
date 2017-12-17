#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <string.h>



#ifndef _WIN32
typedef unsigned char byte;
typedef unsigned short WORD;

#ifdef __amd64__
typedef unsigned int DWORD;
#elif __i386__
typedef unsigned long DWORD;
#endif

typedef unsigned int UINT;
#endif

class Bitmap {
public:
    bool operator== (const Bitmap& other);

	struct Pixel {
		byte b, g, r;

        Pixel() { }

		Pixel(byte b, byte g, byte r) :
			b(b), g(g), r(r) {

        }
        Pixel(const Pixel& other){
            this->b = other.b;
            this->r = other.r;
            this->g = other.g;
        }

    };


private:
	struct _bitmapHeader {
        friend bool Bitmap::operator== (const Bitmap& other);

        WORD headerType;
		DWORD fileSize;
		WORD r0, r1;
		DWORD dataOffset;

        _bitmapHeader& operator= (const _bitmapHeader&& other){
            this->headerType = std::move(other.headerType);
            this->fileSize = std::move(other.fileSize);
            this->dataOffset = std::move(other.dataOffset);
            return *this;
        }

        _bitmapHeader& operator= (const _bitmapHeader& other){
            this->headerType = other.headerType;
            this->fileSize = other.fileSize;
            this->dataOffset = other.dataOffset;
            return *this;
        }

        bool operator== (const _bitmapHeader& other){
            return (this->headerType == other.headerType &&
                    this->fileSize == other.fileSize &&
                    this->dataOffset == other.dataOffset);
        }

        bool operator!= (const _bitmapHeader& other){
            return !(*this == other);
        }
	};

	struct _dibHeader {
        friend bool Bitmap::operator== (const Bitmap& other);
		DWORD headerSize,
			width,
			height;

		WORD colorPlanes;
		WORD bitsPerPixel;
		DWORD compression;
		DWORD dataSize;

        DWORD xResolution,
			yResolution;

		DWORD numPalleteColors;
		DWORD importantColors;

        _dibHeader& operator= (const _dibHeader&& other){
            this->headerSize = std::move(other.headerSize);
            this->width = std::move(other.width);
            this->height = std::move(other.height);
            this->colorPlanes = std::move(other.colorPlanes);
            this->bitsPerPixel = std::move(other.bitsPerPixel);
            this->compression = std::move(other.compression);
            this->dataSize = std::move(other.dataSize);
            this->xResolution = std::move(other.xResolution);
            this->yResolution = std::move(other.yResolution);
            this->numPalleteColors = std::move(other.numPalleteColors);
            this->importantColors = std::move(other.importantColors);

            return *this;
        }

        _dibHeader& operator= (const _dibHeader& other){
            this->headerSize = other.headerSize;
            this->width = other.width;
            this->height = other.height;
            this->colorPlanes = other.colorPlanes;
            this->bitsPerPixel = other.bitsPerPixel;
            this->compression = other.compression;
            this->dataSize = other.dataSize;
            this->xResolution = other.xResolution;
            this->yResolution = other.yResolution;
            this->numPalleteColors = other.numPalleteColors;
            this->importantColors = other.importantColors;

            return *this;
        }

        bool operator!= (const _dibHeader& other){
            return !(*this == other);
        }

        bool operator== (const _dibHeader& other){
            return (this->headerSize == other.headerSize &&
            this->width == other.width &&
            this->height == other.height &&
            this->colorPlanes == other.colorPlanes &&
            this->bitsPerPixel == other.bitsPerPixel &&
            this->compression == other.compression &&
            this->dataSize == other.dataSize &&
            this->xResolution ==  other.xResolution  &&
            this->yResolution == other.yResolution &&
            this->numPalleteColors == other.numPalleteColors &&
            this->importantColors == other.importantColors);
        }
	};
	


	template<typename T>
    void readFromStream(std::ifstream& file, T& dest, const size_t& sz = sizeof(T)) {
		file.read(reinterpret_cast<char*>(&dest), sz);
	}

	template<typename T>
	void writeToStream(std::ofstream& file, T& src, const size_t& sz = sizeof(T)) {
		file.write(reinterpret_cast<char*>(&src), sz);
	}

	size_t fileSize(std::ifstream& file);

	
private:

    std::string fileName;

    std::ifstream file;
	_bitmapHeader bitmapHeader;
	_dibHeader	  dibHeader;

	size_t	_fileSize{ 0 };

	void readBitmapHeader();
	void readDibHeader();

	std::vector<std::vector<Pixel>> pixels;


	DWORD _width, _height;

public:

	
	Bitmap();
    Bitmap(Bitmap&& bitmap);
    Bitmap(const Bitmap& bitmap);
	Bitmap(const std::string& filename);

	bool load(const std::string& filename);

	//create a bitmap from a pixel matrix
    //the given matrix will be moved
	bool load(std::vector<std::vector<Pixel>>& pixels);

	bool save(const std::string& fileName);

    //close the file,
    //but keep all the data
    inline void close(){
        this->file.close();
    }

	void rgbToGrayScale();

	
	std::vector<std::vector<Pixel>>& getPixels();

	//file size in bytes
    inline size_t size() const{
        return this->_fileSize;
    }

    inline DWORD width() const{
        return this->_width;
    }

    inline DWORD height() const{
        return this->_height;
    }

    std::vector<Pixel>& operator[](const size_t& y);

    Bitmap& operator=(const Bitmap& other);    //copy
    Bitmap& operator=(Bitmap&& other);          //move

    operator bool() const{
        return this->file.is_open() || this->pixels.size() > 0;
    }

    bool operator!(){
        return !(this->file.is_open());
    }

	~Bitmap();
};

std::ostream& operator<< (std::ostream& out, const Bitmap::Pixel& pixel);
bool operator== (const Bitmap::Pixel& p1, const Bitmap::Pixel& p2);
bool operator!= (const Bitmap::Pixel& p1, const Bitmap::Pixel& p2);
