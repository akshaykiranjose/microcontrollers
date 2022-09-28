// tx
//These defines are for a single led connected to D2
#define ledPin 2
#define OUT_LED()  
#define SET_LED() PORTD |= ((1 << ledPin))
#define CLR_LED() PORTD &= ~((1 << ledPin))

const int symbolPeriod = 36;
const int Delay = symbolPeriod/3;
unsigned long timeNow = 0;

byte capture = B11;
int op;
char *txChar = "Sare Jahan Se Accha!\n";
//char txChar = 'L';
int txLen;

void setup()
{
  OUT_LED();
  SET_LED();
  delay(3250);
  Serial.begin(9600);
  txLen = strlen(txChar);
}

void loop()
{
  SET_LED();
  delay(100);
  for(int i=0; i < txLen; ++i)
    sendByte(txChar[i]);
  /*
  Serial.print("Word Sent");
  Serial.println(j++);
  */
}

void sendByte(char ch)
{

  CLR_LED();
  timeNow = millis();
  while((millis()-timeNow) < Delay) {}
  /*
  timeNow = millis();
  while((millis()-timeNow) < Delay) {}
  */  
  for(int i=0; i<8; i+=2)
  {
    op = (ch & (capture << i)) >> i;
    //Serial.println(op);
    switch(op)
    {
      case 0:
      {
        CLR_LED();
        timeNow = millis();
        while((millis()-timeNow) < 3*Delay) {}
        break;
      }
      case 1:
      {
        SET_LED();
        timeNow = millis();
        while((millis()-timeNow) < Delay) {}
        CLR_LED();
        timeNow = millis();
        while((millis()-timeNow) < 2*Delay) {}
        break;
      }
      case 2:
      {
        SET_LED();
        timeNow = millis();
        while((millis()-timeNow) < 2*Delay) {}
        CLR_LED();
        timeNow = millis();
        while((millis()-timeNow) < 1*Delay) {}
        break;
      }
      case 3:
      {
        SET_LED();
        timeNow = millis();
        while((millis()-timeNow) < 3*Delay) {}
        break;
      }
      default:
      {
        break;
      }

    }
  
  }
  SET_LED();
  timeNow = millis(); 
  while((millis()-timeNow) < 4*Delay) {}
}
