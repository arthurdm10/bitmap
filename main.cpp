#include <iostream>
#include <time.h>

using namespace std;

int main(){

    //create a Matrix of random pixels...
    std::srand(time(0));
    Bitmap::PixelMatrix pixels(100);
    for(int i = 0; i < 100; i++){
        for(int x = 0; x < 300; x++){
            pixels[i].emplace_back(rand() % 255, rand() % 255, rand() % 255);
        }
    }



    Bitmap img(pixels);

    //print pixel RGB values
    cout << img[32][44] << endl;

    img.save("random.bmp");

    img.load("img.bmp");
    img.rgbToGrayScale();
    img.save("gray.bmp");

    img.load("img2.bmp");
    img.mirror();
    img.save("mirror.bmp");

    return 0;
}

