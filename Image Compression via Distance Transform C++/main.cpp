#include <iostream>
#include <fstream>

using namespace std;

class ImageCompression {
    public:
        int numRows, numCols, minVal, maxVal, newMinVal, newMaxVal, **ZFAry, **skeletonAry;

        void setZero(int **Ary) {
            for (int i = 0; i < numRows + 2; i++)
                for (int j = 0; j < numCols + 2; j++)
                    Ary[i][j] = 0;
        }

        void loadImage(ifstream &inFile) {
            for (int i = 1; i <= numRows; i++)
                for (int j = 1; j <= numCols; j++)
                    inFile >> ZFAry[i][j];
        }

        void loadSkeleton(ifstream &skeletonFile) {
            skeletonFile >> numRows >> numCols >> minVal >> maxVal;
            while (!skeletonFile.eof()) {
                int row, col, value;
                skeletonFile >> row >> col >> value;
                ZFAry[row][col] = value;
            }
        }

        void compute8Distance(ofstream &outFile1) {
            firstPass8Distance();
            reformatPrettyPrint(ZFAry, outFile1, "First pass distance transform");
            secondPass8Distance();
            reformatPrettyPrint(ZFAry, outFile1, "Second pass distance transform");
        }

        void firstPass8Distance() {
            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    if (ZFAry[i][j] > 0) {
                        int min = ZFAry[i - 1][j - 1];
                        if (ZFAry[i - 1][j] < min)
                            min = ZFAry[i - 1][j];
                        if (ZFAry[i - 1][j + 1] < min)
                            min = ZFAry[i - 1][j + 1];
                        if (ZFAry[i][j - 1] < min)
                            min = ZFAry[i][j - 1];
                        ZFAry[i][j] = min + 1;
                    }
                }
            }
            minVal = maxVal = ZFAry[1][1];
            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    if (ZFAry[i][j] > maxVal)
                        maxVal = ZFAry[i][j];
                    if (ZFAry[i][j] < minVal)
                        minVal = ZFAry[i][j];
                }
            }
        }

        void secondPass8Distance() {
            for (int i = numRows; i > 0; i--) {
                for (int j = numCols; j > 0; j--) {
                    if (ZFAry[i][j] > 0) {
                        int min = ZFAry[i][j];
                        if (ZFAry[i][j + 1] + 1 < min)
                            min = ZFAry[i][j + 1] + 1;
                        if (ZFAry[i + 1][j - 1] + 1 < min)
                            min = ZFAry[i + 1][j - 1] + 1;
                        if (ZFAry[i + 1][j] + 1 < min)
                            min = ZFAry[i + 1][j] + 1;
                        if (ZFAry[i + 1][j + 1] + 1 < min)
                            min = ZFAry[i + 1][j + 1] + 1;
                        ZFAry[i][j] = min;
                    }
                }
            }
            minVal = maxVal = ZFAry[1][1];
            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    if (ZFAry[i][j] > maxVal)
                        maxVal = ZFAry[i][j];
                    if (ZFAry[i][j] < minVal)
                        minVal = ZFAry[i][j];
                }
            }
        }

        void imageCompression(ofstream &skeletonFile, ofstream &outFile) {
            computeLocalMaxima();
            reformatPrettyPrint(skeletonAry, outFile, "Local maxima, skeletonAry");
            extractSkeleton(skeletonFile);
        }

        bool isLocalMaxima(int x, int y) {
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (ZFAry[x][y] < ZFAry[x + i][y + j])
                        return false;
                }
            }
            return true;
        }

        void computeLocalMaxima() {
            for (int i = 1; i <= numRows; i++)
                for (int j = 1; j <= numCols; j++)
                    if (isLocalMaxima(i, j))
                        skeletonAry[i][j] = ZFAry[i][j];
                    else
                         skeletonAry[i][j] = 0;
            
            minVal = maxVal = skeletonAry[1][1];
            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    if (skeletonAry[i][j] > maxVal)
                        maxVal = skeletonAry[i][j];
                    if (skeletonAry[i][j] < minVal)
                        minVal = skeletonAry[i][j];
                }
            }
        }

        void extractSkeleton(ofstream &skeletonFile) {
            skeletonFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
            for (int i = 1; i <= numRows; i++)
                for (int j = 1; j <= numCols; j++)
                    if (skeletonAry[i][j] > 0)
                        skeletonFile << i << " " << j << " " << skeletonAry[i][j] << endl;
        }

        void imageDeCompression(ofstream &outFile1) {
            firstPassExpansion();
            reformatPrettyPrint(ZFAry, outFile1, "First pass expansion");
            secondPassExpansion();
            reformatPrettyPrint(ZFAry, outFile1, "Second pass expansion");
        }

        void firstPassExpansion() {
            for (int i = 1; i <= numRows; i++) 
                for (int j = 1; j <= numCols; j++) {
                    if (ZFAry[i][j] == 0) {
                        int max = ZFAry[i - 1][j - 1];
                        for (int x = -1; x <= 1; x++) {
                            for (int y = -1; y <= 1; y++) {
                                if (!(x == 0 && y == 0) && ZFAry[i + x][j + y] > max)
                                    max = ZFAry[i + x][j + y];
                            }
                        }
                        ZFAry[i][j] = (max - 1 >= 0 ? max - 1 : 0);
                    }
                }

            minVal = maxVal = ZFAry[1][1];
            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    if (ZFAry[i][j] > maxVal)
                        maxVal = ZFAry[i][j];
                    if (ZFAry[i][j] < minVal)
                        minVal = ZFAry[i][j];
                }
            }
        }

        void secondPassExpansion() {
            for (int i = numRows; i > 0; i--) 
                for (int j = numCols; j > 0; j--) {
                    int max = ZFAry[i][j];
                    for (int x = -1; x <= 1; x++) {
                        for (int y = -1; y <= 1; y++) {
                            if (ZFAry[i + x][j + y] - 1 > max)
                                max = ZFAry[i + x][j + y] - 1;
                        }
                    }
                    ZFAry[i][j] = max;
                }

            minVal = maxVal = ZFAry[1][1];
            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    if (ZFAry[i][j] > maxVal)
                        maxVal = ZFAry[i][j];
                    if (ZFAry[i][j] < minVal)
                        minVal = ZFAry[i][j];
                }
            }
        }

        void threshold(ofstream &outFile) {
            outFile << numRows << " " << numCols << " " << minVal << " " << (maxVal > 0 ? 1 : 0) << endl;
            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++)
                    outFile << (ZFAry[i][j] >= 1 ? 1 : 0) << " ";
                outFile << endl;
            }
        }

        void reformatPrettyPrint(int **Ary, ofstream &outFile, string caption) {
            outFile << caption << endl;
            outFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
            string str = to_string(maxVal);
            int width = str.length(), WW;
            for (int r = 0; r < numRows + 2; r++) {
                for (int c = 0; c < numCols + 2; c++) {
                    outFile << (Ary[r][c] > 0 ? to_string(Ary[r][c]) : ".");
                    str = to_string(Ary[r][c]);
                    WW = str.length();
                    outFile << " ";
                    while (WW++ < width)
                        outFile << " ";
                }
                outFile << endl;
            }
            outFile << endl;
        }
};

