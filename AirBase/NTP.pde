byte ntpServer[6];
unsigned long epoch;
NtpClass Ntp;

bool NTPInit()
{
  return GetIPAddress("pool.ntp.org",ntpServer);
}

// returns TRUE on success
bool NTPenquiry()
{
  int result;
  // Do NTP enquiry
  Serial.print("NTP enquiry: ");
  Ntp.init(ntpServer);
  Ntp.request();
  
  while(!(result=Ntp.finished())); //wait for NTP to complete
  if(result == COMPLETE_OK)
  {
  //    Serial.println("got timestamp");
      Ntp.getTimestamp(&epoch);
  }
  else 
  {
    switch(result)
    {
      case TIMEOUT_EXPIRED:
        Serial.println("Timeout");
        break;
      case NO_DATA:
        Serial.println("No Data");
        break;
      case COMPLETE_FAIL:
        Serial.println("Failed");
        break;
    }
  }
  return (result == COMPLETE_OK); // true is OK
}

