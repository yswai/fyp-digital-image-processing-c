//#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#include <png.h>
#include <time.h>

#define N2 3
#define N 5
#define max_dimension 512

// --------Function Prototypes --------

void create_histogram(void);
void histogram_equalization(void);
void histogram_specify(void);
void contrast_stretching(void);
void discrete_conv(float * mask);
void dilation(void);
void erosion(void);
void rgb2gray(void);
void brightness(int brightnessadj);
void contrast_correction(int contrastadj);
void flip_vertical(void);
void flip_horizontal(void);
void zoom(int x, int y, int hor, int ver, unsigned char zd);
void opening(void);
void closing(void);
void weighted_mean(void);
void weighted_median(void);

void print_help(void);

// -------------------------------------------

void abort_(const char * s, ...)
{
	va_list args;
	va_start(args, s);
	vfprintf(stderr, s, args);
	fprintf(stderr, "\n");
	printf(s,args);
	printf("\n");
	va_end(args);
	abort();
}

void print_help()
{
		printf("Usage: program_name [options] <file_in> <file_out>\n");
		printf("[options] :\n");	
		printf("\t-erosion\n");
		printf("\t-dilaton\n");
		printf("\t-opening\n");
		printf("\t-closing\n");
		printf("\t-histeq\n");
		printf("\t-sharpen\n");
		printf("\t-flip_h\n");
		printf("\t-flip_v\n");
		printf("\t-weightedavg\n");
		printf("\t-weightedmean\n");
		printf("\t-contrasts\t\tContrast stretching\n");
		printf("\t-stdavg\n");
		printf("\t-gaussianblur\n");
		printf("\t-lapacian\n");
		printf("\t-linedet\n");
		printf("\t-edgedet\n");
		printf("\t-gradient [n|w|e|s]\t:north, west, east, south\n");
		printf("\t-prewitt [h|v]\t\t:horizontal, vertical\n");
		printf("\t-rgb2gray\n");
		printf("\t-brightness [-100 to 100] Percentage of brightness adjustment\n");
		printf("\t-contrastc [-100 to 100] Percentage of contrast correction\n");
}

png_uint_32 width, height, rowbytes, bit_depth;
png_byte color_type;

png_structp png_ptr;
png_infop info_ptr;
int number_of_passes;

png_bytep* read_row_pointers;
png_bytep* write_row_pointers;

int outfile_arg;

png_byte * image_data;			// 1-D image data chunck pointer
png_byte * image_data_copy;		// 1-D image data chunck copy pointer

float HIST[256], SPEC[256];		// HIST = histogram, SPEC = Specified Histogram

int MORPH_MASK[N][N];

char option[256];
char option_gradient[5];
char option_prewitt[5];


// --------------------------------------------------READ PNG FILE -------------------------------------------------------
void read_png_file(char* file_name)
{
	char header[8];	// 8 is the maximum size that can be checked

	/* open file and test for it being a png */
	FILE *fp = fopen(file_name, "rb");
	if (!fp)
		abort_("[read_png_file] File %s could not be opened for reading", file_name);
	fread(header, 1, 8, fp);
	if (png_sig_cmp(header, 0, 8))
		abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);


	/* initialize stuff */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	if (!png_ptr)
		abort_("[read_png_file] png_create_read_struct failed");

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		abort_("[read_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[read_png_file] Error during init_io");

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	width = info_ptr->width;
	height = info_ptr->height;
	color_type = info_ptr->color_type;
	bit_depth = info_ptr->bit_depth;
	rowbytes = info_ptr->rowbytes;

	printf("Width \t\t= \t%d\n", width);
	printf("Height \t\t= \t%d\n", height);
	
	if((width>max_dimension) || (height>max_dimension))
		abort_("[PNG read Error] The image dimension must be less than 512 x 512");
	
	printf("Color type \t= \t%d\n",color_type);
	printf("Rowbytes \t= \t%d\n", rowbytes);
	printf("Bit Depth \t= \t%d\n", bit_depth);
	
	if( bit_depth != 8)	 abort_("[Err] This program only supports PNG images with 8-bits per pixel.\n");
	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);


	/* read file */
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[read_png_file] Error during read_image");

	image_data = (png_byte*) malloc(sizeof(unsigned char)*rowbytes * height);
	read_row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
	write_row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);

	int j;

	 for (j=0; j<height; j++)
	{
		read_row_pointers[j] = image_data + j*rowbytes;
	}

	png_read_image(png_ptr, read_row_pointers);

        fclose(fp);
}


// ---------------------------------------------------------- WRITE PNG FILE --------------------------------------------------------------
void write_png_file(char* file_name)
{
	/* create file */
	FILE *fp = fopen(file_name, "wb");
	if (!fp)
		abort_("[write_png_file] File %s could not be opened for writing", file_name);


	/* initialize stuff */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	if (!png_ptr)
		abort_("[write_png_file] png_create_write_struct failed");

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		abort_("[write_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during init_io");

	png_init_io(png_ptr, fp);


	/* write header */
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during writing header");

	png_set_IHDR(png_ptr, info_ptr, width, height,
		     bit_depth, color_type, PNG_INTERLACE_NONE,
		     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);


	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during writing bytes");

	png_write_image(png_ptr, write_row_pointers);


	/* end write */
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during end of write");

	png_write_end(png_ptr, NULL);

        /* cleanup heap allocation */
	
	free(write_row_pointers); 
	free(read_row_pointers);
	free(image_data);

        fclose(fp);
}
// ------------------------------------------------------------ END OF WRITE PNG FILE -----------------------------------------------



