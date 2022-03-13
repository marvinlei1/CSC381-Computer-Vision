#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Morphology {
  public:
    int numImgRows, numImgCols, imgMin, imgMax, numStructRows, numStructCols, structMin, structMax, rowOrigin, colOrigin, rowFrameSize, colFrameSize, extraRows, extraCols, rowSize, colSize, **zeroFramedAry, **morphAry, **tempAry, **structAry;

    void zero2DAry(int **Ary, int nRows, int nCols) {
      for (int i = 0; i < nRows; i++)
        for (int j = 0; j < nCols; j++)
          Ary[i][j] = 0;
    }

    void loadImg(ifstream &imgFile, int **ary) {
      for (int i = rowOrigin; i < numImgRows + rowOrigin; i++)
        for (int j = colOrigin; j < numImgCols + colOrigin; j++)
          imgFile >> ary[i][j];
    }

    void loadstruct(ifstream &imgFile, int **ary) {
      for (int i = 0; i < numStructRows; i++)
        for (int j = 0; j < numStructCols; j++)
          imgFile >> ary[i][j];
    }

    void ComputeDilation(int **inAry, int **outAry) {
      for (int i = rowFrameSize; i < numImgRows + rowFrameSize; i++)
        for (int j = colFrameSize; j < numImgCols + colFrameSize; j++)
          if (inAry[i][j] > 0)
            onePixelDilation(i, j, inAry, outAry);
    }

    void ComputeErosion(int **inAry, int **outAry) {
      for (int i = rowFrameSize; i < numImgRows + rowFrameSize; i++)
        for (int j = colFrameSize; j < numImgCols + colFrameSize; j++)
          if (inAry[i][j] > 0)
            onePixelErosion(i, j, inAry, outAry);
    }

    void ComputeOpening(int **inAry, int **outAry, int **tmp) {
      ComputeErosion(inAry, tmp);
      ComputeDilation(tmp, outAry);
    }

    void ComputeClosing(int **inAry, int **outAry, int **tmp) {
      ComputeDilation(inAry, tmp);
      ComputeErosion(tmp, outAry);
    }

    void onePixelDilation(int i, int j, int **inAry, int **outAry) {
      int iOffset = i - rowOrigin;
      int jOffset = j - colOrigin;
      for (int rIndex = 0;  rIndex < numStructRows; rIndex++)
        for (int cIndex = 0; cIndex < numStructCols; cIndex++)
          if (structAry[rIndex][cIndex] > 0)
            outAry[iOffset + rIndex][jOffset + cIndex] = 1;
    }

    void onePixelErosion(int i, int j, int **inAry, int **outAry) {
      bool matchFlag = true;
      int iOffset = i - rowOrigin;
      int jOffset = j - colOrigin;
      for (int rIndex = 0; matchFlag && rIndex < numStructRows; rIndex++)
        for (int cIndex = 0; matchFlag && cIndex < numStructCols; cIndex++)
          if (structAry[rIndex][cIndex] > 0 && inAry[iOffset + rIndex][jOffset + cIndex] <= 0)
            matchFlag = false;    
      if (matchFlag)
        outAry[i][j] = 1;
      else
        outAry[i][j] = 0;
    }

    void AryToFile(int **Ary, ofstream &outFile) {
      outFile << numImgRows << " " << numImgCols << " " << imgMin << " " << imgMax << "\n";
      for (int i = rowOrigin; i < numImgRows + rowOrigin; i++) {
        for (int j = colOrigin; j < numImgCols + colOrigin; j++)
          outFile << Ary[i][j] << " ";
        outFile << "\n";
      }
    }

    void prettyPrint(int **ary, int rows, int cols, ofstream &outFile, string caption) {
      outFile << caption << "\n";
      for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++)
          outFile << (ary[i][j] == 0 ? "." : to_string(ary[i][j])) << " ";
        outFile << "\n";
      }
    }
};

