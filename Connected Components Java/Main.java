import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.BufferedWriter;
import java.io.IOException;
import java.util.Scanner;

class CClabel {
    public int numRows, numCols, minVal, maxVal, newMin, newMax, newLabel, trueNumCC, zeroFramedArray[][], NonZeroNeighborAry[], EQAry[];

    public Property CCproperty[];

    CClabel(int numRows, int numCols, int minVal, int maxVal) {
        this.numRows = numRows;
        this.numCols = numCols;
        this.minVal = minVal;
        this.maxVal = maxVal;
        zeroFramedArray = new int[numRows + 2][numCols + 2];
        NonZeroNeighborAry = new int[5];
        EQAry = new int[numRows * numCols / 4];
        for (int i = 0; i < numRows * numCols / 4; i++)
            EQAry[i] = i;
        newLabel = 0;
    }

    public void zero2D() {
        for (int i = 0; i < numRows + 2; i++)
            for (int j = 0; j < numCols + 2; j++)
                zeroFramedArray[i][j] = 0;
    }

    public void loadImage(File inFile) {
        try {
            Scanner reader = new Scanner(inFile);
            reader.nextLine();
            for (int i = 1; i <= numRows; i++) {
                for (int j = 1; j <= numCols; j++) {
                    zeroFramedArray[i][j] = reader.nextInt();
                }
            }
            reader.close();
        } catch (FileNotFoundException e) {
        }
    }

