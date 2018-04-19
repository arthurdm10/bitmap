#include <iostream>
#include <time.h>
#include "Bitmap.h"
#include <algorithm>

using namespace std;

int main(){

    //create a Matrix of random pixels...
    std::srand(time(0));
    Bitmap::PixelMatrix pixels(400);

    for(int i = 0; i < 400; i++){
        for(int x = 0; x < 400; x++){
            pixels[i].emplace_back(rand() % 25, rand() % 100, rand() % 5);
        }
    }


	Bitmap img;
	/* Bitmap img(pixels)*/

	img.load(pixels);

	//print pixel RGB values
    img.save("random.bmp");
 
    return 0;
}

