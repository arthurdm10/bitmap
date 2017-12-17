#include "Bitmap.h"
#include <iostream>
#include <exception>

using namespace std;

Bitmap::Bitmap(){ }

Bitmap::Bitmap(Bitmap &&bitmap){
    this->pixels = std::move(bitmap.pixels);
    this->_width = std::move(bitmap._width);
    this->_height = std::move(bitmap._height);
    this->_fileSize = std::move(bitmap._fileSize);
    this->file = std::move(bitmap.file);
    this->dibHeader = std::move(bitmap.dibHeader);
    this->bitmapHeader = std::move(bitmap.bitmapHeader);
}

Bitmap::Bitmap(const Bitmap &bitmap){
    this->pixels = bitmap.pixels;
    this->_width = bitmap._width;
    this->_height = bitmap._height;
    this->_fileSize = bitmap._fileSize;
    this->dibHeader = bitmap.dibHeader;
    this->bitmapHeader = bitmap.bitmapHeader;
}

Bitmap::Bitmap(const std::string& fileName): fileName(fileName) {
    this->load(fileName);
}

bool Bitmap::load(const std::string& fileName) {
    if (!this->file.is_open()) {
        this->fileName = fileName;
        this->file.open(fileName, std::ios::binary);
        if(file.is_open()){
            this->readBitmapHeader();
            this->readDibHeader();
        }
	}	
	
	this->_width = this->dibHeader.width;
	this->_height = this->dibHeader.height;
	
	this->_fileSize = this->fileSize(this->file);
	
	this->file.seekg(this->bitmapHeader.dataOffset);
	for (int i = 0; i < this->_height; i++) {
		std::vector<Pixel> temp;
		for (int k = 0; k < this->_width; k++) {
			Pixel p;
			readFromStream(this->file, p, 3);
			temp.push_back(p);
		}

		this->pixels.push_back(temp);
		file.seekg(this->_width % 4, file.cur);
	}
	
	return this->file.is_open();
}
bool Bitmap::load(std::vector<std::vector<Pixel>>& pixels){
	if (pixels.size() == 0 || pixels[0].size() == 0) {
		return false;
	}

	this->_width = pixels[0].size();
	this->_height = pixels.size();
	
	this->pixels = std::move(pixels);

	this->bitmapHeader.headerType = 0x4D42;
	this->bitmapHeader.r0 = 0x00;
	this->bitmapHeader.r1 = 0x00;
	this->bitmapHeader.dataOffset = 54;
	this->bitmapHeader.fileSize = 54 + (this->pixels.size() * 3) + (this->_width % 4) * this->pixels.size();

	this->dibHeader.headerSize = 40;
	this->dibHeader.width = this->_width;
	this->dibHeader.height = this->_height;
	this->dibHeader.colorPlanes = 1;
	this->dibHeader.bitsPerPixel = 24;
	this->dibHeader.compression = 0x00;
	this->dibHeader.dataSize = 0x00;
    this->dibHeader.xResolution = 3770;
    this->dibHeader.yResolution = 3770;
	this->dibHeader.numPalleteColors = 0;
	this->dibHeader.importantColors = 0;

	return true;
}

bool Bitmap::save(const std::string & fileName){

	if (this->pixels.size() == 0) {
		return false;
	}

	std::ofstream output(fileName, std::ios::binary);

	if (output.is_open()) {

		writeToStream(output, this->bitmapHeader.headerType);
		writeToStream(output, this->bitmapHeader.fileSize);
		writeToStream(output, this->bitmapHeader.r0);
		writeToStream(output, this->bitmapHeader.r1);
		writeToStream(output, this->bitmapHeader.dataOffset);
	
		writeToStream(output, this->dibHeader.headerSize);
		writeToStream(output, this->dibHeader.width);
		writeToStream(output, this->dibHeader.height);
		writeToStream(output, this->dibHeader.colorPlanes);
		writeToStream(output, this->dibHeader.bitsPerPixel);
		writeToStream(output, this->dibHeader.compression);
		writeToStream(output, this->dibHeader.dataSize);
		writeToStream(output, this->dibHeader.xResolution);
		writeToStream(output, this->dibHeader.yResolution);
		writeToStream(output, this->dibHeader.numPalleteColors);
		writeToStream(output, this->dibHeader.importantColors);
		

		for (auto& row : this->pixels) {
			for (auto& pixel : row) {
				writeToStream(output, pixel, 3);
			}

			//row padding
            WORD paddSz = row.size() % 4;
			char *padd = new char[paddSz];


            memset(padd, 0x00, paddSz);
            writeToStream(output, padd, paddSz);
		}
		output.close();
		return true;
	}

    return false;
}


