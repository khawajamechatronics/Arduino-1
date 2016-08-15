#include <Base64.h>
char outbuf[40];
void setup()
{
  Serial.begin(115200);
  int l = base64_encode(outbuf,"morris59",strlen("morris59"));
  Serial.println(l);
  Serial.println(outbuf);
  l = base64_encode(outbuf,"henry8@barak-online.net",strlen("henry8@barak-online.net"));
  Serial.println(l);
  Serial.println(outbuf);
  l = base64_decode(outbuf,"VXNlcm5hbWU6",strlen("VXNlcm5hbWU6"));
  Serial.println(l);
  Serial.println(outbuf);
  l = base64_decode(outbuf,"UGFzc3dvcmQ6",strlen("UGFzc3dvcmQ6"));
  Serial.println(l);
  Serial.println(outbuf);
}

void loop()
{
}

