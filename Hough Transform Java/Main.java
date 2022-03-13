import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.BufferedWriter;
import java.io.IOException;
import java.util.Scanner;
import java.lang.Math;

class HoughTransform {
    public int numRows, numCols, minVal, maxVal, HoughMinVal, HoughMaxVal, HoughDist, HoughAngle, imgAry[][], HoughAry[][], angleInDegree, offset;
    
    public double angleInRadians;

    public void loadImage(File inFile) {
        try {
            Scanner reader = new Scanner(inFile);
            reader.nextLine();
            for (int i = 0; i < numRows; i++)
                for (int j = 0; j < numCols; j++)
                    imgAry[i][j] = reader.nextInt();
            reader.close();
        } catch (FileNotFoundException e) {
        }
    }

    public void print1() {
        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++)
                System.out.print(imgAry[i][j] + " ");
            System.out.println();
        }
    }

    public void print2() {
        for (int i = 0; i < HoughDist; i++) {
            for (int j = 0; j < HoughAngle; j++)
                System.out.print(HoughAry[i][j] + " ");
            System.out.println();
        }
    }

    public void buildHoughSpace() {
        for (int x = 0; x < numRows; x++)
            for (int y = 0; y < numCols; y++)
                if (imgAry[x][y] > 0)
                    computeSinusoid(x, y);
    }

    public void computeSinusoid(int x, int y) {
        double dist;
        int distInt;
        for (angleInDegree = 0; angleInDegree <= 179; angleInDegree++) {
            angleInRadians = angleInDegree / 180.00 * Math.PI;
            dist = polarDistance((double) x, (double) y);
            distInt = (int) dist;
            HoughAry[distInt][angleInDegree]++;
        }
    }

    public double polarDistance(double x, double y) {
        return x * Math.cos(angleInRadians) + y * Math.sin(angleInRadians) + offset;
    }

    public void determineMinMax() {
        minVal = Integer.MAX_VALUE;
        maxVal = Integer.MIN_VALUE;
        for (int i = 0; i < HoughDist; i++)
            for (int j = 0; j < HoughAngle; j++) {
                if (HoughAry[i][j] > maxVal)
                    maxVal = HoughAry[i][j];
                if (HoughAry[i][j] < minVal)
                    minVal = HoughAry[i][j];
            }
        HoughMinVal = minVal;
        HoughMaxVal = maxVal;
    }

    public void prettyPrint(int ary[][], File outFile) {
        try {
            BufferedWriter bw = new BufferedWriter(new FileWriter(outFile, true));
            bw.write(HoughDist + " " + HoughAngle + " " + minVal + " " + maxVal + "\n");
            String str = String.valueOf(maxVal);
            int width = str.length(), WW;
            for (int r = 0; r < HoughDist; r++) {
                for (int c = 0; c < HoughAngle; c++) {
                    str = String.valueOf(ary[r][c]);
                    bw.write(ary[r][c] > 0 ? str : ".");
                    WW = str.length();
                    bw.write(" ");
                    while (WW++ < width)
                        bw.write(" ");
                }
                bw.write("\n");
            }
            bw.close();
        } catch (IOException e) {
        }
    }

    public void ary2File(int ary[][], File outFile) {
        try {
            BufferedWriter bw = new BufferedWriter(new FileWriter(outFile, true));
            bw.write(HoughDist + " " + HoughAngle + " " + HoughMinVal + " " + HoughMaxVal + "\n");
            String str = String.valueOf(maxVal);
            int width = str.length(), WW;
            for (int r = 0; r < HoughDist; r++) {
                for (int c = 0; c < HoughAngle; c++) {
                    str = String.valueOf(ary[r][c]);
                    bw.write(str);
                    WW = str.length();
                    bw.write(" ");
                    while (WW++ < width)
                        bw.write(" ");
                }
                bw.write("\n");
            }
            bw.close();
        } catch (IOException e) {
        }
    }

};


class Main {
    public static void main(String[] args) {
        File inFile, outFile1, outFile2;
        inFile = new File(args[0]);
        outFile1 = new File(args[1]);
        outFile2 = new File(args[2]);

        HoughTransform ht = new HoughTransform();
        try {
            Scanner reader = new Scanner(inFile);
            ht.numRows = reader.nextInt();
            ht.numCols = reader.nextInt();
            ht.minVal = reader.nextInt();
            ht.maxVal = reader.nextInt();
            reader.close();
        } catch (FileNotFoundException e) {
        }
        ht.HoughAngle = 180;
        ht.HoughDist = (int) Math.ceil(Math.sqrt(Math.pow(ht.numRows, 2) + Math.pow(ht.numCols, 2)) * 2);
        ht.offset = (int) Math.ceil(Math.sqrt(Math.pow(ht.numRows, 2) + Math.pow(ht.numCols, 2)));
        ht.imgAry = new int[ht.numRows][ht.numCols];
        ht.HoughAry = new int[ht.HoughDist][ht.HoughAngle];

        for (int i = 0; i < ht.HoughDist; i++)
            for (int j = 0; j < ht.HoughAngle; j++)
                ht.HoughAry[i][j] = 0;

        ht.loadImage(inFile);

        ht.buildHoughSpace();

        ht.determineMinMax();

        ht.prettyPrint(ht.HoughAry, outFile1);

        ht.ary2File(ht.HoughAry, outFile2);

    }
}