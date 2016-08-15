void printArray(Print *output, char* delimeter, byte* data, int len, int base)
{
  char buf[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  for(int i = 0; i < len; i++)
  {
    if(i != 0)
      output->print(delimeter);
    output->print(itoa(data[i], buf, base));
  }
}

void printIP(uint32_t ip)
{
  Serial.print((ip) & 0xFF);
  Serial.print('.');
  Serial.print((ip>>8) & 0xFF);
  Serial.print('.');
  Serial.print((ip>>16) & 0xFF);
  Serial.print('.');
  Serial.println((ip>>24) & 0xFF);
}

extern unsigned long epoch;
extern char SharpBuffer[];

void PrintMessages()
{    
  Serial.print("Unix Time = ");
  Serial.println(epoch);  
  Serial.print("Sharp: ");
  Serial.println(SharpBuffer);    
  Serial.print("IAQ: ");
  Serial.println(IAQBuffer);
  Serial.print("Oz: ");
  Serial.println(OZBuffer);  
}