int main(int argc, char *argv[]) {
    ifstream inFile, inSkeletonFile;
    ofstream outFile1, outSkeletonFile, deCompressFile;

    inFile.open(argv[1]);
    outFile1.open(argv[2]);

    ImageCompression *ic = new ImageCompression();

    inFile >> ic->numRows >> ic->numCols >> ic->minVal >> ic->maxVal;

    ic->ZFAry = new int*[ic->numRows + 2];
    ic->skeletonAry = new int*[ic->numRows + 2];
    for (int i = 0; i < ic->numRows + 2; i++) {
        ic->ZFAry[i] = new int[ic->numCols + 2];
        ic->skeletonAry[i] = new int[ic->numCols + 2];
    }

    outSkeletonFile.open(string(argv[1]) + "_skeleton.txt");
    deCompressFile.open(string(argv[1]) + "_deCompressed.txt");

    ic->setZero(ic->ZFAry);
    ic->setZero(ic->skeletonAry);

    ic->loadImage(inFile);

    ic->compute8Distance(outFile1);

    ic->imageCompression(outSkeletonFile, outFile1);

    outSkeletonFile.close();
    inSkeletonFile.open(string(argv[1]) + "_skeleton.txt");

    ic->setZero(ic->ZFAry);

    ic->loadSkeleton(inSkeletonFile);

    ic->imageDeCompression(outFile1);

    ic->threshold(deCompressFile);

    inSkeletonFile.close();
    deCompressFile.close();
    inFile.close();
    outFile1.close();
} 