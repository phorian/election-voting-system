#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

#define MAX_LIMIT 50

struct currentValidId{
    int year;
    char branch[6];
    int totalVoters;
};

typedef struct candidate{
    int cid;            //candidate ID
    char cname[MAX_LIMIT];    //candidate name
    int votes;        //candidate votes
}CANDIDATE;

struct currentValidId currentValidId;
CANDIDATE candidateArray[20];
int numOfCandidates;
char votersVotes[500];


//Get the voting year from voterID; voterID: 2022INEC0000010   year:2022
int getYear(char voterID[25])
{
    int year = 0;
    char tmp;
    for(int i =0; i<4; i++){
        tmp = voterID[i];
        year = (year*10)+(tmp-48);
    }
    return year;
}

int getCallNo(char voterID[25])
{
    int callNo = 0;
    char tmp;
    for(int i = 8; i<15; i++){
        tmp = voterID[i];
        callNo = (callNo*10)+(tmp-48);
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

    printf("\nEnter Username:");
    scanf("%s", username);
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
    scanf("%s", currentValidId.branch);
    printf("Enter Total Voters registered: ");
    scanf("%d", &currentValidId.totalVoters);
    printf("Enter number of candidates: ");
    scanf("%d", &numOfCandidates);

    for(int i = 0; i < currentValidId.totalVoters; i++)
    {
        votersVotes[i]='0';
    }
    for (int i = 0; i < numOfCandidates; i++)
    {
        candidateArray[i].cid = i+1;
        printf("Enter name of candidate %d:  ", i+1);
        scanf(" %[^\n]", candidateArray[i].cname );
        //fgets(candidateArray[i].cname, MAX_LIMIT, stdin);
        //stdin;
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
    f1 = fopen("Electiondata.txt", "r+");
    if(f1 == NULL)
        printf("No data");
    fscanf(f1, "%d", &currentValidId.year);
    fseek(f1, 2,SEEK_CUR);
    fscanf(f1,"%s", currentValidId.branch);
    fseek(f1, 2,SEEK_CUR);
    fscanf(f1,"%d", &currentValidId.totalVoters);
    fseek(f1, 2,SEEK_CUR);
    fscanf(f1, "%d", &numOfCandidates);
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
        fscanf(f2,"%s", candidateArray[i-1].cname);
        while (!feof(f2))
        {
            fscanf(f2,"%d", &location);
            votersVotes[location-1] = i+48;
        }
        fclose(f2);
    }

    //load banned Id
    int location;
    f3 = fopen("banned.txt", "r+");
    while(!feof(f3)){
        fscanf(f3, "%d", &location);
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

void adminPanel(){
    while (1)
    {
        if(authAdmin()!=1){
            printf("\n Invalid login details");
            break;
        }

        printf("\n\n LOGGED IN SUCCESSFULLY");
        printf("\n\t\t Press Enter");
        getch();

        while (1)
        {
            //char inputID[15];
            int input;
            char banInput;
            int winCid;
            int sumVoted=0;

            printf("\n1.New Election");
            printf("\n2.Continue Previous Election");
            printf("\n3.Ban User ID");
            printf("\n4.Result");
            printf("\n5.Logout");
            printf("\nOption: ");
            scanf("%d", &input);

        switch (input)
        {
        case 1:
            initiateNewElection();
            saveElectionDataInDB();
            candidateDB();
            break;

        case 2:
            getElectionData();
            break;

        case 3:
            printf("Do you want to ban a Voter ID? \n Press 1 to continue or press 0 to go back");
            scanf("%c", &banInput);
            if(banInput=='1'){
                banID();
            }
            break;

        case 4:
            winCid =getWinner();
            if(winCid != -1){
                printf("\nWinner is %s with %d votes\n",candidateArray[winCid-1].cname, candidateArray[winCid-1].votes);
            }
            else{
                printf("\nITS A TIE");
            }
            printf("\nFULL RESULT");
            for(int i=0; i<numOfCandidates;i++){
                sumVoted+=candidateArray[i].votes;
                printf("\n%d.  %s  -> %d votes\n", candidateArray[i].cid, candidateArray[i].cname, candidateArray[i].votes);
            }
            printf("\nVoting Percentage: %d %%\n\n",(sumVoted*100)/currentValidId.totalVoters);
            break;
        case 5:
            return;
        default:
            printf("Invalid Option");
                     getch();
             }
        }

    }

};


/*************************************************************************
 * This function checks the voter's ID for validity
 * It uses the getyear,getcallno,getbranchcode function to confirm this
*************************************************************************/
int isvalid(char voterID[25])
{
    if(strlen(voterID)!=15)
        return 0;

    int inputedYear = getYear(voterID);
    int inputedCallNo = getCallNo(voterID);

    if(inputedYear != currentValidId.year || checkBranchCode(voterID)!=1 || inputedCallNo > currentValidId.totalVoters){
        return 0;
    }
        else
    return 1;
}

/****************************************************************
 * This function checks for voters that already cast their vote
****************************************************************/

int isVoted(char voterID[25]){
    int loc = getCallNo(voterID);
    if(votersVotes[loc -1]=='0')
        return 0;
    else
        return 1;
}

/********************************************************
 * This function checks if a voter is banned from voting
*********************************************************/
int isBanned(char voterID[25])
{
    int loc = getCallNo(voterID);
    if(votersVotes[loc - 1]=='$')
        return 1;
    else
        return 0;
}

/*******************************
 * This function saves the vote
*******************************/

void saveVote(char voterID[25], char voteInput)
{
    char filename[20];
    sprintf(filename,"candidate%d.txt",voteInput-48);
    FILE *fp = fopen(filename, "r+");
    int loc = getCallNo(voterID);
    votersVotes[loc - 1]= voteInput;
    candidateArray[voteInput-49].votes++;
    fseek(fp, 0, SEEK_SET);
    fprintf(fp, "%d\n", candidateArray[voteInput-49].votes);
    fseek(fp, 0, SEEK_END);
    fprintf(fp, "\n%d", loc);
    fclose(fp);
}


void voterSec(){
    char voterID[25];
    char voteInput;

    while(1)
    {
        printf("\n\n\t\t Press 0 to exit");

        printf("\nEnter Voter ID: ");
        scanf("%s", voterID);

        if(strcmp(voterID, "0")==0)
            return;
        if(isvalid(voterID)!=1)
        {
            printf("\n Invalid Voter ID(PRESS ENTER)");
            getch();
            continue;
        }
        if(isBanned(voterID) != 0)
        {
            printf("\nThis Voter ID is currently banned... \nContact Admin....(Press Enter to continue)");
            getch();
            continue;
        }
        if(isVoted(voterID)!=0)
        {
            printf("\n This voter ID already has a registered vote. \n COntact Admin if this is a mistake");
            getch();
            continue;
        }

        printf("\n\n Candidates for election: ");
                for(int i = 0; i < numOfCandidates; i++){
                    printf("\n %d. %s", i+1, candidateArray[i].cname);
                }

            printf("\n\n Your Vote(Enter Number): ");
            voteInput=getch();
            printf("*");
            if(voteInput-48 < 1 || voteInput-48 > numOfCandidates)
            {
                printf("\nInvalid vote.\nTry again...");
                getch();
                continue;
            }

            saveVote(voterID,voteInput);
            printf("\n\nThanks for voting...(Press Enter)");
            getch();
    }
}
