#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <vector>

using namespace std;

// Node structure for Huffman tree
struct Node {
    unsigned char data; // Character data
    int freq; // Frequency of the character
    Node *left, *right; // Pointers to the left and right child

    // Constructor to initialize a node
    Node(unsigned char data, int freq) {
        left = right = NULL; // Initially, both children are set to NULL
        this->data = data;
        this->freq = freq;
    }
};

// Custom comparator for priority queue to build min heap based on frequency
struct compare {
    bool operator()(Node *left, Node *right) {
        return (left->freq > right->freq);
    }
};

// Recursive function to output Huffman tree as a header in compressed file
void outputHeader(Node *node, ofstream &outfile) {
    if (!node) return;
    if (!node->left && !node->right) {
        outfile << "1" << node->data; // Leaf node prefix '1' followed by character
    } else {
        outfile << "0"; // Internal node prefix '0'
    }
    outputHeader(node->left, outfile);
    outputHeader(node->right, outfile);
}

// Function to calculate and display the entropy of the data
void entropy(map<unsigned char, int> dataFreq, int dataSize) {
    double entropy = 0;
    for (auto iter = dataFreq.begin(); iter != dataFreq.end(); ++iter) {
        double probability = iter->second * 1.0 / dataSize;
        entropy += (probability * log2(probability));
    }
    cout << "[+] Entropy = " << -entropy << endl;
}

// Reconstruct Huffman tree from file header
void rebuildHuffmanTree(Node *node, int &count, int limit, ifstream &infile) {
    if (count != limit) {
        if (infile.get() == '0') {
            node->left = new Node('\x00', 0);
            node->right = new Node('\x00', 0);
            rebuildHuffmanTree(node->left, count, limit, infile);
            rebuildHuffmanTree(node->right, count, limit, infile);
        } else {
            node->data = infile.get();
            node->freq = 1;
            count++;
            return;
        }
    }
}

// Build Huffman tree from character frequencies
Node *buildHuffmanTree(map<unsigned char, int> filedata) {
    priority_queue<Node *, vector<Node *>, compare> minHeap;

    // Insert all characters and their frequencies into the min heap
    for (auto iter = filedata.begin(); iter != filedata.end(); ++iter) {
        minHeap.push(new Node(iter->first, iter->second));
    }

    // Build Huffman tree
    while (minHeap.size() != 1) {
        Node *left = minHeap.top();
        minHeap.pop();
        Node *right = minHeap.top();
        minHeap.pop();
        Node *top = new Node('\x00', left->freq + right->freq); // Internal node
        top->left = left;
        top->right = right;
        minHeap.push(top);
    }
    return minHeap.top();
}

// Generate Huffman codes for characters
void createCodeTable(Node *node, string str, map<unsigned char, string> &input) {
    if (!node) return;
    if (!node->left && !node->right) {
        input[node->data] = str; // Assign code to leaf node
    }
    createCodeTable(node->left, str + "0", input); // Append '0' for left traversal
    createCodeTable(node->right, str + "1", input); // Append '1' for right traversal
}

// Compress input file using Huffman coding
void compress() {
    // Implementation of compression function
    // Includes file handling, Huffman tree building, encoding, and writing to output file
    string inFilename;
    ifstream infile;
    ofstream outfile;
    cout << "Enter the file to compress: ";
    cin >> inFilename;
    infile.open(inFilename.c_str(), ios::in | ios::binary);
    if (!infile) {
        cout << "open " << inFilename << " error!!" << endl;
        exit(0);
    }
    string outFileName = "compressed";
    cout << "[+] The compressd file will be named \"" << outFileName << "\"" << endl;
    outfile.open(outFileName.c_str(), ios::out | ios::binary);
    if (!outfile) {
        cout << "open " << outFileName << " error!!" << endl;
        exit(0);
    }
    map<unsigned char, int> dataFreq; // use map to compute each data's frequency
    while (1) {
        unsigned char temp = infile.get();
        if (infile.eof()) {
            break;
        }
        dataFreq[temp]++;
    }
    infile.clear();
    int originalSize = infile.tellg();
    Node *root = buildHuffmanTree(dataFreq);
    map<unsigned char, string> codeTable;
    createCodeTable(root, "", codeTable);

    cout << "[+] Compressing..." << endl;
    infile.seekg(0, ios::beg);
    int counter = 0;                    // use to count how many bits in buffer
    int buf = 0;                        // buffer to store the compressed data
    outfile << dataFreq.size() << endl; // write how many data use in this file for decode to reconstruct huffman tree
    outputHeader(root, outfile);
    while (1) {
        unsigned char temp = infile.get();
        if (infile.eof()) {
            break;
        }
        for (int i = 0; i < codeTable[temp].size(); i++) {
            if (codeTable[temp][i] == '1') {
                buf++;
            }
            counter++;
            if (counter == 8) // the buffer is full write to compressed file
            {
                outfile << (unsigned char) buf;
                counter = 0;
                buf = 0;
            } else {
                buf = buf << 1;
            }
        }
    }
    if (counter != 0) // if there is still some data in buffer
    {
        for (int i = counter; i < 8; i++) {
            buf = buf << 1;
        }
        outfile << (unsigned char) buf;
    }
    cout << "[+] Compress complete!!" << endl;
    int compressSize = outfile.tellp();
    float compressRate = (compressSize * 1.0 / originalSize) * 100;
    cout << "[+] Original file size = " << originalSize << " byte" << endl;
    cout << "[+] Compressed file size = " << compressSize << " byte" << endl;
    cout << "[+] Compress rate = " << compressRate << " %" << endl;
    entropy(dataFreq, originalSize);
}

