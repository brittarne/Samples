
class Pair{
  private int startTime;
  private int endTime;
  private int duration;

  public Pair(int start, int end){
    this.startTime = start;
    this.endTime = end;
    this.duration = end - start;
  }

  public int getStart(){
    return this.startTime;
  }

  public int getEnd(){
    return this.endTime;
  }

  public int getTime(){
    return this.duration;
  }

  public void updateTime(int addTime){
    this.duration = duration + addTime;
  }

}
