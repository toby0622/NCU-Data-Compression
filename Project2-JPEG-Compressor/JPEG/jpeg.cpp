#include "jpeg.h"

#define PI acos(-1)
double yQTable[8][8] = {{16, 11, 10, 16, 24,  40,  51,  61},
                        {12, 12, 14, 19, 26,  58,  60,  55},
                        {14, 13, 16, 24, 40,  57,  69,  56},
                        {14, 17, 22, 29, 51,  87,  80,  82},
                        {18, 22, 37, 56, 68,  109, 103, 77},
                        {24, 35, 55, 64, 81,  104, 113, 92},
                        {99, 64, 78, 87, 103, 121, 120, 101},
                        {72, 92, 95, 98, 112, 100, 103, 99}};

double cbcrQTable[8][8] = {{17, 18, 24, 47, 99, 99, 99, 99},
                           {18, 21, 26, 66, 99, 99, 99, 99},
                           {24, 26, 56, 99, 99, 99, 99, 99},
                           {47, 66, 99, 99, 99, 99, 99, 99},
                           {99, 99, 99, 99, 99, 99, 99, 99},
                           {99, 99, 99, 99, 99, 99, 99, 99},
                           {99, 99, 99, 99, 99, 99, 99, 99},
                           {99, 99, 99, 99, 99, 99, 99, 99}};

int zigzag[64] = {0, 1, 5, 6, 14, 15, 27, 28,
                  2, 4, 7, 13, 16, 26, 29, 42,
                  3, 8, 12, 17, 25, 30, 41, 43,
                  9, 11, 18, 24, 31, 40, 44, 53,
                  10, 19, 23, 32, 39, 45, 52, 54,
                  20, 22, 33, 38, 46, 51, 55, 60,
                  21, 34, 37, 47, 50, 56, 59, 61,
                  35, 36, 48, 49, 57, 58, 62, 63};

string dcYHuffmanTable[12] = {"00", "010", "011", "100", "101", "110", "1110", "11110", "111110", "1111110", "11111110",
                              "111111110"};
string dcCbCrHuffmanTable[12] = {"00", "01", "10", "110", "1110", "11110", "111110", "1111110", "11111110", "111111110",
                                 "1111111110", "11111111110"};
// (0,0) -> 1010    F/0 (ZRL) -> 11111111001
string acYHuffmanTable[16][10] = {
        {"00",               "01",               "100",              "1011",             "11010",            "1111000",          "11111000",         "1111110110",       "1111111110000010", "1111111110000011"},
        {"1100",             "11011",            "1111001",          "111110110",        "11111110110",      "1111111110000100", "1111111110000101", "1111111110000110", "1111111110000111", "1111111110001000"},
        {"11100",            "11111001",         "1111110111",       "111111110100",     "1111111110001001", "1111111110001010", "1111111110001011", "1111111110001100", "1111111110001101", "1111111110001110"},
        {"111010",           "111110111",        "111111110101",     "1111111110001111", "1111111110010000", "1111111110010001", "1111111110010010", "1111111110010011", "1111111110010100", "1111111110010101"},
        {"111011",           "1111111000",       "1111111110010110", "1111111110010111", "1111111110011000", "1111111110011001", "1111111110011010", "1111111110011011", "1111111110011100", "1111111110011101"},
        {"1111010",          "11111110111",      "1111111110011110", "1111111110011111", "1111111110100000", "1111111110100001", "1111111110100010", "1111111110100011", "1111111110100100", "1111111110100101"},
        {"1111011",          "111111110110",     "1111111110100110", "1111111110100111", "1111111110101000", "1111111110101001", "1111111110101010", "1111111110101011", "1111111110101100", "1111111110101101"},
        {"11111010",         "111111110111",     "1111111110101110", "1111111110101111", "1111111110110000", "1111111110110001", "1111111110110010", "1111111110110011", "1111111110110100", "1111111110110101"},
        {"111111000",        "111111111000000",  "1111111110110110", "1111111110110111", "1111111110111000", "1111111110111001", "1111111110111010", "1111111110111011", "1111111110111100", "1111111110111101"},
        {"111111001",        "1111111110111110", "1111111110111111", "1111111111000000", "1111111111000001", "1111111111000010", "1111111111000011", "1111111111000100", "1111111111000101", "1111111111000110"},
        {"111111010",        "1111111111000111", "1111111111001000", "1111111111001001", "1111111111001010", "1111111111001011", "1111111111001100", "1111111111001101", "1111111111001110", "1111111111001111"},
        {"1111111001",       "1111111111010000", "1111111111010001", "1111111111010010", "1111111111010011", "1111111111010100", "1111111111010101", "1111111111010110", "1111111111010111", "1111111111011000"},
        {"1111111010",       "1111111111011001", "1111111111011010", "1111111111011011", "1111111111011100", "1111111111011101", "1111111111011110", "1111111111011111", "1111111111100000", "1111111111100001"},
        {"11111111000",      "1111111111100010", "1111111111100011", "1111111111100100", "1111111111100101", "1111111111100110", "1111111111100111", "1111111111101000", "1111111111101001", "1111111111101010"},
        {"1111111111101011", "1111111111101100", "1111111111101101", "1111111111101110", "1111111111101111", "1111111111110000", "1111111111110001", "1111111111110010", "1111111111110011", "1111111111110100"},
        {"1111111111110101", "1111111111110110", "1111111111110111", "1111111111111000", "1111111111111001", "1111111111111010", "1111111111111011", "1111111111111100", "1111111111111101", "1111111111111110"}};
