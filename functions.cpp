#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using namespace std;

void loadCorners(std::string filename, Corner imageCorners[MAX_CORNERS], unsigned int& numberOfCorners){
    // Read load corners file
    ifstream file;
    file.open(filename);
    if(!(file.is_open())){
        throw runtime_error("Failed to open " +filename);
    }
    for(int count = 0;count<MAX_CORNERS;count++){
       //if there is not two numbers being inputted it will break, 2 is needed
        if(!(file>>imageCorners[count].x && file>>imageCorners[count].y)){
            break;
        }
        numberOfCorners +=1;
    }
    file.close();
    // for extras
    if(numberOfCorners == MAX_CORNERS){
        throw runtime_error("Too many corners in "+filename);
    }
}

double errorCalculation(Pixel image1[][MAX_HEIGHT], const unsigned int width1, const unsigned int height1,
                      Corner image1corner,
                      Pixel image2[][MAX_HEIGHT], const unsigned int width2, const unsigned int height2,
                      Corner image2corner){
            
    // Error calculation
    const unsigned int ERROR_NEIGHBORHOOD_SIZE = 21;
    double error = 0.0;
    //cut the neighborhood in half cause you want it to be evenly distributed around the center
    int startx1 = image1corner.x -  ERROR_NEIGHBORHOOD_SIZE/2;
    int starty1 = image1corner.y -  ERROR_NEIGHBORHOOD_SIZE/2;
    int startx2 = image2corner.x -  ERROR_NEIGHBORHOOD_SIZE/2;
    int starty2 = image2corner.y -  ERROR_NEIGHBORHOOD_SIZE/2;

//goes through each box
    for(int horiz=0; horiz<ERROR_NEIGHBORHOOD_SIZE; horiz++){
        for(int vert=0; vert<ERROR_NEIGHBORHOOD_SIZE; vert++){
            if (startx1 + horiz >= 0 && startx1 + horiz < width1 && starty1 + vert >= 0 && starty1 + vert < height1 &&
                startx2 + horiz >= 0 && startx2 + horiz < width2 && starty2 + vert >= 0 && starty2 + vert < height2) {
                
                int diffR = image1[startx1 + horiz][starty1 + vert].r - image2[startx2 + horiz][starty2 + vert].r;
                int diffG = image1[startx1 + horiz][starty1 + vert].g - image2[startx2 + horiz][starty2 + vert].g;
                int diffB = image1[startx1 + horiz][starty1 + vert].b - image2[startx2 + horiz][starty2 + vert].b;
                int abR= abs(diffR);
                int abG = abs(diffG);
                int abB = abs(diffB);
                // Sum up the absolute differences for all color channels
                error += abR + abG + abB;
            }
            else{
                return INFINITY;
            }        
        }
    }
    return error;
}

void matchCorners(CornerPair matchedPairs[MAX_CORNERS], unsigned int &matched_count,
                  Pixel image1[][MAX_HEIGHT], const unsigned int width1, const unsigned int height1,
                  Corner image1corners[], unsigned int image1CornerCount,
                  Pixel image2[][MAX_HEIGHT], const unsigned int width2, const unsigned int height2,
                  Corner image2corners[], unsigned int image2CornerCount){

    // Greedy corner matching goes here
    bool used[MAX_CORNERS]= {false};
    for(unsigned int i=0; i<image1CornerCount; ++i){
        Corner MinCorErr; 
        double MinErr = INFINITY;
        int index = 0;
    
        for(unsigned int j=0; j<image2CornerCount; j++){
            if(used[j] ==false){
                double CurrErr= errorCalculation(image1,width1,height1,image1corners[i],image2,width2,height2,image2corners[j]);

                if(CurrErr < MinErr){
                    MinErr = CurrErr;
                    MinCorErr = image2corners[j];
                    index = j;
                }
            }
            
            
        }
        if(MinErr<INFINITY){
            matchedPairs[matched_count].image1Corner = image1corners[i];
            matchedPairs[matched_count].image2Corner = MinCorErr;
            matchedPairs[matched_count].error = MinErr;
            used[index]= true;
            // matched_count = 0;

            matched_count++;
        }
    }
}

void mapCoordinates(const double H[3][3], unsigned int x, unsigned int y,
                    double& mapped_x, double& mapped_y){

    // Mapping function for this week

    double xterm = H[0][0]*x + H[0][1]*y + H[0][2];
    double yterm = H[1][0]*x + H[1][1]*y + H[1][2];
    double zterm = H[2][0]*x +H[2][1]*y + H[2][2];

    mapped_x= xterm/zterm;
    mapped_y = yterm/zterm;


}

void mergeImages( Pixel image1[][MAX_HEIGHT], unsigned int &width1, unsigned int &height1,
                  Pixel image2[][MAX_HEIGHT], const unsigned int width2, const unsigned int height2,
                  double H[3][3] ){
        
    // Similar to image scaling function from last week with some additional steps.
    // width 1 needs to be equal to max width

   // temporary image1 
    Pixel new_pixel = {0,0,0};
    Pixel image1pix = {-1,0,0};
    Pixel image2pix = {-1,0,0};
    double mapped_x, mapped_y;


    // Looping buffer for image1
    for (unsigned int y = 0; y < MAX_HEIGHT; y++) {
        for (unsigned int x = 0; x < MAX_WIDTH; x++) {
            // use the matrix to map transformation 
            image1pix.r = -1;
            image2pix.r = -1;
            mapCoordinates(H, x, y, mapped_x, mapped_y);

            // are coordinates within the bounds of image2?
            if (((mapped_x<width2)&&(mapped_y<height2))&&(mapped_x>=0)&&(mapped_y>=0)) 
                //bilinear interpolation- pixel value from image2 
                image2pix = bilinear_interpolation(image2, width2, height2, mapped_x, mapped_y);
                
                // Get the pixel from image1
            if((x< width1) && (y< height1))
                image1pix = image1[x][y];
                
                    

                // Create the new pixel value as an average if both pixels are defined
                    
                //had to search up is defined- research more
            if (image1pix.r != -1 && image2pix.r != -1) 
                new_pixel.r = (image1pix.r + image2pix.r) / 2;
                new_pixel.g = (image1pix.g + image2pix.g) / 2;
                new_pixel.b = (image1pix.b + image2pix.b) / 2; 
                // If only pixel2 is defined
            if (image1pix.r != -1 && image2pix.r==-1) 
                new_pixel = image1pix;
                
                // If only pixel2 is defined
            if (image1pix.r ==-1 && image2pix.r != -1) 
                new_pixel = image2pix;
                
            if(image1pix.r ==-1 && image2pix.r == -1)
                new_pixel={0,0,0};
                

                // Update image1 buffer w/ new pixel
            image1[x][y] = new_pixel;
        }
            
    }
        
    

    // Copy the temporary image1 back to the original image1 and update its width and height
    width1 = MAX_WIDTH;
    height1 = MAX_HEIGHT;
}
