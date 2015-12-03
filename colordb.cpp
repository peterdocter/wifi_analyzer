#include <stdio.h>
#include <stdlib.h>
#include <tiffio.h>
#include <stdint.h>
#include <string.h> 
#include <math.h>
#include <cfloat>



        
int main(){
	uint32 width, height;
	TIFF* tiff = TIFFOpen("color_to_db.tif", "w");
	int sampleperpixel = 4;    // or 3 if there is no alpha channel, you should get a understanding of alpha in class soon.
	width = 50;
	height = 500;
	uint32 npixels=width*height;
	uint32 *raster=(uint32 *) _TIFFmalloc(npixels *sizeof(uint32));
	TIFFSetField (tiff, TIFFTAG_IMAGEWIDTH, width);  // set the width of the image
	TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, height);    // set the height of the image
	TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, sampleperpixel);   // set number of channels per pixel
	TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 8);    // set the size of the channels
	TIFFSetField(tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);    // set the origin of the image.
	//   Some other essential fields to set that you do not have to understand for now.
	TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
	tsize_t linebytes = sampleperpixel * width;     // length in memory of one row of pixel in the image.
	unsigned char *buf = NULL;        // buffer used to store the row of pixel information for writing to file
	//    Allocating memory to store the pixels of current row
	if (TIFFScanlineSize(tiff)==linebytes)
		buf =(unsigned char *)_TIFFmalloc(linebytes);
	else
		buf = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(tiff));

	// We set the strip size of the file to be size of one row of pixels
	TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tiff, width*sampleperpixel));

	//Now writing image to the file one strip at a time
	int row;
	int i;
	int j;
	int k;
	char *cr = (char *)raster;
	
	int step = 8;
	int r,g,b;
    r=0;
    g=0;
    b=255;
	int count=0;
	//clear_raster((char*)raster, width, height);
	for(i=0;i<height;i++){
		for(j=0;j<width;j++){
			cr[i*width*4+j*4  ]=r;
			cr[i*width*4+j*4+1]=g;
			cr[i*width*4+j*4+2]=b;
			cr[i*width*4+j*4+3]=255;
		}
		count++;
		if(count%(height/100)==0){
			count=0;
	
			if(b>0) {
				b-=step;
				if(b<0) b=0;
				g+=step;
				if(g>255 || b==0) g=255;
			}else
			if(b==0 && g==255 && r!=255){
				r+=step;
				if(r>255) r=255;
			}else
			if(b==0 && g<=255 && r==255){
				g-=step;
				if(g<0) g=0;
			}
		}
	}

	for (row = 0; row < height; row++){
		if (TIFFWriteScanline(tiff, (uint32 *)((unsigned long)raster + row*linebytes), row, 0) < 0)
		break;
	}
	_TIFFfree(buf);
	_TIFFfree(raster);
	TIFFClose(tiff);
	return 0;
}
