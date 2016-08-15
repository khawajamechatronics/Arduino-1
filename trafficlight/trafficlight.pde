#define RED 5
#define YELLOW 6
#define GREEN 7

#define STOP 0
#define READY 1
#define GO 2
#define SLOW 3

int time;
void setup()
{
  pinMode(RED,OUTPUT);
  pinMode(YELLOW,OUTPUT);
  pinMode(GREEN,OUTPUT);
  LedAction(RED,false);
  LedAction(YELLOW,false);
  LedAction(GREEN,false);
  Serial.begin(115200);
}

void LedAction(int led, boolean action)
{
  if (action)
    digitalWrite(led,HIGH);
  else
    digitalWrite(led,LOW);
}

int lasttime;
void loop()
{
  time = ((millis()/1000))%17;
  if (time != lasttime)
    Serial.println(time);
  lasttime = time;
  switch (time)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
      LedAction(RED,true);
      break;
    case 5:
    case 6:
    case 7:
      LedAction(RED,false);
      LedAction(YELLOW,true);
      break;
    case 8:
    case 9:
    case 10:
      LedAction(YELLOW,false);
      LedAction(GREEN,true);
      break;
    case 11:
    case 13:
    case 15:
      LedAction(YELLOW,true);
      LedAction(GREEN,false);
      break;
    case 12:
    case 14:
    case 16:
      LedAction(YELLOW,false);
      break;
  }
}
