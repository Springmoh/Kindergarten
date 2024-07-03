#include <stdio.h>
#include <string.h>

int main()
{
    char userinput[50];
    int flag = 0;
    while (1)
    {
        printf("Enter a palindrome: ");
        scanf("%s", userinput);
        for (int i = 0; i < strlen(userinput); i++)
        {
            if (userinput[i] != userinput[strlen(userinput) - i - 1])
            {

                flag = 1;
            }
        }
        if (flag)
        {
            printf("The input is not palindrome\n");
        }
        else
        {
            printf("The input is palindrome\n");
        }
        printf("\n");
    }
}