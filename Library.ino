class TMTimer;
class TimerEventHandler
{
public:
virtual void onInterval(TMTimer *timer)=0;
};
class TMTimer
{
private:
int interval;
TimerEventHandler *timerEventHandler;
bool repeat;
bool isActive;
public:
TMTimer(int interval,TimerEventHandler *timerEventHandler,bool repeat=true);
void start();
void stop();
friend class TMTimerManager;  
};
class TMTimerManager
{
private:
class TMTimerNode
{
public:
long int lastCalledOn;
TMTimer *timer;
TMTimerNode *next;
TMTimerNode(TMTimer *timer)
{
this->timer=timer;
this->next=NULL;
this->lastCalledOn=0;
}
};
static TMTimerNode *start,*end;
TMTimerManager(){}
public:
static void addTimer(TMTimer *timer);
static void emitEvents();
};
TMTimerManager::TMTimerNode *TMTimerManager::start=NULL;
TMTimerManager::TMTimerNode *TMTimerManager::end=NULL;
void TMTimerManager::emitEvents()
{
long int now=millis();
TMTimerNode *t,*p1,*p2;
for(t=start;t!=NULL;t=t->next)
{
if(t->timer->isActive && ((now-t->lastCalledOn)>t->timer->interval || now<t->lastCalledOn))
{
t->timer->timerEventHandler->onInterval(t->timer);
t->lastCalledOn=now;
if(t->timer->repeat==false) t->timer->stop();
}
}
t=start;
while(t!=NULL)
{
  if(t->timer->isActive==false)
  {
    //logic to delete starts here...
    if(t==start && t==end)  //only one node is there...
    {
      start=NULL;
      end=NULL;
      delete t;
      break;
    }
    else if(t==start)  //first node to be deleted...
    {
      start=start->next;
      delete t;
      t=start;
      break;
    }
    else if(t==end) //last node to be deleted
    {
      p2->next=NULL;
      end=p2;
      delete t;
      break;
    }
    else  //any of the middle to be deleted...
    {
      p2->next=t->next;
      delete t;
      t=p2->next;
    }
    //logic to delete ends here...
  }
  else
  {
    p2=t;
    t=t->next;
  }
}
}
void TMTimerManager::addTimer(TMTimer *timer)
{
if(timer==NULL) return;
if(timer->interval<=0) return;
TMTimerNode *node;
for(node=start;node!=NULL;node=node->next)
{
if(node->timer==timer) return;
}
node=new TMTimerNode(timer); 
if(start==NULL)
{
start=node;
end=node;
}
else
{
end->next=node;
end=node;
}
}
TMTimer::TMTimer(int interval,TimerEventHandler *timerEventHandler,bool repeat)
{
this->interval=interval;
this->timerEventHandler=timerEventHandler;
this->repeat=repeat;
this->isActive=true;
}
void TMTimer::start()
{
TMTimerManager::addTimer(this);
}
void TMTimer::stop()
{
this->isActive=false;
}

class TMLED;
class LEDEventHandler
{
  public:
  virtual void turnedOn(TMLED *led)=0;
  virtual void turnedOff(TMLED *led)=0;  
};

class TMLED
{
  private:
  int pin;
  bool isOn;
  LEDEventHandler *ledEventHandler;
  public:
  TMLED(int pin);
  void turnOn();
  void turnOff();
  void toggle();  
  void setLEDEventHandler(LEDEventHandler *ledEventHandler);
  bool isLEDOn();
  bool isLEDOff();
};
TMLED::TMLED(int pin)
{
  this->pin=pin;
  this->isOn=false;
  pinMode(this->pin,OUTPUT);
  this->ledEventHandler=NULL;
}
void TMLED::turnOn()
{
  if(this->isOn) return;
  digitalWrite(this->pin,HIGH);
  this->isOn=true;
  if(this->ledEventHandler!=NULL) this->ledEventHandler->turnedOn(this);
}
void TMLED::turnOff()
{
  if(this->isOn==false) return;
  digitalWrite(this->pin,LOW);
  this->isOn=false;
  if(this->ledEventHandler!=NULL) this->ledEventHandler->turnedOff(this);
}
void TMLED::toggle()
{
  if(this->isOn)
  {
    this->turnOff();
    this->isOn=false;
  }
  else
  {
    this->turnOn();
    this->isOn=true;
  }
}
void TMLED::setLEDEventHandler(LEDEventHandler *ledEventHandler)
{
  this->ledEventHandler=ledEventHandler;
}
bool TMLED::isLEDOn()
{
  return this->isOn==true;
}
bool TMLED::isLEDOff()
{
  return this->isOn==false;
}
class TMBlinkingLED;
class BlinkingLEDEventHandler
{
  public:
  virtual void ledBlinked(TMBlinkingLED *tmBlinkingLED,int blinksCount);
};
class TMBlinkingLED:public TimerEventHandler
{
  private:
  TMLED *tmled;
  TMTimer *tmtimer;
  int timesToBlink=0; 
  bool isBlinking;
  BlinkingLEDEventHandler *target;
  int blinkCount;
  public:
  TMBlinkingLED(int pin,int timesToBlink,byte speed);
  ~TMBlinkingLED();
  void startBlinking();
  void stopBlinking();
  void onInterval(TMTimer *tmtimer);
  void setBlinkingLEDEventHandler(BlinkingLEDEventHandler *target);
};
TMBlinkingLED::TMBlinkingLED(int pin,int timesToBlink,byte speed)
{
  if(speed<1) speed=1;
  else if(speed>3) speed=3;
  this->tmled=new TMLED(pin);
  this->tmtimer=new TMTimer(1000*speed,this);
  this->isBlinking=false;
  if(timesToBlink<-1) timesToBlink=-1;  //-1 means , keep blinking
  this->timesToBlink=timesToBlink;
  this->target=NULL;
  this->blinkCount=0;
}
TMBlinkingLED::~TMBlinkingLED()
{
  delete this->tmled;
  delete this->tmtimer;
}
void TMBlinkingLED::startBlinking()
{
  if(this->timesToBlink==0) return;
  this->tmtimer->start();
  this->isBlinking=true;
}
void TMBlinkingLED::stopBlinking()
{
  this->tmtimer->stop();
  this->isBlinking=false;
  this->tmled->turnOff();
}
void TMBlinkingLED::onInterval(TMTimer *tmtimer)
{
  if(this->tmled->isLEDOff()) 
  {
    this->blinkCount++;
    if(this->target!=NULL) 
    {
      this->target->ledBlinked(this,this->blinkCount); 
    }
    if(this->timesToBlink!=-1 && this->blinkCount==this->timesToBlink+1)
    {
      this->stopBlinking();
    }
  }
  this->tmled->toggle();
}
void TMBlinkingLED::setBlinkingLEDEventHandler(BlinkingLEDEventHandler *target)
{
  this->target=target;
}

class Application
{
public:
virtual void start()=0;
};
class ApplicationManager
{
private:
static Application *application;
ApplicationManager(){}
public:
static void startApplication(Application *application);
};
Application *ApplicationManager::application=NULL;
void ApplicationManager::startApplication(Application *application)
{
delay(5000);
if(application==NULL) return;
ApplicationManager::application=application;
ApplicationManager::application->start();
}
void loop()
{
TMTimerManager::emitEvents();
}
//Framework part ends
//The following code is being written by IOTFramework user...
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
void setup()
{
Serial.begin(9600);
ApplicationManager::startApplication(new  HomeAutomation());
}