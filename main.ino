#include <IRremote.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

// default Mega2560
#define MiniPro
#define _DEBUG




IRsend *irsend = NULL; // ping 9


#ifdef MiniPro
	SoftwareSerial BTSerial(7, 8); // RX | TX
#else
	#define BTSerial Serial3
#endif




#define bufLen 600
unsigned int cmd;
char dataBuf[bufLen];
int idx = 0;
unsigned int codes[80];

/****************************************************



****************************************************/
void setup()
{

  BTSerial.begin(9600);
#ifdef _DEBUG
  Serial.begin(9600);
  Serial.print("Debug mode\n");
#endif
  
  irsend = new IRsend;

#ifdef _DEBUG
    Serial.print("BT 9600\n");
#endif 
  

}
/****************************************************



****************************************************/
int ConvCode()
{
  char tmp[16];
  int idx0 = 0;
  int idx1 = 0;

  for(int x=1; x<bufLen;x++) {
    if(dataBuf[x] == 0)  {
      tmp[idx0] = 0;
      if(idx0 > 0)
        codes[idx1++] = (unsigned int)atol(tmp);
        
#ifdef _DEBUG
    Serial.print(codes[idx1-1]);
    Serial.print(" ");
#endif

      break;
    }
    if(dataBuf[x] == ',') {
      tmp[idx0] = 0;
      codes[idx1++] = (unsigned int)atol(tmp);
#ifdef _DEBUG
    Serial.print(codes[idx1-1]);
    Serial.print(" ");
#endif      
      idx0 = 0;
      continue;
    }
    tmp[idx0++] = dataBuf[x];
  }
  
#ifdef _DEBUG
    Serial.print("Len:");
    Serial.println(idx1, HEX);
#endif  

  return idx1;
}
/****************************************************



****************************************************/
void loop()
{
  int len;
  while (BTSerial.available()) //connected
  { 
    cmd = BTSerial.read();
    
    if(cmd == '[') { // begin code
      dataBuf[0] = (char)cmd;
      idx = 1;
    }
    else if(cmd == ']') { // end code
      dataBuf[idx] = 0;
      idx = 0;
      len = ConvCode();

      irsend->sendRaw(codes,len,38);

    }
    else if(idx >= 1 && idx < bufLen) {
      if((cmd >= '0' && cmd <= '9') || cmd == ',') {  
        dataBuf[idx] = (char)cmd;
        idx++;
      }
      else { // error
        idx = 0;
      }

    }



  }
}


