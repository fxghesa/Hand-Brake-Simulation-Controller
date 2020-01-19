#include <LiquidCrystal.h>
#include <Servo.h>

//setting & global variable
LiquidCrystal lcd(7,8,9,10,11,12,13);
int LEDPin = 13;
#define vcc 7

//setting servo
Servo myservo;
int pos = 0;

//setting us
#define echoPin 2 //Echo Pin
#define trigPin 3 //Trigger Pin
int maximumRange = 200; //kebutuhan akan maksimal range
int minimumRange = 0; //kebutuhan akan minimal range
long duration, distance; //waktu untuk kalkulasi jarak

void setup() {
  // put your setup code here, to run once:
  lcd.begin(20,4);
//  pinMode(led, OUTPUT);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
//  Serial.println("Serial Ready!");
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LEDPin, OUTPUT);
  pinMode(vcc, OUTPUT);
  digitalWrite(vcc, HIGH);
  myservo.attach(4);
  pos = 30; // 30-150
  fuzzy(pos,jarak());
  fuzzy(pos,jarak());
//

  
}

int counter = 0;

void loop() {
  // put your main code here, to run repeatedly:
  pos = 30;
  while(counter >= 0 && counter <= 120){
    pos++;
    fuzzyRealTime(pos,jarak());
    counter++;
  }
  while(counter >= 120 && counter <= 240){
    pos--;
    fuzzyRealTime(pos,jarak());
    counter++;
  }
  counter = 0;
}

void putar(){
  for(pos = 0; pos < 180; pos += 1)  {
    myservo.write(pos);
  // tunggu 15ms untuk pencapaian  posisi servo    
  delay(15);                  
 } 
 // start dari 180 derajat ke 0 derajat 
 for(pos = 180; pos>=1; pos-=1)  
 {
  // memberitahu servo untuk pergi ke posisi  'pos'                                
  myservo.write(pos);                 
  // tunggu 15ms untuk pencapaian  posisi servo    
  delay(15);                        
 }
}

long jarak(){
  digitalWrite(trigPin, LOW);delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  //perhitungan untuk dijadikan jarak
  distance = duration/58.2;
//  Serial.println(distance);
  return distance;
}

float MFx(float a,float b,float z){
    float hasil_MFx;
  if ((z >= a) && (z < b))hasil_MFx=(b - z)/(b - a);
    if (z <= a) hasil_MFx=1;
  if (z >= b) hasil_MFx=0;
  return hasil_MFx;
}

float MFy(float a,float b,float z){
    float hasil_MFy;
  if ((z >= a) && (z < b))hasil_MFy=(z - a)/(b - a);
    if (z <= a) hasil_MFy=0;
  if (z >= b) hasil_MFy=1;
  return hasil_MFy;
}

float Min(float a,float b){
  float hasil_min;
  if (a<b) 
    hasil_min=a;
  else  
    hasil_min=b;
  return hasil_min;
}