// 0/0  -> 00  F/0 (ZRL) -> 1111111010
string acCbCrHuffmanTable[16][10] = {
        {"01",              "100",              "1010",             "11000",            "11001",            "111000",           "1111000",          "111110100",        "1111110110",       "111111110100"},
        {"1011", "111001", "11110110", "111110101", "11111110110",                                          "111111110101",     "1111111110001000", "1111111110001001", "1111111110001010", "1111111110001011"},
        {"11010",           "11110111",         "1111110111",       "111111110110",     "111111111000010",  "1111111110001100", "1111111110001101", "1111111110001110", "1111111110001111", "1111111110010000"},
        {"11011",           "11111000",         "1111111000",       "111111110111",     "1111111110010001", "1111111110010010", "1111111110010011", "1111111110010100", "1111111110010101", "1111111110010110"},
        {"111010",          "111110110",        "1111111110010111", "1111111110011000", "1111111110011001", "1111111110011010", "1111111110011011", "1111111110011100", "1111111110011101", "1111111110011110"},
        {"111011",          "1111111001",       "1111111110011111", "1111111110100000", "1111111110100001", "1111111110100010", "1111111110100011", "1111111110100100", "1111111110100101", "1111111110100110"},
        {"1111001",         "11111110111",      "1111111110100111", "1111111110101000", "1111111110101001", "1111111110101010", "1111111110101011", "1111111110101100", "1111111110101101", "1111111110101110"},
        {"1111010",         "11111111000",      "1111111110101111", "1111111110110000", "1111111110110001", "1111111110110010", "1111111110110011", "1111111110110100", "1111111110110101", "1111111110110110"},
        {"11111001",        "1111111110110111", "1111111110111000", "1111111110111001", "1111111110111010", "1111111110111011", "1111111110111100", "1111111110111101", "1111111110111110", "1111111110111111"},
        {"111110111",       "1111111111000000", "1111111111000001", "1111111111000010", "1111111111000011", "1111111111000100", "1111111111000101", "1111111111000110", "1111111111000111", "1111111111001000"},
        {"111111000",       "1111111111001001", "1111111111001010", "1111111111001011", "1111111111001100", "1111111111001101", "1111111111001110", "1111111111001111", "1111111111010000", "1111111111010001"},
        {"111111001",       "1111111111010010", "1111111111010011", "1111111111010100", "1111111111010101", "1111111111010110", "1111111111010111", "1111111111011000", "1111111111011001", "1111111111011010"},
        {"111111010",       "1111111111011011", "1111111111011100", "1111111111011101", "1111111111011110", "1111111111011111", "1111111111100000", "1111111111100001", "1111111111100010", "1111111111100011"},
        {"11111111001",     "1111111111100100", "1111111111100101", "1111111111100110", "1111111111100111", "1111111111101000", "1111111111101001", "1111111111101010", "1111111111101011", "1111111111101100"},
        {"11111111100000",  "1111111111101101", "1111111111101110", "1111111111101111", "1111111111110000", "1111111111110001", "1111111111110010", "1111111111110011", "1111111111110100", "1111111111110101"},
        {"111111111000011", "1111111111110110", "1111111111110111", "1111111111111000", "1111111111111001", "1111111111111010", "1111111111111011", "1111111111111100", "1111111111111101", "1111111111111110"}};