// Compress input file using DPCM and Huffman coding
void compressDPCM(int width = 256, int height = 256) {
    // Implementation of DPCM compression function
    // Includes reading image data, applying DPCM, Huffman coding, and writing to output file
    string inFileName;
    cout << "Enter the file to compress: ";
    cin >> inFileName;
    ifstream infile;
    ofstream outfile;
    infile.open(inFileName.c_str(), ios::in | ios::binary);
    if (!infile) {
        cout << "Can't open " << inFileName << " !" << endl;
        exit(0);
    }
    string outFileName = "compressed_DPCM";
    cout << "[+] The compressd file will be named \"" << outFileName << "\"" << endl;
    outfile.open(outFileName.c_str(), ios::out | ios::binary);
    if (!outfile) {
        cout << "Can't open " << outFileName << " !" << endl;
        exit(0);
    }

    map<unsigned char, int> dataFreq; // use to compute data frequency
    vector<unsigned char> errBuf;     // use to store the difference between the data and last data
    vector<unsigned char> rebuildBuf; // use errbuff to rebuild original image
    int originalSize;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0) {
                errBuf.push_back((infile.get() - 128) / 2 + 128);
                rebuildBuf.push_back(128 + (errBuf.back() - 128) * 2);
            } else {
                errBuf.push_back((infile.get() - rebuildBuf.back()) / 2 + 128);
                rebuildBuf.push_back(rebuildBuf.back() + (errBuf.back() - 128) * 2);
            }
            dataFreq[errBuf.back()]++;
        }
    }
    originalSize = infile.tellg();

    Node *root = buildHuffmanTree(dataFreq);
    map<unsigned char, string> codeTable;
    createCodeTable(root, "", codeTable);
    cout << "[+] Compressing..." << endl;
    outfile << dataFreq.size() << endl;
    outfile << height << endl;
    outfile << width << endl;
    outputHeader(root, outfile);
    int counter = 0;
    int buf = 0;
    // use huffman encode to encode errbuf
    for (int i = 0; i < errBuf.size(); i++) {
        for (int j = 0; j < codeTable[errBuf[i]].size(); j++) {
            if (codeTable[errBuf[i]][j] == '1') {
                buf++;
            }
            counter++;
            if (counter == 8) {
                outfile << (unsigned char) buf;
                buf = 0;
                counter = 0;
            } else {
                buf = buf << 1;
            }
        }
    }
    if (counter != 0) {
        for (int i = counter; i < 8; i++) {
            buf = buf << 1;
        }
        outfile << (unsigned char) buf;
    }
    cout << "[+] Compress complete!!" << endl;
    int compressSize = outfile.tellp();
    float compressRate = (compressSize * 1.0 / originalSize) * 100;
    cout << "[+] Original file size = " << originalSize << " byte" << endl;
    cout << "[+] Compressed file size = " << compressSize << " byte" << endl;
    cout << "[+] Compress rate = " << compressRate << " %" << endl;
    entropy(dataFreq, originalSize);
}

