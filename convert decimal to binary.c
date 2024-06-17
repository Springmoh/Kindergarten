#include <stdint.h>
#include <stdio.h>
#include <string.h>

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
    char binString[5];
  decimalToBinary(4, binString, 4);

  // Find the index of the first '1'
  int firstOneIndex = 0;
  while (firstOneIndex < 4 && binString[firstOneIndex] == '0') {
    firstOneIndex++;
  }
  printf("1stOneIndex: %d\n",firstOneIndex);
  // If all bits are zero, handle this case separately
  if (firstOneIndex == 4) {
    printf("0\n");
  } else {
    printf("%s\n", &binString[0]);
  }

  return 0;
}