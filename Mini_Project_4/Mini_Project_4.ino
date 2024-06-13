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

int view = Decimal;
int counter = 0;
int state = STOP1;
int nDigits = 0;
float timer = millis();
char buffer[20];
char binbuffer[20];
char hexbuffer[20];

void setup() {
  // put your setup code here, to run once:
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  Serial.begin(9600);
  max7219.Begin();
  max7219.Clear();
}

void decimalToBinary(int decimal, char *binaryString) {
  int index = 0;
  int temp = decimal;

  // Handle the special case for 0
  if (decimal == 0) {
    strcpy(binaryString, "0");
    return;
  }

  // Calculate the binary representation
  while (temp > 0) {
    binaryString[index++] = (temp % 2) ? '1' : '0';
    temp /= 2;
  }

  // Null-terminate the string
  binaryString[index] = '\0';

  // Reverse the string since the binary digits are in reverse order
  int start = 0;
  int end = index - 1;
  char ch;
  while (start < end) {
    ch = binaryString[start];
    binaryString[start] = binaryString[end];
    binaryString[end] = ch;
    start++;
    end--;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!digitalRead(3)) {
    while (!digitalRead(3)) {
      delay(10);
    }
    if (view < 3) {
      view++;
    } else {
      view = Decimal;
    }
  }

  switch (view) {
    case Decimal:
      if(strlen(buffer) != nDigits){
        max7219.Clear();
      }
      
      max7219.DisplayChar(7, 'd', 0);
      max7219.DisplayChar(6, 'E', 0);
      max7219.DisplayChar(5, 'c', 0);
      
/*
      if (counter < 10000) {
        sprintf(buffer, "%d", counter);
        if (counter < 10) {
          max7219.DisplayChar(0, buffer[0], 0);
        } else if (counter < 100) {
          max7219.DisplayChar(1, buffer[0], 0);
          max7219.DisplayChar(0, buffer[1], 0);
        } else if (counter < 1000) {
          max7219.DisplayChar(2, buffer[0], 0);
          max7219.DisplayChar(1, buffer[1], 0);
          max7219.DisplayChar(0, buffer[2], 0);
        } else if (counter < 10000) {
          max7219.DisplayChar(3, buffer[0], 0);
          max7219.DisplayChar(2, buffer[1], 0);
          max7219.DisplayChar(1, buffer[2], 0);
          max7219.DisplayChar(0, buffer[3], 0);
        }
      } else {
        max7219.DisplayChar(3, '9', 0);
        max7219.DisplayChar(2, '9', 0);
        max7219.DisplayChar(1, '9', 0);
        max7219.DisplayChar(0, '9', 0);
      }
*/

      if (counter < 10000) {
        sprintf(buffer, "%d", counter);
        Serial.println(strlen(buffer));

        for (int i = 0; i <= strlen(buffer); i++) {
          max7219.DisplayChar(i-1, buffer[strlen(buffer) - i], 0);
        }

      } else {
        max7219.DisplayChar(3, '9', 0);
        max7219.DisplayChar(2, '9', 0);
        max7219.DisplayChar(1, '9', 0);
        max7219.DisplayChar(0, '9', 0);
      }

      nDigits = strlen(buffer);

      break;


    case Binary:
      max7219.DisplayChar(7, 'b', 0);
      max7219.DisplayChar(6, 'i', 0);
      max7219.DisplayChar(5, 'n', 0);

      if (counter <= 15) {
        decimalToBinary(counter, binbuffer);
        if (counter < 2) {
          max7219.DisplayChar(0, binbuffer[0], 0);
        } else if (counter < 4) {
          max7219.DisplayChar(1, binbuffer[0], 0);
          max7219.DisplayChar(0, binbuffer[1], 0);
        } else if (counter < 8) {
          max7219.DisplayChar(2, binbuffer[0], 0);
          max7219.DisplayChar(1, binbuffer[1], 0);
          max7219.DisplayChar(0, binbuffer[2], 0);
        } else if (counter < 16) {
          max7219.DisplayChar(3, binbuffer[0], 0);
          max7219.DisplayChar(2, binbuffer[1], 0);
          max7219.DisplayChar(1, binbuffer[2], 0);
          max7219.DisplayChar(0, binbuffer[3], 0);
        }
      } else {
        max7219.DisplayChar(3, '1', 0);
        max7219.DisplayChar(2, '1', 0);
        max7219.DisplayChar(1, '1', 0);
        max7219.DisplayChar(0, '1', 0);
      }

      break;

    case Hexadecimal:
      max7219.DisplayChar(7, 'h', 0);
      max7219.DisplayChar(6, 'E', 0);
      max7219.DisplayChar(5, 'H', 0);

      if (counter <= 0xffff) {
        sprintf(hexbuffer, "%x", counter);
        if (counter < 0x10) {
          max7219.DisplayChar(0, hexbuffer[0], 0);
        } else if (counter < 0x100) {
          max7219.DisplayChar(1, hexbuffer[0], 0);
          max7219.DisplayChar(0, hexbuffer[1], 0);
        } else if (counter < 0x1000) {
          max7219.DisplayChar(2, hexbuffer[0], 0);
          max7219.DisplayChar(1, hexbuffer[1], 0);
          max7219.DisplayChar(0, hexbuffer[2], 0);
        } else if (counter < 0x10000) {
          max7219.DisplayChar(3, hexbuffer[0], 0);
          max7219.DisplayChar(2, hexbuffer[1], 0);
          max7219.DisplayChar(1, hexbuffer[2], 0);
          max7219.DisplayChar(0, hexbuffer[3], 0);
        }
      } else {
        max7219.DisplayChar(3, 'f', 0);
        max7219.DisplayChar(2, 'f', 0);
        max7219.DisplayChar(1, 'f', 0);
        max7219.DisplayChar(0, 'f', 0);
      }



      break;
  }

  if (!digitalRead(BUTTON)) {
    while (!digitalRead(BUTTON)) {
      delay(10);
    }
    if (state < 3) {
      state++;
    } else {
      state = STOP1;
    }
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
}
