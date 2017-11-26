#pragma once
#include <fstream>
#include <Windows.h>
#include <string>
#include <vector>

typedef signed long SIGDWORD;

class Bitmap {
public:
	struct Pixel {
		byte b, g, r;
		Pixel() { };

		Pixel(byte b, byte g, byte r) :
			b(b), g(g), r(r) {

		};
	};


private:
	struct _bitmapHeader {
		WORD headerType;
		DWORD fileSize;
		WORD r0, r1;
		DWORD dataOffset;
	};

	struct _dibHeader {
		DWORD headerSize,
			width,
			height;

		WORD colorPlanes;
		WORD bitsPerPixel;
		DWORD compression;
		DWORD dataSize;

		//because the resolution may be negative
		SIGDWORD xResolution,
			yResolution;

		DWORD numPalleteColors;
		DWORD importantColors;
	};
	


	template<typename T>
	void readFromStream(std::ifstream& file, T& dest, size_t sz = sizeof(T)) {
		file.read(reinterpret_cast<char*>(&dest), sz);
	}

	template<typename T>
	void writeToStream(std::ofstream& file, T& src, const size_t& sz = sizeof(T)) {
		file.write(reinterpret_cast<char*>(&src), sz);
	}

	size_t fileSize(std::ifstream& file);

	
private:
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
	Bitmap(const std::string& filename);

	bool load(const std::string& filename);

	//create a bitmap from a pixel matrix
	//the given pixel matrix will be moved to this class
	bool load(std::vector<std::vector<Pixel>>& pixels);

	bool save(const std::string& fileName);

	void rgbToGrayScale();

	
	std::vector<std::vector<Pixel>>& getPixels();

	//file size in bytes
	size_t size();

	DWORD width();
	DWORD height();

	std::vector<Pixel>& operator[](const size_t& y);

	~Bitmap();
};

std::ostream& operator<< (std::ostream& out, const Bitmap::Pixel& pixel);
