/*
 * Use this code how and where you want, for the sake of common good.
 *
 * Dmitry <dkorobkov at mail dot ru>
 */

#include <stdio.h>

// Link with -lopencv_core -lopencv_videoio

#include <opencv2/opencv.hpp>
#include "opencv2/videoio.hpp"

using namespace cv;

#define WIDTH 	320
#define HEIGHT 	240
#define DEPTH 	3

#define GRAYMOVIE 		"/tmp/gray.avi"
#define COLOURMOVIE 	"/tmp/colour.avi"

// Array will hold grayscale data and space for conversion into I420 (total 1.5 * WIDTH * HEIGHT),
// or RGB data, depending on selected movie.
unsigned char arFrame[WIDTH * HEIGHT * DEPTH];

// Function draws vertical stripes starting from "seed".
void CreateVideoFrame(int seed, bool bColour)
{
	if(bColour == false)
	{
		// Create grayscale movie
		unsigned char gray = seed;

		for(int x = 0; x < WIDTH; x++)
		{
			for(int y = 0; y < HEIGHT; y++)
				arFrame[y*WIDTH + x] = gray;

			gray++;
		}
	}
	else
	{
		// Create coloured movie
		unsigned char red = seed;
		unsigned char green = 0x80;
		unsigned char blue = 255 - seed;

		for(int x = 0; x < WIDTH; x++)
		{
			for(int y = 0; y < HEIGHT; y++)
			{
				arFrame[y*WIDTH*DEPTH + x*DEPTH] = red;
				arFrame[y*WIDTH*DEPTH + x*DEPTH + 1] = green;
				arFrame[y*WIDTH*DEPTH + x*DEPTH + 2] = blue;
			}

			red++;
			blue--;
		}
	}
}

// Creates 8-bit grayscale uncompressed AVI movie
void CreateMovie(bool bColor)
{
	VideoWriter vw;
	Size size;
	size.width = WIDTH;
	size.height = HEIGHT;

	const char* pFilename = GRAYMOVIE;
	const char* pFileType = "grayscale";

	if(bColor == true)
	{
		pFilename = COLOURMOVIE;
		pFileType = "colour";
	}

	try
	{
		vw.open(pFilename, vw.fourcc('I','4','2','0'), 10, size, bColor);
	}
	catch(cv::Exception& e)
	{
		fprintf(stderr, "vw.open() caused an exception: %s\n", e.what());
		return;
	}

	for(int frame = 0; frame < 100; frame++)
	{
		// Fill frame with some data, changing from frame to frame
		CreateVideoFrame(frame, bColor);

		// Load frame into Mat object
		int type = CV_8UC1;  // For grayscale, we need 1-channel matrix
		if(bColor)
			type = CV_8UC3;  // Fro colours, we need RGB (3-channel) array
		Mat m(HEIGHT, WIDTH, type, arFrame);

		// Write next frame into movie. VideoWriter will convert it itself.
		vw.write(m);
	}

	vw.release();

	printf("Done writing %s movie\n", pFileType);
}

int main()
{
	printf("This is a test for creating grayscale or color UNCOMPRESSED AVI movie with OpenCV\n");

	while(true)
	{
		int key = 0;

		printf( "Press G <Enter> to create " GRAYMOVIE " grayscale movie\n"
				"Press C <Enter> to create " COLOURMOVIE " RGB movie\n"
				"Press Q <Enter> to quit\n");
		scanf("%c", &key);
		key = toupper(key);

		if(key  == 'G')
			CreateMovie(false);

		if(key == 'C')
			CreateMovie(true);

		if(key == 'Q')
			return 0;
	}

	return 0;
}
