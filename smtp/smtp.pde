#include <Base64.h>
#include <AClient.h>
#include <AEthernet.h>
#include <AServer.h>
#include <ASocket.h>
#include <Dhcp.h>
#include <Dns.h>

char server[] = "mail.barak-online.net";
char username[] = "henry8@barak-online.net";
char password[] = "morris59";
char realname[] = "David Henry";
char receiver[] = "mgadriver@gmail.com";

char genbuf[80];

byte macaddress[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
/*
logon to SMTP server, send message and exit
*/
void setup()
{
  Serial.begin(115200);
  // get IP address from DHCP
  while (NetInit() == false);
  // open session 
  while (SMTPInit(server,username,password) == false);
  SMTPclose();
}

void loop()
{
}

