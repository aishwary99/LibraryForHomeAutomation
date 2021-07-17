class TMTimer;
class TimerEventHandler     //an abstract class for TMTimer
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
  friend class TMTimerManager;    //TMTimerManager can access the private members now
};
class TMTimerManager
{
  private:
  class TMTimerNode
  {
    public:
    TMTimer *timer;
    TMTimerNode *next;
    long int lastCalledOn;
    TMTimerNode(TMTimer *timer)
    {
      this->lastCalledOn=0;
      this->timer=timer;
      this->next=NULL;
    }
  };
  static TMTimerNode *start,*end;
  TMTimerManager(){}      //private constructor so that no one can instantitate
  public:
  static void addTimer(TMTimer *timer);
  static void emitEvents();
};
TMTimerManager::TMTimerNode *TMTimerManager::start=NULL;
TMTimerManager::TMTimerNode *TMTimerManager::end=NULL;
void TMTimerManager::addTimer(TMTimer *timer)
{
  if(!timer) return;
  if(timer->interval<=0) return;    //because it will never be 0 according to logic
  TMTimerNode *node;
  for(node=start;node;node=node->next)
  {
    if(node->timer==timer) return;
  }
  node=new TMTimerNode(timer);
  if(start==NULL)     //first node
  {
    start=end=node;
  }
  else
  {
    end->next=node;
    end=node;
  }
}
void TMTimerManager::emitEvents()
{
  //just to handle overflow case , in unsigned case it will start round trip from 0 if exceeds to upperbound
  unsigned long int now=millis();
  for(TMTimerNode *current=start;current;current=current->next)
  {
    if(current->timer->isActive && ((now-current->lastCalledOn)>current->timer->interval) || current->lastCalledOn>now)
    {
      current->timer->timerEventHandler->onInterval(current->timer);
      current->lastCalledOn=now;
      if(current->timer->repeat==false) current->timer->stop();
    }
  }
  TMTimerNode *prev,*current;
  current=start;
  prev=NULL;
  bool isDeletionDone=false;
  while(current)
  {
    if(!current->timer->isActive)
    {
      //logic to delete the node to which current is pointing to...
      if(current==start && current==end)
      {
        //only one node is there and it itself is inactive
        start=end=NULL;
        delete current;
        isDeletionDone=true;
      }
      else if(current==start)
      {
        //first node
        prev=current;
        current=current->next;
        //delete is used because the memory is allocated using new
        delete prev;
        start=current;
        isDeletionDone=true;
      }
      else if(current==end)
      {
        //last node
        prev->next=NULL;
        end=prev;
        delete current;
        isDeletionDone=true;
      }
      else
      {
        //any of the middle node
        prev->next=current->next;
        delete current;
        isDeletionDone=true;
      }
      if(isDeletionDone) break;
    }
    else 
    {
      prev=current;
      current=current->next;
    }
  }
}
TMTimer::TMTimer(int interval,TimerEventHandler *timerEventHandler,bool repeat)
{
  //interval in ms & address of timer
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

class TMLED
{
  private:
  int pin;
  bool isOn;
  public:
  TMLED(int pin);
  void turnOn();
  void turnOff();
  void toggle();
};
TMLED::TMLED(int pin)
{
  this->pin=pin;
  this->isOn=false;
  pinMode(this->pin,OUTPUT);
}
void TMLED::turnOn()
{
  digitalWrite(this->pin,HIGH);
  this->isOn=true;
}
void TMLED::turnOff()
{
  digitalWrite(this->pin,LOW);
  this->isOn=false;
}
void TMLED::toggle()
{
  if(this->isOn) this->turnOff();
  else this->turnOn();
}
class Application
{
  public:
  virtual void start()=0;
};
class ApplicationManager
{
  private:
  static Application *application;  //will contain address of homeAutomation created by user
  ApplicationManager(){}    //private constructor so it can't be instantitated
  public:
  static void startApplication(Application *application);
};
Application *ApplicationManager::application=NULL;
void ApplicationManager::startApplication(Application *application)
{
  if(!application) return;
  ApplicationManager::application=application;    //will store the address of homeAutomation created by user
  ApplicationManager::application->start();   //will place a call to start() of homeAutomation
}
void loop()
{
  TMTimerManager::emitEvents();
}
//framework part ends here...
//Assume that the following code is being written by IOTFramework User
class HomeAutomation:public Application,TimerEventHandler
{
  private:
  TMTimer *t1,*t2;
  TMLED *led1,*led2;
  int blinkCount;
  public:
  HomeAutomation()
  {
    t1=new TMTimer(1000,this);
    t2=new TMTimer(2000,this);
    led1=new TMLED(10);
    led2=new TMLED(11);
    blinkCount=0;
  }
  ~HomeAutomation()
  {
    delete t1;
    delete t2;  
    delete led1;
    delete led2;
  }
  void start()    //defining the pure virtual function declared in the abstract class Application
  {
    t1->start();    //start the timer and register it in TMTimerManager
    t2->start();    //start the timer and register it in TMTimerManager
  }
  //onInterval will get called on specified intervals for respective timer's
  void onInterval(TMTimer *timer)   //defining the pure virtual function declared in the abstract class TimerEventHandler
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
};
void setup()
{
  ApplicationManager::startApplication(new HomeAutomation());
}