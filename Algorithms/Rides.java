import java.util.*;
import java.io.*;

class Rides{

  static Pair[] activities;
  static HashMap<Pair,LinkedList<Pair>> sets;

  public static void main(String[] args){
    Scanner input = new Scanner(System.in);
    String fileName = input.next();

    try{
      Scanner file = new Scanner(new File(fileName));

      activities = addActivies(file);
      sets = new HashMap<Pair,LinkedList<Pair>>();
      int numActivities = activities.length;

      int maxIndex = numActivities;
      int max = 0;
      //gets the max sets for each pair and keeps track of the maximum over all sets
      for(int i = 0; i < numActivities; i++){
        getMaxPrevTime(activities[i], i);
        if(activities[i].getTime() > max){
          max = activities[i].getTime();
          maxIndex = i;
        }
      }
      //prints out the activities that maximize time
      LinkedList<Pair> bestSet = sets.get(activities[maxIndex]);
      while(bestSet.peek() != null){
        Pair nextActivity = bestSet.remove();
        System.out.printf("(%d %d) ",nextActivity.getStart(), nextActivity  .getEnd());
      }
      System.out.printf("(%d %d) \n Max Time: %d\n", activities[maxIndex].getStart(), activities[maxIndex].getEnd(), activities[maxIndex].getTime());

    }catch(Exception e){
      System.err.println("Error " + e);
    }
  }

  /*
  * Input: activity() start, end, duration), index of activity in Pair array
  * This function finds the maximum time for all the activities(that are valid) before the current time
  * updates the time of the current activity and adds the set of activities that are before it
  */
  public static void getMaxPrevTime(Pair activity, int k ){
    int max = 0;
    int maxIndex = k;
    //finds the maximum time before the activity
    for(int i = k; i >= 0; i--){
      if(activities[i].getEnd() < activity.getStart() && activities[i].getTime() > max){
        max = activities[i].getTime();
        maxIndex = i;
      }
    }
    //if there was a valid activity before it add it to the set of activities matched to the current activity
    if(maxIndex != k){
      LinkedList<Pair> prevActivities = sets.get(activities[maxIndex]);
      LinkedList<Pair> newPrev = new LinkedList<Pair>(prevActivities);
      newPrev.add(activities[maxIndex]);
      activity.updateTime(max);
      sets.put(activity, newPrev);
      //else create an empty set
    }else {
      LinkedList<Pair> emptySet = new LinkedList<Pair>();
      sets.put(activity, emptySet);
    }
  }

  /*
  * Input: File name, Output: array of activities
  * Tokenizes the file and adds start and end dates to the pairs
  */
  public static Pair[] addActivies(Scanner file){
    String allActivities = file.nextLine();
    String[] tokens = allActivities.split("[() ]");
    int[] times = new int[tokens.length];
    int t = 0;
    //finds the times(integers) in tokens and puts them in an array
    for(int i = 0; i < times.length; i++){
      if(!tokens[i].equals("")){
        times[t] = Integer.parseInt(tokens[i]);
        t++;
      }
    }
    //creates an array for the pairs of start and end times
    int numActivities = (int)((t / 2) + .5);
    Pair[] pairs = new Pair[numActivities];
    int j = 0;
    for(int i = 0; i < t; i+=2){
      Pair activity = new Pair(times[i], times[i + 1]);
      pairs[j] = activity;
      j++;
    }
    return pairs;
  }
}
