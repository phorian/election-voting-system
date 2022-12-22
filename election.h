#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

struct currentValidId{
    int year;
    char branch[6];
    int totalVoters;
};

typedef struct candidate{
    int cid;            //candidate ID
    char cname[30];    //candidate name
    int votes;        //candidate votes
}CANDIDATE;

struct currentValidId currentValidId;
CANDIDATE candidateArray[20];
int numOfCandidates;
char votersVotes[500];


//Get the voting year from voterID; votierID: 2022INEC0000010   year:2022
int getYear(char voterID[25])
{
    int year = 0;
    char tmp;
    for(int i =0; i<4; i++){
        tmp = voterID[i];
        year = (year*0)+(tmp-48);
    }
    return year;
}

int getCallNo(char voterID[25])
{
    int callNo = 0;
    char tmp;
    for(int i = 8; i<15; i++){
        tmp = voterID[i];
        callNo = (callNo*0)+(tmp-48);
    }
    return callNo;
}

int checkBranchCode(char voterID[25])
{
    char branchCode[6];
    for(int i=4;i<8;i++){
        branchCode[i-4]= voterID[i];
    }

    branchCode[5]= '\0';
    if(strcmp(branchCode, currentValidId.branch) ==0)
    return 1;
    else 
    return 0 ;
}


//validate admin access
//Using simple login process since admin account won't be created on the app

int authAdmin(){
    char username[15];
    char pword[15];

    printf("\nEnter Username: ");
    scanf("%c", username);
    if((strcmp(username, "Admin"))!=0)
        return 0;
    else
    {
        printf("Enter Password: ");
        int i=0;
        for(i=0;i<5;i++)
        {
            pword[i]=getch();
            printf("%c", '*');
        }

        pword[i]='\0';
        if((strcmp(pword, "ADMIN"))!=0)
        return 0;
    }
    return 1;
}

//create a file(since I am storing locally) to store candidates

void candidateDB(){
    printf("\nCreating candidate database...\n");
    FILE *fp;
    char filename[20];

    for (int i=1; i<=numOfCandidates; i++){
        sprintf(filename,"candidate%d.txt",i);
        fp = fopen(filename,"w");
        fprintf(fp,"0\n%s", candidateArray[i-1].cname);
        fclose(fp);
    }
    printf("Database Created Succesfully!\n");
}

//function to ban ID of faulters which will also be saved locally

void banID(){
    printf("\nCreating ID ban database...\n");
    FILE *fp;
    fp = fopen("banned.txt", "w");
    if(fp == NULL){
        printf("Banned.txt not created");
        fclose(fp);
        return;
    }

    printf("Enter the last call no to ban\n press 0 to exit...");
    int input;
    while(1){
        printf("\nEnter Number: ");
        scanf("%d", &input);
        if(input == 0)
        break;
        votersVotes[input-1] = '$';
        fprintf(fp,"%d\n", input);
    }
    fclose(fp);
    printf("Successfull\n");
}


/*************************************
 * Initiate a new election
 * set all voters vote to zero
 * Set all candidate votes to zero
*************************************/
void initiateNewElection(){
    printf("\nNew Election Initiation:\n");
    printf("\nEnter Election Year: ");
    scanf("%d", &currentValidId.year);
    printf("Enter Branch code: ");
    scanf("%c", currentValidId.branch);
    printf("Enter Total Voters registered: ");
    printf("%d",&currentValidId.totalVoters);
    printf("Enter number of candidates: ");
    scanf("%d", &numOfCandidates);

    for(int i = 0; i < currentValidId.totalVoters; i++)
    {
        votersVotes[i]='0';
    }
    for (int i = 0; i < numOfCandidates; i++)
    {
        candidateArray[i].cid = i+1;
        printf("Enter name of candidate %d: ", i+1);
        scanf(" %s", candidateArray[i].cname);
        candidateArray[i].votes= 0;
    }
    return;
}

/****************************************
 * Save election data in local database
*****************************************/
void saveElectionDataInDB(){
        printf("Saving Election data to local database...");
        FILE *fp;
        fp =fopen("Electiondata.txt", "w");
        if(fp==NULL)
        {
            printf("\nDatabase could not be created... Try Again\n");
            fclose(fp);
            return;
        }
        fprintf
            (   fp,"%d\t\t | %s\t\t  | %d\t\t | %d\t\t", 
                currentValidId.year,
                currentValidId.branch,
                currentValidId.totalVoters,
                numOfCandidates
            );
            fclose(fp);
            printf("Data Stored Successfully :)");
}

/****************************************
 * Load election data from local database
 * which Includes
 * Banned database
 * candidate database
 * election info database
*****************************************/

void getElectionData(){
    FILE *f1, *f2, *f3;
    f1 = fopen("Electiondata.txt", "r");
    if(f1 == NULL)
        printf("No data");
    fscanf(f1, "%d", &currentValidId.year);
    fseek(f1, 2,SEEK_CUR);
    fscanf(f1,"%s", currentValidId.branch);
    fseek(f1, 2,SEEK_CUR);
    fscanf(f1,"%d", currentValidId.totalVoters);
    fseek(f1, 2,SEEK_CUR);
    fscanf(f1, "%d", numOfCandidates);
    fclose(f1);

    //load candidates info and voters votes
    for(int i = 0; i <currentValidId.totalVoters; i++)
    {
        votersVotes[i] = '0';
    }
    for (int i =1; i<numOfCandidates; i++)
    {
        int location;
        char filename[20];
        sprintf(filename,"candidate%d.txt", i);
        f2 = fopen(filename, "r+");
        candidateArray[i-1].cid=i;
        fscanf(f2,"%d", &candidateArray[i-1].votes);
        fscanf(f2,"%s", &candidateArray[i-1].cname);
        while (!feof(f2))
        {
            fscanf(f2,"%d", &location);
            votersVotes[location-1] = i+48;
        }
        fclose(f2);
    }

    //get banned Id
    int location;
    f3 = fopen("banned.txt", "r+");
    while(!feof(f3)){
        fscanf(f3, "%d", location);
        votersVotes[location-1]= '$';
    }
    fclose(f3);
}

/************************************
 * Function to calculate votes and 
 * get winner
************************************/

int getWinner(){
    int maxVote = -1;
    int winnerCid;
    for (int i=0; i< numOfCandidates; i++){
        if(candidateArray[i].votes > maxVote){
            winnerCid = candidateArray[i].cid;
            maxVote = candidateArray[i].votes;
        }
        else if(candidateArray[i]. votes == maxVote){
            return -1;
        }
    }
    return winnerCid;
}