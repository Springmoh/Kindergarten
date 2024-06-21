#include <LedControl.h>

/*
 Now we need a LedControl to work with.
 ** These pin numbers will probably not work with your hardware **
 pin 9 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 */
LedControl lc = LedControl(9, 11, 10, 1);

#include <string.h>
#define BUTTON 2  //assign pin 2 to the button

enum {
  Decimal = 1,
  Binary,
  Hexadecimal,
};

enum {
  STOP1,
  UP,
  STOP2,
  DOWN,
};

int view = Decimal;
int counter = 0;
int state = STOP1;
int DecDigitCheck = 1;
int BinDigitCheck = 4;
int HexDigitCheck = 1;
float timer = millis();
float pressedtime = millis();
char Dec_buffer[20];
char Bin_buffer[20];
char Hex_buffer[20];

void setup() {
  // put your setup code here, to run once:
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  Serial.begin(9600);
}


//From ChatGPT:
void decimalToBinary(int decimal, char *binaryString, int bitSize) {
  int index = 0;
  unsigned int mask = 1 << (bitSize - 1);  // Mask to extract the most significant bit

  // Handle the special case for 0
  if (decimal == 0) {
    for (int i = 0; i < bitSize; i++) {
      binaryString[i] = '0';
    }
    binaryString[bitSize] = '\0';
    return;
  }

  // For negative numbers, convert to two's complement
  unsigned int num = (unsigned int)decimal;
  if (decimal < 0) {
    num = (unsigned int)(~(-decimal) + 1);  // Two's complement
  }

  // Calculate the binary representation
  while (index < bitSize) {
    binaryString[index++] = (num & mask) ? '1' : '0';
    num <<= 1;
  }

  // Null-terminate the string
  binaryString[index] = '\0';

  // No need to reverse the string since we fill it from the most significant bit
}
//End from Chatgpt

void loop() {
  // put your main code here, to run repeatedly:

  /*
  if (!digitalRead(BUTTON)) {       //如果按board上面的button
    pressedtime = millis();         //reset pressedtime
    while (!digitalRead(BUTTON)) {  //等你把button放开
      delay(10);
    }
    if (millis() - pressedtime > 200) {  //如果你按的时间大过200ms

      if (view < 3) {  //换view
        view++;
      } else {
        view = Decimal;
      }

    } else {            //如果小过或者等于200ms
      if (state < 3) {  //换state
        state++;
      } else {
        state = 0;
      }
      lc.clearDisplay(0);
    }
  }
  */

  //跟着你们的题目改了，但是你们需要自己connect pull-up resistor PB
  if (!digitalRead(BUTTON)) {       //如果按board上面的button
    while (!digitalRead(BUTTON)) {  //等你放手
      delay(10);
    }
    if (view < 3) {  //换view
      view++;
    } else {
      view = Decimal;
    }
  }

  if (!digitalRead(3)) {       //按接去3号的那个PB
    while (!digitalRead(3)) {  //等你放手
      delay(10);
    }
    if (state < 3) {  //换state
      state++;
    } else {
      state = 0;
    }
  }

  switch (view) {
    case Decimal:
      if (counter < 10000) {                        //set limit prevent overlap with "dEc"
        sprintf(Dec_buffer, "%d", counter);         //把 counter sprintf 进 Dec_buffer
        if (strlen(Dec_buffer) != DecDigitCheck) {  //这个是如果你的号码从比较多 digit 掉到比较少 digit，clear掉全部 （不明白我说什么的和可以再问我）
          lc.clearDisplay(0);
        }
        lc.setChar(0, 7, 'd', false);
        lc.setChar(0, 6, 'E', false);
        lc.setChar(0, 5, 'c', false);
        for (int i = 0; i <= strlen(Dec_buffer); i++) {
          lc.setChar(0, i - 1, Dec_buffer[strlen(Dec_buffer) - i], false);
        }
      }
      DecDigitCheck = strlen(Dec_buffer);
      break;

    case Binary:
      decimalToBinary(counter, Bin_buffer, 4);  //decimalToBinary 这个 function 有点复杂，是 chatgpt 写的，你们可以把整个 function copy 进去问他先，实在不明白的和再问我
      if (strlen(Bin_buffer) != BinDigitCheck) {
        lc.clearDisplay(0);
      }
      lc.setChar(0, 7, 'b', false);
      lc.setChar(0, 6, 'i', false);
      lc.setChar(0, 5, 'n', false);
      //print the 4 LSB:
      for (int i = 0; i <= strlen(Bin_buffer); i++) {
        lc.setChar(0, i - 1, Bin_buffer[strlen(Bin_buffer) - i], false);
      }
      BinDigitCheck = strlen(Bin_buffer);
      break;

    case Hexadecimal:
      if (counter <= 0xffff) {
        sprintf(Hex_buffer, "%x", counter);
        if (strlen(Hex_buffer) != HexDigitCheck) {
          lc.clearDisplay(0);
        }
        lc.setChar(0, 7, 'h', false);
        lc.setChar(0, 6, 'E', false);
        lc.setChar(0, 5, 'H', false);
        for (int i = 0; i <= strlen(Hex_buffer); i++) {
          lc.setChar(0, i - 1, Hex_buffer[strlen(Hex_buffer) - i], false);
        }
        HexDigitCheck = strlen(Hex_buffer);
      }
      break;
  }

  switch (state) {
    case UP:
      if (millis() - timer > 500) {  //每500ms+1
        counter++;
        timer = millis();
      }
      break;

    case DOWN:
      if (millis() - timer > 500) {  //每500ms-1
        counter--;
        timer = millis();
      }
      break;

    default:
      counter = counter;
      break;
  }

  Serial.print("View: ");
  Serial.print(view);
  Serial.print("  State: ");
  Serial.print(state);
  Serial.print("  Count: ");
  Serial.println(counter);
}