int main(int argc, char **argv)
{

	int c;
	if ((argc<4) || (argc>5))
	{
		print_help();
		exit(1);
	}

	clock_t start = clock();
	
	if(strcmp(argv[1], "-erosion")==0)
	{
		if(argc==5)
			abort_("[Error] Too many arguments.");
		read_png_file(argv[2]);
		erosion();
		outfile_arg=3;
	} else if(strcmp(argv[1], "-dilation")==0)
	{
		if(argc==5)
			abort_("[Error] Too many arguments.");
		read_png_file(argv[2]);
		dilation();
		outfile_arg=3;
	} else if(strcmp(argv[1], "-histeq")==0)
	{
		if(argc==5)
			abort_("[Error] Too many arguments.");
		read_png_file(argv[2]);
		create_histogram();
		histogram_equalization();
		outfile_arg=3;
	} else if(strcmp(argv[1], "-contrasts")==0)
	{
		if(argc==5)
			abort_("[Error] Too many arguments.");
		read_png_file(argv[2]);
		create_histogram();
		contrast_stretching();
		outfile_arg=3;
	} else if(strcmp(argv[1], "-sharpen")==0)
	{
		if(argc==5)
			abort_("[Error] Too many arguments.");
		read_png_file(argv[2]);
		float mask[9] = {1./9.,-8./9.,1./9.,-8./9.,37./9.,-8./9.,1./9.,-8./9.,1./9.};
		discrete_conv(mask);
		outfile_arg=3;
	} else if(strcmp(argv[1], "-stdavg")==0)
	{
		if(argc==5)
			abort_("[Error] Too many arguments.");
		read_png_file(argv[2]);
		float mask[9] = {1./9.,1./9.,1./9.,1./9.,1./9.,1./9.,1./9.,1./9.,1./9.};
		discrete_conv(mask);
		outfile_arg=3;
	} else if(strcmp(argv[1], "-weightedavg")==0)
	{
		if(argc==5)
			abort_("[Error] Too many arguments.");
		read_png_file(argv[2]);
		float mask[9] = {1./16.,2./16.,1./16.,2./16.,4./16.,2./16.,1./16.,2./16.,1./16.};
		discrete_conv(mask);
		outfile_arg=3;
	}else if(strcmp(argv[1], "-weightedmean")==0)
	{
		if(argc==5)
			abort_("[Error] Too many arguments.");
		read_png_file(argv[2]);
		weighted_mean();
		outfile_arg=3;
	}  else if(strcmp(argv[1], "-weightedmedian")==0)
	{
		if(argc==5)
			abort_("[Error] Too many arguments.");
		read_png_file(argv[2]);
		weighted_median();
		outfile_arg=3;
	} else if(strcmp(argv[1], "-gaussianblur")==0)	
	{
		if(argc==5)
			abort_("[Error] Too many arguments.");
		read_png_file(argv[2]);
		float mask[9] = {1./15.,2./15.,1./15.,2./15.,3./15.,2./15.,1./15.,2./15.,1./15.};
		discrete_conv(mask);
		outfile_arg=3;
	} else if(strcmp(argv[1], "-laplacian")==0)
	{
		if(argc==5)
			abort_("[Error] Too many arguments.");
		read_png_file(argv[2]);
		float mask[9] = {-1,-1,-1,-1,8,-1,-1,-1,-1};
		discrete_conv(mask);
		outfile_arg=3;
	} else if(strcmp(argv[1], "-linedet")==0)
	{
		if(argc==5)
			abort_("[Error] Too many arguments.");
		float mask[9] = {-1,-1,-1,2,2,2,-1,-1,-1};
		read_png_file(argv[2]);
		discrete_conv(mask);
		outfile_arg=3;
	} else if(strcmp(argv[1], "-edgedet")==0)
	{
		if(argc==5)
			abort_("[Error] Too many arguments.");
		read_png_file(argv[2]);
		float mask[9] = {-2,0,2,-1,0,1,-2,0,2};
		discrete_conv(mask);
		outfile_arg=3;
	} else if(strcmp(argv[1], "-rgb2gray")==0)
	{
		if(argc==5)
			abort_("[Error] Too many arguments.");
		read_png_file(argv[2]);
 		rgb2gray();
		outfile_arg=3;
	} else if(strcmp(argv[1], "-opening")==0)
	{
		if(argc==5)
			abort_("[Error] Too many arguments.");
		read_png_file(argv[2]);
 		opening();
		outfile_arg=3;
	} else if(strcmp(argv[1], "-fliph")==0)
	{
		if(argc==5)
			abort_("[Error] Too many arguments.");
		read_png_file(argv[2]);
 		flip_horizontal();
		outfile_arg=3;
	} else if(strcmp(argv[1], "-flipv")==0)
	{
		if(argc==5)
			abort_("[Error] Too many arguments.");
		read_png_file(argv[2]);
 		flip_vertical();
		outfile_arg=3;
	} else if(strcmp(argv[1], "-closing")==0)
	{
		if(argc==5)
			abort_("[Error] Too many arguments.");
		read_png_file(argv[2]);
 		closing();
		outfile_arg=3;
	}else if(strcmp(argv[1], "-brightness")==0)
	{
		if(argc!=5)
			abort_("Invalid Option: -brightness [value]");
		outfile_arg=4;
		float mask[9];
		read_png_file(argv[3]);
		int bvalue;
		bvalue = atoi(argv[2]);
		printf("Brightness adjustment of %d\n", bvalue);
		if((bvalue<=100) && (bvalue>=-100))
		{
			brightness(bvalue);	
		} else {
			abort_("Invalid option: -brightness [value]");
		}

	} else if(strcmp(argv[1], "-contrastc")==0)
	{
		if(argc!=5)
			abort_("Invalid Option: -contrastc [value]");
		outfile_arg=4;
		float mask[9];
		read_png_file(argv[3]);
		int cvalue;
		cvalue = atoi(argv[2]);
		printf("Contrast adjustment of %d\n", cvalue);
		if((cvalue<=100) && (cvalue>=-100))
		{
			contrast_correction(cvalue);	
		} else {
			abort_("Invalid option: -contrastc [value]");
		}

	}else if(strcmp(argv[1], "-gradient")==0)
	{
		if(argc!=5)
			abort_("Invalid Option: -gradient [n|e|s|w]");
		outfile_arg=4;
		float mask[9];
		read_png_file(argv[3]);
		if(strcmp(argv[2],"n")==0)
		{
			float mask_init[9]= {-1,-2,-1,0,0,0,1,2,1};
			for(c=0 ; c<9 ; c++)
				mask[c]=mask_init[c];
		} else if(strcmp(argv[2],"w")==0)
		{
			float mask_init[9]= {-1,0,1,-2,0,2,-1,0,1};
			for(c=0 ; c<9 ; c++)
				mask[c]=mask_init[c];
		} else if(strcmp(argv[2],"e")==0)	
		{
			float mask_init[9]= {1,0,-1,2,0,-2,1,0,-1};
			for(c=0 ; c<9 ; c++)
				mask[c]=mask_init[c];
		} else if(strcmp(argv[2],"s")==0)
		{
			float mask_init[9]= {1,2,1,0,0,0,-1,-2,-1};
			for(c=0 ; c<9 ; c++)
				mask[c]=mask_init[c];
		} else {
			abort_("Invalid Option: -gradient [n|e|s|w]");	
		}
 		discrete_conv(mask);
	} else if(strcmp(argv[1], "-prewitt")==0)
	{
		if(argc!=5)
			abort_("Invalid Option: -prewitt [h|v]");
		outfile_arg=4;
		float mask[9];
		read_png_file(argv[3]);
		if(strcmp(argv[2],"v")==0)
		{
			float mask_init[9] = {-1,0,1,-1,0,1,-1,0,1};
			for(c=0 ; c<9 ; c++)
				mask[c]=mask_init[c];
		} else if(strcmp(argv[2],"h")==0)
		{
			float mask_init[9] = {-1,-1,-1,0,0,0,1,1,1};
			for(c=0 ; c<9 ; c++)
				mask[c]=mask_init[c];
		} else {
			abort_("Invalid option: -prewitt [h|v]");
		}
		discrete_conv(mask);
	}else {
		print_help();
		abort_("");
	}

	write_png_file(argv[outfile_arg]);

	printf("Execution Time = %.3f s\n", (float)(clock()-start)/(float)CLOCKS_PER_SEC);

        return 0;
}

