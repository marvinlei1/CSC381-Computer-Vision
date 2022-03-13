import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.BufferedWriter;
import java.io.IOException;
import java.util.Scanner;

class imageProcessing {
  public int numRows, numCols, minVal, maxVal, maskRows, maskCols, maskMin, maskMax, maskWeight, newMin, newMax, thrVal, mirrorFramedAry[][], medianAry[][], GaussAry[][], thrAry[][], maskAry[][], neighborAry[];

  public void threshold(int ary1[][], int ary2[][]) {
    newMin = 0;
    newMax = 1;
    for (int i = 1; i < numRows + 2; i++) {
      for (int j = 1; j < numCols + 2; j++) {
        if (ary1[i][j] >= thrVal)
          ary2[i][j] = 1;
        else
          ary2[i][j] = 0;
      }
    }
  }

  public void imgReformat (int inAry[][], int newMin, int newMax, File OutImg) {
    try {
      BufferedWriter bw = new BufferedWriter(new FileWriter(OutImg, true));
      bw.write(numRows + " " + numCols + " " + newMin + " " + newMax + "\n");
      String str = String.valueOf(newMax);
      int width = str.length(), WW;
      for (int r = 1; r <= numRows; r++) {
        for (int c = 1; c <= numCols; c++) {
          bw.write(String.valueOf(inAry[r][c]));
          str = String.valueOf(inAry[r][c]);
          WW = str.length();
          bw.write(" ");
          while (WW++ < width) {
            bw.write(" ");
          }
        }
        bw.write("\n");
      }
      bw.close();
    } catch (IOException e) {
    }
  }

  public void mirrorFraming() {
    for (int i = 1; i < numRows + 1; i++) {
      mirrorFramedAry[i][0] = mirrorFramedAry[i][1];
      mirrorFramedAry[i][numRows + 1] = mirrorFramedAry[i][numRows];
    }
    for (int i = 0; i < numCols + 2; i++) {
      mirrorFramedAry[0][i] = mirrorFramedAry[1][i];
      mirrorFramedAry[numRows + 1][i] = mirrorFramedAry[numRows][i];
    }
  }

  public void loadImage(File inFile) {
    try {
      Scanner reader = new Scanner(inFile);
      reader.nextLine();
      for (int i = 1; i <= numRows; i++) {
        for (int j = 1; j <= numCols; j++) {
          mirrorFramedAry[i][j] = reader.nextInt();
        }
      }
      reader.close();
    } catch (FileNotFoundException e) {
    }
  }

  public void loadMask(File maskFile) {
    maskWeight = 0;
    try {
      Scanner reader = new Scanner(maskFile);
      reader.nextLine();
      for (int i = 0; i < maskRows; i++) {
        for (int j = 0; j < maskCols; j++) {
          maskAry[i][j] = reader.nextInt();
          maskWeight += maskAry[i][j];
        }
      }
      reader.close();
    } catch (FileNotFoundException e) {
    }
  }

  public void loadNeighbors(int i, int j) {
    int index = 0;
    for (int r = i - 1; r <= i + 1; r++) 
      for (int c = j - 1; c <= j + 1; c++)
        neighborAry[index++] = mirrorFramedAry[r][c];
  }

  public void sort() {
    int min;
    for (int i = 0; i < 9; i++) {
      min = i;
      for (int j = i + 1; j < 9; j++) {
        if (neighborAry[j] < neighborAry[min])
          min = j;
      }
      if (min != i) {
        int temp = neighborAry[i];
        neighborAry[i] = neighborAry[min];
        neighborAry[min] = temp;
      }
    }
  }

  public void computeMedian() {
    newMin = 9999;
    newMax = 0;
    for (int i = 1; i <= numRows; i++) {
      for (int j = 1; j <= numRows; j++) {
        loadNeighbors(i, j);
        sort();
        medianAry[i][j] = neighborAry[4];
        if (newMin > medianAry[i][j])
          newMin = medianAry[i][j];
        if (newMax < medianAry[i][j])
          newMax = medianAry[i][j];
      }
    }
  }

