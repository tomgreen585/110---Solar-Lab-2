#include "image.hpp"
#include <vector>

void convolve(ImagePPM& outputImage, ImagePPM& inputImage, double kernel[3][3], int num){
	for(unsigned int col = 1; col<inputImage.width-1; col++){
		for(unsigned int row = 1; row<inputImage.height-1; row++){
			int result = 0;
			
			result = get_pixel(inputImage, row-1, col-1, 3) * kernel[0][0] +
					 get_pixel(inputImage, row-1, col, 3) * kernel[0][1] +
					 get_pixel(inputImage, row-1, col+1, 3) * kernel[0][2] +
					 get_pixel(inputImage, row, col-1, 3) * kernel[1][0] +
					 get_pixel(inputImage, row, col, 3) * kernel[1][1] +
					 get_pixel(inputImage, row, col+1, 3) * kernel[1][2] +
					 get_pixel(inputImage, row+1, col-1, 3) * kernel[2][0] +
					 get_pixel(inputImage, row+1, col, 3) * kernel[2][1] +
					 get_pixel(inputImage, row+1, col+1, 3) * kernel[2][2];
			
			if (result < 0) result = -result;
			if (result > 255) result = 255;
			unsigned char clamped = result;
			
			set_pixel(outputImage, row-1, col-1, clamped, clamped, clamped);
		}
	}	
}

void detectEdges(ImagePPM& outputImage){
	
	//kournals
	double kernelX [3][3];
	kernelX[0][0] = -1.0; kernelX[0][1] = 0.0; kernelX[0][2] = 1.0;
	kernelX[1][0] = -2.0; kernelX[1][1] = 0.0; kernelX[1][2] = 2.0;
	kernelX[2][0] = -1.0; kernelX[2][1] = 0.0; kernelX[2][2] = 1.0;
	double kernelY [3][3];
	kernelY[0][0] = 1.0; kernelY[0][1] = 2.0; kernelY[0][2] = 1.0;
	kernelY[1][0] = 0.0; kernelY[1][1] = 0.0; kernelY[1][2] = 0.0;
	kernelY[2][0] = -1.0; kernelY[2][1] = -2.0; kernelY[2][2] = -1.0;
	
	//outputs
	ImagePPM outputX;
	init_image(outputX, outputImage.width, outputImage.height);
	convolve(outputX, outputImage, kernelX, 1);
	SavePPMFile("xImg.ppm", outputX);
	ImagePPM outputY;
	init_image(outputY, outputImage.width, outputImage.height);
	convolve(outputY, outputImage, kernelY, 1);
	SavePPMFile("yImg.ppm", outputY);
	ImagePPM outputZ;
	init_image(outputZ, outputImage.width, outputImage.height);
	
	for(int i=4; i<outputZ.height - 4; i++){
		for(int j=4; j<outputZ.width - 4; j++){
			int c = (int) get_pixel(outputX, i, j, 3);
			int d = (int) get_pixel(outputY, i, j, 3);
			
			float formula = sqrt((c*c) + (d*d));
			if(formula>90) {
				set_pixel(outputZ, i, j, 255, 255, 255);
			} else {
				set_pixel(outputZ, i, j, 0, 0, 0);
			}
		}
	}
	SavePPMFile("zFinalImg.ppm", outputZ);
}

void findEdgeParameters(ImagePPM& outputImage){
	std::vector<int> outputX;
	std::vector<int> outputY;
	
	float a = 0;
	float b = 0;
	float xBar = 0;
	float yBar = 0;
	float sigmaXiYi = 0;
	float sigmaXi = 0;
	float sigmaXi2 = 0;

	for(unsigned int row = 4; row < outputImage.height-4; row++){
		for(unsigned int col = 4; col < outputImage.width-4; col ++){
			float G_x = -1.0 * get_pixel(outputImage,row-1,col-1,3) +
						0.0 * get_pixel(outputImage,row-1,col,3) +
						1.0 * get_pixel(outputImage,row-1,col+1,3) +
						-2.0 * get_pixel(outputImage,row,col-1,3) +
						0.0 * get_pixel(outputImage,row,col,3) +
						2.0 * get_pixel(outputImage,row,col+1,3) +
						-1.0 * get_pixel(outputImage,row+1,col-1,3) +
						0.0 * get_pixel(outputImage,row+1,col,3) +
						1.0 * get_pixel(outputImage,row+1,col+1,3);
						
			float G_y = -1.0 * get_pixel(outputImage,row-1,col-1,3) +
						-2.0 * get_pixel(outputImage,row-1,col,3) +
						-1.0 * get_pixel(outputImage,row-1,col+1,3) +
						0.0 * get_pixel(outputImage,row,col-1,3) +
						0.0 * get_pixel(outputImage,row,col,3) +
						0.0 * get_pixel(outputImage,row,col+1,3) +
						1.0 * get_pixel(outputImage,row+1,col-1,3) +
						2.0 * get_pixel(outputImage,row+1,col,3) +
						1.0 * get_pixel(outputImage,row+1,col+1,3);
						
			float G = sqrt(G_x * G_x + G_y * G_y);
			
			int threshold = 90;
			if (G >= threshold){
				outputX.push_back(col);
				outputY.push_back(row);
			} 
		}
	}

	for (int i = 0; i < outputX.size(); i++){
		float x = outputX.at(i);
		float y = outputY.at(i);

		xBar += x;
		yBar += y;
		sigmaXiYi += x * y;
		sigmaXi += x;
		sigmaXi2 += x * x;
	}	

	yBar = yBar/outputY.size();
	xBar = xBar/outputX.size();			
	a = (sigmaXiYi - yBar * sigmaXi) / (sigmaXi2 - xBar * sigmaXi);
	b = yBar - a * xBar;
							
	std::cout<<"Edge in the form of : "<<std::endl;
	std::cout<<"y = ax + b "<<std::endl;
	std::cout<<"y = "<<a<<"x + "<<b<<std::endl;						
}		

std::vector<int> printLuminosity(ImagePPM& image){
	std::vector<int> temp;
	for(unsigned int row = 0; row < image.height; row++){
		int a = (int) get_pixel(image, row, 320, 3);
		temp.push_back(a);
	}
	return temp;
}

int main() {
	//print out core
	ImagePPM image;
	OpenPPMFile("line.ppm", image);
	
	ImagePPM filt_image;
	init_image(filt_image, image.width, image.height);
	
	double kernel[3][3];
	kernel[0][0] = 1.0/9.0; kernel[0][1] = 1.0/9.0; kernel[0][2] = 1.0/9.0;
	kernel[1][0] = 1.0/9.0; kernel[1][1] = 1.0/9.0; kernel[1][2] = 1.0/9.0;
	kernel[2][0] = 1.0/9.0; kernel[2][1] = 1.0/9.0; kernel[2][2] = 1.0/9.0;
	
	convolve(filt_image, image, kernel, 1);
	//core
	SavePPMFile("filtered.ppm", filt_image);
	std::vector<int> originalLums = printLuminosity(image);
	std::vector<int> filteredLums = printLuminosity(filt_image);
	
	for(int a : originalLums){
		std::cout<<"original: "<<a<<std::endl;}
	
	for(int b : filteredLums){
		std::cout<<""<<b<<std::endl;}
	//completion
	detectEdges(filt_image);
	//challenge
	findEdgeParameters(filt_image);
}
