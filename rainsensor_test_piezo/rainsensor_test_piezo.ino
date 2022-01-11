#define rainfall A0
//#define piezo 8
int rainvalue;
int set=900;// set a value to test the rain sensor


void setup() {
  Serial.begin(9600);
  pinMode(rainfall,INPUT);
  
  }

 void loop() {
 rainvalue = analogRead(rainfall);// read rainfall data 
 delay(1000);
 Serial.println(rainvalue);
 //rainvalue = map(rainvalue,0,1023,225,0);
 if(rainvalue<=set){
  Serial.println("rain detected");
  tone(8,440,20);// set piezo buzzles
  delay(10);
  
 }
 else{
  tone(8,0,20);// set piezo off
 }

 }
 