// use for decoder
map<string, int> dcYDecodeTable;
map<string, int> dcCbCrDecodeTable;
map<string, string> acYDecodeTable;
map<string, string> acCbCrDecodeTable;

// initial the maps for decoder
void initDecodeTable() {
    // dc y table
    for (int i = 0; i < 12; i++) {
        dcYDecodeTable[dcYHuffmanTable[i]] = i;
    }
    // dc cbcr table
    for (int i = 0; i < 12; i++) {
        dcCbCrDecodeTable[dcCbCrHuffmanTable[i]] = i;
    }

    // ac y table
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 10; j++) {
            string run;
            string size;
            if (i < 10) {
                run = (char) (i + 48);
            } else {
                run = (char) (i + 55);
            }
            if (j + 1 < 10) {
                size = (char) (j + 1 + 48);
            } else {
                size = (char) (j + 1 + 55);
            }
            string value = run + "/" + size;
            acYDecodeTable[acYHuffmanTable[i][j]] = value;
        }
    }
    acYDecodeTable["11111111001"] = "F/0";
    acYDecodeTable["1010"] = "0/0";

    // ac cbcr table
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 10; j++) {
            string run;
            string size;
            if (i < 10) {
                run = (char) (i + 48);
            } else {
                run = (char) (i + 55);
            }
            if (j + 1 < 10) {
                size = (char) (j + 1 + 48);
            } else {
                size = (char) (j + 1 + 55);
            }
            string value = run + "/" + size;
            acCbCrDecodeTable[acCbCrHuffmanTable[i][j]] = value;
        }
    }
    acCbCrDecodeTable["1111111010"] = "F/0";
    acCbCrDecodeTable["00"] = "0/0";
}

int bit2int(string decode) {
    bool negative = false;
    string input = decode;
    if (input[0] == '0') // it is negative number
    {
        negative = true;
        for (int i = 0; i < input.size(); i++) {
            if (input[i] == '0') {
                input[i] = '1';
            } else {
                input[i] = '0';
            }
        }
    }
    int buffer = 0;
    for (int i = 0; i < input.size(); i++) {
        if (input[i] == '1') {
            buffer++;
        }
        if (i != input.size() - 1) {
            buffer <<= 1;
        }
    }
    return negative ? (-buffer) : buffer;
}

string valueCodeWord(int input) {
    string result;
    int temp = abs(input);
    while (temp != 0) {
        if (temp & 1) {
            result = '1' + result;
        } else {
            result = '0' + result;
        }
        temp = temp >> 1;
    }
    if (input < 0) {
        for (int i = 0; i < result.size(); i++) {
            if (result[i] == '0') {
                result[i] = '1';
            } else {
                result[i] = '0';
            }
        }
    }
    return result;
}

void quantize(double input[8][8], bool isY, int QF) {
    // calculate quality factor
    int quality;
    if (QF < 50) {
        quality = 5000 / QF;
    } else if (QF == 100) {
        quality = 1;
    } else {
        quality = 200 - 2 * QF;
    }

    // quantize according to quantization table and quality factor
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (isY) {
                input[i][j] = (int) round(input[i][j] / (yQTable[i][j] * quality / 100));
            } else {
                input[i][j] = (int) round(input[i][j] / (cbcrQTable[i][j] * quality / 100));
            }
        }
    }
}

void dequantize(double input[8][8], bool isY, int QF) {
    // calculate quality factor
    int quality;
    if (QF < 50) {
        quality = 5000 / QF;
    } else if (QF == 100) {
        quality = 1;
    } else {
        quality = 200 - 2 * QF;
    }

    // quantize according to quantization table and quality factor
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (isY) {
                input[i][j] = input[i][j] * (yQTable[i][j] * quality / 100);
            } else {
                input[i][j] = input[i][j] * (cbcrQTable[i][j] * quality / 100);
            }
        }
    }
}

