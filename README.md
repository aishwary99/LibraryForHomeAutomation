# Generic Library For Smart Home Developers

## Description
A Generic Library is a library written from scratch for smart home users. It's a mini framework that gives support for timer event handlers , Led event handlers & blinking led's

### Features
* TM Timer
* TM Blinking LED
* TM LED
* Timer Manager
* Application Manager
* Event Handlers


## Sample Code Snippet

* Assume that the following code is being written by IOTFramework user !

```
class HomeAutomation:public Application,TimerEventHandler,LEDEventHandler,BlinkingLEDEventHandler
{
TMTimer *t1,*t2;
TMLED *led1,*led2;
TMBlinkingLED *blinkingLED1;
TMBlinkingLED *blinkingLED2;
bool led1Status,led2Status;
int blinkCount;
public:
HomeAutomation()
{
t1=new TMTimer(2000,this);
t2=new TMTimer(2000,this);
led1=new TMLED(6);
led1->setLEDEventHandler(this);
led2=new TMLED(7);
led2->setLEDEventHandler(this);
led1Status=false;
led2Status=false;
blinkingLED1=new TMBlinkingLED(12,5,1); //blink 5 times
blinkingLED1->setBlinkingLEDEventHandler(this);
blinkingLED2=new TMBlinkingLED(11,-1,3); //blink infinite times
blinkingLED2->setBlinkingLEDEventHandler(this);
this->blinkCount=0;
}
~HomeAutomation()
{
delete t1;
delete t2;
delete led1;
delete led2;
delete blinkingLED1;
delete blinkingLED2;
}
public:
void start()
{
t1->start();
t2->start();
blinkingLED1->startBlinking();
blinkingLED2->startBlinking();
}
void onInterval(TMTimer *timer)
{
  if(timer==t1)
  { 
    led1->toggle();
  }
  if(timer==t2)
  {
    led2->toggle();
  }
}
void turnedOn(TMLED *led)
{
  //if(led==led1) Serial.println("LED 1 connected to pin 6 turned on");
  //else if(led==led2) Serial.println("LED 2 connected to pin 7 turned on");
}
void turnedOff(TMLED *led)
{
  //if(led==led1) Serial.println("LED 1 connected to pin 6 turned off");
  // else if(led==led2) Serial.println("LED 2 connected to pin 7 turned off");
}
void ledBlinked(TMBlinkingLED *tmBlinkingLED,int blinksCount)
{
  if(tmBlinkingLED==blinkingLED1)
  {
    Serial.print("First led blinked for ");
    Serial.print(blinksCount);
    Serial.println(" times");
  }
  if(tmBlinkingLED==blinkingLED2)
  {
    Serial.print("Second led blinked for ");
    Serial.print(blinksCount);
    Serial.println(" times");
  }
}
};

```
