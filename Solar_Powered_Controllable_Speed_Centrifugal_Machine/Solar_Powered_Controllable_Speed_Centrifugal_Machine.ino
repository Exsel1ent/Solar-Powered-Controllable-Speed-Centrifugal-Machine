#include <Wire.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
#include <Keypad.h>
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {13, 12, 11, 10};
byte colPins[COLS] = {9, 8, 7, 6};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
String action;
long timein[4], minute = 0, second = 0, countdown_time = 0, initialsecond = 0, spd[4], RPM = 0,countdowntime_seconds = 0, strtm = 0, selisih = 0, starttime = 0;
int i = 0;
int ctn = 0;
int RP = 0;
int ank = 0;
int sv = 0;
int TI = 6;
int j = 0;
int g = 1;
int enA = 5;
int in1 = 4;
int in2 = 3;
int pwmStorage = 0;
int k = 0;
int Timer = 0;
int buzz= 1;

float value = 0;
float rev = 0;
long rpm;
int oldtime = 0;
int time;

void isr() {
  rev++;
}

void setup() {
  
  //Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("A");
  lcd.setCursor(3, 0);
  lcd.print("SPEED =");
  lcd.setCursor(11, 0);
  lcd.print("0");

  lcd.setCursor(1, 1);
  lcd.print("B");
  lcd.setCursor(3, 1);
  lcd.print("Time  =");
  lcd.setCursor(11, 1);
  lcd.print("00:00");

  lcd.setCursor(1, 2);
  lcd.print("C");
  lcd.setCursor(3, 2);
  lcd.print("File  =       0-4");
  //lcd.setCursor(11, 1);
  //lcd.print("00:00");
  lcd.setCursor(1, 3);
  lcd.print("D Start");
  lcd.setCursor(13, 3);
  lcd.print("* Erase"); 
  
  // Set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(buzz, OUTPUT);
  
    // Set initial rotation direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(buzz, LOW);
  
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    switch (key) {

      //if key is A set speed.
      case 'A'  :
      spd[0] = 0;
      spd[1] = 0;
      spd[2] = 0;
      spd[3] = 0;
      spd[4] = 0;
        action = "set_speed";
        lcd.setCursor(11, 0) ;
        lcd.blink();
        lcd.print("0     ");
        j = 0;
        RPM = 0;
        k = 0;
        //pwmStorage = 0;

        lcd.setCursor(11, 0) ;
        break;

      //If key is B set time.
      case 'B'  :
      lcd.setCursor(11, 1);
      lcd.blink();
        lcd.print("00:00");
        timein[0] = 0;
        timein[1] = 0;
        timein[2] = 0;
        timein[3] = 0;
        action = "set_time";
        lcd.setCursor(11, 1);
        //lcd.blink();
        i = 0;
        break;

      //If key is C set file.
      case 'C'  :
        sv = 0;
        g = 1;
        lcd.setCursor(13, 2);
        lcd.print("       ");
        lcd.setCursor(13, 3);
        lcd.print("       ");
        lcd.setCursor(3, 2);
        lcd.print("File  =       0-4"); 
        lcd.setCursor(11, 2) ;
        lcd.blink();       

        action = "set_file";
        
        break;

      //If key is D start the system.
      case 'D'  :
        selisih = 0;
        initialsecond = 0;
        countdown_time = strtm;
        lcd.setCursor(1, 3);
        lcd.print("D Stop ");
        pwmStorage = RPM;
        action  = "start_all";
        starttime = countdown_time-2;
        lcd.noBlink();
        attachInterrupt(digitalPinToInterrupt (2), isr, RISING); //interrupt pin
          
      
        break;
      default :

        //pengaturan untuk file
        if (action == "set_file" && sv == 0) {
            g=1;
            lcd.setCursor(3, 2);
            lcd.print("File  =       0-4");
            lcd.setCursor(11, 2);
            RP = 0;
            TI = 6;
            int address[2];
            g++;
            int z = g - 1;
            address[z] = key-48;
            
            lcd.print(address[1]);
            //Serial.print(z);
            ank = address[1];
         
       
          
          
            RP = RP+address[1];
            TI = TI+address[1];
           
              if (address[1] >= 5){  //pembatas input angka
                g--;
                //long RP = 0;
                //long TI = 6;
                //long address = 0;                
                lcd.setCursor(11, 2) ;
                lcd.print("hanya");
                lcd.noBlink();
                delay(3000);  
                lcd.setCursor(11, 2) ;
                lcd.print("     ");             
                lcd.setCursor(11, 2) ;
                lcd.blink();
                action = "set_file"; 
              }
                //Serial.print(RP);
                //Serial.println("  RP");
                //Serial.print(TI);
                //Serial.println("  TI");
                //Serial.print(address[1]);
                //Serial.println("  address \n");

                          
              if (address[1] <5){  //lanjutan setelah input angka
        lcd.setCursor(13, 2);
        lcd.print("# Use  ");
        lcd.setCursor(13, 3);
        lcd.print("* Save "); 
        lcd.noBlink();     
          if(z>=1){
          action = "set_move";          
              }
           }          
        }
        
         if (action == "set_move") { //next move dari file

            sv = 0;
            lcd.noBlink();
            
           //pengaturan Save
          if (key == '*'){
            //Serial.println(TI);
            //Serial.println(RP);            
            EEPROM.put(RP, RPM);
            //Serial.println(RPM);
            EEPROM.put(TI, countdown_time);   
            //Serial.println(countdown_time);
            lcd.setCursor(1, 2) ;
            lcd.print("data sudah disimpan");  
            delay(1000);
            lcd.setCursor(1, 2);
            lcd.print("C File  = ");
            lcd.print(ank);
            lcd.print("     0-4");                 
          }
          
          //pengaturan Use
          if (key == '#'){            
            EEPROM.get(RP, RPM);
            lcd.setCursor(11, 0);
            lcd.print(RPM);
            EEPROM.get(TI, countdown_time);
            long showmin = (countdown_time / 60);
            long showsec = countdown_time % 60;
            lcd.setCursor(11, 1);
            lcd.print("      ");
            if(showmin >= 10){  
            lcd.setCursor(11, 1);
            lcd.print(showmin);
                if(showsec >= 10){
                  lcd.print(":");
                  lcd.print(showsec);
                }
                if(showsec < 10){
                  lcd.print(":");
                  lcd.setCursor(14, 1);
                  lcd.print('0');
                  lcd.print(showsec);
                }
            }
            if(showmin < 10){
            lcd.setCursor(11, 1);
            lcd.print('0');    
            lcd.setCursor(12, 1);
            lcd.print(showmin);
                if(showsec >= 10){
                  lcd.print(":");
                  lcd.print(showsec);
                }
                if(showsec < 10){
                  lcd.print(":");
                  lcd.setCursor(14, 1);
                  lcd.print('0');
                  lcd.print(showsec);
                }
            }
            lcd.setCursor(13, 2);
            lcd.print("    0-4");
            lcd.setCursor(13, 3);
            lcd.print("      ");
            lcd.setCursor(13, 3);
            lcd.print("* Erase"); 
                              
          }
         }

//pengaturan untuk waktu
       if (action == "set_time") {
          
          i++;
          int c = i - 1;

          //untuk erase waktu
          if(key == '*'){
           i=i-2;
           c--;
           if (i<2){
           minute = (minute-timein[c])/10;
           lcd.setCursor(11+c, 1);
           lcd.print(' ');
           lcd.setCursor(11+c, 1);
           }
           if (i>=2){
            second = (second-timein[c])/10;
            if(i==2){
             lcd.setCursor(11+c+1, 1); 
             lcd.print(' ');
             lcd.setCursor(11+c+1, 1);
            }
            if(i>2){
              lcd.setCursor(11+c, 1);
              lcd.print(' ');
              lcd.setCursor(11+c, 1);
            }
           }
          }

          //untuk mencegah erase masuk menjadi angka
          if (key!='*'){        
          timein[c] = key - 48;
          //initialsecond = 0;
          minute = (timein[0] * 10) + timein[1];
          second = (timein[2] * 10) + timein[3]; //second
          countdown_time = (minute * 60) + second;
          strtm = countdown_time;
          lcd.print(key);
          if (i % 2 == 0 && i < 4) {
            lcd.print(":");
          }
          if(c>=3)
          action = "set_input";
        }
        }
        
//pengaturan untuk speed
        if (action == "set_speed" && k == 0) {

          j++;
          int d = j - 1;
          
          //untuk erase speed angka kurang dari 5
          if (key == '*' && j<5){
          j=j-2;
          d--;
          RPM = (RPM-spd[d])/10;
          lcd.setCursor(11+d, 0);
          lcd.print(' ');
          lcd.setCursor(11+d, 0);
          }
          
        //untuk erase speed angka 5 digit
          if (key == '*' && j==5){
          j=j-2;
          d--;
          RPM = (RPM-spd[d])/10;
          lcd.setCursor(11+d+1, 0);
          lcd.print(' ');
          lcd.setCursor(11+d+1, 0);
          }

          //untuk mencegah erase masuk menjadi angka
          if (key != '*'){
          spd[d] = key - 48;
          RPM = (RPM * 10) + spd[d];            
          //Serial.println(RPM);
          //Serial.println(j);
          //Serial.println(d);
          lcd.print(spd[d]);
          if (j == 4) {
             lcd.setCursor(11, 0) ;
            lcd.print(spd[0]);
            lcd.print(".");
            lcd.print(spd[1]);
            lcd.print(spd[2]);
            lcd.print(spd[3]);
          }
          if (j == 5) {
            lcd.setCursor(11, 0) ;
            lcd.print(spd[0]);
            lcd.print(spd[1]);
            lcd.print(".");
            lcd.print(spd[2]);
            lcd.print(spd[3]);
            lcd.print(spd[4]);
            action = "set_input";
          }
          
          //untuk membatasi RPM
          if ( RPM > 13000){
           lcd.setCursor(0, 0) ;
            lcd.print("RPM MAX HANYA= 13000");
            lcd.noBlink();
             delay(3000);
            lcd.setCursor(0, 0);
            lcd.print("                    "); 
            lcd.setCursor(1, 0);
            lcd.print("A");
            lcd.setCursor(3, 0);
            lcd.print("SPEED =");
             action = "set_speed";
        lcd.setCursor(11, 0) ;            
        lcd.blink();
        lcd.print("0     ");
        j = 0;
        RPM = 0;
        k = 0;
        //pwmStorage = 0;
        lcd.setCursor(11, 0) ;
          }
        }
        }
          if (action == "set_input") {        
            //pwmStorage = RPM;
            //Serial.println(pwmStorage);
            k = 1;
            lcd.noBlink();
        }
        break;
    }
  }   
  
  //untuk memulai putaran waktu dan motor
  if (action  == "start_all") {
 

    //Serial.println(countdown_time);
    
    //loop waktu
    if (initialsecond == 0) {
      initialsecond = millis() / 1000;
    }
    countdowntime_seconds = countdown_time - (millis() / 1000) + initialsecond;
    if (countdowntime_seconds >= 0) {
      long countdown_minute = (countdowntime_seconds / 60);
      long counddown_sec = countdowntime_seconds % 60;
      lcd.setCursor(11, 1);
      if (countdown_minute < 10) {
        lcd.print("0");
      }
      lcd.print(countdown_minute);
      lcd.print(":");
      if (counddown_sec < 10) {
        lcd.print("0");
      }
      lcd.print(counddown_sec);

//tachometer
delay(1000);
detachInterrupt(digitalPinToInterrupt(2));    
  time = millis() - oldtime;    
  //Serial.print(time);
  //Serial.print("  ");
  rpm = (rev / (time*2.1)) * 60000;   
  //Serial.println(rev);
    //Serial.print("  ");


  rev = 0;
  lcd.setCursor(11, 0);
  lcd.print("     "); 
  lcd.setCursor(11, 0);
  lcd.print(rpm);
  //Serial.println(rpm);

   //ketentuan motor
       if (countdowntime_seconds % 2 == 0 && starttime > countdowntime_seconds) {
        
        selisih = RPM - rpm;             
        //close loop 
       
         if (selisih >= 3000){
        //defide = selisih/1000; 
        pwmStorage = pwmStorage + 2000;  
        }
         if (selisih <= -3000){
        //defide = selisih/1000;
        pwmStorage = pwmStorage - 2000;  
        }
         if (selisih >= 1000 && selisih < 3000){
        //defide = selisih/1000; 
        pwmStorage = pwmStorage + 1000;  
        }
         if (selisih <= -1000 && selisih > -3000){
        //defide = selisih/1000;
        pwmStorage = pwmStorage - 1000;  
        }
         if (selisih >= 400 && selisih < 1000){
        //defide = selisih/100;
        pwmStorage = pwmStorage + 200;  
        }
         if (selisih <= -400 && selisih > -1000){
        //defide = selisih/100;
        pwmStorage = pwmStorage - 200;  
        }
         if (selisih >= 100 && selisih < 400){
        //defide = selisih/100;
        pwmStorage = pwmStorage + 100;  
        }
         if (selisih <= -100 && selisih > -400){
        //defide = selisih/100;
        pwmStorage = pwmStorage - 100;  
        }
         if (selisih >= 40 && selisih < 100){
        //defide = selisih/10;
        pwmStorage = pwmStorage + 20;    
        }
         if (selisih <= -40 && selisih > -100){
        //defide = selisih/10;
        pwmStorage = pwmStorage - 20;    
        }
        if (selisih >= 10 && selisih < 40){
        //defide = selisih/100;
        pwmStorage = pwmStorage + 10;  
        }
         if (selisih <= -10 && selisih > -40){
        //defide = selisih/100;
        pwmStorage = pwmStorage - 10;  
        }
         if (selisih >= 1 && selisih < 10){
        pwmStorage = pwmStorage + selisih;    
        }
         if (selisih <= -1 && selisih > -10){
        pwmStorage = pwmStorage - selisih;    
        }
        if (selisih == 0){ 
        pwmStorage = pwmStorage;
        }  
        //loop RPM
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);        
        int potValue = pwmStorage; // Read number of RPM
        float pwmOutput = map(potValue, 0, 13000, 0 , 255); // Map the potentiometer value from 0 to 255
        analogWrite(enA, pwmOutput );
       }


  if (countdowntime_seconds == 0) {
    pwmStorage = 0;   
    delay(500);    
         //loop RPM
      digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
   int potValue = 0000; // Read potentiometer value
  float pwmOutput = map(potValue, 0, 13000, 0 , 255); // Map the potentiometer value from 0 to 255
  analogWrite(enA, pwmOutput );
  //Serial.println(enA);
  
      // time show
      lcd.setCursor(11, 1) ;
      lcd.print('0');
      lcd.print('0');
      lcd.print(":");
      lcd.print('0');
      lcd.print('0');
      
    //RPM show
        lcd.setCursor(11, 0);
    lcd.print("0       ");
             if (j == 3) {
            lcd.setCursor(11, 0) ;
            lcd.print(spd[0]);
            lcd.print(spd[1]);
            lcd.print(spd[2]);
          }
              if (j == 4) {
            lcd.setCursor(11, 0) ;
            lcd.print(spd[0]);
            lcd.print(".");
            lcd.print(spd[1]);
            lcd.print(spd[2]);
            lcd.print(spd[3]);
          }
          if (j == 5) {
            lcd.setCursor(11, 0) ;
            lcd.print(spd[0]);
            lcd.print(spd[1]);
            lcd.print(".");
            lcd.print(spd[2]);
            lcd.print(spd[3]);
            lcd.print(spd[4]);
            action = "set_input";
          }
          
  for(ctn=0;ctn < 5;ctn++){
digitalWrite(buzz, HIGH);
delay(500);
     // time show
      lcd.setCursor(11, 1) ;
      lcd.print("      ");
      
digitalWrite(buzz, LOW);
delay(500);
     // time show
      lcd.setCursor(11, 1) ;
      lcd.print('0');
      lcd.print('0');
      lcd.print(":");
      lcd.print('0');
      lcd.print('0');
      
if (ctn > 3){
        // time show
      lcd.setCursor(11, 1) ;
      lcd.print(timein[0]);
      lcd.print(timein[1]);
      lcd.print(":");
      lcd.print(timein[2]);
      lcd.print(timein[3]);
      
    lcd.setCursor(1, 3);
     lcd.print("D Start ");
  }
  }
  }
             
      attachInterrupt(digitalPinToInterrupt (2), isr, RISING);
        oldtime = millis(); 
    }
  }
}
