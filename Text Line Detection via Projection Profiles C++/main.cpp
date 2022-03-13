#include <iostream>
#include <fstream>

using namespace std;

class docImage {
        class box {
            public:
                int minR, minC, maxR, maxC;

                box() {}

                box(int minR, int minC, int maxR, int maxC) {
                    this->minR = minR;
                    this->minC = minC;
                    this->maxR = maxR;
                    this->maxC = maxC;
                }
        };

        class boxNode {
            public:
                int boxType;
                box *BBox;
                boxNode *next;

                boxNode() {
                    boxType = -1;
                    next = NULL;
                }

                boxNode(int boxType, int minR, int minC, int maxR, int maxC) {
                    this->boxType = boxType;
                    BBox = new box(minR, minC, maxR, maxC);
                    next = NULL;
                }
        };

        class boxQ {
            public:
                boxNode *Qfront, *Qback;

                boxQ() {
                    boxNode *dummy = new boxNode();
                    Qfront = Qback = dummy;
                }

                void insertQ(boxNode *newNode) {
                    Qback->next = newNode;
                    Qback = newNode;
                }
        };

    public:
        int numRows, numCols, minVal, maxVal, numStructRows = 1, numStructCols = 3, structMin = 1, structMax = 1, rowOrigin = 0, colOrigin = 1, **imgAry, structElem[3] = {1, 1, 1}, *HPP, *VPP, *binHPP, *binVPP, *morphHPP, *morphVPP, thrVal, runsHPP, runsVPP, readingDirection;
        
        boxQ QHead;
        boxNode *zoneBox;

        docImage() {}

        docImage(int numRows, int numCols, int minVal, int maxVal, int thrVal) {
            this->numRows = numRows;
            this->numCols = numCols;
            this->minVal = minVal;
            this->maxVal = maxVal;
            this->thrVal = thrVal;

            imgAry = new int*[numRows + 2];
            for (int i = 0; i < numRows + 2; i++)
                imgAry[i] = new int[numCols + 2];
            
            for (int i = 0; i < numRows + 2; i++)
                for (int j = 0; j < numCols + 2; j++)
                    imgAry[i][j] = 0;

            HPP = new int[numRows + 2];
            VPP = new int[numCols + 2];
            binHPP = new int[numRows + 2];
            binVPP = new int[numCols + 2];
            morphHPP = new int[numRows + 2];
            morphVPP = new int[numCols + 2];

            for (int i = 0; i < numRows + 2; i++) {
                HPP[i] = 0;
                binHPP[i] = 0;
                morphHPP[i] = 0;
            }

            for (int i = 0; i < numCols + 2; i++) {
                VPP[i] = 0;
                binVPP[i] = 0;
                morphVPP[i] = 0;
            }
        }

        void loadImage(ifstream &inFile) {
            for (int i = 1; i <= numRows; i++)
                for (int j = 1; j <= numCols; j++)
                    inFile >> imgAry[i][j];
        }

        void computePP() {
            for (int i = 1; i <= numRows; i++)
                for (int j = 1; j <= numCols; j++) {
                    if (imgAry[i][j] > 0) {
                        HPP[i]++;
                        VPP[j]++;
                    }
                }
        }

        void threshold(int *PP, int *binPP, int size) {
            for (int i = 1; i <= size; i++)
                binPP[i] = PP[i] < thrVal ? 0 : 1;
        }

        void printPP(int *ary, int size, string caption, ofstream &outFile) {
            outFile << caption << endl;
            for (int i = 1; i <= size; i++)
                outFile << ary[i] << " ";
            outFile << endl;
        }

        void computeZoneBox() {
            int minR = 1, minC = 1, maxR = numRows, maxC = numCols;

            while (binHPP[minR] == 0 && minR <= numRows)
                if (binHPP[minR] == 0)
                    minR++;

            while (binHPP[maxR] == 0 && maxR >= 1)
                if (binHPP[maxR] == 0)
                    maxR--;

            while (binVPP[minC] == 0 && minC <= numCols)
                if (binVPP[minC] == 0)
                    minC++;

            while (binVPP[maxC] == 0 && maxC >= 1)
                if (binVPP[maxC] == 0)
                    maxC--;

            zoneBox = new boxNode(3, minR, minC, maxR, maxC);
            insertQ(zoneBox);
        }

        void morphClosing(int *PP, int *morphPP, int size) {
            int *temp = new int[size + 2];
            for (int i = 1; i <= size; i++)
                temp[i] = 0;

            for (int i = 1; i <= size; i++) {
                if (PP[i] > 0) {
                    int jOffset = i - colOrigin;
                    for (int cIndex = 0; cIndex < numStructCols; cIndex++)
                        if (structElem[cIndex] > 0)
                            temp[jOffset + cIndex] = 1;
                }
            }

            for (int i = 1; i <= size; i++) {
                if (temp[i] > 0) {
                    bool matchFlag = true;
                    int jOffset = i - colOrigin;
                    for (int cIndex = 0; cIndex < numStructCols; cIndex++)
                        if (structElem[cIndex] > 0 && temp[jOffset + cIndex] <= 0)
                            matchFlag = false;

                    morphPP[i] = matchFlag ? 1 : 0;
                }
            }
            delete[] temp;
        }

        void insertQ(boxNode *newNode) {
            QHead.insertQ(newNode);
        }

        int computePPruns(int *PP, int lastIndex) {
            int numRuns = 0, index = 1;
            while (index <= lastIndex) {
                while (PP[index] == 0 && index <= lastIndex)
                    index++;
                while (PP[index] > 0 && index <= lastIndex)
                    index++;
                numRuns++;
            }
            return numRuns;
        }

