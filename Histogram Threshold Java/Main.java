import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.BufferedWriter;
import java.io.IOException;
import java.util.Scanner;

class image {
  public int numRows, numCols, minVal, maxVal, histAry[], thresholdValue;

  public void computeHist(File inFile) {
    try {
      Scanner reader = new Scanner(inFile);
      while (reader.hasNextInt())
        histAry[reader.nextInt()]++;
      reader.close();
    } catch (FileNotFoundException e) {
    }
  }

  public void printHist(File outFile1) {
    try {
      BufferedWriter bw = new BufferedWriter(new FileWriter(outFile1, true));
      bw.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");
      for (int i = 0; i <= maxVal; i++)
        bw.write(i + " " + histAry[i] + "\n");
      bw.close();
    } catch (IOException e) {
    }
  }

  public void dispHist(File outFile2) {
    try {
      BufferedWriter bw = new BufferedWriter(new FileWriter(outFile2, true));
      bw.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");
      for (int i = 0; i <= maxVal; i++)
        bw.write(i + " (" + histAry[i] + ")" + plus(histAry[i]) + "\n");
      bw.close();
    } catch (IOException e) {
    }
  }

  public void threshold(File inFile, File outFile3, File outFile4) {
    try {
      BufferedWriter bw3 = new BufferedWriter(new FileWriter(outFile3, true));
      BufferedWriter bw4 = new BufferedWriter(new FileWriter(outFile4, true));
      bw3.write("The threshold value uses is " + thresholdValue + "\n");
      bw4.write("The threshold value uses is " + thresholdValue + "\n");
      minVal = 0;
      maxVal = 1;
      bw3.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");
      bw4.write(numRows + " " + numCols + " " + minVal + " " + maxVal + "\n");
      Scanner reader = new Scanner(inFile);
      reader.nextLine();
      int counter = 0;
      while (reader.hasNextInt()) {
        counter++;
        if (reader.nextInt() >= thresholdValue) {
          bw3.write("1" + (counter % numCols == 0 ? "\n" : " "));
          bw4.write("1" + (counter % numCols == 0 ? "\n" : " "));
        } else {
          bw3.write("0" + (counter % numCols == 0 ? "\n" : " "));
          bw4.write("." + (counter % numCols == 0 ? "\n" : " "));
        }
      }
      reader.close();
      bw3.close();
      bw4.close();
    } catch (IOException e) {
    }
  }

  public String plus(int amount) {
    String s = ":";
    for (int i = 0; i < (amount > 80 ? 80 : amount); i++)
      s += "+";
    return s;
  }
}

class Main {
  public static void main(String[] args) {
    File inFile, outFile1, outFile2, outFile3, outFile4;
    image img = new image();
    inFile = new File(args[0]);
    outFile1 = new File(args[2]);
    outFile2 = new File(args[3]);
    outFile3 = new File(args[4]);
    outFile4 = new File(args[5]);
    img.thresholdValue = Integer.parseInt(args[1]);
    try {
      Scanner reader = new Scanner(inFile);
      img.numRows = reader.nextInt();
      img.numCols = reader.nextInt();
      img.minVal = reader.nextInt();
      img.maxVal = reader.nextInt();
      img.histAry = new int[img.maxVal + 1];
      reader.close();
    } catch (FileNotFoundException e) {
    }
    img.computeHist(inFile);
    img.printHist(outFile1);
    img.dispHist(outFile2);
    img.threshold(inFile, outFile3, outFile4);
  }
}