// ------------------------------------------------- HISTOGRAM OPERATIONS -----------------------------------------------------

void create_histogram() 
{

	if( info_ptr->color_type == PNG_COLOR_TYPE_PALETTE )
	{
		abort_("[process file] This program does not support paletted color images\n");
	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_GRAY)
	{	
		int X, Y, I, J;
		long int IHIST[256], SUM;
		
		for(I=0; I<=255 ; I++)
		{
			IHIST[I]=0;
		}
		
		SUM = 0;
	
		for(Y=0 ; Y<height ; Y++)
		{
			for(X=0 ; X<rowbytes ; X++)
			{
				J = *(image_data + (long)Y*rowbytes + X);
				IHIST[J] = IHIST[J] + 1;
				SUM = SUM+1;
			}
		}

		for(I=0 ; I<=255 ; I++)
		{
			HIST[I] = ((float) IHIST[I] ) / ((float)SUM);
		}
	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB)						// RGB Image
	{
		printf("[process file] Image is RGB with depth = %d\n", bit_depth);
		abort_("Does not support RGB Images\n");

	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA )						// RGB Image
	{
		printf("[process file] Image is RGBA with depth = %d\n", bit_depth);
		abort_("Does not support RGBA Images\n");
	}


}

void histogram_equalization()
{

	if( info_ptr->color_type == PNG_COLOR_TYPE_PALETTE )
	{
		abort_("[process file] This program does not support paletted color images\n");
	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_GRAY)	
	{
		int X, Y, I, J;
		int HISTEQ[256];
		float SUM_EQ;
		for(I=0 ; I<=255 ; I++)
		{
			SUM_EQ = 0.0;
			for(J=0 ; J<=I ; J++)
				SUM_EQ = SUM_EQ + HIST[J];
			HISTEQ[I] = (int)(255*SUM_EQ+.5);	
		}
		for( Y=0 ; Y<height ; Y++)
		{
			for( X=0 ; X<rowbytes ; X++)
			{
				*(image_data+X+(long)Y*width)=HISTEQ[*(image_data+X+(long)Y*width)];
			}
		}


	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB)						// RGB Image
	{
		printf("[process file] Image is RGB with depth = %d\n", bit_depth);
		abort_("Does not support RGB Images\n");

	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA )						// RGB Image
	{
		printf("[process file] Image is RGBA with depth = %d\n", bit_depth);
		abort_("Does not support RGBA Images\n");
	}


	int j;
	for (j=0; j<height; j++)
	{
		write_row_pointers[j] = image_data + j*rowbytes;
	}
}

