
import processing.serial.*;
import java.util.Scanner;
ComPortMenu portSelect;
PositionSelectButton positionSelect;
String inString;

float angle, angle_old, polar, polar_old;
float angle_base = 0, polar_base;

float radius = 50;

float frequency = 2;
float frequency2 = 2;
float x, x2;
ArrayList<Point> points = new ArrayList<Point>();

void setup(){
  size(600, 400);
  background (127);
  portSelect = new ComPortMenu();
  
  positionSelect = new PositionSelectButton(width/8, height*3/4);
  //selectPolar = new PositionSelectButton(width/8, height*3/4);
}

void draw(){
  background (127);
  noStroke();
  fill(255);
  ellipse(width/8, height/2, radius*2, radius*2);
  
  portSelect.drawMenu();
  positionSelect.drawButton();
  Serial comPort = portSelect.getComPort();
  if(comPort != null)
  {
    try
    {
      comPort.bufferUntil('\n');
      Scanner comInput = new Scanner(inString);
      comInput.useLocale(java.util.Locale.US);
      while(!comInput.hasNextFloat())
        comInput.next();
     angle_old = angle;   
     angle  = comInput.nextFloat();
     if (angle-angle_old < -5){
       angle_base += 6.28;
     }
     else if(angle-angle_old > 5){
       angle_base -= 6.28;
     }
     
     while(!comInput.hasNextFloat())
        comInput.next();
      polar_old = polar;  
      polar = comInput.nextFloat();
    
     if (polar-polar_old < -2.8){
       polar_base += 3.14;
     }
     else if(polar-polar_old > 2.8){
       polar_base -= 3.14;
     }
     
    }
    catch(Exception e)
    {
      System.err.print("Invalid COM input: ");
      System.err.println(inString);
      System.err.print("Original message: ");
      System.err.println(e.getMessage());
    }
  } 

  if(positionSelect.azimuthSelected()){
   drawSine(angle + angle_base);
  }
  
  else{
  // scaling 1.52 -> 1.57
  drawSine(polar * 1.03 + polar_base);
  }
}

void drawSine(float angle){
  float px, py, px2;
  // Rotates rectangle around circle
  px = width/8 + cos(angle)*(radius);
  py = height/2 + sin(angle)*(radius);
  fill(0);

  rect (px, py, 5, 5);
  stroke(100);
  line(width/8, height/2, px, py);
  stroke(200);

  noStroke();
  px2= width/8+radius+angle*20;
  ellipse(px2, py, 5, 5);
  if (angle >0)
    points.add(new Point(px2,py));
  for(Point pt:points){
    stroke(255);
    fill(255);
    ellipse(pt.x, pt.y, 3, 3);
  }

  // Draw dynamic line connecting circular path with wave
  stroke(50);
  line(px, py, width/8+radius+angle*20, py);

  // Output calculations to screen
  text("Angle rotated = " + int((angle)/PI*180), width/6, height/4);
}

class Point{
  float x;
  float y;
  Point(float _x, float _y){
    x = _x;
    y = _y;
  }
}
////////////////////////////////////////////////

Serial createSerial(String name)
{
  try
  {
    return new Serial(this, name, 9600);
  }
  catch(Exception e)
  {
    System.err.print(e.getMessage());
    return null;
  }
}

void serialEvent(Serial port)
{
  inString = port.readString();
}

void mousePressed()
{
  portSelect.clickHandler();
  positionSelect.clickHandler();
}

class PositionSelectButton
{
  private int mX;
  private int mY;
  private int mWidth = 10;
  private int mHeight;
  
  private boolean mIsAzimuth;

  public PositionSelectButton(int btnX, int btnY)
  {
    mX = btnX;
    mY = btnY;
    mWidth = 20;
    mHeight = 40;
    mIsAzimuth= true;
  }
  void clickHandler(){
   mIsAzimuth = !mIsAzimuth;
   if(mIsAzimuth){
     mHeight = 40;
   }
   else{
     mHeight = 10;
   }
  }
  
  boolean azimuthSelected(){
    return mIsAzimuth;
  }
  
  boolean mouseOverBtn()
  {
    return (mouseX>mX && mouseX<mX+mWidth && mouseY>mY && mouseY<mY+mHeight);
  }

  public void drawButton()
  {
    if (mIsAzimuth)
    {
      fill(150, 30, 30);
      textSize(18);
      text("Top view", width/4, height*31/40);
    }
    else{
      fill(150, 20, 20);
      textSize(18);
      text("Side view", width/4, height*31/40);
    }
    
    if (mouseOverBtn())
    {
      fill(150, 40, 40);
    }
    rect(mX, mY, mWidth, mHeight);
    fill(255);
  }
}

class ComPortMenuButton
{
  private int mX;
  private int mY;
  private int mWidth;
  private int mHeight;
  private String mName;
  private boolean mIsActive;

  public ComPortMenuButton(int btnX, int btnY, int btnWidth, int btnHeight, String btnName)
  {
    mX = btnX;
    mY = btnY;
    mWidth = btnWidth;
    mHeight = btnHeight;
    mName = btnName;
    mIsActive = false;
  }

  boolean mouseOverBtn()
  {
    return (mouseX>mX && mouseX<mX+mWidth && mouseY>mY && mouseY<mY+mHeight);
  }

  public void drawButton()
  {
    fill(70);
    if (mIsActive)
    {
      fill(120, 80, 0);
    }
    if (mouseOverBtn())
    {
      fill(180, 120, 0);
    }
    rect(mX, mY, mWidth, mHeight);
    fill(255);
    textAlign(CENTER);
    text(mName, mX+mWidth/2, mY+mHeight/2+8);
  }

  void setInActive()
  {
    mIsActive = false;
  }

  Serial setActive()
  {
    mIsActive = true;
    return createSerial(this.mName);
  }
}

class ComPortMenu
{
  private ArrayList<ComPortMenuButton> mButtons;
  private ComPortMenuButton mActiveBtn = null;
  private Serial mComPort = null;

  public ComPortMenu()
  {
    int i = 0;
    mButtons = new ArrayList();
    for (String current : Serial.list())
    {
      int x = i % 5;
      int y = i / 5;
      mButtons.add(new ComPortMenuButton(100*x+5, 50*y+5, 90, 40, current));
      i++;
    }
  }

  public void drawMenu()
  {
    for (ComPortMenuButton current : mButtons)
    {
      current.drawButton();
    }
  }

  public void clickHandler()
  {
    for (ComPortMenuButton current : mButtons)
    {
      if (current.mouseOverBtn())
      {
        if (mActiveBtn != null)
          mActiveBtn.setInActive();
        mActiveBtn = current;
        mComPort = current.setActive();
      }
    }
  }

  public Serial getComPort()
  {
    return mComPort;
  }
}
