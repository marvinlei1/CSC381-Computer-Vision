#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class point {
    public:
        int row, col;

};

class chainCode {
    public:
        int numRows, numCols, minVal, maxVal, label, **imgAry, **reconstructAry, zeroTable[8] = {6, 6, 0, 0, 2, 2, 4, 4}, lastQ, chainDir;
        
        point coordOffset[8], neighborCoord[8], startP, currentP, nextP;

        chainCode() {
            coordOffset[0].row = 0;
            coordOffset[0].col = 1;
            coordOffset[1].row = -1;
            coordOffset[1].col = 1;
            coordOffset[2].row = -1;
            coordOffset[2].col = 0;
            coordOffset[3].row = -1;
            coordOffset[3].col = -1;
            coordOffset[4].row = 0;
            coordOffset[4].col = -1;
            coordOffset[5].row = 1;
            coordOffset[5].col = -1;
            coordOffset[6].row = 1;
            coordOffset[6].col = 0;
            coordOffset[7].row = 1;
            coordOffset[7].col = 1;
        }

        void loadImage(ifstream &inFile) {
            for (int i = 1; i <= numRows; i++)
                for (int j = 1; j <= numCols; j++)
                    inFile >> imgAry[i][j];
        }

        void getChainCode(ofstream &chainCodeFile) {
            chainCodeFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
            for (int i = 1; i <= numRows; i++)
                for (int j = 1; j <= numCols; j++) {
                    if (imgAry[i][j] > 0) {
                        startP.row = i;
                        startP.col = j;
                        chainCodeFile << imgAry[i][j] << " " << i << " " << j << " ";
                        label = imgAry[i][j];
                        currentP.row = i;
                        currentP.col = j;
                        lastQ = 4;
                        i = numRows + 1;
                        j = numCols + 1;
                    }
                }

            do {
                lastQ = (lastQ + 1) % 8;
                chainDir = findNextP();
                chainCodeFile << chainDir << " ";
                nextP = neighborCoord[chainDir];
                currentP = nextP;
                lastQ = zeroTable[chainDir];
            } while (currentP.row != startP.row || currentP.col != startP.col);
        }

        void loadNeighborsCoord() {
            for (int i = 0; i < 8; i++) {
                neighborCoord[i].row = coordOffset[i].row + currentP.row;
                neighborCoord[i].col = coordOffset[i].col + currentP.col;
            }
        }

        int findNextP() {
            loadNeighborsCoord();
            int index = lastQ;
            while (true) {
                if (imgAry[neighborCoord[index].row][neighborCoord[index].col] == label)
                    return index;
                index = (index + 1) % 8;
            }
        }

        void constructBoundary1(ifstream &inFile) {
            int x, y, code;
            inFile >> numRows >> numCols >> minVal >> maxVal >> label >> x >> y;
            reconstructAry[x][y] = label;
            while (inFile >> code) {
                x += coordOffset[code].row;
                y += coordOffset[code].col;
                reconstructAry[x][y] = label;
            }
        }

        void constructBoundary2(ifstream &inFile) {
            int x, y, code;
            inFile >> numRows >> numCols >> minVal >> maxVal >> label >> x >> y;
            reconstructAry[x][y] = label;
            while (inFile >> code) {
                x += coordOffset[code].row;
                y += coordOffset[code].col;
                reconstructAry[x][y] = label;
                if (reconstructAry[x + coordOffset[zeroTable[code]].row][y + coordOffset[zeroTable[code]].col] != label)
                    reconstructAry[x + coordOffset[zeroTable[code]].row][y + coordOffset[zeroTable[code]].col] = -1;
            }
        }
        