  public void computeGauss() {
    newMin = 9999;
    newMax = 0;
    for (int i = 1; i <= numRows; i++) {
      for (int j = 1; j <= numCols; j++) {
        GaussAry[i][j] = convolution(i, j);
        if (newMin > GaussAry[i][j])
          newMin = GaussAry[i][j];
        if (newMax < GaussAry[i][j])
          newMax = GaussAry[i][j];
      }
    }
  }

  public int convolution(int i, int j) {
    int sum = 0;
    int x = 0;
    int y = 0;
    for (int r = i - 1; r <= i + 1; r++) {
      for (int c = j - 1; c <= j + 1; c++) {
        sum += (mirrorFramedAry[r][c] * maskAry[x][y++]);
      }
      x++;
      y = 0;
    }
    return sum/maskWeight;
  }

  public void prettyPrint(int inAry[][], File outFile) {
    try {
      BufferedWriter bw = new BufferedWriter(new FileWriter(outFile, true));
      for (int i = 1; i <= numRows; i++) {
        for (int j = 1; j <= numCols; j++) {
          if (inAry[i][j] > 0)
            bw.write(inAry[i][j] + " ");
          else
            bw.write(". ");
        }
        bw.write("\n");
      }
      bw.close();
    } catch (IOException e) {
    }
  }
}


class Main {
  public static void main(String[] args) {
    File inFile, maskFile, inputImg, MedianOutImg, MedianThrImg, MedianPrettyPrint, GaussOutImg, GaussThrImg, GaussPrettyPrint;
    imageProcessing img = new imageProcessing();

    inFile = new File(args[0]);
    maskFile = new File(args[1]);
    inputImg = new File(args[3]);
    MedianOutImg = new File(args[4]);
    MedianThrImg = new File(args[5]);
    MedianPrettyPrint = new File(args[6]);
    GaussOutImg = new File(args[7]);
    GaussThrImg = new File(args[8]);
    GaussPrettyPrint = new File(args[9]);
    
    img.thrVal = Integer.parseInt(args[2]);

    try {
      Scanner reader = new Scanner(inFile);
      img.numRows = reader.nextInt();
      img.numCols = reader.nextInt();
      img.minVal = reader.nextInt();
      img.maxVal = reader.nextInt();
      reader.close();
      reader = new Scanner(maskFile);
      img.maskRows = reader.nextInt();
      img.maskCols = reader.nextInt();
      img.maskMin = reader.nextInt();
      img.maskMax = reader.nextInt();
      reader.close();
    } catch (FileNotFoundException e) {
    }
    img.mirrorFramedAry = new int[img.numRows + 2][img.numCols + 2];
    img.medianAry = new int[img.numRows + 2][img.numCols + 2];
    img.GaussAry = new int[img.numRows + 2][img.numCols + 2];
    img.thrAry = new int[img.numRows + 2][img.numCols + 2];
    img.maskAry = new int[img.maskRows][img.maskCols];
    img.neighborAry = new int[9];
    img.loadMask(maskFile);
    img.loadImage(inFile);
    img.mirrorFraming();
    img.imgReformat(img.mirrorFramedAry, img.minVal, img.maxVal, inputImg);
    img.computeMedian();
    img.imgReformat(img.medianAry, img.newMin, img.newMax, MedianOutImg);
    img.threshold(img.medianAry, img.thrAry);
    img.imgReformat(img.thrAry, img.newMin, img.newMax, MedianThrImg);
    img.prettyPrint(img.thrAry, MedianPrettyPrint);
    img.computeGauss();
    img.imgReformat(img.GaussAry, img.newMin, img.newMax, GaussOutImg);
    img.threshold(img.GaussAry, img.thrAry);
    img.imgReformat(img.thrAry, img.newMin, img.newMax, GaussThrImg);
    img.prettyPrint(img.thrAry, GaussPrettyPrint);
  }
}