/*
* Brittany Arneson
* CSCI 405 Winter 2017
* Program takes in a file name of a two lines, first is the min interval, second is a sequence of values
* the maximum sum of values of the sequence is found with no value positons being closer than the min interval
*/

import java.util.*;
import java.io.*;
import java.lang.*;

class Nuggets {

  static int[] positions;

  public static void main(String[] args){
    Scanner input = new Scanner(System.in);
    String fileName = input.next();
    try {
      Scanner file = new Scanner(new File(fileName));
      int minInterval = file.nextInt();

      int[] nuggets = getNuggets(file);
      int [] nugCounts = fillArray(nuggets, minInterval);

      getOptimalSubsequence(nugCounts, nuggets, minInterval);

    }catch (Exception e){
      System.err.println("Error " + e);
    }
  }
/*
* Finds optimal subsequence and builds string to print values and positions
*/
  public static void getOptimalSubsequence(int[] nuggetCounts, int[] nuggets, int interval){

    int length = nuggetCounts.length;
    int sum = nuggetCounts[length - 1];
    //find the optimal subsequence by moving backward from the highest value.
    for(int i = length - 1; i > 0; i--){
      if(positions[i] == 1){
        int zeroOut = interval;
        if(i < interval){
          zeroOut = i + 1;
        }
        for(int j = 1; j < zeroOut; j++){
          positions[i - j] = 0;
        }
        i = i - interval + 1;
      }
    }
    //build string display by going back through the positions array
    StringBuilder valOutput = new StringBuilder("the optimal subsequence is ");
    StringBuilder posOutput = new StringBuilder(", consisting of positions ");
    for(int i = 0; i < length; i++){
      if (positions[i] == 1){
        if(nuggetCounts[i] != sum){
          valOutput.append(nuggets[i] + " + ");
          posOutput.append(i + ", ");
        }else {
          valOutput.append(nuggets[i]);
          posOutput.append(i);
        }
      }
    }
    valOutput.append(" = " + sum);
    posOutput.append(".");
    valOutput.append(posOutput);
    String output = valOutput.toString();
    System.out.println(output);
  }
/*
* Function creates an array that is filled with sums of subsequences from the nuggets array
* Also keeps track of where the subsequences are being added to in the positions array
*/
  public static int[] fillArray(int[] nuggets, int interval){
    //initialize
    int numNugs = nuggets.length;
    int[] counts = new int[numNugs];
    positions = new int[numNugs];

    //update counts and positions with the values of the first nugget
    counts[0] = nuggets[0];
    positions[0] = 1;

    for(int i = 1; i < numNugs; i++){
      // nug_0 to nug_INTERVAL-1 counts are just the value of the nugget because i - interval is null
      if((i - interval ) < 0){
        int choice1 = nuggets[i];
        int choice2 = counts[i - 1];
        counts[i] = Math.max(choice1, choice2);
        // if the count value is the value of the nugget and not a previous one
        // then it is part of a subsequence and should be tracked in positions
        if (counts[i] == choice1){
          positions[i] = 1;
        }
      }else {
        int choice1 = counts[i - interval] + nuggets[i];
        int choice2 = counts[i - 1];
        counts[i] = Math.max(choice1, choice2);
        if (counts[i] == choice1){
          positions[i] = 1;
        }
      }
    }
    return counts;
  }
  /*
  *reads input file and returns the nugget values as an array;
  */
  public static int[] getNuggets(Scanner file){
    //minInterval is read without reading the newLine, this removes it
    file.nextLine();

    //gets exact length that array should be
    String nugs = file.nextLine();
    String[] tokens = nugs.split(" ");
    int len = tokens.length;

    int[] nuggets = new int[len];

    for(int i = 0; i < len; i++){
      nuggets[i] = Integer.parseInt(tokens[i]);
    }
    return nuggets;
  }
}
