#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>

struct Pixel{
   unsigned char r,g,b;
   unsigned int row, col;   
};


struct ImagePPM{
	unsigned int width, height;
	int n_bytes;
	char* data;
};
 
void init_image(ImagePPM& image, int w, int h){
	image.width = w;
	image.height = h;
	image.n_bytes =  image.width*image.height*3;
    image.data = new char[image.n_bytes];
} 
 
// returns color component (color==0 -red,color==1-green,color==2-blue
// color == 3 - luminocity
// for pixel located at (row,column)
unsigned char get_pixel( ImagePPM image,unsigned int row,unsigned int col, int color)
{
    // calculate address in 1D array of pixels
    int address = image.width*row*3 + col*3;  // standard ppm
    //int address = image.width*row*4 + col*4;
    if ((row < 0 ) || (row > image.height) )  {
        printf("row is out of range\n");
        return -1;
    }
    if ( (col< 0) || (col > image.width))    {
        printf("column is out of range\n");
        return -1;
    }
    unsigned char r = image.data[address];
    unsigned char g = image.data[address+1];
    unsigned char b = image.data[address+2];
//std::cout<<"GP "<<(int)r<<" "<<(int)g<<" "<<(int)b<<std::endl;		          
    if (color==0) { return r;}
    if (color==1) { return g;}
    if (color==2) { return b;}
    if (color==3)    {
		int yi = r+g+b;
        unsigned char y = yi/3;
//std::cout<<"GP y="<<(int)y<<std::endl;		          
        
        return y;
    }
    printf("Color encoding wrong: 0-red, 1-green,2-blue,3 - luminosity\n");
    return -2; //error
}


// return pixel at these coordinates
Pixel get_pixel(  ImagePPM& image,const unsigned int row,const unsigned int col){
    Pixel p_out ={100,100,100,100,100};
    int address = image.width*row*3 + col*3;  // standard ppm
    //int address = image.width*row*4 + col*4;
    if ((row < 0 ) || (row > image.height) )  {
        printf("row is out of range\n");
        return p_out;
    }
    if ( (col< 0) || (col > image.width))    {
        printf("column is out of range\n");
        return p_out;
    }
    p_out.r = image.data[address];
    p_out.g = image.data[address+1];
    p_out.b = image.data[address+2];
   p_out.row = row;
   p_out.col = col;
   /*
    auto found = std::find_if(image.pixels.begin(), image.pixels.end(), 
                                            [row,col](Pixel p){
                                                std::cout<<"row="<<row<<" ";
                                                return ((row==p.row)&&(col==p.col));});
      if (found   !=  image.pixels.end()){    
          std::cout<<"pixel found"<<std::endl;          
          std::cout<<found->row; 
          p_out = *found;                   
      }
      */ 
       return p_out;
} 



int set_pixel(ImagePPM& image, int row, int col,
              unsigned char red,unsigned char green,unsigned char blue)
{
    //int address = image.width*row*4 + col*4; // RPI
    unsigned int address = image.width*row*3 + col*3;  // standard ppm
    if ((address < 0) || (address>image.width*image.height*3))
    {
        printf("SetPixel(): wrong x,y coordinates\n");
        printf(" row= %d col=%d address=%d\n",row,col,address);
        printf(" width= %d height=%d\n",image.width,image.height);
        return -1;
    }
    image.data[address] = red;
    image.data[address+1] = green;
    image.data[address+2] = blue;
    return 0;
}

int SavePPMFile(std::string filename, ImagePPM& image){
	std::ofstream fout;
	fout.open(filename,std::ofstream::binary);
	if (!fout.is_open()){ std::cout<<"no file"; return -1;}
	fout<<"P6"<<std::endl;
	fout<<(int)image.width<<" "<<(int)image.height<<" 255"<<std::endl;
	
	fout.write(image.data,image.n_bytes);
	fout.close();
	return 0;
}

int OpenPPMFile(std::string filename,ImagePPM& image){
	std::cout<<"Opening single ppm file"<<std::endl;
	
	std::ifstream fin;
	fin.open(filename);
	if (!fin.is_open()){ std::cout<<"no file"; return -1;}
    // get first line - contains P6
    std::string fd1;
    getline(fin,fd1);
    std::cout<<fd1<<std::endl; 
    // get secondline
    getline(fin,fd1);
    std::cout<<fd1<<std::endl; 
    int depth;
    sscanf( fd1.c_str(),"%d %d %d\n",&image.width ,&image.height,&depth );
    // size of the image
    // parse string
    std::cout<<"image.height = "<<image.height;
    std::cout<<" image.width = "<<image.width<<std::endl;
    
    image.n_bytes =  image.width*image.height*3;
    image.data = new char[image.n_bytes];
    fin.read(image.data, image.n_bytes);
    fin.close();
    return 0;
}