void DCT(double **input, double **output, int row, int col) {
    double ALPHA, BETA;
    int u = 0;
    int v = 0;
    int i = 0;
    int j = 0;

    for (u = 0; u < row; u++) {
        for (v = 0; v < col; v++) {
            if (u == 0) {
                ALPHA = sqrt(1.0 / row);
            } else {
                ALPHA = sqrt(2.0 / row);
            }

            if (v == 0) {
                BETA = sqrt(1.0 / col);
            } else {
                BETA = sqrt(2.0 / col);
            }

            double tmp = 0.0;
            for (i = 0; i < row; i++) {
                for (j = 0; j < col; j++) {
                    tmp += *((double *) input + col * i + j) * cos((2 * i + 1) * u * PI / (2.0 * row)) *
                           cos((2 * j + 1) * v * PI / (2.0 * col));
                }
            }
            *((double *) output + col * u + v) = ALPHA * BETA * tmp;
        }
    }
}

// Inverse DCT
void IDCT(double **input, double **output, int row, int col) {
    double ALPHA, BETA;
    int u = 0;
    int v = 0;
    int i = 0;
    int j = 0;

    for (i = 0; i < row; i++) {
        for (j = 0; j < col; j++) {
            double tmp = 0.0;
            for (u = 0; u < row; u++) {
                for (v = 0; v < col; v++) {
                    if (u == 0) {
                        ALPHA = sqrt(1.0 / row);
                    } else {
                        ALPHA = sqrt(2.0 / row);
                    }
                    if (v == 0) {
                        BETA = sqrt(1.0 / col);
                    } else {
                        BETA = sqrt(2.0 / col);
                    }
                    tmp += ALPHA * BETA * *((double *) input + col * u + v) * cos((2 * i + 1) * u * PI / (2.0 * row)) *
                           cos((2 * j + 1) * v * PI / (2.0 * col));
                }
            }
            *((double *) output + col * i + j) = tmp;
        }
    }
}

void rgb2ycbcr(double r, double g, double b, double &y, double &cb, double &cr) {
    y = 0.299 * r + 0.587 * g + 0.114 * b;
    cb = 128 - (0.169 * r) - (0.331 * g) + (0.500 * b);
    cr = 128 + (0.500 * r) - (0.419 * g) - (0.081 * b);
}

void ycbcr2rgb(double y, double cb, double cr, double &r, double &g, double &b) {
    r = y + 1.402 * (cr - 128);
    g = y - 0.344 * (cb - 128) - 0.714 * (cr - 128);
    b = y + 1.772 * (cb - 128);
}

int buffer = 0;
int counter = 0;
int preDC = 0;

