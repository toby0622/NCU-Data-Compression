#include "jpeg.h"

#define TRUE 1

int main() {
    int choice;
    string filename;
    int QF;

    while (TRUE) {
        menu();
        cin >> choice;
        switch (choice) {
            case 1:
                cout << "Enter which file to encode: ";
                cin >> filename;
                cout << "Enter the quality factor to encode: ";
                cin >> QF;
                encode(false, 512, 512, filename, QF);
                decode(false);
                PSNR(false, 512, 512, filename);
                break;
            case 2:
                cout << "Enter which file to encode: ";
                cin >> filename;
                cout << "Enter the quality factor to encode: ";
                cin >> QF;
                encode(true, 512, 512, filename, QF);
                decode(true);
                PSNR(true, 512, 512, filename);
                break;
            case 3:
                exit(0);
            default:
                cout << "Invalid choice!" << endl;
                break;
        }
        cout << endl;
    }
}