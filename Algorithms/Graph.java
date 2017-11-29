import java.util.*;
import java.lang.*;
import java.io.*;

class Graph{
  public static HashMap<Integer, GraphNode> graph;
  public static GraphNode source;
  public static GraphNode sink;
  public static void main(String[] args){
    graph = new HashMap<Integer,GraphNode>();
    sink = new GraphNode(-1);
    source = new GraphNode(1000000000); //find better representation

    Scanner input = new Scanner(System.in);
    // String fileName = input.next();
    String fileName = "input.txt";
    try{
      Scanner file = new Scanner(new File(fileName));
      String next = file.nextLine();
      getNodes(next);
      next = file.nextLine();
      getCapacities(next);
      // System.out.println("hello");
      int maxFlow = getMaxFlow();


    }catch(Exception e){
      System.err.println("Error " + e);
    }
  }
  public static int getMaxFlow(){
    int maxFlow = 0;
    // GraphNode sourceNode;
    // sourceNode = graph.get(source);
    while(source.capacity > 0){ //gonna  need an and in here as well
      ArrayList<GraphNode> path = new ArrayList<GraphNode>();
      path.add(source);
      getPath(path, 0);
      System.out.println(path.size());
      // for(int i = 0; i < path.size(); i++){
      //   System.out.println(path.get(i).nodeID);
      // }
    }
    return maxFlow;
  }
  public static void getPath(ArrayList<GraphNode> paths, int index){
    // GraphNode curr = paths.get(index);
    if(paths.contains(sink)){
      paths.add(sink);
    }else {
      
      // while(paths.get(i).adjacencyList)
      System.out.println(paths.size());
    }
  }

  public static void getNodes(String nodeLine){
    String[] tokens = nodeLine.split("[()]");
    for(String tok : tokens){
      if(tok.length() > 1){
        String[] vals = tok.split(" ");
        int nodeID = Integer.parseInt(vals[0]);
        int adjNode = Integer.parseInt(vals[1]);
        GraphNode node;

        if(graph.containsKey(nodeID)){
          node = graph.get(nodeID);
          node.addToAdjList(adjNode);
        }else{
          node = new GraphNode(nodeID);
          node.addToAdjList(adjNode);
          graph.put(nodeID, node);
        }
        //set source and sink
        if(node.nodeID > sink.nodeID){
          sink = node;
        }
        if(nodeID < source.nodeID){
          source = node;
        }
      }
    }
  }
  public static void getCapacities(String capLine){
    String[] tokens = capLine.split("[()]");
    for(String tok : tokens){
      if(tok.length() > 1){
        String[] vals = tok.split(" ");
        int nodeID = Integer.parseInt(vals[0]);
        int capacity = Integer.parseInt(vals[1]);
        if(graph.containsKey(nodeID)){
          graph.get(nodeID).setCapacity(capacity);
        }else {
          System.err.println(nodeID + " Not Valid Node");
        }
      }
    }
  }

  static class GraphNode{
    public int nodeID;
    public ArrayList<Integer> adjacencyList;
    public int capacity;
    public GraphNode(int nodeID){
      this.nodeID = nodeID;
      adjacencyList = new ArrayList<Integer>();
    }
    public void addToAdjList(int nodeID){
      this.adjacencyList.add(nodeID);
    }
    public void setCapacity(int cap){
      this.capacity = cap;
    }
  }
}
