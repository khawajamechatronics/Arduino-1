// observed values from the Keyes IR dongle
#define ONOFF 0XffA25D
#define MODE 0Xff629D
#define MUTE 0XffE21D
#define FF 0Xff22DD
#define REWIND 0Xff02FD
#define NEXTTRACK 0XffC23D
#define EQ 0XffE01F
#define PLUS 0Xff906F
#define MINUS 0XffA857
#define ZERO 0Xff6897
#define CHANGEDIR 0Xff9867
#define USD 0XffB04F
#define ONE  0Xff30CF
#define TWO 0Xff18E7
#define THREE 0Xff7A85
#define FOUR 0Xff10EF
#define FIVE 0Xff38C7
#define SIX 0Xff5AA5
#define SEVEN 0Xff42BD
#define EIGHT 0Xff4AB5
#define NINE 0Xff52AD
// array and enum must be in the same order
static unsigned long codes[] = {ZERO,ONE,TWO,THREE,FOUR,FIVE,SIX,SEVEN,EIGHT,NINE,
  ONOFF,MODE,MUTE,FF,REWIND,NEXTTRACK,EQ,PLUS,MINUS,CHANGEDIR,USD};
enum eIR {zero=0,one,two,three,four,five,six,seven,eight,nine,
  onoff,mode,mute,ff,rw,nt,eq,plus,minus,cd,usd,none};
enum eIR irdecode(unsigned long code)
{
  for (int i=0;i<none;i++)
    if (codes[i] == code)
      return i;
  return none;
}