void histogram_specify()
{
	if( info_ptr->color_type == PNG_COLOR_TYPE_PALETTE )
	{
		abort_("[process file] This program does not support paletted color images\n");
	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_GRAY)	
	{
	int X,Y,I, minval, minj;
	int HISTSPEC[256], J;
	int InvHist[256];
	float SUM;
	// histogram_equalization();
	memcpy(SPEC,HIST,256);
	for(I=0 ; I<=255 ; I++)
	{
		SUM=0.0;
		for(J=0 ; J<=I; J++)
			SUM = SUM + SPEC[J];
		HISTSPEC[I] = (int) (255*SUM+.5);
	}
	for(I=0 ; I<=255 ;I++)
	{
		minval = abs(I-HISTSPEC[0]);
		minj=0;
		for(J=0 ; J<=255 ; J++)
		{
			if(abs(I - HISTSPEC[J])<minval)
			{
				minval = abs(I - HISTSPEC[J]);
				minj = J;
			}
			InvHist[I] = minj;
		}
	}
	for(Y=0 ; Y<height ; Y++)
		for(X=0 ; X<rowbytes ; X++)
			*(image_data + X + (long)Y*rowbytes) = InvHist[*(image_data + X + (long)Y*rowbytes)];
	}
}

void contrast_stretching()
{
	if( info_ptr->color_type == PNG_COLOR_TYPE_PALETTE )
	{
		abort_("[process file] This program does not support paletted color images\n");
	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_GRAY)
 	{	
		int X, Y, I, J;
		int highthresh, lowthresh;
		float scale_factor;
		long totalpixels;
		unsigned char LUT[256];

		lowthresh = 255;
		highthresh = 0;

		// Compute low and high threshold
		for(I=0 ; I<256 ; I++)
		{
			if(HIST[I])
			{
				lowthresh = I;
				break;
			}
		}
	
		for(I=255 ; I>=0 ; I--)
		{
			if(HIST[I])
			{
				highthresh = I;
				break;
			}
		}

		printf("\nLow threshold is %d \nHigh threshold is %d.\n", lowthresh, highthresh);

		// Compute new LUT
		for(I=0 ; I<lowthresh ; I++)
			LUT[I]=0;
	
		for(I=255 ; I>highthresh ; I--)
			LUT[I] = 255;

		scale_factor = 255.0 / (highthresh - lowthresh);
		for(I=lowthresh ; I<=highthresh ; I++)
			LUT[I] = (unsigned char) ((I-lowthresh) * scale_factor);

		for( Y=0 ; Y<height ; Y++)
		{
			for( X=0 ; X<rowbytes ; X++)
			{
				*(image_data+X+(long)Y*width)=LUT[*(image_data+X+(long)Y*width)];
			}
		}
	
	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB)						// RGB Image
	{
		printf("[process file] Image is RGB with depth = %d\n", bit_depth);
		abort_("Does not support RGB Images\n");

	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA )						// RGB Image
	{
		printf("[process file] Image is RGBA with depth = %d\n", bit_depth);
		abort_("Does not support RGBA Images\n");
	}

	int j;
	for (j=0; j<height; j++)
	{
		write_row_pointers[j] = image_data + j*rowbytes;
	}
}

// -------------------------------------------------END OF HISTOGRAM OPERATIONS -----------------------------------------------------