// encode 8*8 block
void encodeBlock(ofstream &outfile, double input[8][8], int QF, bool isY) {
    // do DCT
    double result[8][8] = {0};
    DCT((double **) input, (double **) result);

    // quantize
    quantize(result, isY, QF);

    // use zigzag transform the 8*8 black to 1D array
    vector<int> block(64, 0);
    for (int i = 0; i < 64; i++) {
        block[zigzag[i]] = (int) result[i / 8][i % 8];
    }
    int dc = block[0] - preDC;
    preDC = block[0];

    // run length encode
    int zeroNum = 0;
    vector<int> rle;
    for (int i = 1; i < 64; i++) {
        if (block[i] != 0) {
            rle.push_back(zeroNum);
            rle.push_back(block[i]);
            zeroNum = 0;
        } else {
            if (zeroNum == 15) {
                rle.push_back(15);
                rle.push_back(0);
                zeroNum = 0;
            }
            zeroNum++;
        }
    }

    // encode DC
    string encode = "";
    if (dc == 0) {
        if (isY) {
            encode = dcYHuffmanTable[0];
        } else {
            encode = dcCbCrHuffmanTable[0];
        }
    } else {
        if (isY) {
            encode = dcYHuffmanTable[(int) log2(abs(dc)) + 1];
        } else {
            encode = dcCbCrHuffmanTable[(int) log2(abs(dc)) + 1];
        }

        encode = encode + valueCodeWord(dc);
    }

    //write to buffer and file
    for (int i = 0; i < encode.size(); i++) {
        if (encode[i] == '1') {
            buffer += 1;
        }
        counter++;
        if (counter == 8) {
            outfile << (unsigned char) buffer;
            buffer = 0;
            counter = 0;
        } else {
            buffer <<= 1;
        }
    }
    encode = "";

    // encode AC
    for (int i = 0; i <= rle.size(); i += 2) {
        if (i == rle.size()) // end of block
        {
            if (isY) {
                encode = "1010";
            } else {
                encode = "00";
            }
        } else {
            if (rle[i] != 15) {
                if (isY) {
                    encode = acYHuffmanTable[rle[i]][(int) log2(abs(rle[i + 1]))];
                } else {
                    encode = acCbCrHuffmanTable[rle[i]][(int) log2(abs(rle[i + 1]))];
                }

                encode = encode + valueCodeWord(rle[i + 1]);
            } else // 15 zero
            {
                if (isY) {
                    encode = "11111111001";
                } else {
                    encode = "1111111010";
                }
            }
        }

        //write to buffer and file
        for (int i = 0; i < encode.size(); i++) {
            if (encode[i] == '1') {
                buffer += 1;
            }
            counter++;
            if (counter == 8) {
                outfile << (unsigned char) buffer;
                buffer = 0;
                counter = 0;
            } else {
                buffer <<= 1;
            }
        }
        encode = "";
    }
}

void encode(bool isRGB, int width, int height, string filename, int QF) {
    ifstream infile;
    ofstream outfile;
    if (QF < 1 || QF > 100) // set quality factory in [1,100]
    {
        cout << "Invalid quality factor!!" << endl;
        exit(0);
    }
    infile.open(filename.c_str(), ios::in | ios::binary);
    if (!infile) {
        cout << "Open " << filename << " error!!" << endl;
        exit(0);
    }
    if (isRGB) {
        outfile.open("RGB.jpg", ios::out | ios::binary);
    } else {
        outfile.open("gray.jpg", ios::out | ios::binary);
    }
    if (!outfile) {
        cout << "Open Compressed file error!!" << endl;
        exit(0);
    }

    //write header for decoder
    outfile << (int) isRGB << endl;
    outfile << width << endl;
    outfile << height << endl;
    outfile << QF << endl;

    // read the whole image file to imageData
    vector<int> imageData;
    while (1) {
        unsigned char temp = infile.get();
        if (infile.eof()) {
            break;
        }
        imageData.push_back((int) temp);
    }

    // clean previous data
    buffer = 0;
    counter = 0;
    preDC = 0;

    cout << "[+] Start encoding..." << endl;
    vector<double> cb((height / 2) * (width / 2), 0);
    vector<double> cr((height / 2) * (width / 2), 0);
    for (int row = 0; row < height; row += 8) {
        for (int col = 0; col < width; col += 8) {
            double y[8][8] = {0};
            double tempCb;
            double tempCr;

            // get 8*8 image data
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    // gray level image only has y
                    if (!isRGB) {
                        y[i][j] = imageData[((row + i) * width) + (col + j)] - 128;
                    } else {
                        int r = imageData[((row + i) * width * 3) + col * 3 + j * 3];
                        int g = imageData[((row + i) * width * 3) + col * 3 + j * 3 + 1];
                        int b = imageData[((row + i) * width * 3) + col * 3 + j * 3 + 2];
                        rgb2ycbcr(r, g, b, y[i][j], tempCb, tempCr);
                        y[i][j] -= 128;
                        // subsampling 4:2:0
                        if ((i % 2 == 0) && (j % 2 == 0)) {
                            int index = ((row + i) * width * 3) + col * 3 + j * 3;
                            index = (index / width / 3 / 2) * (width / 2) + index % (width * 3) / 6;
                            cb[index] = tempCb;
                            cr[index] = tempCr;
                        }
                    }
                }
            }
            // encode Y
            encodeBlock(outfile, y, QF, true);
        }
    }
    // if it is rgb image need to encode cb cr
    if (isRGB) {
        preDC = 0;
        // encode Cb
        for (int row = 0; row < height / 2; row += 8) {
            for (int col = 0; col < width / 2; col += 8) {
                double block[8][8] = {0};
                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 8; j++) {
                        block[i][j] = cb[(row + i) * (width / 2) + (col + j)];
                    }
                }
                encodeBlock(outfile, block, QF, false);
            }
        }

        // encode Cr
        preDC = 0;
        for (int row = 0; row < height / 2; row += 8) {
            for (int col = 0; col < width / 2; col += 8) {
                double block[8][8] = {0};
                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 8; j++) {
                        block[i][j] = cr[(row + i) * (width / 2) + (col + j)];
                    }
                }
                encodeBlock(outfile, block, QF, false);
            }
        }
    }
    if (counter != 0) {
        buffer <<= (8 - counter);
    }
    outfile << (unsigned char) buffer;
    cout << "[+] Encode complete!!" << endl;
    cout << "[+] Compressed file size = " << outfile.tellp() << " bytes" << endl;
}