    public void imgReformat(File RFprettyPrintFile) {
        try {
            BufferedWriter bw = new BufferedWriter(new FileWriter(RFprettyPrintFile, true));
            bw.write(numRows + " " + numCols + " " + newMin + " " + newMax + "\n");
            String str = String.valueOf(newLabel);
            int width = str.length(), WW;
            for (int r = 0; r < numRows + 2; r++) {
                for (int c = 0; c < numCols + 2; c++) {
                    bw.write(String.valueOf(zeroFramedArray[r][c]));
                    str = String.valueOf(zeroFramedArray[r][c]);
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

    public void connect8Pass1() {
        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
                if (zeroFramedArray[i][j] > 0) {
                    int lowest = -1;
                    boolean sameLabel = true;
                    if (zeroFramedArray[i - 1][j - 1] == 0 && zeroFramedArray[i - 1][j] == 0 && zeroFramedArray[i - 1][j + 1] == 0 && zeroFramedArray[i][j - 1] == 0) {
                        zeroFramedArray[i][j] = ++newLabel;
                    } else {
                        if (zeroFramedArray[i - 1][j - 1] > 0)
                            if (lowest == -1) {
                                lowest = zeroFramedArray[i - 1][j - 1];
                            }
                        if (zeroFramedArray[i - 1][j] > 0)
                            if (lowest == -1) {
                                lowest = zeroFramedArray[i - 1][j];
                            }
                            else if (zeroFramedArray[i - 1][j] != lowest) {
                                sameLabel = false;
                                if (zeroFramedArray[i - 1][j] < lowest) {
                                    lowest = zeroFramedArray[i - 1][j];
                                }
                            }
                        if (zeroFramedArray[i - 1][j + 1] > 0)
                            if (lowest == -1) {
                                lowest = zeroFramedArray[i - 1][j + 1];
                            }
                            else if (zeroFramedArray[i - 1][j + 1] != lowest) {
                                sameLabel = false;
                                if (zeroFramedArray[i - 1][j + 1] < lowest) {
                                    lowest = zeroFramedArray[i - 1][j + 1];
                                }
                            }
                        if (zeroFramedArray[i][j - 1] > 0)
                            if (lowest == -1) {
                                lowest = zeroFramedArray[i][j - 1];
                            }
                            else if (zeroFramedArray[i][j - 1] != lowest) {
                                sameLabel = false;
                                if (zeroFramedArray[i][j - 1] < lowest) {
                                    lowest = zeroFramedArray[i][j - 1];
                                }
                            }
                        zeroFramedArray[i][j] = lowest;
                        if (!sameLabel) {
                            if (zeroFramedArray[i - 1][j - 1] > 0 && zeroFramedArray[i - 1][j - 1] != lowest)
                                updateEQ(zeroFramedArray[i - 1][j - 1], lowest);
                            if (zeroFramedArray[i - 1][j] > 0 && zeroFramedArray[i - 1][j] != lowest)
                                updateEQ(zeroFramedArray[i - 1][j], lowest);
                            if (zeroFramedArray[i - 1][j + 1] > 0 && zeroFramedArray[i - 1][j + 1] != lowest)
                                updateEQ(zeroFramedArray[i - 1][j + 1], lowest);
                            if (zeroFramedArray[i][j - 1] > 0 && zeroFramedArray[i][j - 1] != lowest)
                                updateEQ(zeroFramedArray[i][j - 1], lowest);    
                        }
                    }
                }
            }
        }
        newMax = newLabel;
    }

    public void connect8Pass2() {
        for (int i = numRows; i > 0; i--) {
            for (int j = numCols; j > 0; j--) {
                if (zeroFramedArray[i][j] > 0) {
                    int lowest = zeroFramedArray[i][j];
                    boolean sameLabel = true;
                    if (zeroFramedArray[i + 1][j + 1] > 0)
                        if (zeroFramedArray[i + 1][j + 1] != lowest) {
                            sameLabel = false;
                            if (zeroFramedArray[i + 1][j + 1] < lowest)
                                lowest = zeroFramedArray[i + 1][j + 1];
                        }
                    if (zeroFramedArray[i + 1][j] > 0)
                        if (zeroFramedArray[i + 1][j] != lowest) {
                            sameLabel = false;
                            if (zeroFramedArray[i + 1][j] < lowest) {
                                lowest = zeroFramedArray[i + 1][j];
                            }
                        }
                    if (zeroFramedArray[i + 1][j - 1] > 0)
                        if (zeroFramedArray[i + 1][j - 1] != lowest) {
                            sameLabel = false;
                            if (zeroFramedArray[i + 1][j - 1] < lowest) {
                                lowest = zeroFramedArray[i + 1][j - 1];
                            }
                        }
                    if (zeroFramedArray[i][j + 1] > 0)
                        if (zeroFramedArray[i][j + 1] != lowest) {
                            sameLabel = false;
                            if (zeroFramedArray[i][j + 1] < lowest) {
                                lowest = zeroFramedArray[i][j + 1];
                            }
                        }
                    if (!sameLabel) {
                        zeroFramedArray[i][j] = lowest;
                        if (zeroFramedArray[i + 1][j + 1] > 0 && zeroFramedArray[i + 1][j + 1] != lowest)
                            updateEQ(zeroFramedArray[i + 1][j + 1], lowest);
                        if (zeroFramedArray[i + 1][j] > 0 && zeroFramedArray[i + 1][j] != lowest)
                            updateEQ(zeroFramedArray[i + 1][j], lowest);
                        if (zeroFramedArray[i + 1][j - 1] > 0 && zeroFramedArray[i + 1][j - 1] != lowest)
                            updateEQ(zeroFramedArray[i + 1][j - 1], lowest);
                        if (zeroFramedArray[i][j + 1] > 0 && zeroFramedArray[i][j + 1] != lowest)
                            updateEQ(zeroFramedArray[i][j + 1], lowest);    
                    }
                }
            }
        }
    }

    public void connect4Pass1() {
        for (int i = 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++) {
                if (zeroFramedArray[i][j] > 0) {
                    if (zeroFramedArray[i - 1][j] == 0 && zeroFramedArray[i][j - 1] == 0) {
                        zeroFramedArray[i][j] = ++newLabel;
                    } else if (zeroFramedArray[i - 1][j] == zeroFramedArray[i][j - 1]) {
                        zeroFramedArray[i][j] = zeroFramedArray[i - 1][j];
                    } else if (zeroFramedArray[i - 1][j] != zeroFramedArray[i][j - 1]) {
                        if (zeroFramedArray[i - 1][j] == 0) {
                            zeroFramedArray[i][j] = zeroFramedArray[i][j - 1];
                        } else if (zeroFramedArray[i][j - 1] == 0) {
                            zeroFramedArray[i][j] = zeroFramedArray[i - 1][j];
                        } else if (zeroFramedArray[i - 1][j] < zeroFramedArray[i][j - 1]) {
                            zeroFramedArray[i][j] = zeroFramedArray[i - 1][j];
                            updateEQ(zeroFramedArray[i][j - 1], zeroFramedArray[i][j]);
                        } else {
                            zeroFramedArray[i][j] = zeroFramedArray[i][j - 1];
                            updateEQ(zeroFramedArray[i - 1][j], zeroFramedArray[i][j]);
                        }
                    }
                }
            }
        }
        newMax = newLabel;
    }

    public void connect4Pass2() {
        for (int i = numRows; i > 0; i--) {
            for (int j = numCols; j > 0; j--) {
                if (zeroFramedArray[i][j] > 0) {
                    if (zeroFramedArray[i + 1][j] == 0 && zeroFramedArray[i][j + 1] == 0) {
                        
                    } else if (zeroFramedArray[i + 1][j] == zeroFramedArray[i][j + 1]) {
                        if (zeroFramedArray[i][j] > zeroFramedArray[i + 1][j]) {
                            updateEQ(zeroFramedArray[i][j], zeroFramedArray[i + 1][j]);
                            zeroFramedArray[i][j] = zeroFramedArray[i + 1][j];
                        } else if (zeroFramedArray[i][j] < zeroFramedArray[i + 1][j]) {
                            updateEQ(zeroFramedArray[i + 1][j], zeroFramedArray[i][j]);
                        }                       
                    } else if (zeroFramedArray[i + 1][j] != zeroFramedArray[i][j + 1]) {
                        int lowest = zeroFramedArray[i][j];
                        if (zeroFramedArray[i + 1][j] > 0 && zeroFramedArray[i + 1][j] < lowest)
                            lowest = zeroFramedArray[i + 1][j];
                        if (zeroFramedArray[i][j + 1] > 0 && zeroFramedArray[i][j + 1] < lowest)
                            lowest = zeroFramedArray[i][j + 1];
                        if (lowest != zeroFramedArray[i][j]) {
                            updateEQ(zeroFramedArray[i][j], lowest);
                            zeroFramedArray[i][j] = lowest;
                        }
                        if (zeroFramedArray[i + 1][j] > lowest)
                            updateEQ(zeroFramedArray[i + 1][j], lowest);
                        if (zeroFramedArray[i][j + 1] > lowest)
                            updateEQ(zeroFramedArray[i][j + 1], lowest);
                    }
                }
            }
        }
    }

    public void connectPass3() {
        int labelIndex;
        for (int i = 1; i <= numRows; i++)
            for (int j = 1; j <= numCols; j++) {
                zeroFramedArray[i][j] = EQAry[zeroFramedArray[i][j]];
                labelIndex = zeroFramedArray[i][j];
                if (labelIndex > 0) {
                    if (CCproperty[labelIndex].numpixels++ == 0) {
                        CCproperty[labelIndex].minR = i - 1;
                        CCproperty[labelIndex].minC = j - 1;
                        CCproperty[labelIndex].maxR = i - 1;
                        CCproperty[labelIndex].maxC = j - 1;
                    } else {
                        CCproperty[labelIndex].numpixels++;
                        if (CCproperty[labelIndex].minR > i)
                            CCproperty[labelIndex].minR = i - 1;
                        if (CCproperty[labelIndex].maxR < i)
                            CCproperty[labelIndex].maxR = i - 1;
                        if (CCproperty[labelIndex].minC > j)
                            CCproperty[labelIndex].minC = j - 1;
                        if (CCproperty[labelIndex].maxC < j)
                            CCproperty[labelIndex].maxC = j - 1;
                    }
                }
            }

    }

    public void drawBoxes() {
        int minRow, minCol, maxRow, maxCol, label;
        for (int index = 1; index <= trueNumCC; index++) {
            minRow = CCproperty[index].minR + 1;
            minCol = CCproperty[index].minC + 1;
            maxRow = CCproperty[index].maxR + 1;
            maxCol = CCproperty[index].maxC + 1;
            label = CCproperty[index].label;

            for (int i = minCol; i <= maxCol; i++) {
                zeroFramedArray[minRow][i] = label;
                zeroFramedArray[maxRow][i] = label;
            }
            for (int i = minRow; i <= maxRow; i++) {
                zeroFramedArray[i][minCol] = label;
                zeroFramedArray[i][maxCol] = label;
            }
        }

    }

    public void updateEQ(int index, int newValue) {
        EQAry[index] = newValue;
    }

    public int manageEQAry() {
        int realLabel = -1;
        for (int i = 0; i <= newLabel; i++)
            if (EQAry[i] == i)
                EQAry[i] = ++realLabel;
            else
                EQAry[i] = EQAry[EQAry[i]];
        newMin = 0;
        newMax = realLabel;
        CCproperty = new Property[realLabel + 1];
        for (int i = 1; i <= realLabel; i++) {
            CCproperty[i] = new Property(i);
        }
        return realLabel;
    }

    public void printCCproperty(File propertyFile) {
        try {
            BufferedWriter bw = new BufferedWriter(new FileWriter(propertyFile, true));
            bw.write(numRows + " " + numCols + " " + newMin + " " + newMax + "\n" + trueNumCC + "\n");

            for (int i = 1; i <= trueNumCC; i++)
                bw.write(CCproperty[i].label + "\n" + CCproperty[i].numpixels + "\n" + CCproperty[i].minR + "\t" + CCproperty[i].minC + "\n" + CCproperty[i].maxR + "\t" + CCproperty[i].maxC + "\n");
            bw.close();
        } catch (IOException e) {
        }
    }

    public void printEQAry(File RFprettyPrintFile) {
        try {
            BufferedWriter bw = new BufferedWriter(new FileWriter(RFprettyPrintFile, true));
            bw.write("EQAry\n");
            for (int i = 0; i <= newLabel; i++)
                bw.write("[" + i + "]=" + EQAry[i] + " ");
            bw.write("\n");
            bw.close();
        } catch (IOException e) {
        }
    }

    public void printImg(File labelFile) {
        try {
            BufferedWriter bw = new BufferedWriter(new FileWriter(labelFile, true));
            String str = String.valueOf(newLabel);
            int width = str.length(), WW;
            for (int r = 1; r <= numRows; r++) {
                for (int c = 1; c <= numCols; c++) {
                    bw.write(String.valueOf(zeroFramedArray[r][c]));
                    str = String.valueOf(zeroFramedArray[r][c]);
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

}

class Property {
    public int label = 0, numpixels = 0, minR = 0, minC = 0, maxR = 0, maxC = 0;

    Property(int label) {
        this.label = label;
    }
};

class Main {
    public static void main(String[] args) {
        File inFile, RFprettyPrintFile, labelFile, propertyFile;

        inFile = new File(args[0]);
        RFprettyPrintFile = new File(args[1]);
        labelFile = new File(args[2]);
        propertyFile = new File(args[3]);

        CClabel cc;
        int numRows = 0, numCols = 0, minVal = 0, maxVal = 0, connectedness;

        try {
            Scanner reader = new Scanner(inFile);
            numRows = reader.nextInt();
            numCols = reader.nextInt();
            minVal = reader.nextInt();
            maxVal = reader.nextInt();
            reader.close();
        } catch (FileNotFoundException e) {
        }

        cc = new CClabel(numRows, numCols, minVal, maxVal);
        cc.zero2D();
        cc.loadImage(inFile);

        Scanner input = new Scanner(System.in);
        System.out.println("Enter 4 or 8");
        connectedness = input.nextInt();
        input.close();

        if (connectedness == 4) {
            cc.connect4Pass1();
            cc.imgReformat(RFprettyPrintFile);
            cc.printEQAry(RFprettyPrintFile);
            cc.connect4Pass2();
            cc.imgReformat(RFprettyPrintFile);
            cc.printEQAry(RFprettyPrintFile);
        }

        if (connectedness == 8) {
            cc.connect8Pass1();
            cc.imgReformat(RFprettyPrintFile);
            cc.printEQAry(RFprettyPrintFile);
            cc.connect8Pass2();
            cc.imgReformat(RFprettyPrintFile);
            cc.printEQAry(RFprettyPrintFile);
        }
        
        cc.trueNumCC = cc.manageEQAry();
        cc.printEQAry(RFprettyPrintFile);

        cc.connectPass3();
        cc.imgReformat(RFprettyPrintFile);
        cc.printEQAry(RFprettyPrintFile);

        try {
            BufferedWriter bw = new BufferedWriter(new FileWriter(labelFile, true));
            bw.write(cc.numRows + " " + cc.numCols + " " + cc.newMin + " " + cc.newMax + "\n");
            bw.close();
        } catch (IOException e) {
        }
        cc.printImg(labelFile);
        cc.printCCproperty(propertyFile);
        cc.drawBoxes();
        cc.imgReformat(RFprettyPrintFile);

        try {
            BufferedWriter bw = new BufferedWriter(new FileWriter(RFprettyPrintFile, true));
            bw.write("trueNumCC : " + cc.trueNumCC);
            bw.close();
        } catch (IOException e) {
        }
    }
}