// Decompress file using Huffman coding
void decompress() {
    // Implementation of decompression function
    // Includes reading compressed file, reconstructing Huffman tree, and decoding data
    ifstream infile;
    ofstream outfile;
    string infileName = "compressed";
    string outfileName = "extract";
    infile.open(infileName.c_str(), ios::in | ios::binary);
    if (!infile) {
        cout << "Can't open " << infileName << endl;
        exit(0);
    }
    outfile.open(outfileName.c_str(), ios::out | ios::binary);
    if (!outfile) {
        cout << "Can't open " << outfileName;
        exit(0);
    }
    Node *root = new Node('\x00', 0);
    int limit, count = 0;
    infile >> limit; // read how many data use in the original file
    infile.get();    // use to read the '\n' in the header
    rebuildHuffmanTree(root, count, limit, infile);

    cout << "[+] Decompressing..." << endl;
    Node *tempNode = root;
    while (1) {
        unsigned char temp = infile.get();
        if (infile.eof()) {
            break;
        }
        for (int i = 0; i < 8; i++) {
            if (((int) temp & 0x80) != 0) // if the result is not 0 means the highest bit is 1
            {
                tempNode = tempNode->right;
            } else // the highest bit is 0
            {
                tempNode = tempNode->left;
            }
            if (tempNode->freq == 1) {
                outfile << tempNode->data;
                tempNode = root;
            }
            temp = temp << 1;
        }
    }
    cout << "[+] Decompress complete!!" << endl;
}

// Decompress file using DPCM and Huffman coding
void decompressDPCM() {
    // Implementation of DPCM decompression function
    // Includes reading compressed file, reconstructing Huffman tree, decoding, and DPCM inversion
    ifstream infile;
    ofstream outfile;
    string infileName = "compressed_DPCM";
    string outfileName = "extract_DPCM";
    infile.open(infileName.c_str(), ios::in | ios::binary);
    if (!infile) {
        cout << "Can't open " << infileName << endl;
        exit(0);
    }
    outfile.open(outfileName.c_str(), ios::out | ios::binary);
    if (!outfile) {
        cout << "Can't open " << outfileName;
        exit(0);
    }
    Node *root = new Node('\x00', 0);
    int limit, count = 0, height, width;
    infile >> limit; // read how many data use in the original file
    infile.get();    // use to read the '\n' in the header
    infile >> height;
    infile.get();
    infile >> width;
    infile.get();
    rebuildHuffmanTree(root, count, limit, infile);

    cout << "[+] Decompressing..." << endl;
    vector<unsigned char> errBuf;
    vector<unsigned char> rebuildBuf;
    Node *tempNode = root;
    // decompress to get the difference between each data and it's left data
    while (1) {
        unsigned char temp = infile.get();
        if (infile.eof()) {
            break;
        }
        for (int i = 0; i < 8; i++) {
            if (((int) temp & 0x80) != 0) {
                tempNode = tempNode->right;
            } else {
                tempNode = tempNode->left;
            }
            if (tempNode->freq == 1) {
                errBuf.push_back(tempNode->data);
                tempNode = root;
            }
            temp = temp << 1;
        }
    }
    // use errbuf to rebuild the original image
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0) {
                rebuildBuf.push_back(128 + (errBuf[i * width + j] - 128) * 2);
            } else {
                rebuildBuf.push_back(rebuildBuf.back() + (errBuf[i * width + j] - 128) * 2);
            }
        }
    }
    // write the rebuild image to get decompressed file
    for (int i = 0; i < rebuildBuf.size(); i++) {
        outfile << rebuildBuf[i];
    }
    cout << "[+] Decompress DPCM complete!!" << endl;
}

int menu() {
    int choice;
    // Display menu and return user's choice
    cout << "Which function to use?" << endl;
    cout << "1) compress" << endl;
    cout << "2) compress with DPCM" << endl;
    cout << "3) decompress" << endl;
    cout << "4) decompress DPCM" << endl;
    cout << "5) exit" << endl;
    cout << "> ";
    cin >> choice;
    return choice;
}

int main() {
    int choice = menu();
    // Main function to execute based on user choice
    switch (choice) {
        case 1:
            compress();
            break;
        case 2:
            compressDPCM();
            break;
        case 3:
            decompress();
            break;
        case 4:
            decompressDPCM();
            break;
        case 5:
            return 0; // Exit program
        default:
            cout << "Invalid choice!!" << endl;
            break;
    }
    cout << endl;
    return 0;
}