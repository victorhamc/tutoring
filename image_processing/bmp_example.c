/*
 * bmp_reader.c
 *
 *  Created on: May 23, 2019
 *      Author: victorhc
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// https://stackoverflow.com/questions/2654480/writing-bmp-image-in-pure-c-c-without-other-libraries

int writeBmp(char *filename, int w, int h){

	int r, g, b, x, y, i, j;

	//unsigned char red[w][h];
	//unsigned char green[w][h];
	//unsigned char blue[w][h];

	// this will hold the image data
	unsigned char *img = NULL;

	//w for image width, h for image height
	// 54 is the header, 3 is the number of channels
	int filesize = 54 + 3*w*h;

	// image data
	img = (unsigned char *)malloc(3*w*h);
	memset(img,0,3*w*h);

	for(i = 0; i < w; i++){
		for(j = 0; j < h; j++){
			x=i; y=(h-1)-j;
			/*
			r = red[i][j]*255;
			g = green[i][j]*255;
			b = blue[i][j]*255;
			*/

			r = 0;
			g = 255;
			b = 255;

			/*
			if (r > 255) r=255;
			if (g > 255) g=255;
			if (b > 255) b=255;
			*/
			img[(x+y*w)*3+2] = (unsigned char)(r);
			img[(x+y*w)*3+1] = (unsigned char)(g);
			img[(x+y*w)*3+0] = (unsigned char)(b);
		}
	}

	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
	unsigned char bmppad[3] = {0,0,0};

	bmpfileheader[ 2] = (unsigned char)(filesize    );
	bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
	bmpfileheader[ 4] = (unsigned char)(filesize>>16);
	bmpfileheader[ 5] = (unsigned char)(filesize>>24);

	bmpinfoheader[ 4] = (unsigned char)(       w    );
	bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
	bmpinfoheader[ 6] = (unsigned char)(       w>>16);
	bmpinfoheader[ 7] = (unsigned char)(       w>>24);
	bmpinfoheader[ 8] = (unsigned char)(       h    );
	bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
	bmpinfoheader[10] = (unsigned char)(       h>>16);
	bmpinfoheader[11] = (unsigned char)(       h>>24);

	FILE *f = fopen(filename,"wb");

	if(f == NULL){
		printf("Can't write BMP\n");
		return -1;
	}

	fwrite(bmpfileheader,1,14,f);
	fwrite(bmpinfoheader,1,40,f);

	for(i = 0; i < h; i++){
		fwrite(img+(w*(h-i-1)*3),3,w,f);
		fwrite(bmppad,1,(4-(w*3)%4)%4,f);
	}

	fflush(f);
	fclose(f);

	free(img);

	return 0;
}



// static unsigned char *texels;
static int width, height;

static void readBmp(char *filename){
	printf("Opening file: %s\n", filename);

	int i, j;

	FILE *fd;
	fd = fopen(filename, "rb");
	if (fd == NULL)	{
		printf("Error: fopen failed\n");
		return;
	}

	unsigned char header[54];

	// Read header
	fread(header, sizeof(unsigned char), 54, fd);

	// Capture dimensions
	width = *(int*)&header[18];
	height = *(int*)&header[22];

	printf("width: %d header: %u\n", width, header[18]);
	printf("height: %d header: %u\n", height, header[22]);

	int padding = 0;

	// Calculate padding
	while ((width * 3 + padding) % 4 != 0){
		padding++;
	}

	printf("Padding: %d\n", padding);

	// Compute new width, which includes padding
	int widthnew = width * 3 + padding;

	printf("width new: %d\n", widthnew);

	// Allocate memory to store image data (non-padded)
	// texels = (unsigned char *)malloc(width * height * 3 * sizeof(unsigned char));
	/*
	if (texels == NULL){
		printf("Error: Malloc failed\n");
		return;
	}
	*/
	// Allocate temporary memory to read widthnew size of data
	unsigned char* data = (unsigned char *)malloc(widthnew * sizeof (unsigned int));

	// Read row by row of data and remove padded data.
	for (i = 0; i < height; i++){
		// Read widthnew length of data
		fread(data, sizeof(unsigned char), widthnew, fd);

		// Retain width length of data, and swizzle RB component.
		// BMP stores in BGR format, my usecase needs RGB format
		for (j = 0; j < width * 3; j += 3){
			// int index = (i * width * 3) + (j);
			printf("%u %u %u - ", data[j], data[j+1], data[j+2]);
			//texels[index + 0] = data[j + 2];
			//texels[index + 1] = data[j + 1];
			//texels[index + 2] = data[j + 0];
		}
		printf("\n");
	}

	free(data);
	// free(texels);
	fclose(fd);
}

int main(){
	readBmp("test.bmp");
	writeBmp("out.bmp", 200, 144);
	return 0;
}
