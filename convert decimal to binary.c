#include <stdint.h>
#include <stdio.h>

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

int main(){
  char binString[4];
  decimalToBinary(4,binString,4);
  for(int i = 0; i < 4; i++){
    if(binString[i]=='0'){
      binString[i]='\0';
    }else{
      break;
    }
  }

  for(int i = 0; i<4; i++){
  }
  
  // printf("%d\n",binString[1] == '0');
  printf("%s\n",binString);
    return 0;
}