
    #include "LedControlMS.h"  // 8X8 LED Matrix 부품을 사용하기 위한 LedControlMS 라이브러리 포함  
    #include <SoftwareSerial.h>  //앱과 블루투스 통신을 위한 softwareSerial 라이브러리 포함

    
   

    
    #define LED 13 // LED, 13핀
    #define AIR 12 // AIR_Cleaner, 12핀
    
    
    
    #define PIEZO 8          //효과음을 내는 피에조 부조음 8핀
    #define LIGHT_SENSOR A0  //조도 센서 PIN 설정
    
    /* 
    *  블루투스 통신은 RX(받고), TX(주고) 신호가 필요합니다  
    *  앱으로 블루투스 신호를 보냈으면 잘 받았는지 확인하는 것이 필요하죠. 
    */    
    
    #define RX 2   // 블루투스 통신에서 받는 D12핀
    #define TX 3    //블루투스 통신에서 주는 D11핀
  
    
    /* 
     *   8X8 LED Matrix를 사용하기 위해서는
     *   세가지 핀이 필요함. 
     */
    
     #define DIN A1
     #define CS  A2
     #define CLK A3
     
    
     
     #define NBR_MTX 1 // 8X8 LED Matrix를 몇개 사용할 지를 정한다. 현재 하나만 셋팅.
     LedControl lc=LedControl(DIN,CLK,CS, NBR_MTX); //8X8 LED Matrix 클래스를 세개의 핀으로 연결한다. 
    
    
    
    //bluetooth ,Seraila buffer Data 셋팅
    char inBTChar; //blueTooth 글자 체크
    char inSLChar; //Serial 글자 체크
    
    //ON,OFF,DUST 스트링 셋팅시상
    String inputBTString ;  //On,OFF 체크... 
    
    
    //Bluetooth (3,4)번 연결로 셋팅  
    SoftwareSerial BTSerial(TX,RX);  //bluetooth

    bool toggle_check = true;
   
    int light = 0;
 

    /*  8X8 LED Matrix 
     *  64개 LED를 하나씩 제어가 가능하다. 
     *  이모티콘 및 다양한 모양들을 제작할 수 있다.
     *  0은 LED OFF / 1은 LED ON 이다. 
     */
    
     byte all[] =  //LED가 다 켜진 상태
    {
       B11111111,  
       B11111111,  
       B11111111,  
       B11111111,  
       B11111111,
       B11111111,  
       B11111111,  
       B11111111  
    };
    
      byte smile1[] = //스마일 이모티콘 모양 
    {
       B00111100,  
       B01111110,
       B01111110,
       B11111111,
       B11111111,
       B01111110,
       B01111110,
       B00111100
    };

    byte smile0[] = //스마일 이모티콘 모양 
    {
       B00000000,  
       B00111100,
       B01111110,
       B01111110,
       B01111110,
       B01111110,
       B00111100,
       B00000000
    };
    
     byte angry0[] = //화난 이모티콘 모양
    {
       B10000001,   
       B01000010,
       B00111100,
       B00111100,
       B00111100,
       B00111100,
       B01000010,
       B10000001
    };

      byte angry1[] = //화난 이모티콘 모양
    {
       B00000000,   
       B00000000,
       B00011000,
       B00100100,
       B00100100,
       B00011000,
       B00000000,
       B00000000
    }; 
    
    
    byte arrow0[] = //완성된 화살표 이모티콘
    {
       B00000000,  
       B00000000,
       B00000000,
       B00000000,
       B00000000,
       B00000000,
       B00011000,
       B00111100,
    };

    
    
    
    void printByte(byte character );  //8X8 LED Matrix 이모티콘 동작하는 함수 
  
    
    void setup() { // 아두이노 스케치를 시작할 때, 사용하는 함수입니다.
     
       BTSerial.begin(9600);  // 블루투스 통신 속도를 셋팅함. 9600은 최대 1초당 9600byte 주고 받음.
       Serial.begin(9600);
    
       pinMode(AIR, OUTPUT);   //SSR OUTPUT Mode Setting
       pinMode(LED, OUTPUT);   //LED OUTPUT Mode Setting
       pinMode(PIEZO,OUTPUT);  //PIEZO OUTPUT Mode Setting  

       pinMode(LIGHT_SENSOR,INPUT); //light Sensor mode setting

       
       Serial.print("light:");



      // 8X8 LED Matrix 클래스에  일일히 하나씩 LED 위치 등록하여 초기화 
       for (int i=0; i< NBR_MTX; i++){
        lc.shutdown(i,false);
        lc.setIntensity(i,8);
        lc.clearDisplay(i);
        
        delay(200);
      }

       airOn();
    
       tone(PIEZO,3200,100);  // tone은 소리내는 함수임 . tone (디지털 신호핀, 음 높이, 음 길이)를 뜻함.  
                              // 음 높이는 숫자가 높을수록 고음 음 길이는 1000은 1초, 100은 0.1초동안 울림 

        lc.clearAll();   //  화살표 1
        delay(50);
        printByte(arrow0);   //화살표 2
        delay(100);

        printByte(arrow1);   //화살표 3
        delay(200);

        printByte(arrow2);   //화살표 4
        delay(100);

        printByte(arrow3);   //화살표 5
        delay(300);
    
    }


    void loop(){
    
 
       while(BTSerial.available()){ //블루통신 
           
           
             inBTChar = (char)BTSerial.read();
                  
                 if(inBTChar == '\n' || inBTChar == '\r' || inBTChar == '_'){
                        if(inputBTString.equals("ON")){ //ON 신호  
                             toggle_check = false;
                             shift_string ("ON ON",70);  

                            airOn();
                            tone(PIEZO,500,100);

                        }else if(inputBTString.equals("OFF")){ //OFF 신호
                            toggle_check = true;
                           shift_string ("OFF OFF",70);  
                        
                            airOff();
                            tone(PIEZO,500,100);
      
                       }else if(inputBTString.equals("LIGHT")){ //light 신호
                           lightSensor();
                           BTSerial.print(String(light));
                           Serial.print("light");        
                           
                           delay(5);   
                       }else{  //블루투스 통신이 되었을 때... 디폴트 값
                         
                          inputBTString = "";
                         }
                 }//if closed
              else  inputBTString += inBTChar;
       } //while closed

   if(!BTSerial.isListening()) {toggle_check= true;}  //블루투스가 연결되지 않을경우에는 자동제어로 동작한다. 
  
      if(toggle_check == true){ // 자동제어로 동작한다 
         lightSensor();
         
         if(light < 30){  
           airOff();
         }else {  
           airOn(); 
         }
    } //if closed

   delay(50);    
  
  } //loop


  void lightSensor(){
        light= analogRead(LIGHT_SENSOR);  
        light = map(light, 0, 1023, 0, 100); 
        light = 100-light; 

        Serial.println("light:");
        Serial.println(light);
        delay(5);
  }

  void airOn(){

      digitalWrite(LED,HIGH);  // LED ON
      digitalWrite(AIR,HIGH); // 공기청정기 ON

      int toggle = random(0,15);

      if(toggle == 2) { tone(PIEZO,100,50);} 
      
        printByte(smile0);
        delay(100);
        printByte(smile1);
        delay(200);
       
      
  }

  void airOff(){

      digitalWrite(LED,LOW);  //LED OFF
      digitalWrite(AIR,LOW); // 공기청정기 ON

      printByte(angry0);
      delay(200);
      printByte(angry1);
      delay(300);
     
          
  }


    void shift_string(String str,int delay_time){  //좌로 불러오기 (Sting/ 대기 시간)
      
           int count = ((str.length()-1)*6)+1; 
           int i = 0;
                            
           for(int j=count+1; j>-3;j--){  //좌우 바뀌는 것..
               for(int z=str.length()-1; z>-1;z--){
                       Serial.print(",Z:");
                       int display_char = lc.getCharArrayPosition(str[z]);
                       displayShiftChar(j-count+(z*6),display_char);                           

                               }
                  delay(delay_time);
                  lc.clearAll();
               }
        }

    void displayShiftChar(int matrix, int charIndex) { //문자 좌우로 보여주기
              int count =0; 
              for (int i=matrix; i< matrix+6;i++) {
                  lc.setRow(0,i, alphabetBitmap[charIndex][count++]);
              } 
      }

          
  void printByte(byte character [])
  {
    int i = 0;
    for(i=0;i<8;i++)
    {
      lc.setColumn(0,7-i,character[i]);
    }
  }
          
     