// -------------------------------------------------- DISCRETE CONVOLUTION --------------------------------------------------------------
void discrete_conv(float* mask)
{
	int i, j, m, n, idx, jdx;
	float ms, im, val;
	unsigned char* tmp;

	image_data_copy = malloc(sizeof(png_byte)*rowbytes*height);

	if( info_ptr->color_type == PNG_COLOR_TYPE_PALETTE )
	{
		abort_("[process file] This program does not support paletted color images\n");
	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_GRAY)					
	{
		printf("[process file] Image is grayscale with depth = %d\n", bit_depth);		// Grayscale Image
		memcpy(image_data_copy, image_data, sizeof(png_byte) * width * height);
	
		for(i=0 ; i<height ; ++i)
		{

			for(j=0 ; j<width ; ++j)
			{
				val = 0;
				for(m=0 ; m<3 ; ++m)
				{
					for(n=0 ; n<3 ; ++n)
					{
						ms = *(mask + m*3 + n);
						idx = i-m;
						jdx = j-n;
						if((idx>=0) && (jdx>=0))
							im = *(image_data_copy + idx*width + jdx);
							val += (ms * im);
					}
				}
				if(val>255) val = 255;
				if(val<0) val = 0;
				tmp = image_data + i*width + j;
				*tmp = (unsigned char)val;
			}
		}
		printf("[msg] completed.\n");
	}


	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB)						// RGB Image
	{

		printf("[process file] Image is RGB with depth = %d\n", bit_depth);
		memcpy(image_data_copy, image_data, sizeof(png_byte) * height * rowbytes);
	
		for(i=0 ; i<height ; ++i)
		{

			for(j=0 ; j<rowbytes ; ++j)
			{
				val = 0;
				for(m=0 ; m<3 ; ++m)
				{
					for(n=0 ; n<3 ; ++n)
					{
						ms = *(mask + m*3 + n);
						idx = i-m;
						jdx = j-n;
						if((idx>=0) && (jdx>=0))
							im = *(image_data_copy + idx*rowbytes + (jdx*3));
							val += (ms * im);
					}
				}
				if(val>255) val = 255;
				if(val<0) val = 0;
				tmp = image_data + i*rowbytes + j*3;
				*tmp = (unsigned char)val;
			}
		}
		printf("[msg] completed.\n");
	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA )						// RGB Image
	{
		printf("[process file] Image is RGBA with depth = %d\n", bit_depth);
		memcpy(image_data_copy, image_data, sizeof(png_byte) * height * rowbytes);
	
		for(i=0 ; i<height ; ++i)
		{

			for(j=0 ; j<rowbytes ; ++j)
			{
				val = 0;
				for(m=0 ; m<3 ; ++m)
				{
					for(n=0 ; n<3 ; ++n)
					{
						ms = *(mask + m*3 + n);
						idx = i-m;
						jdx = j-n;
						if((idx>=0) && (jdx>=0))
							im = *(image_data_copy + idx*rowbytes + (jdx*4));
							val += (ms * im);
					}
				}
				if(val>255) val = 255;
				if(val<0) val = 0;
				tmp = image_data + i*rowbytes + j*4;
				*tmp = (unsigned char)val;
			}
		}
		printf("[msg] completed.\n");
	}

	free(image_data_copy);


	for (j=0; j<height; j++)
	{
		write_row_pointers[j] = image_data + j*rowbytes;
	}
}
// ------------------------------------------------------END OF DISCRETE CONVOLUTION ----------------------------------------------------

// -------------------------------------------------------- MORPHOLOGY OPERATIONS ---------------------------------------------------------
void erosion()
{
	image_data_copy = (png_byte*) malloc( sizeof(png_byte)* rowbytes * height);

	if( info_ptr->color_type == PNG_COLOR_TYPE_PALETTE )
	{
		abort_("[process file] This program does not support paletted color images\n");
	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_GRAY)					
	{
		printf("[process file] Image is grayscale with depth = %d\n", bit_depth);		// Grayscale Image

		memcpy(image_data_copy, image_data, sizeof(png_byte)*rowbytes*height);
		
		int a[N][N];
		int X, Y, I, J, smin;
		for(Y=N/2 ; Y<(height -N/2); Y++)
		{
			for(X=N/2 ; X<(rowbytes - N/2); X++)
			{
				smin = 255;
				for(J=-N/2 ; J<=N/2 ; J++)
				{
					for(I = -N/2 ; I<=N/2 ; I++)
					{
						a[I+N/2][J+N/2]=*(image_data_copy+X+I+(long)(Y+J)*rowbytes)-MORPH_MASK[I+N/2][J+N/2];
					}
				}
				for(J=-N/2; J<=N/2 ; J++)
				{
					for(I = -N/2 ; I<=N/2; I++)
					{
						if(a[I+N/2][J+N/2]<smin)
							smin=a[I+N/2][J+N/2];
					}
				}
				if(smin < 0)
					smin =0;
				*(image_data+X+Y*rowbytes) = smin;
			}
		}	
		printf("Completed\n");
	}


	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB)						// RGB Image
	{
		printf("[process file] Image is RGB with depth = %d\n", bit_depth);
		abort_("Does not support RGB Images\n");

	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA )						// RGB Image
	{
		printf("[process file] Image is RGBA with depth = %d\n", bit_depth);
		abort_("Does not support RGBA Images\n");
	}

	int j;
	for (j=0; j<height; j++)
	{
		write_row_pointers[j] = image_data + j*rowbytes;
	}
	printf("row pointers\n");
}