int main(int argc, char *argv[]) {
  ifstream imgFile, structFile;
  ofstream dilateOutFile, erodeOutFile, openingOutFile, closingOutFile, prettyPrintFile;
  Morphology *m = new Morphology();
  
  imgFile.open(argv[1]);
  structFile.open(argv[2]);
  dilateOutFile.open(argv[3]);
  erodeOutFile.open(argv[4]);
  closingOutFile.open(argv[5]);
  openingOutFile.open(argv[6]);
  prettyPrintFile.open(argv[7]);
  
  imgFile >> m->numImgRows >> m->numImgCols >> m->imgMin >> m->imgMax;

  structFile >> m->numStructRows >> m->numStructCols >> m->structMin >> m->structMax >> m->rowOrigin >> m->colOrigin;

  m->rowFrameSize = m->numStructRows / 2;
  m->colFrameSize = m->numStructCols / 2;

  m->extraRows = m->rowFrameSize * 2;
  m->extraCols = m->colFrameSize * 2;
  m->rowSize = m->numImgRows + m->extraRows;
  m->colSize = m->numImgCols + m->extraCols;

  m->zeroFramedAry = new int*[m->rowSize];
  m->morphAry = new int*[m->rowSize];
  m->tempAry = new int*[m->rowSize];
  for (int i = 0; i < m->rowSize; i++) {
    m->zeroFramedAry[i] = new int[m->colSize];
    m->morphAry[i] = new int[m->colSize];
    m->tempAry[i] = new int[m->colSize];
  }

  m->structAry = new int*[m->numStructRows];
  for (int i = 0; i < m->numStructRows; i++)
    m->structAry[i] = new int[m->numStructCols];

  m->zero2DAry(m->zeroFramedAry, m->rowSize, m->colSize);

  m->loadImg(imgFile, m->zeroFramedAry);
  m->prettyPrint(m->zeroFramedAry, m->rowSize, m->colSize, prettyPrintFile, "Pretty Print of zeroFramedAry");

  m->zero2DAry(m->structAry, m->numStructRows, m->numStructCols);
  m->loadstruct(structFile, m->structAry);
  m->prettyPrint(m->structAry, m->numStructRows, m->numStructCols, prettyPrintFile, "Pretty Print of structAry");

  m->zero2DAry(m->morphAry, m->rowSize, m->colSize);
  m->ComputeDilation(m->zeroFramedAry, m->morphAry);
  m->AryToFile(m->morphAry, dilateOutFile);
  m->prettyPrint(m->morphAry, m->rowSize, m->colSize, prettyPrintFile, "Pretty Print of Dilation");

  m->zero2DAry(m->morphAry, m->rowSize, m->colSize);
  m->ComputeErosion(m->zeroFramedAry, m->morphAry);
  m->AryToFile(m->morphAry, erodeOutFile);
  m->prettyPrint(m->morphAry, m->rowSize, m->colSize, prettyPrintFile, "Pretty Print of Erosion");

  m->zero2DAry(m->morphAry, m->rowSize, m->colSize);
  m->zero2DAry(m->tempAry, m->rowSize, m->colSize);
  m->ComputeOpening(m->zeroFramedAry, m->morphAry, m->tempAry);
  m->AryToFile(m->morphAry, openingOutFile);
  m->prettyPrint(m->morphAry, m->rowSize, m->colSize, prettyPrintFile, "Pretty Print of Opening");

  m->zero2DAry(m->morphAry, m->rowSize, m->colSize);
  m->zero2DAry(m->tempAry, m->rowSize, m->colSize);
  m->ComputeClosing(m->zeroFramedAry, m->morphAry, m->tempAry);
  m->AryToFile(m->morphAry, closingOutFile);
  m->prettyPrint(m->morphAry, m->rowSize, m->colSize, prettyPrintFile, "Pretty Print of Closing");

  imgFile.close();
  structFile.close();
  prettyPrintFile.close();
  dilateOutFile.close();
  erodeOutFile.close();
  closingOutFile.close();
  openingOutFile.close();
}