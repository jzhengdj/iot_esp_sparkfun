 
volatile float riseCount; //number of interrupts generated from Hallsensor.
float flowAmount; //calculate amount from rise count.
float flowSum; //sum of the flow duration.

long useCount = 1;
long flowDuration = 0;

#define hallsensor D3 // Wemos D0 D4 cannot use interrupt.

void countRise ()//this is the function that the interrupt calls
{
  riseCount++;// this function measure the rising and falling edge if the Hall sensor  
}

void disp ()
{
    Serial.println("");
    Serial.print ("amount:"); 
    Serial.println (flowSum);
    //Serial.print("   ");
    Serial.print("duration: ");
    Serial.println(flowDuration);
    //Serial.print("   ");
    Serial.print("Number of use: ");
    Serial.println(useCount);
 }


void setup()
{
  pinMode(hallsensor, INPUT);//initialise Hallsensor Pin as input
  Serial.begin(9600);//set up for the serial port
  attachInterrupt(hallsensor, countRise, RISING);//Attach interrupt to HallSensor output.  
}

void loop() {
  //Serial.println("test");
  riseCount = 0;//set RiseCount to 0

  sei();        //enable interrupt
  delay(1000);  //wait 1 second
  cli();        //disable interrupt
  
  flowAmount = (riseCount*2.1909);
  //Serial.println(RiseCount);
  
  if(flowAmount!=0)
  {
    flowSum+=flowAmount;
    flowDuration++;
    //disp();
  }
  else if(flowSum!=0)
  {
    disp();
    useCount++;
    flowDuration=0;
    flowSum=0;
  }

}

