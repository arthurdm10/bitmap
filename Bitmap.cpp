#include "Bitmap.h"
#include <iostream>
#include <algorithm>
#include <exception>

using namespace std;

Bitmap::Bitmap(){ }

Bitmap::Bitmap(Bitmap &&bitmap){
    this->pixels = std::move(bitmap.pixels);
    this->_width = std::move(bitmap._width);
    this->_height = std::move(bitmap._height);
    this->_fileSize = std::move(bitmap._fileSize);
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

Bitmap::Bitmap(const std::string& fileName) {
    this->load(fileName);
}

Bitmap::Bitmap(Bitmap::PixelMatrix &pixels){
    this->load(pixels);
}

bool Bitmap::load(const std::string& fname) {
    if (!this->file.is_open()) {
        this->file.open(fname.c_str(), std::ios::binary | std::ios::in);
        if(file.is_open()){
            this->readBitmapHeader();
            if(this->bitmapHeader.headerType != 0x4D42){
                throw std::runtime_error("Invalid file");
            }

            this->_fileSize = this->bitmapHeader.fileSize;

            this->readDibHeader();
            if(this->dibHeader.bitsPerPixel != 24){
                throw std::runtime_error("Bitmap file must by 24bits per pixel");
            }

            if(this->dibHeader.compression != 0){
                throw std::runtime_error("Compressed bitmap file not supported");
            }

        }else{
            return false;
        }
    }

    this->_width = this->dibHeader.width;
    this->_height = this->dibHeader.height;

    this->rowSz = 3 * this->_width;         //1 pixel = 3 bytes

    if(this->_width == 0 || this->_height == 0){
        this->file.close();
        return false;
    }

    this->file.seekg(this->bitmapHeader.dataOffset);
    this->pixels.resize(this->_height * this->_width);

    for(int i = 0; i < this->_height; ++i){
        auto pos = this->pixels.size() - (i * this->_width) - this->_width;
        file.read(reinterpret_cast<char*>(&this->pixels[pos]), this->rowSz);
        file.seekg(this->_width % 4, file.cur);
    }

    this->file.close();
    return true;
}

bool Bitmap::load(PixelMatrix& pixels){
    if (!pixels.size() || !pixels[0].size()) {
        return false;
    }

    //check if all rows have the same size
    auto sz = pixels[0].size();
    auto p2 = std::find_if(std::begin(pixels),
                           std::end(pixels),
                           [&sz](const auto& item){
        return item.size() != sz;
    });

    if(p2 != std::end(pixels)){
        //found one
        throw std::runtime_error("All rows must have the same size");
    }

    this->_width = sz;
    this->_height = pixels.size();
    this->rowSz = 3 * this->_width;


    this->pixels.reserve(this->_width * this->_height);

    std::for_each(std::begin(pixels),
                  std::end(pixels),
                  [this](auto& row){
        this->pixels.insert(this->pixels.end(), row.begin(), row.end());
    });


    this->bitmapHeader.headerType = 0x4D42;
    this->bitmapHeader.dataOffset = 54;
    this->bitmapHeader.fileSize = 54 + (this->pixels.size() * 3) + (this->_width % 4) * this->pixels.size();

    this->dibHeader.headerSize = 40;
    this->dibHeader.width = this->_width;
    this->dibHeader.height = this->_height;
    this->dibHeader.colorPlanes = 1;
    this->dibHeader.bitsPerPixel = 24;
    this->dibHeader.compression = 0x00;
    this->dibHeader.dataSize = 0x00;
    this->dibHeader.xResolution = 0;
    this->dibHeader.yResolution = 0;
    this->dibHeader.numPalleteColors = 0;
    this->dibHeader.importantColors = 0;

    return true;
}

bool Bitmap::save(const std::string& fileName){
    if (this->pixels.size() == 0) {
        return false;
    }

    std::fstream output(fileName, std::ios::binary | std::ios::out);

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

        //row padding
        WORD paddSz = this->_width % 4;
        char *padd = new char[paddSz];
        memset(padd, 0x00, paddSz);

        for(int i = 0; i < this->_height; i++){
            int pos = this->pixels.size() - (i * this->_width) - this->_width;
            output.write(reinterpret_cast<char*>(&this->pixels[pos]), this->rowSz);
            output.write(padd, paddSz);
        }

        output.close();
        return true;
    }
    return false;
}

void Bitmap::mirror(){
    for(int i = 0; i < this->_height; i++){
        int row = i * this->_width;
        auto itBeg = this->pixels.begin() + row;
        std::reverse(itBeg, itBeg + this->_width);
    }
}


void Bitmap::rgbToGrayScale(){
    for (auto& pixel: this->pixels) {
        DWORD sum = pixel.r + pixel.g + pixel.b;
        sum /= 3;
        pixel.r = pixel.b = pixel.g = sum;
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

Bitmap::Pixel& Bitmap::pixel(const DWORD& x, const DWORD& y){
    if(y >= this->_height || x >= this->_width){
        throw std::range_error(std::string("Invalid Pixel position {" + to_string(x) + ", " + to_string(y) + " }"));
    }

    return this->pixels[y * this->_width + x];
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
    return(other._width == this->_width &&
           other._height == this->_height &&
           other.pixels == this->pixels);
}

Bitmap::~Bitmap() { }

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
