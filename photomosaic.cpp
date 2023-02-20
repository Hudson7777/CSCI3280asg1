/*

CSCI 3280, Introduction to Multimedia Systems
Spring 2023

Assignment 01 Skeleton

photomosaic.cpp

*/

#include "stdio.h"
#include <iostream>
#include <vector>
#include <string>
#include "malloc.h"
#include "memory.h"
#include "math.h"
#include "bmp.h"		//	Simple .bmp library
#include "list_files.h" //	Simple list file library

using namespace std;

#define SAFE_FREE(p)  { if(p){ free(p);  (p)=NULL;} }

// Function to replace the tile region with the photo tiles.
void assemble(Bitmap* source, Bitmap* tile, int i, int j, int cell_w, int cell_h);
// Function to resize the source image and the photo tiles.
Bitmap* resize(Bitmap source_image, int output_h, int output_w);

int main(int argc, char** argv)
{
	// 1.1 (Done) First of all, I need to parse the arguments stored in argv[3], which is 4 integers separated by commas.
	char* parser;
	int output_w = strtol(argv[3], &parser, 10);
	parser++;
	int output_h = strtol(parser, &parser, 10);
	parser++;
	int tile_w = strtol(parser, &parser, 10);
	parser++;
	int tile_h = strtol(parser, &parser, 10);
	int regions_num = (output_w / tile_w) * (output_h / tile_h);
	cout << "The output width is: " << output_w << endl;
	cout << "The output height is: " << output_h << endl;
	cout << "The tile width is: " << tile_w << endl;
	cout << "The tile height is: " << tile_h << endl;
	cout << "The number of regions is: " << regions_num << endl;
	cout << "End of 1.1" << endl;

	// 1.2 (Done) Then I need to read the source bitmap and the photo tiles from argv[1], and calculate the number of photo tiles.
	Bitmap source_image;
	source_image.create(argv[1]);
	vector<string> file_path_vector;
	list_files(argv[2], ".bmp", file_path_vector, false);
	Bitmap** origin_tiles = new Bitmap*[50000];
	int tiles_num = 0;
	for (std::string path : file_path_vector) {
		origin_tiles[tiles_num] = new Bitmap(path.c_str());
		tiles_num++;
	}
	cout << "The number of tiles is " << tiles_num << endl;
	cout << "End of 1.2" << endl;

	// 1.3 (Done) Then I need to resize the source image to the desired output size

	Bitmap* resized_source_image = resize(source_image, output_w, output_h);
	// I need to save the resized source image to a new file.
	// resized_source_image->save("resized_source_image.bmp");
	cout << "End of 1.3" << endl;

	//1.4 (Done) I need to calculate the average brightness of each cell region in the resized source image.
	int cell_per_row = output_w / tile_w;;
	int cell_per_column = output_h / tile_h;
	int counter = 0;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	double* cell_brightness = new double[cell_per_row * cell_per_column];
	for(int i = 0; i < output_w; i+=tile_w){
		for(int j = 0; j < output_h; j +=tile_h){
			double rgb_sum = 0;
			for(int k = i; k < tile_w + i; k++){
				for(int l = j; l < tile_h + j; l++){
					resized_source_image->getColor(k, l, r, g, b);
					rgb_sum = rgb_sum + r * 0.299 + g * 0.587 + b * 0.114;
				}
			}
			cell_brightness[counter] = rgb_sum / (double)(tile_w * tile_h);
			counter++;
		}
	}
	cout << "End of 1.4" << endl;

	// 1.5(Done) I need to resize the photo tiles, and during the process of resizing, I can calculate the average brightness of each resized photo tile.
	
	// I need to resize the photo tiles, and calculate the brightness of each resized tile.
	Bitmap** resize_photo_tiles = new Bitmap*[tiles_num];
	double* brightness_tiles = new double[tiles_num];
	for(int i = 0; i < tiles_num; i++){
		resize_photo_tiles[i] = resize(*origin_tiles[i], tile_w, tile_h);
		double rgb_sum = 0;
		unsigned char r;
		unsigned char g;
		unsigned char b;
		for (int j = 0; j < tile_w; j++)
		{
			for (int k = 0; k < tile_h; k++)
			{
				resize_photo_tiles[i]->getColor(j, k, r, g, b);
				rgb_sum = rgb_sum + r * 0.299 + g * 0.587 + b * 0.114;
			}
		}
		brightness_tiles[i] = rgb_sum / (double)(tile_w * tile_h);
	}
	cout << "End of 1.5" << endl;

	// 1.6 I need to replace each cell region with the photo tile that has the closest brightness to the average brightness of the cell region using the assemble function.
	Bitmap* output_image = new Bitmap(output_w, output_h);
	for(int i = 0; i < cell_per_row; i++){
		for(int j = 0; j < cell_per_column; j++){
			int index = 0;
			double min_diff = 255;
			for(int k = 0; k < tiles_num; k++){
				double diff = abs(cell_brightness[i * cell_per_column + j] - brightness_tiles[k]);
				if(diff < min_diff){
					min_diff = diff;
					index = k;
				}
			}
			assemble(resized_source_image, resize_photo_tiles[index], i, j, tile_w, tile_h);
		}
	}
	resized_source_image->save("output_image.bmp");
	cout << "End of 1.6" << endl;

}

