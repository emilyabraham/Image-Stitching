#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using namespace std;

// Copy code from last week for local run. Gradescope will use it's own copy

void loadImage(string filename, Pixel image[][MAX_HEIGHT], unsigned int& width, unsigned int& height) {
  // TODO: implement image loading
  // Reading file
  // TODO: implement image loading

//opening the file
  ifstream thefile;
  thefile.open(filename);
  if(!thefile.is_open()){
    throw runtime_error("Failed to open "+ filename);
  }

  string firstline;
  thefile>>firstline;
//checking to see if the firstline is the text below
  if((firstline == "P3")||(firstline == "p3")){
    thefile>>width;
    thefile>>height;
  }
  else{
    thefile.close();
    throw runtime_error("Invalid type " + firstline);
  }
  //when the width or height is not within restrictions
  if(!(width>0 && width<= MAX_WIDTH && height>0 && height<=MAX_HEIGHT)){
    thefile.close();
    throw runtime_error("Invalid dimensions");
  }
  //check pixels
  int maxpixval;
  thefile>>maxpixval;
  if(maxpixval != 255){
    thefile.close();
    throw runtime_error("Invalid color value");
  }

  for (unsigned int row = 0; row < height; ++row) {
    for (unsigned int col = 0; col < width; ++col) {
      int r, g, b;
      if (!(thefile >> r >> g >> b) || r < 0 || r >= 256 || g < 0 || g >= 256 || b < 0 || b >= 256) {
        thefile.close();
        throw runtime_error("Invalid color value");
      }
      
      image[col][row].r = r;
      image[col][row].g = g;
      image[col][row].b = b;
    }
  } 
  //too see if their is extra pixels
  int extra;
  if(thefile>>extra){
    thefile.close();
    throw runtime_error("Too many values");
  }
  thefile.close();
}


void outputImage(string filename, Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int height) {
  // TODO: implement writing image to file
  // TODO: implement writing image to file
  //to put into file you use ofstream
  ofstream thefile;
  //check to see if file is open, if not throw argument
  thefile.open(filename);
  if(!thefile.is_open()){
    throw invalid_argument("Failed to open " + filename);
  }
//file preambles
thefile<<"P3"<<endl;
thefile<<width<<" "<<height<<endl;
thefile<<"255"<<endl;

//pixel values
for (unsigned int row = 0; row < height; ++row) {
    for (unsigned int col = 0; col < width; ++col) {
      thefile<< image[col][row].r <<" ";
      thefile<<image[col][row].g<<" ";
      thefile<<image[col][row].b<< " ";
    }
}
thefile.close();
}


Pixel bilinear_interpolation(Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int height, double x, double y) {
  width=width;
  height=height;
  int floorX= floor(x);

  int ceilX= ceil(x);
  if(ceil(x) ==x){
    ceilX = ceil(x) +1;
  }
  

  int floorY= floor(y);
  int ceilY= ceil(y);
  if(ceil(y)==y){
    ceilY = ceil(y) +1;
  }


  Pixel afterpixel;
  // create a loop to get red, green, blue
 
  double fractionX = x- floorX; 
  double fractionY= y-floorY;

 // // Interpolate the red component

  afterpixel.r = static_cast<int>(round((fractionY)*((fractionX)*image[ceilX][ceilY].r+(ceilX-x)*image[floorX][ceilY].r)
  +(ceilY-y)*((fractionX)*image[ceilX][floorY].r+(ceilX-x)*image[floorX][floorY].r)));
 // Interpolate the green component
  afterpixel.g = static_cast<int>(round((fractionY)*((fractionX)*image[ceilX][ceilY].g+(ceilX-x)*image[floorX][ceilY].g)
  +(ceilY-y)*((fractionX)*image[ceilX][floorY].g+(ceilX-x)*image[floorX][floorY].g)));
  // // Interpolate the blue component

  afterpixel.b = static_cast<int>(round((fractionY)*((fractionX)*image[ceilX][ceilY].b+(ceilX-x)*image[floorX][ceilY].b)
  +(ceilY-y)*((fractionX)*image[ceilX][floorY].b+(ceilX-x)*image[floorX][floorY].b)));
 

return afterpixel;

}