        /*
        First method: (very slow, many passes)
        from top to bottom of each column, mark spot as -1 until it reaches label
        from bottom to top of each column, mark spot as -1 until it reaches label
        from left to right of each row, mark spot as -1 until it reaches label
        from right to left of each row, mark spot as -1 until it reaches label
        After these 4 passes, we will loop the following until no more changes are made.

        from top to bottom, left to right, if spot is -1, look at the right and bottom of current spot. If any are 0, set them to -1
        from bottom to top, right to left, if spot is -1, look at the left and up of current spot. If any are 0, set them to -1

        final pass - relabel
        from top to bottom, left to right, if spot is -1, spot = 0. If spot is 0, spot = label
        */
        void method1() {
            bool found;
            bool changed = true;
            for (int i = 1; i <= numCols; i++) {
                found = false;
                for (int j = 1; j <= numRows && !found; j++) {
                    if (reconstructAry[j][i] == label) 
                        found = true;
                    else
                        reconstructAry[j][i] = -1;
                }
            }

            for (int i = numCols; i >= 1; i--) {
                found = false;
                for (int j = numRows; j >= 1 && !found; j--) {
                    if (reconstructAry[j][i] == label) 
                        found = true;
                    else
                        reconstructAry[j][i] = -1;
                }
            }

            for (int i = 1; i <= numRows; i++) {
                found = false;
                for (int j = 1; j <= numCols && !found; j++) {
                    if (reconstructAry[i][j] == label) 
                        found = true;
                    else
                        reconstructAry[i][j] = -1;
                }
            }

            for (int i = numRows; i >= 1; i--) {
                found = false;
                for (int j = numCols; j >= 1 && !found; j--) {
                    if (reconstructAry[i][j] == label) 
                        found = true;
                    else
                        reconstructAry[i][j] = -1;
                }
            }

            while (changed) {
                changed = false;
                for (int i = 1; i < numRows; i++) {
                    for (int j = 1; j < numCols; j++) {
                        if (reconstructAry[i][j] == -1 && reconstructAry[i][j + 1] == 0) {
                            reconstructAry[i][j + 1] = -1;
                            changed = true;
                        }
                        if (reconstructAry[i][j] == -1 && reconstructAry[i + 1][j] == 0) {
                            reconstructAry[i + 1][j] = -1;
                            changed = true;
                        }
                    }
                }

                for (int i = numRows; i > 1; i--) {
                    for (int j = numCols; j > 1; j--) {
                        if (reconstructAry[i][j] == -1 && reconstructAry[i][j - 1] == 0) {
                            reconstructAry[i][j - 1] = -1;
                            changed = true;
                        }
                        if (reconstructAry[i][j] == -1 && reconstructAry[i - 1][j] == 0) {
                            reconstructAry[i - 1][j] = -1;
                            changed = true;
                        }
                    }
                }
            }

            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    if (reconstructAry[i][j] == 0)
                        reconstructAry[i][j] = label;
                    else if (reconstructAry[i][j] == -1)
                        reconstructAry[i][j] = 0;
                }
            }
        }


        /* Second method : (very fast, 2-3 passes)
        During the construction of the boundary, we set the last zero to -1 so that we know where the boundary ends.
        Then we can fill the object by looking at each column from top to bottom. If it is -1, the starting point of fill sets to -1. If it is = to label, we check if there has already been a starting point. If starting point exist, we fill from start to current with label and set start to current row. If start point doesn't exist, we set start point to next row.
        */
        void method2() {
            int start;
            for (int i = 1; i <= numCols; i++) {
                start = -1;
                for (int j = 1; j <= numRows; j++) {
                    if (reconstructAry[j][i] == -1) {
                        start = -1;
                        reconstructAry[j][i] = 0;
                    } else if (reconstructAry[j][i] == label) {
                        if (start == -1)
                            start = j + 1;
                        else {
                            for (; start < j; start++)
                                reconstructAry[start][i] = label;
                        }
                    }
                }
            }
        }

        void ary2File(ofstream &outFile) {
            outFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
            string str = to_string(maxVal);
            int width = str.length(), WW;
            for (int r = 1; r <= numRows; r++) {
                for (int c = 1; c <= numCols; c++) {
                    outFile << (reconstructAry[r][c] > 0 ? to_string(reconstructAry[r][c]) : ".");
                    str = to_string(reconstructAry[r][c]);
                    WW = str.length();
                    outFile << " ";
                    while (WW++ < width)
                        outFile << " ";
                }
                outFile << endl;
            }
        }

        void ary2File2(ofstream &outFile) {
            outFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
            string str = to_string(maxVal);
            int width = str.length(), WW;
            for (int r = 1; r <= numRows; r++) {
                for (int c = 1; c <= numCols; c++) {
                    outFile << reconstructAry[r][c];
                    str = to_string(reconstructAry[r][c]);
                    WW = str.length();
                    outFile << " ";
                    while (WW++ < width)
                        outFile << " ";
                }
                outFile << endl;
            }
        }
};

int main(int argc, char **argv) {
	ifstream inFile, inChainCodeFile;
    ofstream outChainCodeFile, boundaryFile, deCompressedFile, prettyPrintFile;
    inFile.open(argv[1]);
    outChainCodeFile.open(string(argv[1]) + "_chainCode.txt");
    boundaryFile.open(string(argv[1]) + "_boundary.txt");
    deCompressedFile.open(string(argv[1]) + "_deCompressed.txt");
    prettyPrintFile.open(string(argv[1]) + "_prettyPrint.txt");

    chainCode *cc = new chainCode();

    inFile >> cc->numRows >> cc->numCols >> cc->minVal >> cc->maxVal;

    cc->imgAry = new int*[cc->numRows + 2];
    cc->reconstructAry = new int*[cc->numRows + 2];
    for (int i = 0; i < cc->numRows + 2; i++) {
        cc->imgAry[i] = new int[cc->numCols + 2];
        cc->reconstructAry[i] = new int[cc->numCols + 2];
    }

    for (int i = 0; i < cc->numRows + 2; i++)
        for (int j = 0; j < cc->numCols + 2; j++) {
            cc->imgAry[i][j] = 0;
            cc->reconstructAry[i][j] = 0;
        }

    cc->loadImage(inFile);

    cc->getChainCode(outChainCodeFile);

    outChainCodeFile.close();

    inChainCodeFile.open(string(argv[1]) + "_chainCode.txt");

    int whichMethod = stoi(argv[2]);
    switch (whichMethod) {
        case 1:
            cc->constructBoundary1(inChainCodeFile);
            cc->ary2File(boundaryFile);
            cc->method1();
            break;
        case 2:
            cc->constructBoundary2(inChainCodeFile);
            cc->ary2File(boundaryFile);
            cc->method2();
            break;
        default:
            cout << "argv[2] only accept 1 or 2";
            inChainCodeFile.close();
            boundaryFile.close();
            deCompressedFile.close();
            prettyPrintFile.close();
            inFile.close();
            exit(0);
    }

    cc->ary2File(prettyPrintFile);
    cc->ary2File2(deCompressedFile);

    inChainCodeFile.close();
    boundaryFile.close();
    deCompressedFile.close();
    prettyPrintFile.close();
    inFile.close();
	return 0;
}