        void computeDirection(ofstream &outFile) {
            int factor = 2;
            readingDirection = 0;
            if (runsHPP <= 2 && runsVPP <= 2)
                outFile << "the zone may be a non-text zone" << endl;
            else if (runsHPP >= factor * runsVPP) {
                outFile << "the document reading direction is horizontal!" << endl;
                readingDirection = 1;
            } else if (runsVPP >= factor * runsHPP) {
                outFile << "the document reading direction is vertical!" << endl;
                readingDirection = 2;
            } else
                outFile << "the zone may be a non-text zone" << endl;

        }

        void computeTBoxHorizontal() {
            int minR = zoneBox->BBox->minR, minC = zoneBox->BBox->minC, maxR = minR, maxC = zoneBox->BBox->maxC;

            while (morphHPP[maxR] == 0 && maxR <= numRows)
                maxR++;

            minR = maxR;

            while (minR <= numRows) {
                while (morphHPP[maxR] > 0 && maxR <= numRows)
                    maxR++;

                boxNode *newNode = new boxNode(4, minR, minC, maxR - 1, maxC);
                insertQ(newNode);

                minR = maxR;

                while (morphHPP[minR] == 0 && minR <= numRows)
                    minR++;

                maxR = minR;
            }
        }

        void computeTBoxVertical() {
            int minR = zoneBox->BBox->minR, minC = zoneBox->BBox->minC, maxR = zoneBox->BBox->maxR, maxC = minC;

            while (morphVPP[maxC] == 0 && maxC <= numCols)
                maxC++;
            
            minC = maxC;

            while (minC <= numCols) {
                while (morphVPP[maxC] > 0 && maxC <= numCols)
                    maxC++;

                boxNode *newNode = new boxNode(4, minR, minC, maxR, maxC - 1);
                insertQ(newNode);

                minC = maxC;

                while (morphVPP[minC] == 0 && minC <= numCols)
                    minC++;

                maxC = minC;
            }
        }

        void reformatPrettyPrint(ofstream &outFile) {
            outFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
            string str = to_string(maxVal);
            int width = str.length(), WW;
            for (int r = 1; r <= numRows; r++) {
                for (int c = 1; c <= numCols; c++) {
                    outFile << (imgAry[r][c] > 0 ? to_string(imgAry[r][c]) : ".");
                    str = to_string(imgAry[r][c]);
                    WW = str.length();
                    outFile << " ";
                    while (WW++ < width)
                        outFile << " ";
                }
                outFile << endl;
            }
        }

        void overlayBox() {
            boxNode *temp = QHead.Qfront->next;
            int minRow, minCol, maxRow, maxCol;
            while (temp != NULL) {
                minRow = temp->BBox->minR;
                minCol = temp->BBox->minC;
                maxRow = temp->BBox->maxR;
                maxCol = temp->BBox->maxC;

                for (int i = minCol; i <= maxCol; i++) {
                    imgAry[minRow][i] = 9;
                    imgAry[maxRow][i] = 9;
                }
                for (int i = minRow; i <= maxRow; i++) {
                    imgAry[i][minCol] = 9;
                    imgAry[i][maxCol] = 9;
                }

                temp = temp->next;
            }
        }

        void printBoxQueue(ofstream &outFile) {
            outFile << "Box Queue" << endl;
            boxNode *temp = QHead.Qfront->next;
            while (temp != NULL) {
                outFile << temp->boxType << endl << temp->BBox->minR << " " << temp->BBox->minC << " " << temp->BBox->maxR << " " << temp->BBox->maxC << endl;
                temp = temp->next;
            }
        }
};

int main(int argc, char **argv) {

    ifstream inFile;
    ofstream outFile1, outFile2;

    inFile.open(argv[1]);
    outFile1.open(argv[3]);
    outFile2.open(argv[4]);

    int numRows, numCols, minVal, maxVal;

    inFile >> numRows >> numCols >> minVal >> maxVal;

    docImage *doc = new docImage(numRows, numCols, minVal, maxVal, atoi(argv[2]));

    doc->loadImage(inFile);
    doc->reformatPrettyPrint(outFile1);

    doc->computePP();
    doc->printPP(doc->HPP, doc->numRows, "HPP", outFile2);
    doc->printPP(doc->VPP, doc->numCols, "VPP", outFile2);

    doc->threshold(doc->HPP, doc->binHPP, doc->numRows);
    doc->threshold(doc->VPP, doc->binVPP, doc->numCols);
    doc->printPP(doc->binHPP, doc->numRows, "binHPP", outFile2);
    doc->printPP(doc->binVPP, doc->numCols, "binVPP", outFile2);

    doc->computeZoneBox();
    doc->printBoxQueue(outFile2);

    doc->morphClosing(doc->binHPP, doc->morphHPP, doc->numRows);
    doc->morphClosing(doc->binVPP, doc->morphVPP, doc->numCols);
    doc->printPP(doc->morphHPP, doc->numRows, "morphHPP", outFile2);
    doc->printPP(doc->morphVPP, doc->numCols, "morphVPP", outFile2);

    doc->runsHPP = doc->computePPruns(doc->morphHPP, doc->numRows);
    doc->runsVPP = doc->computePPruns(doc->morphVPP, doc->numCols);
    doc->printPP(doc->morphHPP, doc->numRows, "morphHPP", outFile2);
    doc->printPP(doc->morphVPP, doc->numCols, "morphVPP", outFile2);

    doc->computeDirection(outFile1);

    if (doc->readingDirection == 1)
        doc->computeTBoxHorizontal();
    else if (doc->readingDirection == 2)
        doc->computeTBoxVertical();

    doc->overlayBox();

    doc->reformatPrettyPrint(outFile1);

    doc->printBoxQueue(outFile1);


    outFile2.close();
    outFile1.close();
    inFile.close();
} 
