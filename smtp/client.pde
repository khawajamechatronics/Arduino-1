uint8_t smtpServer[4];
uint8 smtfbuffer[1];

//ASocket ss;

bool SMTPInit(char *sv,char *un, char *pw)
{
  // open up a socket on port 25
  if (GetIPAddress(sv,smtpServer) == true)
  {
    Serial.print("connecting to:");
    printArray(&Serial,".",smtpServer,4,10);
    Serial.println("");
    Client client(smtpServer,25);
    //ss.connectTCP(smtpServer,25);
    if (client.connect())
    delay(2000);
    if (client.connected())
    {
      Serial.println("Connected");
   //   readfromclient(client);
#if 1
      sprintf(genbuf,"ehlo %s\r\n",sv);
      client.print(genbuf);
      readfromclient(client);
      client.println("auth login");
      readfromclient(client);
      base64_encode(genbuf,un,strlen(un));
      client.println(genbuf);
      readfromclient(client);
      base64_encode(genbuf,pw,strlen(pw));
      client.println(genbuf);
      readfromclient(client);
      while(Serial.read() != 'q')
      { 
        sprintf(genbuf,"MAIL FROM: <%s\r\n",username);
       // Serial.println(genbuf);
        client.print(genbuf);
        readfromclient(client);

        sprintf(genbuf,"RCPT TO: <%s>\r\n",receiver);
        client.print(genbuf);
        readfromclient(client);

        client.println("DATA");
        readfromclient(client);

        sprintf(genbuf,"From:\"%s\" <%s>\r\n",realname,username);
        client.print(genbuf);
        readfromclient(client);

        sprintf(genbuf,"To:\"%s\" <%s>\r\n","you",receiver);
        client.print(genbuf);
        readfromclient(client);

        client.println("Date: Tue, 15 Jan 2008 16:02:43 -0500");
        readfromclient(client);

        client.println("Subject: blah xxxxx");
        readfromclient(client);

        client.println("line 1");
        readfromclient(client);

        client.println("line 2");
        readfromclient(client);

        client.println(".");
        readfromclient(client);
        delay(2000);
      }
#endif
      client.println("QUIT");
      readfromclient(client);
    }
    else
    {
      Serial.println("connection failed");
    }
  }
}

void readfromclient(Client c)
{
    int x = c.read();
    while (x != -1)
    {
      Serial.print((char)x);
      x = c.read();
    }
}

void SMTPclose()
{
//  client.println("QUIT");
//  readfromclient(client);
}