void decode(bool isRGB) {
    ifstream infile;
    ofstream outfile;
    string infileName;
    string outfileName;
    if (isRGB) {
        infileName = "RGB.jpg";
        outfileName = "RGB.raw";
    } else {
        infileName = "gray.jpg";
        outfileName = "gray.raw";
    }

    infile.open(infileName.c_str(), ios::in | ios::binary);
    if (!infile) {
        cout << "open " << infileName << " error !!" << endl;
        exit(0);
    }
    outfile.open(outfileName.c_str(), ios::out);
    if (!outfile) {
        cout << "open " << outfileName << " error !!" << endl;
        exit(0);
    }

    initDecodeTable();
    int height = 512;
    int width = 512;
    int QF;
    int rgb = 1; // 1 is gray image 3 is rgb image

    // read header to decode
    infile >> rgb;
    infile >> width;
    infile >> height;
    infile >> QF;
    if (rgb) {
        rgb = 3;
    } else {
        rgb = 1;
    }

    infile.get();
    /*
    // it works well on mac os / linux but it will crash on windows
    // double imageData[height][width][rgb]; // use to store the decoded image data
    */
    vector<vector<vector<double>>> decodeimageData(height, vector<vector<double>>(width, vector<double>(rgb,
                                                                                                        0))); // use to store the decoded image data
    double blockData[8][8];                                                                                        // use to stroe the data recover from zigzag
    vector<int> rleData;                                                                                           // use to stroe the run length code data
    bool isDc = true;                                                                                              // use to store which stage is current state
    int remainBit = 0;                                                                                             // use to check how many bit should receive
    int dc = 0;
    int prevDC = 0;
    int blockNum = 0;
    string decode = "";
    int phase = 0;
    cout << "[+] Start decoding..." << endl;
    while (1) {
        unsigned char temp = infile.get();
        if (infile.eof()) {
            break;
        }
        for (int loop = 0; loop < 8; loop++) {
            if (blockNum >
                ((width / 8) * (height / 8) + (width / 16) * (height / 16) * 2 - 1)) // prevent decode too much block
            {
                break;
            }
            if ((int) temp & 0x80) // highest bit is 1
            {
                decode = decode + '1';
            } else {
                decode = decode + '0';
            }
            temp <<= 1;
            remainBit--;
            if (isDc) // decode DC
            {
                if (remainBit < 0) // not find how many bit should receive
                {
                    if (phase == 0) // decoding Y
                    {
                        if (dcYDecodeTable.find(decode) == dcYDecodeTable.end()) // not found in y dc table
                        {
                            continue;
                        } else {
                            remainBit = dcYDecodeTable.find(decode)->second;
                        }
                    } else // decoding cb/cr
                    {
                        if (dcCbCrDecodeTable.find(decode) == dcCbCrDecodeTable.end()) // not found in cb cr dc table
                        {
                            continue;
                        } else {
                            remainBit = dcCbCrDecodeTable.find(decode)->second;
                        }
                    }

                    if (remainBit == 0) // if DPCM dc is 0
                    {
                        dc = 0 + prevDC;
                        rleData.push_back(dc);
                        prevDC = dc;
                        isDc = false;
                    }
                    decode = "";
                } else if (remainBit == 0) {
                    dc = bit2int(decode);
                    dc = dc + prevDC;
                    rleData.push_back(dc);
                    prevDC = dc;
                    isDc = false;
                    decode = "";
                }
            } else {
                if (remainBit < 0) {
                    if ((acYDecodeTable.find(decode) == acYDecodeTable.end() && phase == 0)) // not found in y ac table
                    {
                        continue;
                    } else if ((acCbCrDecodeTable.find(decode) == acCbCrDecodeTable.end() &&
                                phase != 0)) // not found in cb cr ac table
                    {
                        continue;
                    } else {
                        // if receive EOB
                        if ((acYDecodeTable.find(decode) != acYDecodeTable.end() &&
                             acYDecodeTable.find(decode)->second == "0/0" && phase == 0) ||
                            (acCbCrDecodeTable.find(decode) != acCbCrDecodeTable.end() &&
                             acCbCrDecodeTable.find(decode)->second == "0/0" && phase != 0)) {
                            // set rleData size to 64
                            for (int i = rleData.size(); i < 64; i++) {
                                rleData.push_back(0);
                            }
                            // tranfer the block to the original block (before zigzag)
                            for (int i = 0; i < 64; i++) {
                                blockData[i / 8][i % 8] = rleData[zigzag[i]];
                            }

                            // dequantize
                            if (phase == 0) // Y
                            {
                                dequantize(blockData, true, QF);
                            } else // Cb/Cr
                            {
                                dequantize(blockData, false, QF);
                            }

                            // IDCT
                            double result[8][8];
                            IDCT((double **) blockData, (double **) result);

                            // write image data
                            for (int i = 0; i < 8; i++) {
                                for (int j = 0; j < 8; j++) {
                                    if (phase == 0) {
                                        decodeimageData[
                                                (((blockNum % ((height / 8) * (width / 8))) / (width / 8)) * 8) + i][
                                                (((blockNum % ((height / 8) * (width / 8))) % (width / 8)) * 8) +
                                                j][phase] = result[i][j] + 128;
                                    } else {
                                        int start;
                                        if (phase == 1) {
                                            start = (blockNum % ((height / 8) * (width / 8)));
                                        }
                                        if (phase == 2) {
                                            start = (blockNum % ((height / 8) * (width / 8) +
                                                                 ((height / 2 / 8) * (width / 2 / 8))));
                                        }
                                        decodeimageData[((start / (width / 8 / 2)) * 16) + i * 2][
                                                ((start % (width / 8 / 2)) * 16) + j * 2][phase] = result[i][j];
                                        decodeimageData[((start / (width / 8 / 2)) * 16) + i * 2 + 1][
                                                ((start % (width / 8 / 2)) * 16) +
                                                j * 2][phase] = result[i][j];     // down
                                        decodeimageData[((start / (width / 8 / 2)) * 16) + i * 2][
                                                ((start % (width / 8 / 2)) * 16) + j * 2 +
                                                1][phase] = result[i][j];     // right
                                        decodeimageData[((start / (width / 8 / 2)) * 16) + i * 2 + 1][
                                                ((start % (width / 8 / 2)) * 16) + j * 2 +
                                                1][phase] = result[i][j]; // right down
                                    }
                                }
                            }

                            blockNum++;
                            if (blockNum == ((height / 8) * (width / 8))) // Y is done
                            {
                                isDc = true;
                                prevDC = 0;
                                phase = 1;
                            }
                            if (blockNum ==
                                ((height / 8) * (width / 8) + ((height / 2 / 8) * (width / 2 / 8)))) // Cb is done
                            {
                                isDc = true;
                                prevDC = 0;
                                phase = 2;
                            }
                            decode = "";
                            isDc = true;
                            rleData.clear();
                            rleData.shrink_to_fit();

                            // clean blockData
                            for (int i = 0; i < 8; i++) {
                                for (int j = 0; j < 8; j++) {
                                    blockData[i][j] = 0;
                                }
                            }
                        } else {
                            // how many bits should receive
                            if (phase == 0) {
                                if (acYDecodeTable.find(decode)->second[2] == 'A') {
                                    remainBit = 10;
                                } else {
                                    remainBit = (int) (acYDecodeTable.find(decode)->second[2] - 48);
                                }
                                // how many zero
                                if (acYDecodeTable.find(decode)->second[0] >= 'A') {
                                    for (int i = 0; i < (int) (acYDecodeTable.find(decode)->second[0] - 55); i++) {
                                        rleData.push_back(0);
                                    }
                                } else {
                                    for (int i = 0; i < (int) (acYDecodeTable.find(decode)->second[0] - 48); i++) {
                                        rleData.push_back(0);
                                    }
                                }
                                decode = "";
                            } else {
                                if (acCbCrDecodeTable.find(decode)->second[2] == 'A') {
                                    remainBit = 10;
                                } else {
                                    remainBit = (int) (acCbCrDecodeTable.find(decode)->second[2] - 48);
                                }
                                // how many zero
                                if (acCbCrDecodeTable.find(decode)->second[0] >= 'A') {
                                    for (int i = 0; i < (int) (acCbCrDecodeTable.find(decode)->second[0] - 55); i++) {
                                        rleData.push_back(0);
                                    }
                                } else {
                                    for (int i = 0; i < (int) (acCbCrDecodeTable.find(decode)->second[0] - 48); i++) {
                                        rleData.push_back(0);
                                    }
                                }
                                decode = "";
                            }
                        }
                    }
                } else if (remainBit == 0) {
                    rleData.push_back(bit2int(decode));
                    decode = "";
                }
            }
        }
    }

    // rgb image should transfer ycbcr back to rgb
    if (isRGB) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                double y, cb, cr;
                y = decodeimageData[i][j][0];
                cb = decodeimageData[i][j][1];
                cr = decodeimageData[i][j][2];
                ycbcr2rgb(y, cb, cr, decodeimageData[i][j][0], decodeimageData[i][j][1], decodeimageData[i][j][2]);
            }
        }
    }

    // write image data to file
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < rgb; k++) {
                outfile << (unsigned char) round(decodeimageData[i][j][k]);
            }
        }
    }
    cout << "[+] Decode complete!!" << endl;
}