void assemble(Bitmap* source, Bitmap* tile, int i, int j, int tile_w, int tile_h)
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char grey;
	for (int w = 0; w < tile_w; w++)
	{
		for (int h = 0; h < tile_h; h++)
		{
			tile->getColor(w, h, r, g, b);
			grey = r * 0.299 + g * 0.587 + b * 0.114;
			source->setColor(i * tile_w + w, j * tile_h + h, grey, grey, grey);
		}
	}
}

Bitmap* resize(Bitmap source_image, int output_w, int output_h)
{
	Bitmap* resized_source_image = new Bitmap(output_w, output_h);
	double width_ratio = (double)source_image.getWidth() / (double)output_w;
	double height_ratio = (double)source_image.getHeight() / (double)output_h;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	int r_sum = 0;
	int g_sum = 0;
	int b_sum = 0;
	for (int i = 0; i < output_w; i++)
	{
		for (int j = output_h - 1; j >= 0; j--)
		{
			// I need to calculate the position of the pixel in the source image
			int source_x = (int)(i * width_ratio);
			int source_y = (int)(j * height_ratio);
			// I need to calculate the position of the pixel in the resized source image
			int resized_source_x = i;
			int resized_source_y = j;
			// I need to store the decimal parts of the position of the pixel in the source image
			double x_diff = (i * width_ratio) - source_x;
			double y_diff = (j * height_ratio) - source_y;
			// I need to calculate the position of the 4 pixels surrounding the pixel in the source image
			int source_x1 = source_x;
			int source_y1 = source_y;
			int source_x2 = source_x + 1;
			int source_y2 = source_y;
			int source_x3 = source_x;
			int source_y3 = source_y + 1;
			int source_x4 = source_x + 1;
			int source_y4 = source_y + 1;
			// I need to calculate the position of the 4 pixels surrounding the pixel in the resized source image
			int resized_source_x1 = resized_source_x;
			int resized_source_y1 = resized_source_y;
			int resized_source_x2 = resized_source_x + 1;
			int resized_source_y2 = resized_source_y;
			int resized_source_x3 = resized_source_x;
			int resized_source_y3 = resized_source_y + 1;
			int resized_source_x4 = resized_source_x + 1;
			int resized_source_y4 = resized_source_y + 1;
			// (Q)I need to calculate the weight of the 4 pixels surrounding the pixel in the source image
			double weight1 = (1 - x_diff) * (1 - y_diff);
			double weight2 = x_diff * (1 - y_diff);
			double weight3 = (1 - x_diff) * y_diff;
			double weight4 = x_diff * y_diff;
			// (Q)I need to calculate， and here the r,g and b should be unsigned char.
			r = (unsigned char)(source_image.getData()[source_y1 * source_image.getWidth() + source_x1].R * weight1 + source_image.getData()[source_y2 * source_image.getWidth() + source_x2].R * weight2 + source_image.getData()[source_y3 * source_image.getWidth() + source_x3].R * weight3 + source_image.getData()[source_y4 * source_image.getWidth() + source_x4].R * weight4);
			g = (unsigned char)(source_image.getData()[source_y1 * source_image.getWidth() + source_x1].G * weight1 + source_image.getData()[source_y2 * source_image.getWidth() + source_x2].G * weight2 + source_image.getData()[source_y3 * source_image.getWidth() + source_x3].G * weight3 + source_image.getData()[source_y4 * source_image.getWidth() + source_x4].G * weight4);
			b = (unsigned char)(source_image.getData()[source_y1 * source_image.getWidth() + source_x1].B * weight1 + source_image.getData()[source_y2 * source_image.getWidth() + source_x2].B * weight2 + source_image.getData()[source_y3 * source_image.getWidth() + source_x3].B * weight3 + source_image.getData()[source_y4 * source_image.getWidth() + source_x4].B * weight4);

			r_sum += r;
			g_sum += g;
			b_sum += b;
			// Without this k, the output file would be upside down.
			int k = output_h - j - 1;
			resized_source_image->setColor(i, k, r, g, b);		
		}
	}
	return resized_source_image;
}