void dilation()
{
	image_data_copy = (png_byte*) malloc( sizeof(png_byte)* rowbytes * height);

	if( info_ptr->color_type == PNG_COLOR_TYPE_PALETTE )
	{
		abort_("[process file] This program does not support paletted color images\n");
	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_GRAY)					
	{
		printf("[process file] Image is grayscale with depth = %d\n", bit_depth);		// Grayscale Image
		memcpy(image_data_copy, image_data, sizeof(png_byte)*rowbytes*height);
		
		int a[N][N];
		int X, Y, I, J, smax;
		for(Y=N/2 ; Y<(height -N/2); Y++)
		{
			for(X=N/2 ; X<(rowbytes - N/2); X++)
			{
				smax = 0;
				for(J=-N/2 ; J<=N/2 ; J++)
				{
					for(I = -N/2 ; I<=N/2 ; I++)
					{
						a[I+N/2][J+N/2]=*(image_data_copy+X+I+(long)(Y+J)*rowbytes)+MORPH_MASK[I+N/2][J+N/2];
					}
				}
				for(J=-N/2; J<=N/2 ; J++)
				{
					for(I = -N/2 ; I<=N/2; I++)
					{
						if(a[I+N/2][J+N/2] > smax)
							smax=a[I+N/2][J+N/2];
					}
				}
				if(smax > 255)
					smax = 255;
				*(image_data+X+Y*rowbytes) = smax;
			}
		}	
	}


	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB)						// RGB Image
	{
		printf("[process file] Image is RGB with depth = %d\n", bit_depth);
		abort_("Does not support RGB Images\n");

		/*int a[N][N];
		int X, Y, I, J, smax;
		for(Y=3*(N/2) ; Y<(height - (N/2)*3); Y++)
		{
			for(X=3*(N/2) ; X<(rowbytes - (N/2)*3); X++)
			{
				smax = 0;
				for(J=-N/2 ; J<=N/2 ; J++)
				{
					for(I = -N/2 ; I<=N/2 ; I++)
					{
						a[I+N/2][J+N/2]=*(image_data_copy+X+(I*3)+(long)(Y+J)*rowbytes)+MORPH_MASK[I+N/2][J+N/2];
					}
				}
				for(J=-N/2; J<=N/2 ; J++)
				{
					for(I = -N/2 ; I<=N/2; I++)
					{
						if(a[I+N/2][J+N/2] > smax)
							smax=a[I+N/2][J+N/2];
					}
				}
				if(smax > 255)
					smax = 255;
				*(image_data+X+Y*rowbytes) = smax;
			}
		}	*/
	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA )						// RGBA Image
	{
		printf("[process file] Image is RGBA with depth = %d\n", bit_depth);
		abort_("Does not support RGBA Images\n");
	}

	int j;
	for (j=0; j<height; j++)
	{
		write_row_pointers[j] = read_row_pointers[j];
	}
}

void opening()
{
	erosion();
	dilation();
}

void closing()
{
	dilation();
	erosion();
}

// ----------------------------------------- END OF MORPHOLOGY OPERATIONS ----------------------------------------------

// -------------------------------------------IMAGE DATA TYPE CONVERSION---------------------------------------------------

void rgb2gray() {

	int x,y,j;
	float sum;
	image_data_copy = malloc( sizeof(png_byte) * width * height);
	float red, green, blue;

	if( info_ptr->color_type == PNG_COLOR_TYPE_PALETTE )
		abort_("[RGB2Grayscale] This program does not support paletted color images\n");

	if( info_ptr->color_type == PNG_COLOR_TYPE_GRAY)					
		abort_("[RGB2Grayscale] Not RGB color type.\n");

	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB)							// RGB Image
	{
		printf("[process file] Image is RGB with depth = %d\n", bit_depth);
		for( y=0 ; y<height ; y++)
		{
			for( x=0 ; x<width; x++) 
			{	
				sum = 0;
				red = *(image_data+3*x+y*rowbytes);
				green = *(image_data+3*x+1+y*rowbytes);
				blue = *(image_data+3*x+2+y*rowbytes);
				sum = red*0.3 + green*0.59 + blue*0.11;
				if(sum>255) 
					sum=255;

				*(image_data_copy + (long)y*width + x) = (unsigned char) (int) sum;
			}
		}
			
		color_type = PNG_COLOR_TYPE_GRAY;
		rowbytes = rowbytes/3;

		printf("Completed\n");
		printf("rowbytes = %d\n", rowbytes);
		printf("color_type = %d\n", color_type);
	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA )						// RGBA Image
	{
		printf("[process file] Image is RGBA with depth = %d\n", bit_depth);
		abort_("[RGB2Grayscale]Does not support RGBA Images\n");
	}

	for (j=0; j<height; j++)
	{
		write_row_pointers[j] = image_data_copy + j*rowbytes;
	}
}

// ----------------------------------------END OF IMAGE DATA TYPE CONVERSION-----------------------------------------

//-----------------------------------------BRIGHTNESS AND CONTRAST ADJUSTMENT-------------------------------------



void brightness(int brightnessadj)
{
	int tmp, X, Y;	

	for(Y=0 ; Y<height ; Y++)
	{
		for(X=0 ; X<rowbytes ; X++)
		{
			tmp = *(image_data + Y*rowbytes + X) + brightnessadj;
			if(tmp > 255)
				tmp = 255;
			if(tmp<0)
				tmp = 0;
			*(image_data + Y*rowbytes + X) = tmp;
		}
	}

	int j;
	for (j=0; j<height; j++)
	{
		write_row_pointers[j] = image_data + j*rowbytes;
	}
	
}

