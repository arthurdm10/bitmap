#pragma once

#define _HAS_STD_BYTE 0

#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <utility>


using byte = unsigned char;
using WORD = unsigned short;
using DWORD = unsigned int;
using UINT = unsigned int;


class Bitmap {
public:
	struct Pixel {
		byte b, g, r;

		Pixel() {}

		Pixel(byte b, byte g, byte r) :
			b(b), g(g), r(r) {}

		Pixel(Pixel&& p) noexcept :
			b(std::exchange(p.b, 0x00)),
			g(std::exchange(p.g, 0x00)),
			r(std::exchange(p.r, 0x00)) {}

		Pixel(const Pixel& other)
			:b(other.b), g(other.g), r(other.r) {}

		Pixel& operator=(const Pixel& rhs) {
			if (this != &rhs) {
				this->b = rhs.b;
				this->g = rhs.g;
				this->r = rhs.r;
			}
			return *this;
		}

		bool operator==(const Pixel& rhs) {
			return (this->b == rhs.b &&
					this->g == rhs.g &&
					this->r == rhs.r);
		}

		bool operator!=(const Pixel& rhs) { return !(*this == rhs); }

	};


private:
	template<class T>
	inline void readFromStream(std::ifstream& file, T& dest, const size_t& sz = sizeof(T)) {
		file.read(reinterpret_cast<char*>(&dest), sz);
	}

	template<class T>
	inline void writeToStream(std::ofstream& file, const T& src, const size_t& sz = sizeof(T)) {
		file.write(reinterpret_cast<const char*>(&src), sz);
	}

public:
	typedef std::vector<std::vector<Bitmap::Pixel>> PixelMatrix;

private:
	std::vector<Pixel> pixels;


	void readBitmapHeader(std::ifstream& file);
	void readDibHeader(std::ifstream& file);
	size_t rowSz;                           //amount of bytes needed to get a entire row
	
	DWORD m_fileSize,
		m_width,
		m_height;

public:
	Bitmap() = default;
	Bitmap(Bitmap&& bitmap) noexcept;
	Bitmap(const Bitmap& bitmap) noexcept;
	Bitmap(const std::string& fileName);
	Bitmap(const PixelMatrix& pixels);
	Bitmap(PixelMatrix&& pixels);

	bool load(const std::string& fileName);

	//create a bitmap from a matrix of pixels
	bool load(const PixelMatrix& pixels);
	bool save(const std::string& fileName);


	Bitmap roi(DWORD x, DWORD y, DWORD width, DWORD height);

	void toGrayScale();
	
	void mirror();
	void horizontalMirror() { flip180(); mirror(); }
	void flip180() { std::reverse(pixels.begin(), pixels.end()); }
	void flipLeft();
	void flipRight();

	Pixel& operator()(DWORD x, DWORD y);

	std::size_t fileSize()	const noexcept { return m_fileSize; }
	std::size_t dataSize()	const noexcept { return m_width * m_height * 3; }
	DWORD width()			const noexcept { return m_width; }
	DWORD height()			const noexcept { return m_height; }

	Bitmap& operator=(const Bitmap& other);    //copy
	Bitmap& operator=(Bitmap&& other);          //move
	bool operator== (const Bitmap& other);

	operator bool() const { return this->pixels.size() > 0; }
	bool operator!() const { return !(*this); }

	~Bitmap() = default;
};

std::ostream& operator<< (std::ostream& out, const Bitmap::Pixel& pixel);