void fuzzy(float sudut, float jarak){
  float u1x,u2x,u3x,u1y,u2y,u3y;
  float minR1,minR2,minR3,minR4;
  float proR1,proR2,proR3,proR4;

  //batasi jarak
  if(jarak >=25) jarak=25;
  if(jarak <=5) jarak=5;
  Serial.println("##############");
  Serial.println("Memulai fuzzy");
  Serial.println("##############\n");
  //Fuzzyfication
//  float sudut=190;  //permintaan 100-200 -> 30-150
//  float jarak=jarak();   //persediaan 10-20 -> 5-25
  Serial.print("Persoalan:\n");
  Serial.print("sudut: "); Serial.println(sudut);
  Serial.print("jarak: "); Serial.println(jarak);
  Serial.println("Berapa jumlah sudut?");
  u1x=MFx(30,150,sudut);//permintaan turun
  Serial.print("Fungsi keanggotaan sudut turun: "); Serial.println(u1x);
  u1y=MFy(30,150,sudut);//permintaan naik
  Serial.print("Fungsi keanggotaan sudut naik: "); Serial.println(u1y);
  
  u2x=MFx(5,25,jarak);//persediaan sedikit
  Serial.print("Fungsi keanggotaan jarak dekat: "); Serial.println(u2x);
  u2y=MFy(5,25,jarak);//persediaan banyak
  Serial.print("Fungsi keanggotaan jarak jauh: "); Serial.println(u2y);

  Serial.print("Rule yang digunakan : \n");
  //rule 1 : IF Permintaan TURUN And Persediaan BANYAK THEN Produksi Barang BERKURANG
  Serial.print("rule 1 : IF Sudut TURUN And jarak JAUH THEN Posisi BERKURANG");
  minR1=Min(u1x,u2y);
  Serial.println("1. Minimum sudut turun And jarak sepi: "); Serial.println(minR1);
  proR1=150-minR1*120;
  Serial.print("sudut turun: "); Serial.println(proR1);
  
  //rule 2 : IF Permintaan TURUN And Persediaan SEDIKIT THEN Produksi Barang BERKURANG
  Serial.println("rule 2 : IF Sudut TURUN And Jarak DEKAT THEN Posisi BERKURANG");
  minR2=Min(u1x,u2x);
  Serial.print("2. Minimum sudut turun And jarak macet: "); Serial.println(minR2);
  proR2=150-minR2*120;
  Serial.print("sudut turun: "); Serial.println(proR2);
  
  //rule 3 : IF Permintaan NAIK And Persediaan BANYAK THEN Produksi Barang BERTAMBAH
  Serial.println("rule 3 : IF Sudut NAIK And Jarak JAUH THEN Posisi BERTAMBAH");
  minR3=Min(u1y,u2y);
  Serial.print("3. Minimum sudut naik And jarak sepi: "); Serial.println(minR3);
  proR3=30+minR3*120;
  Serial.print("sudut naik: "); Serial.println(proR3);
  
  //rule 4 : IF Permintaan NAIK And Persediaan SEDIKIT THEN Produksi Barang BERTAMBAH
  Serial.println("rule 4 : IF Sudut NAIK And Jarak DEKAT THEN Posisi BERTAMBAH");
  minR4=Min(u1y,u2x);
  Serial.print("4. Minimum sudut naik And jarak macet: "); Serial.println(minR4);
  proR4=30+minR4*120;
  Serial.print("sudut naik: "); Serial.println(proR4);

  float pembilang = minR1 * proR1 + minR2 * proR2 + minR3 * proR3 + minR4 * proR4;
    float penyebut  = minR1 + minR2 + minR3 + minR4;
    float hasil_COA = pembilang/penyebut;
  Serial.print("Jadi sudut yang harus ditentukan: "); 
  Serial.println(hasil_COA);
  myservo.write(hasil_COA);

  Serial.println("");
}

void fuzzyRealTime(float sudut, float jarak){
  float u1x,u2x,u3x,u1y,u2y,u3y;
  float minR1,minR2,minR3,minR4;
  float proR1,proR2,proR3,proR4;

  //batasi jarak
  if(jarak >=25) jarak=25;
  if(jarak <=5) jarak=5;
  //cetak
  Serial.print(sudut);
  Serial.print("  ");
  Serial.print(jarak);
  Serial.print("  ");
  u1x=MFx(30,150,sudut);
  u1y=MFy(30,150,sudut);
  u2x=MFx(5,25,jarak);
  u2y=MFy(5,25,jarak);
  minR1=Min(u1x,u2y);
  proR1=150-minR1*120;
  minR2=Min(u1x,u2x);
  proR2=150-minR2*120;
  minR3=Min(u1y,u2y);
  proR3=30+minR3*120;
  minR4=Min(u1y,u2x);
  proR4=30+minR4*120;
  float pembilang = minR1 * proR1 + minR2 * proR2 + minR3 * proR3 + minR4 * proR4;
  float penyebut  = minR1 + minR2 + minR3 + minR4;
  float hasil_COA = pembilang/penyebut;
  Serial.println(hasil_COA);
  myservo.write(hasil_COA);
}