void contrast_correction(int contrastadj)
{
	int X, Y, I;
	long int SUM, J, R;
	float AVG;
	J=0;
	float contrast;
	
	contrast = 1.0+((float)contrastadj/100.0)*0.5;

	R=rowbytes;
	SUM=0;
	
	for(Y=0 ; Y<=height ; Y++)
		for(X=0 ; X<=rowbytes ; X++)
		{
			SUM = SUM + *(image_data+X+(long)Y*R);
			J++;
		}
	AVG = (float)SUM/(float)J;
	for(Y=0 ; Y<height ; Y++)
	{
		for(X=0 ; X<rowbytes ; X++)
		{
			I=contrast *((float) *(image_data+X+(long)Y*R)-AVG)+AVG;
			if(I>255)
				I=255;
			if(I<0)
				I=0;
			*(image_data + X +(long) Y*R) = I;
		}
	}

	int j;
	for (j=0; j<height; j++)
	{
		write_row_pointers[j] = image_data + j*rowbytes;
	}
}

//---------------------------- END OF BRIGHTNESS AND CONTRAST ADJUSTMENT-------------------------------------

//-------------------------------------FLIP ---------------------------------------------------

void flip_horizontal()
{
	int X,Y;
	png_byte temp;
	png_byte tempR,tempG,tempB,tempA;
	
	if( info_ptr->color_type == PNG_COLOR_TYPE_PALETTE )
		abort_("[RGB2Grayscale] This program does not support paletted color images\n");

	if( info_ptr->color_type == PNG_COLOR_TYPE_GRAY)
	{
		printf("[process file] Image is grayscale with depth = %d\n", bit_depth);
		for(Y=0 ; Y<height ; Y++)
		{
			for(X=0 ; X<(rowbytes/2) ; X++)
			{
				temp = *(image_data + X + Y*rowbytes);
				*(image_data + X+Y*rowbytes) = *(image_data +Y*rowbytes + rowbytes - X);
				*(image_data +Y*rowbytes+rowbytes- X) = temp;
			}
		}
		printf("done!\n");
	}
	
	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB )						// RGB Image
	{
		for(Y=0 ; Y<height ; Y++)
		{
			for(X=0 ; X<(width/2) ; X++)
			{
				tempR = *(image_data + Y*rowbytes + 3*X );
				tempG = *(image_data + Y*rowbytes + 3*X + 1);
				tempB = *(image_data + Y*rowbytes + 3*X + 2);
				*(image_data + Y*rowbytes + 3*X ) = *(image_data +Y*rowbytes + rowbytes - 3*X - 3);
				*(image_data + Y*rowbytes + 3*X + 1) = *(image_data +Y*rowbytes + rowbytes - 3*X - 2);
				*(image_data + Y*rowbytes + 3*X + 2) = *(image_data +Y*rowbytes + rowbytes - 3*X - 1);
				*(image_data + Y*rowbytes + rowbytes - 3*X - 3) = tempR;
				*(image_data + Y*rowbytes + rowbytes - 3*X - 2) = tempG;
				*(image_data + Y*rowbytes + rowbytes - 3*X - 1) = tempB;
			}
		}
	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA )						// RGBA Image
	{
		for(Y=0 ; Y<height ; Y++)
		{
			for(X=0 ; X<(width/2) ; X++)
			{
				tempR = *(image_data + Y*rowbytes + 4*X );
				tempG = *(image_data + Y*rowbytes + 4*X + 1);
				tempB = *(image_data + Y*rowbytes + 4*X + 2);
				tempA = *(image_data + Y*rowbytes + 4*X + 3);
				*(image_data + Y*rowbytes + 4*X ) = *(image_data +Y*rowbytes + rowbytes - 4*X - 4);
				*(image_data + Y*rowbytes + 4*X + 1) = *(image_data +Y*rowbytes + rowbytes - 4*X - 3);
				*(image_data + Y*rowbytes + 4*X + 2) = *(image_data +Y*rowbytes + rowbytes - 4*X - 2);
				*(image_data + Y*rowbytes + 4*X + 3) = *(image_data +Y*rowbytes + rowbytes - 4*X - 1);
				*(image_data + Y*rowbytes + rowbytes - 4*X - 4) = tempR;
				*(image_data + Y*rowbytes + rowbytes - 4*X - 3) = tempG;
				*(image_data + Y*rowbytes + rowbytes - 4*X - 2) = tempB;
				*(image_data + Y*rowbytes + rowbytes - 4*X - 1) = tempA;
			}
		}
	}

	int j;
	for (j=0; j<height; j++)
	{
		write_row_pointers[j] = image_data + j*rowbytes;
	}
	
}

void flip_vertical()
{
	int Y, X, C;
	image_data_copy = malloc(sizeof(png_byte)*rowbytes*height);

	C = height;
	for(Y=0;Y<height;Y++)
	{
		for(X=0;X<rowbytes;X++){
			*(image_data_copy+Y*rowbytes+X) = *(image_data +C*rowbytes +X);
		}
		C--;
	}

	int j;
	for (j=0; j<height; j++)
	{
		write_row_pointers[j] = image_data_copy + j*rowbytes;
	}
}

