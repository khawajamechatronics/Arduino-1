
#include <ctype.h>
#include <string.h>


//-- Start of the library -------------------------------------------------------------

// Size of the parser-buffer. Make sure no tag is longer than this!
#define BUFFER_SIZE 256

// Maximum number of attributes the parser can handle in one tag
#define ATTR_LEN 10

typedef enum {
 IN_TEXT, IN_TAG
} 
ParserState;

typedef struct {
 char *name, *val;
} 
NameVal;

class UXMLParser {
private:
 char buff[BUFFER_SIZE];
 uint16_t buffLen;
 ParserState state;

 void buffAppend(char c);
 void buffClear();
 void parseTag();
 char* findName(char* p, char* endP, size_t *len);
 char* findVal(char* p, size_t *len);
 char charBeforeName(char* p);
 char charBeforeTagEnd(char* p);
 bool isName(char c);
public:
 UXMLParser();
 void parse(char c);

 virtual void handleOpenTag(char *tag, NameVal attr[], uint8_t attrLen) = 0;
 virtual void handleCloseTag(char *tag) = 0;
 virtual void handleText(char *text) = 0;
};

UXMLParser::UXMLParser() {
 buffClear();
 state = IN_TEXT;
}

void UXMLParser::buffAppend(char c) {
 buff[buffLen] = c;
 if (buffLen < BUFFER_SIZE) {
   buffLen++;
 }
}

void UXMLParser::buffClear() {
 buffLen = 0;
}

bool UXMLParser::isName(char c) {
 return isalnum(c) | c=='.' | c==':' | c=='-' | c=='_';
}

char* UXMLParser::findName(char* p, char* endP, size_t *len) {
 while (p<endP && !isName(*p))
   p++; // find start of name
 *len = 0;
 if (p>=endP) return 0;
 while (p+*len < endP && isName(p[*len])) (*len)++; // find end of name
 if (p+*len>=endP) return 0;
 return p;
}

char* UXMLParser::findVal(char* p, size_t *len) {
 char qmark;
 while ((*p)!='"' && (*p)!='\'') p++; // find start of val
 qmark = *p;
 p++;
 *len = 0;
 while (p[*len]!=qmark) (*len)++; // find end of val
 return p;
}

char UXMLParser::charBeforeName(char* p) {
 while(!isalnum(*p)) {
   if (!isspace(*p)) return *p;
   p++;
 }
 return 0;
}

char UXMLParser::charBeforeTagEnd(char* p) {
 p -= 2;
 while(isspace(*p)) {
   p--;
 }
 return *p;
}

void UXMLParser::parseTag() {
 char *p, *endP, *tag, c;
 NameVal attrList[ATTR_LEN];
 uint8_t attrP;
 bool openTag, closeTag, emptyTag;
 size_t len;

 p = buff;
 endP = buff + buffLen;
 p++; // skip the '<'

 openTag = true; 
 closeTag = false;
 c = charBeforeName(p);
 if (c == '?') return;   // skip this tag
 if (c == '/') {
   openTag = false;
   closeTag = true;
 }

 c = charBeforeTagEnd(endP);
 if (c == '/') {
   closeTag = true;
 }

 tag = findName(p, endP, &len);
 if (tag==0) return;

 emptyTag = tag[len] == '>';
 tag[len] = 0;
 p = tag+len+1;

 attrP=0;
 if (!emptyTag) {
   while (attrP<ATTR_LEN) {
     char *attrN, *attrV;

     attrN = findName(p, endP, &len);
     if (attrN==0) break;        // reached end of tag
     attrN[len] = 0;
     p = attrN+len+1;

     attrV = findVal(p, &len);
     attrV[len] = 0;
     p = attrV+len+1;

     attrList[attrP].name = attrN;
     attrList[attrP].val = attrV;

     attrP++;
   }
 }

 if (openTag) handleOpenTag(tag, attrList, attrP);
 if (closeTag) handleCloseTag(tag);
}

void UXMLParser::parse(char c) {
 switch (state) {
 case IN_TEXT:
   if (c=='<') {
     buffAppend(0);
     handleText(buff);
     buffClear();
     buffAppend(c);
     state = IN_TAG;
   } 
   else {
     buffAppend(c);
   }
   break;

 case IN_TAG:
   if (c=='>') {
     buffAppend(c);
     parseTag();
     buffClear();
     state = IN_TEXT;
   } 
   else {
     buffAppend(c);
   }
   break;

 default: 
   break;
 }
}

// -- End of library -----------------------------------------------------------------


// -- Start of test/example program --------------------------------------------------


// UXML uses inheritance, so you have to create your own class,
// and overweite the methods handleOpenTag, handleCloseTag and handleText.
class TestParser : public UXMLParser {
public:
 TestParser();
 virtual void handleOpenTag(char *tag, NameVal attr[], uint8_t attrLen);
 virtual void handleCloseTag(char *tag);
 virtual void handleText(char *text);
};

TestParser::TestParser() : UXMLParser() {
}

// what should be done, if a tag opens?
void TestParser::handleOpenTag(char *tag, NameVal attr[], uint8_t attrLen) {
 Serial.print("handleOpenTag: ");
 Serial.print(tag);
 for(int i=0; i<attrLen; i++) {
   Serial.print(" ");
   Serial.print(attr[i].name);
   Serial.print("='");
   Serial.print(attr[i].val);
   Serial.print("'");
 }
 Serial.println();
}

// what should be done, if a tag closes?
void TestParser::handleCloseTag(char *tag) {
 Serial.print("handleCloseTag: ");
 Serial.println(tag);
}

// what should be done, with text?
void TestParser::handleText(char *text) {
 Serial.print("handleText: '");
 Serial.print(text);
 Serial.println("'");
}

TestParser testParser = TestParser();
//char test[] = "<bc></bc>< a n = 'value' asdf='' jkl=\"'\"> b <emptyTag / >< / a >";
char test[] = " <?xml version=\"1.0\" encoding=\"utf-16\"?><C I=\"238\" T=\"GUP\" UDP=\"8000\" />";
void setup() {
 Serial.begin(115200);
 
 Serial.println("Test started");
 
 // Send the test-string to the parser char by char.
 // The parser will call the handle* functions.
 for(int i=0; test[i]!=0; i++) {
   testParser.parse(test[i]);
 }

 Serial.println("Test ended");
}

void loop() {
}

// -- End of test/example program --------------------------------------------------
