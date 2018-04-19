#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <string.h>


#ifndef _WIN32
typedef unsigned char byte;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int UINT;
#else
#include <Windows.h>
#endif

class Bitmap {
public:

	struct Pixel {
		byte b, g, r;

		Pixel() { }

		Pixel(byte b, byte g, byte r) :
			b(b), g(g), r(r) { }

		Pixel(const Pixel& other)
			:b(other.b), g(other.g), r(other.r) { }

		Pixel& operator=(const Pixel& other) {
			this->b = other.b;
			this->g = other.g;
			this->r = other.r;
			return *this;
		}

	
	};

	bool operator== (const Bitmap& other);

private:
	struct _bitmapHeader {
		friend bool Bitmap::operator== (const Bitmap& other);

		WORD	headerType = 0x4D42;
		DWORD	fileSize;
		WORD	r0 = 0,
				r1 = 0;

		DWORD	dataOffset = 54;
		
		_bitmapHeader() = default;

		_bitmapHeader(const _bitmapHeader& other)
			:headerType(other.headerType),
			fileSize(other.fileSize),
			dataOffset(other.dataOffset) { }

		_bitmapHeader(_bitmapHeader&& other)
			:headerType(std::move(other.headerType)),
			fileSize(std::move(other.fileSize)),
			dataOffset(std::move(other.dataOffset)) { }


		_bitmapHeader& operator= (const _bitmapHeader&& other) {
			this->headerType = std::move(other.headerType);
			this->fileSize = std::move(other.fileSize);
			this->dataOffset = std::move(other.dataOffset);
			return *this;
		}

		_bitmapHeader& operator= (const _bitmapHeader& other) {
			this->headerType = other.headerType;
			this->fileSize = other.fileSize;
			this->dataOffset = other.dataOffset;
			return *this;
		}

		bool operator== (const _bitmapHeader& other) const{
			return (this->headerType == other.headerType &&
				this->fileSize == other.fileSize &&
				this->dataOffset == other.dataOffset);
		}

		bool operator!= (const _bitmapHeader& other) const { return !(*this == other); }
	};

	struct _dibHeader {
		friend bool Bitmap::operator== (const Bitmap& other);
		DWORD headerSize = 40,
			width,
			height;

		WORD colorPlanes = 1;
		WORD bitsPerPixel = 24;
		DWORD compression = 0;
		DWORD dataSize;

		DWORD xResolution = 0,
			yResolution = 0;

		DWORD numPalleteColors = 0;
		DWORD importantColors = 0;
		
		_dibHeader() = default;

		_dibHeader(const _dibHeader& other)
			:headerSize(other.headerSize),
			width(other.width),
			height(other.height),
			colorPlanes(other.colorPlanes),
			bitsPerPixel(other.bitsPerPixel),
			compression(other.compression),
			dataSize(other.dataSize),
			xResolution(other.xResolution),
			yResolution(other.yResolution),
			numPalleteColors(other.numPalleteColors),
			importantColors(other.importantColors) { }
		
		_dibHeader(_dibHeader&& other)
			:headerSize(std::move(other.headerSize)),
			width(std::move(other.width)),
			height(std::move(other.height)),
			colorPlanes(std::move(other.colorPlanes)),
			bitsPerPixel(std::move(other.bitsPerPixel)),
			compression(std::move(other.compression)),
			dataSize(std::move(other.dataSize)),
			xResolution(std::move(other.xResolution)),
			yResolution(std::move(other.yResolution)),
			numPalleteColors(std::move(other.numPalleteColors)),
			importantColors(std::move(other.importantColors)) { }

		

		_dibHeader& operator= (_dibHeader&& other) {
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

		_dibHeader& operator= (const _dibHeader& other) {
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

		bool operator!= (const _dibHeader& other) const { return !(*this == other); }

		bool operator== (const _dibHeader& other) const {
			return (this->headerSize == other.headerSize &&
				this->width == other.width &&
				this->height == other.height &&
				this->colorPlanes == other.colorPlanes &&
				this->bitsPerPixel == other.bitsPerPixel &&
				this->compression == other.compression &&
				this->dataSize == other.dataSize &&
				this->xResolution == other.xResolution  &&
				this->yResolution == other.yResolution &&
				this->numPalleteColors == other.numPalleteColors &&
				this->importantColors == other.importantColors);
		}
	};



	template<class T>
	void readFromStream(std::fstream& file, T& dest, const size_t& sz = sizeof(T)) {
		file.read(reinterpret_cast<char*>(&dest), sz);
	}

	template<class T>
	void writeToStream(std::fstream& file, T& src, const size_t& sz = sizeof(T)) {
		file.write(reinterpret_cast<char*>(&src), sz);
	}

public:
	typedef std::vector<std::vector<Bitmap::Pixel>> PixelMatrix;

private:
	std::fstream file;
	_bitmapHeader bitmapHeader;
	_dibHeader	  dibHeader;

	size_t	_fileSize{ 0 };
	std::vector<Pixel> pixels;
	DWORD _width, _height;


	void readBitmapHeader();
	void readDibHeader();
	size_t rowSz;                           //amount of bytes needed to get a entire row

public:
	Bitmap() = default;
	Bitmap(Bitmap&& bitmap);
	Bitmap(const Bitmap& bitmap);
	Bitmap(const std::string& fileName);
	Bitmap(const PixelMatrix& pixels);
	bool load(const std::string& fileName);

	//create a bitmap from a matrix of pixels
	bool load(const PixelMatrix& pixels);
	bool save(const std::string& fileName);


	void mirror();

	void rgbToGrayScale();

	Pixel& getPixel(const DWORD& x, const DWORD& y);

	//file size in bytes
	size_t size()   const noexcept { return this->_fileSize; }
	DWORD width()   const noexcept { return this->_width; }
	DWORD height()  const noexcept { return this->_height; }

	Bitmap::Pixel& pixel(const DWORD& x, const DWORD& y);

	Bitmap& operator=(const Bitmap& other);    //copy
	Bitmap& operator=(Bitmap&& other);          //move

	operator bool() const { return this->pixels.size() > 0; }
	bool operator!() const { return !(this); }

	~Bitmap() = default;
};

std::ostream& operator<< (std::ostream& out, const Bitmap::Pixel& pixel);
bool operator== (const Bitmap::Pixel& p1, const Bitmap::Pixel& p2);
bool operator!= (const Bitmap::Pixel& p1, const Bitmap::Pixel& p2);
