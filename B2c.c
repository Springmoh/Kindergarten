#include <stdio.h>
#include <math.h>

int main()
{
    int data[10] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int sum = 0;
    for (int i = 0; i < 10; i++){
        sum = sum + data[i];
    }

    int data_n = sizeof(data) / sizeof(data[0]);
    float average = (float)sum / (float)data_n;

    printf("Sum: %d\n", sum);
    printf("Average: %.2f\n", average);

    float buffer1 = 0;
    for (int i = 0; i < 10; i++){
        buffer1 = buffer1 + pow(fabs(data[i] - average), 2);
    }
    float variance = buffer1 / data_n;
    // printf("variance: %f\n", variance);
    float SD = sqrt(variance);
    printf("Standard deviation: %f\n", SD);
}