std::vector<std::vector<Bitmap::Pixel>>& Bitmap::getPixels(){
	return this->pixels;
}

void Bitmap::rgbToGrayScale(){
	for (auto& row : this->pixels) {
		for (auto& pixel : row) {
			DWORD sum = pixel.r + pixel.g + pixel.b;
			sum /= 3;

			pixel.r = pixel.b = pixel.g = sum;
		}
	}
}

void Bitmap::readBitmapHeader(){
	readFromStream(this->file, this->bitmapHeader.headerType);
	readFromStream(this->file, this->bitmapHeader.fileSize);
	readFromStream(this->file, this->bitmapHeader.r0);
	readFromStream(this->file, this->bitmapHeader.r1);
	readFromStream(this->file, this->bitmapHeader.dataOffset);
}

void Bitmap::readDibHeader() {
	readFromStream(this->file, this->dibHeader.headerSize);
	readFromStream(this->file, this->dibHeader.width);
	readFromStream(this->file, this->dibHeader.height);
	readFromStream(this->file, this->dibHeader.colorPlanes);
	readFromStream(this->file, this->dibHeader.bitsPerPixel);
	readFromStream(this->file, this->dibHeader.compression);
	readFromStream(this->file, this->dibHeader.dataSize);
	readFromStream(this->file, this->dibHeader.xResolution);
	readFromStream(this->file, this->dibHeader.yResolution);
	readFromStream(this->file, this->dibHeader.numPalleteColors);
	readFromStream(this->file, this->dibHeader.importantColors);
}

std::vector<Bitmap::Pixel>& Bitmap::operator[](const size_t& y){
    if (y < this->_height) {
		return this->pixels[y];
    }
    throw std::runtime_error("Invalid pixel index!");
}

Bitmap& Bitmap::operator=(const Bitmap &other){
    if(*this == other){
        return *this;
    }
    this->pixels = other.pixels;
    this->_width = other._width;
    this->_height = other._height;
    this->_fileSize = other._fileSize;
    this->dibHeader = other.dibHeader;
    this->bitmapHeader = other.bitmapHeader;
}

Bitmap& Bitmap::operator=(Bitmap&& other){
    if(*this == other){
        return *this;
    }
    this->pixels = std::move(other.pixels);
    this->_width = std::move(other._width);
    this->_height = std::move(other._height);
    this->_fileSize = std::move(other._fileSize);
    this->file = std::move(other.file);
    this->dibHeader = std::move(other.dibHeader);
    this->bitmapHeader = std::move(other.bitmapHeader);
}

bool Bitmap::operator==(const Bitmap& other){
    if(other._width != this->width() ||
            other._height != this->height() || this->_fileSize != other._fileSize){
        return  false;
    }

    if(other.pixels != this->pixels){
        return false;
    }

    if(this->dibHeader !=  other.dibHeader||
            this->bitmapHeader != other.bitmapHeader){
        return false;
    }

    return true;
}

Bitmap::~Bitmap(){
	this->file.close();
}

size_t Bitmap::fileSize(std::ifstream & file){
	if (file.is_open()) {
		file.clear();
		file.seekg(0, file.beg);
		file.seekg(0, file.end);
		size_t sz = file.tellg();

		file.clear();
		file.seekg(0, file.beg);
		return sz;
	}

	return -1;
}

std::ostream& operator<<(std::ostream & out, const Bitmap::Pixel & pixel){
	return out << "R:" << static_cast<int>(pixel.r) << "\nG:" << static_cast<int>(pixel.g) << "\nB:" << static_cast<int>(pixel.b);
}


bool operator== (const Bitmap::Pixel& p1, const Bitmap::Pixel& p2){
    return (p1.b == p2.b &&
            p1.g == p2.g &&
            p1.r == p2.r);
}

bool operator!= (const Bitmap::Pixel& p1, const Bitmap::Pixel& other){
    return !(p1 == other);
}