void PSNR(bool isRGB, int width, int height, string filename) {
    ifstream originalFile;
    originalFile.open(filename.c_str(), ios::in | ios::binary);
    ifstream compressedFile;
    if (isRGB) {
        compressedFile.open("RGB.raw", ios::in | ios::binary);
    } else {
        compressedFile.open("gray.raw", ios::in | ios::binary);
    }
    if (!originalFile || !compressedFile) {
        cout << "open file error!!" << endl;
        exit(0);
    }

    double MSE = 0;
    double PSNR;
    if (isRGB) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                for (int k = 0; k < 3; k++) {
                    MSE += pow(originalFile.get() - compressedFile.get(), 2);
                }
            }
        }
        MSE = MSE / 3.0 / (height * width);
    } else {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                MSE += pow(((int) originalFile.get() - (int) compressedFile.get()), 2);
            }
        }
        MSE = MSE / (height * width);
    }
    PSNR = 10 * log10(255.0 * 255.0 / MSE);
    cout << "[+] PSNR = " << PSNR << endl;
}

void menu() {
    cout << "which function to use?" << endl;
    cout << "1) encode gray level image and decode" << endl;
    cout << "2) encode color image and decode" << endl;
    cout << "3) exit" << endl;
    cout << "> ";
}

void test() // just for quick testing
{
    int qf[6] = {90, 80, 50, 20, 10, 5};
    string filename[4] = {"GrayImages/Baboon.raw", "GrayImages/Lena.raw", "ColorImages/BaboonRGB.raw",
                          "ColorImages/LenaRGB.raw"};
    for (int i = 0; i < 4; i++) {
        cout << "encoding \"" << filename[i] << "\"" << endl;
        for (int j = 0; j < 6; j++) {
            cout << "quality factor = " << qf[j] << endl;
            if (i < 2) {
                encode(false, 512, 512, filename[i], qf[j]);
                decode(false);
                PSNR(false, 512, 512, filename[i]);
            } else {
                encode(true, 512, 512, filename[i], qf[j]);
                decode(true);
                PSNR(true, 512, 512, filename[i]);
            }
        }
        cout << endl;
    }
}