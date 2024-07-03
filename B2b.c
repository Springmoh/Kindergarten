#include <stdio.h>

int main()
{
    int array1[10] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int array2[10], odd[10], even[10];
    int odd_index = 0;
    int even_index = 0;

    printf("copied array:");
    // copy elements in array1 into array2
    for (int i = 0; i < 10; i++)
    {
        array2[i] = array1[i];
        printf("%d", array2[i]);
    }

    // seperate even and odd numberz
    for (int i = 0; i < 10; i++)
    {
        if (array1[i] % 2)
        {
            odd[odd_index] = array1[i];
            odd_index++;
        }
        else
        {
            even[even_index] = array1[i];
            even_index++;
        }
    }

    printf("\nOdd Number: ");
    for (int i = 0; i < odd_index; i++)
    {
        printf("%d ", odd[i]);
    }

    printf("\nEven Number: ");
    for (int i = 0; i < even_index; i++)
    {
        printf("%d ", even[i]);
    }

    printf("\n");

    return 0;
}