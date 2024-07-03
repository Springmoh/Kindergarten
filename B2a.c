#include <stdio.h>

int main()
{
    int elements[10] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int max = elements[0];
    int min = elements[0];
    printf("%s", "Revese order: ");
    for (int i = 9; i >= 0; i--)
    {
        printf("%d", elements[i]);
        max = max < elements[i] ? elements[i] : max;
        min = min > elements[i] ? elements[i] : min;
    }
    printf("\nMax:%d Min:%d\n", max, min);
    return 0;
}