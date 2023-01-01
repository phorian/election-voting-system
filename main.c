#include"election.h"

int main (){
    while(1){
        int choice;
        printf("Board");
        printf("\nNB: VOTERS SHOULD ONLY INPUT OPTION 1");
        printf("\n 1. Voter \n 2. Admin panel \n 3. Exit \n");
        printf("Enter Choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            voterSec();
            break;

        case 2:
            adminPanel();
            break;

        case 3:
            return 0;

        default:
            printf("\nInvalid option");
            getch();
        }
    }
    return 0;
}
