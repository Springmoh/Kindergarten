#include <max7219.h>
#include <string.h>
#define BUTTON 2  //assign pin 2 to the button
MAX7219 max7219;
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

int view = 1;
int counter = 0;
int state = STOP1;
int nDigits = 0;
int nbinDigits = 0;
int nhexDigits = 0;
float timer = millis();
float pressedtime = millis();
char buffer[20];
char binbuffer[5];
char hexbuffer[20];

void setup() {
  // put your setup code here, to run once:
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  Serial.begin(9600);
  max7219.Begin();
  max7219.Clear();
}

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

void loop() {
  // put your main code here, to run repeatedly:
  if (!digitalRead(BUTTON)) {
    pressedtime = millis();
    while (!digitalRead(BUTTON)) {
      delay(10);
    }
    if (millis() - pressedtime > 200) {

      if (view < 3) {
        view++;
      } else {
        view = Decimal;
      }


    } else {
      if (state < 3) {
        state++;
      } else {
        state = 0;
      }
    }
  }

  switch (view) {
    case Decimal:

      if (counter < 10000) {
        sprintf(buffer, "%d", counter);
        max7219.DisplayChar(7, 'd', 0);
        max7219.DisplayChar(6, 'E', 0);
        max7219.DisplayChar(5, 'c', 0);
        if (strlen(buffer) != nDigits) {
          max7219.Clear();
        }

        for (int i = 0; i <= strlen(buffer); i++) {
          max7219.DisplayChar(i - 1, buffer[strlen(buffer) - i], 0);
        }
      }

      nDigits = strlen(buffer);
      break;


    case Binary:


      decimalToBinary(counter, binbuffer, 4);



      max7219.DisplayChar(7, 'b', 0);
      max7219.DisplayChar(6, 'i', 0);
      max7219.DisplayChar(5, 'n', 0);
      if (strlen(binbuffer) != nbinDigits) {
        max7219.Clear();
      }
      // Serial.println(binbuffer);
      if (abs(counter) <= 15) {
        for (int i = 0; i <= strlen(binbuffer); i++) {
          max7219.DisplayChar(i - 1, binbuffer[strlen(binbuffer) - i], 0);
        }
      }
      // }
      nbinDigits = strlen(binbuffer);

      break;

    case Hexadecimal:

      if (counter <= 0xffff) {
        sprintf(hexbuffer, "%x", counter);
        if (strlen(hexbuffer) != nhexDigits) {
          max7219.Clear();
        }

        max7219.DisplayChar(7, 'h', 0);
        max7219.DisplayChar(6, 'E', 0);
        max7219.DisplayChar(5, 'H', 0);

        for (int i = 0; i <= strlen(hexbuffer); i++) {
          max7219.DisplayChar(i - 1, hexbuffer[strlen(hexbuffer) - i], 0);
        }
      }

      nhexDigits = strlen(hexbuffer);



      break;
  }

  switch (state) {
    case UP:
      if (millis() - timer > 500) {
        counter++;
        timer = millis();
      }
      break;

    case DOWN:
      if (millis() - timer > 500) {
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