void zoom(int x, int y, int hor, int ver, unsigned char zd)
{
	int i, j, m=0, n=0;
	
	if(zd)	/* Zoom */ 
	{	
		for(i=y ; i<ver; ++i)
		{
			for(j=x; j<hor; ++j) 
			{
				*(image_data_copy + rowbytes*m + n) = 
				*(image_data_copy + rowbytes*(m+1) + n) =
				*(image_data_copy + rowbytes*m + (n+1)) =
				*(image_data_copy + rowbytes*(m+1) + (n+1)) = *(image_data + rowbytes*j + i);
				n +=2;
			}
		m += 2;
		n = 0;
		}
	} else {	/* Dezoom */
		for(i=y ; i<ver; ++i)
		{
			for(j=x; j<hor; ++j) 
			{
				*(image_data_copy + rowbytes*m + n) = *(image_data + rowbytes*i + j);
				++n;
			}
		++m;
		n = 0;
		}
	}
}

//-------------------------------------END OF FLIP, ROTATE, ZOOM AND SCALING----------------------------------------

// ------------------------------------------WEIGHTED MEAN & WEIGHTED MEDIAN---------------------------------------
void weighted_mean()
{	
	float WMF_MASK[5][5] = {{1,1,1,1,1},{1,2,2,2,1},{1,2,3,2,1},{1,2,2,2,1},{1,1,1,1,1}};
	
	image_data_copy = malloc(sizeof(png_byte)*rowbytes*height);
	int X, Y;
	int I, J;

	float SUM, SUMW;

	if( info_ptr->color_type == PNG_COLOR_TYPE_PALETTE )
		abort_("[RGB2Grayscale] This program does not support paletted color images\n");

	if( info_ptr->color_type == PNG_COLOR_TYPE_GRAY)	
	{
		for(Y=N/2 ; Y<(height-N/2) ; Y++)
		{
			for( X=N/2 ; X<(rowbytes-N/2) ; X++)
			{
				SUMW=0.0; SUM=0.0;
				for(J=-N/2 ; J<=N/2 ; J++)
				{
					for(I=(-N/2); I<=N/2; I++)
					{
						SUM=SUM + *(image_data+X+I+(long)(Y+J)*rowbytes) * WMF_MASK[I+N/2][J+N/2];
						SUMW=SUMW+WMF_MASK[I+N/2][J+N/2];
					}
				}
			*(image_data_copy + X + (long)Y*rowbytes) = (unsigned char)(SUM/SUMW + .5);
			}
		}
	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB)						// RGB Image
	{
		printf("[process file] Image is RGB with depth = %d\n", bit_depth);
		abort_("Does not support RGB Images\n");

	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA )						// RGB Image
	{
		printf("[process file] Image is RGBA with depth = %d\n", bit_depth);
		abort_("Does not support RGBA Images\n");
	}

	int j;
	for (j=0; j<height; j++)
	{
		write_row_pointers[j] = image_data_copy + j*rowbytes;
	}
}	

void weighted_median()
{
	float Median_MASK[N2][N2] = {{1,1,1},{3,3,3},{1,1,1}};
	image_data_copy = malloc(sizeof(png_byte)*rowbytes*height);
	int X, Y, I, J, M, Z;
	int AR[150], A;

	if( info_ptr->color_type == PNG_COLOR_TYPE_PALETTE )
		abort_("[RGB2Grayscale] This program does not support paletted color images\n");

	if( info_ptr->color_type == PNG_COLOR_TYPE_GRAY)	
	{
		for( Y =N2/2; Y<(height-N2/2); Y++)
			for(X=N2/2 ; X<(rowbytes-N2/2) ; X++)
			{	
				Z=0;
				for(J=-N2/2; J<=N2/2 ; J++)
					for(I=-N2/2; I<=N2/2 ; I++)
					{
						for(M=1; M<=Median_MASK[(int)N2/2+I][(int)N2/2+J];M++)
						{
							AR[Z] = *(image_data + X +I + (long)(Y+J)*rowbytes);
							Z++;
						}
					}
				for(J=1;J<=Z-1;J++)
				{
					A = AR[J];
					I=J-1;
					while(I>=0 && AR[I]>A)
					{
						AR[I+1] = AR[I];
						I=I-1;
					}
					AR[I+1]=A;
				}
			*(image_data_copy+X+(long)Y*rowbytes) = AR[Z/2];
		}
	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB)						// RGB Image
	{
		printf("[process file] Image is RGB with depth = %d\n", bit_depth);
		abort_("Does not support RGB Images\n");

	}

	if( info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA )						// RGB Image
	{
		printf("[process file] Image is RGBA with depth = %d\n", bit_depth);
		abort_("Does not support RGBA Images\n");
	}

	int j;
	for (j=0; j<height; j++)
	{
		write_row_pointers[j] = image_data_copy + j*rowbytes;
	}
}


// -----------------------------------END OF WEIGHTED MEAN & WEIGHTED MEDIAN-----------------------------------
