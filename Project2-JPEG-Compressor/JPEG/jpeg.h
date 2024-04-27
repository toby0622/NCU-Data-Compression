#ifndef _HAVE_SUPPORT_H
#define _HAVE_SUPPORT_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <map>
#include <sstream>

using namespace std;

// for encoder
string valueCodeWord(int input);

void quantize(double input[8][8], bool isY, int QF);

void DCT(double **input, double **output, int row = 8, int col = 8);

void rgb2ycbcr(double r, double g, double b, double &y, double &cb, double &cr);

void encodeBlock(ofstream &outfile, double input[8][8], int QF, bool isY);

void encode(bool isRGB, int width, int height, string filename, int QF);

// for encoder
void initDecodeTable();

int bit2int(string decode);

void dequantize(double input[8][8], bool isY, int QF);

void IDCT(double **input, double **output, int row = 8, int col = 8);

void ycbcr2rgb(double y, double cb, double cr, double &r, double &g, double &b);

void decode(bool isRGB);


void PSNR(bool isRGB, int width, int height, string filename);

void menu();

void test(); // just for quick testing
#endif