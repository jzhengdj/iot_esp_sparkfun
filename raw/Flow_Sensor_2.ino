 
volatile float RiseCount; //measuring the rising edge of the signal
float cal;
float counter=0;
float sum=0;
float total=0;

int state = 0;
int use=1;
#define hallsensor D0 //pin location of sensor output


void rpm ()//this is the function that the interrupt calls
{
  RiseCount++;// this function measure the rising and falling edge if the Hall sensor  
}

void disp ()
{
    Serial.println("");
    Serial.print ("amount:"); 
    Serial.println (sum);
    //Serial.print("   ");
    Serial.print("total: ");
    Serial.println(total);
    //Serial.print("   ");
    Serial.print("Number of use: ");
    Serial.println(use);
 }

void setup()
{
  pinMode(hallsensor, INPUT);//initialise pin2 as input
  Serial.begin(9600);//set up for the serial port
  attachInterrupt(0, rpm, RISING);//interrupt function
  
}
  
void loop() {
 
  RiseCount = 0;//set RiseCount to 0

  sei();        //enable interrupt
  delay(1000);  //wait 1 second
  cli();        //disable interrupt

  cal = (RiseCount*2.1909);

  if(Serial.available() > 0)  // Checks whether data is comming from the serial port
  { 
    state = Serial.read(); 
    if(state == '0')
    { Serial.println ("");
      Serial.println ("total is reset");
      total = 0;
      use = 1;
      }
  }
  if(cal!=0)
  {
    sum=sum+cal;
    
  }
  else if(sum!=0)
  {
    total=total+sum;
    disp();
    use++;
    sum=0;
  }

}
