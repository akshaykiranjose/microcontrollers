//rx

#define threshold 500
#define ldrPin 2
#define IN_LDR() DDRB &= ~((1 << ldrPin))
#define GET_LDR() analogRead(ldrPin) > threshold ? 0: 1
//#define GET_LDR() !digitalRead(ldrPin)


const int symbolPeriod = 36;
const int Delay = symbolPeriod/3;
//const int samplesPerSymbol = 24;

unsigned long currentMillis = 0;
unsigned long previousMillis = 0; 

float avgSampleEng = 0.0;
int numSamplesTakenPerSymbol = 0;
int ldrValSum = 0;

char decodedChar;
bool previous_state;
bool current_state;

void setup()
{
  IN_LDR();
  Serial.begin(9600);
}

void loop()
{
  current_state = GET_LDR();
  if(!current_state && previous_state)
  {
    currentMillis = millis();
    while((millis() - currentMillis) < Delay) {}
    getByte();   
  }
  previous_state = current_state;
}

void getByte()
{
  decodedChar = 0;
  for(int i=0; i<8; i += 2)
  {
    ldrValSum = 0;
    numSamplesTakenPerSymbol = 0;

    currentMillis = millis();
    while((millis() - currentMillis) < symbolPeriod) // nearly 430 samples can be taken
    {
      ldrValSum += GET_LDR();
      ++numSamplesTakenPerSymbol;
    }
    avgSampleEng = (float) ldrValSum/numSamplesTakenPerSymbol;
    
    if(avgSampleEng < 0.17)
      decodedChar |= ((0 << i));
    else if(avgSampleEng < 0.50)
      decodedChar |= ((1 << i));
    else if(avgSampleEng < 0.84)
      decodedChar |= ((2 << i));
    else
      decodedChar |= ((3 << i));
  }
  Serial.print(decodedChar);
}
