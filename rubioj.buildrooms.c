/*
 name: Jose-Antonio D. Rubio
 OSUID: 932962915
 Class: 344-400
 Program 1 Matrices 
 Comment: I ran the grading script and it went just fine although it did
          take a bit of time and I am worried about timing out when you
          go to grade it
*/





#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

//GLOBALS
#define MIN_ROOM_CONNECTIONS 3
#define MAX_ROOM_CONNECTIONS 6
#define MAX_NUM_ROOMS 7
#define TOTAL_NUM_ROOMS 10
#define TRUE 1
#define FALSE 0

/// NAME: boolean
/// DESC: This helps me not mix up my true and false while programming.
///       And since we are using an earlier version of C.
/// SOURCE: http://stackoverflow.com/questions/1909825/error-with-the-declaration-of-enum

//typedef enum {FALSE = 0, TRUE = 1} boolean;

/// NAME: ROOM_NAMES
/// DESC: Global of possible ROOM names.
char *ROOM_NAMES[TOTAL_NUM_ROOMS] = {
        "Dixon_Rec_Center",
        "Memorial_Union",
        "Valley_Library",
        "Reser_Stadium",
        "DearBorn_Hall",
        "Rogers_Hall",
        "Kearney",
        "Kelly_Engineering",
        "Buxton_Dorm",
        "West_Dining"
    };
//END_GLOBALS


///NAME: ROOM_TYPES
///DESC: Determines if start,mid, or end ROOM.
enum ROOM_TYPES{
    START_ROOM,
    MID_ROOM,
    END_ROOM
};


///NAME: ROOM
///DESC: Defines ROOM specs.
struct ROOM 
{
    int TotalConnections;
    struct ROOM* Connections[MAX_ROOM_CONNECTIONS];
    enum ROOM_TYPES RType;
    char Name[100];
};

//ROOM GLOBAL
struct ROOM ROOMList[MAX_NUM_ROOMS];
int  ROOMState[TOTAL_NUM_ROOMS];
char folderName[256];


// Returns TRUE if all rooms have 3 to 6 outbound connections, FALSE otherwise
int IsGraphFull()  
{

  int i;
  for(i=0; i< MAX_NUM_ROOMS; i++)
  {
  
    if (!(ROOMList[i].TotalConnections >= MIN_ROOM_CONNECTIONS))
    {
        return FALSE;
    }
  }

return TRUE;
}


// Returns TRUE if a connection can be added from ROOM x (< 6 outbound connections), FALSE otherwise
int CanAddConnectionFrom(struct ROOM x) 
{
    if(x.TotalConnections == MAX_ROOM_CONNECTIONS)
    {
        return FALSE;
    }else
    {
      return TRUE;
    }
}

// Returns a random ROOM, does NOT validate if connection can be added
struct ROOM GetRandomROOM()
{
  int p;

    do
    {
        p = rand() % MAX_NUM_ROOMS + 0; 

    }while(CanAddConnectionFrom(ROOMList[p]) == FALSE && p <= MAX_NUM_ROOMS && p >= 0);
    // check if numbers are out of range.

    return ROOMList[p];
}


// Returns TRUE if a connection from ROOM x to ROOM y already exists, FALSE otherwise
int ConnectionAlreadyExists(struct ROOM x, struct ROOM y)
{

  int i;
    //parse through list of connections
    for(i = 0; i < x.TotalConnections;i++)
    {
        //check to see if the ROOMs are already connected
        if(strcmp(x.Connections[i]->Name,y.Name) == 0)
        {
            return TRUE; 
        }
    }
    return FALSE;
}

// Connects ROOMs x and y together, does not check if this connection is valid
void ConnectROOM(struct ROOM x, struct ROOM y) 
{
 
  int posx, posy,i;

  for(i=0; i<MAX_NUM_ROOMS; i++)
  {
    if(strcmp(ROOMList[i].Name,x.Name) == 0)
    {
        posx=i;
      
    }
  }
printf("x.Name: %s is at %d in ROOMList\n", x.Name, posx);

  for(i=0; i<MAX_NUM_ROOMS; i++)
  {
    if(strcmp(ROOMList[i].Name,y.Name) == 0)
    {
        posy=i;
       
    }
  }
    //connect ROOMs and increment their connection counts
    ROOMList[posx].Connections[x.TotalConnections] = &ROOMList[posy];
    ROOMList[posx].TotalConnections++;

}

// Returns TRUE if ROOMs x and y are the same ROOM, FALSE otherwise
int IsSameROOM(struct ROOM x, struct ROOM y) 

{   //compare the names of the two ROOMs to make sure they are not the same

   if(strcmp(x.Name, y.Name)== 0)
  {
    return TRUE;
  }else
  {
    return FALSE;
  }
} 

// Adds a random, valid outbound connection from a ROOM to another ROOM
void AddRandomConnection()  
{
  struct ROOM A;  // Maybe a struct, maybe global arrays of ints
  struct ROOM B;

  
  while(TRUE)
  {
    A = GetRandomROOM();

    if (CanAddConnectionFrom(A) == TRUE)
      break;
  }

  do
  {
    B = GetRandomROOM();
  }
  while(CanAddConnectionFrom(B) == FALSE || IsSameROOM(A, B) == TRUE || ConnectionAlreadyExists(A, B) == TRUE);

  ConnectROOM(A, B);  // TODO: Add this connection to the real variables, 
  ConnectROOM(B, A);  //  because this A and B will be destroyed when this function terminates
}



/// NAME: InitROOMList
/// DESC: gens a the ROOMlist global with default values
void InitROOMList()
{
   
}

/// NAME: FillROOMList
/// DESC: Begins generating the graph.
void FillROOMList()
{
     int i;   
    // tells me which 7 ROOMs I picked out of 10
    for(i = 0; i < MAX_NUM_ROOMS;i++){
        ROOMState[i] = FALSE;
    }

   
    for(i = 0; i < MAX_NUM_ROOMS;i++){
        ROOMList[i].TotalConnections = 0;

        //set all ROOM connections to NULL
        int j;
        for(j = 0; j < MAX_ROOM_CONNECTIONS; j++)
        {
            ROOMList[i].Connections[j] = NULL;
        }

        while(TRUE){
            int RandomROOM = rand() % TOTAL_NUM_ROOMS; // sets a ROOM name to be used.
            if(ROOMState[RandomROOM] == FALSE)
            {  
                // change ROOM state to TRUE so as not to cbe chosen again
                ROOMState[RandomROOM] = TRUE;

                //remove garbage data from ROOM arrray
                memset(ROOMList[i].Name,'\0',sizeof(ROOMList[i].Name));
                strcpy(ROOMList[i].Name,ROOM_NAMES[RandomROOM]);

                ROOMList[i].RType = MID_ROOM;
                break;
            }
        }
    }
    //defines a start and winning ROOM.
    ROOMList[0].RType = START_ROOM;
    ROOMList[MAX_NUM_ROOMS - 1].RType = END_ROOM;


    while(IsGraphFull() == FALSE)
     {


                AddRandomConnection();
    } 
}


/// NAME: PrintROOMs_DEBUG
/// DESC: DEBUG FUNCTION lets me check if a ROOMlist is correct.
void PrintROOMs_DEBUG()
{
    int i,j;
    for(i = 0; i < MAX_NUM_ROOMS;i++){
        printf("\n%d: ",i);
        printf("Name: %s",ROOMList[i].Name);
        printf("\nTotalConnections: %d",ROOMList[i].TotalConnections);
        if(ROOMList[i].TotalConnections > 0){
            printf("\n\tConnections:");
            for(j = 0;j < ROOMList[i].TotalConnections;j++){
                printf("\n\tC%d:%s",j,ROOMList[i].Connections[j]->Name);
                printf("\n\tC%d connectTotal: %d",j,ROOMList[i].Connections[j]->TotalConnections);
            }
        }
        
        if(ROOMList[i].RType == START_ROOM){
            printf("\nROOM Type: START_ROOM");
        }
        else if(ROOMList[i].RType == END_ROOM){
            printf("\nROOM Type: END_ROOM");
        }
        else{
            printf("\nROOM Type: MID_ROOM");
        }
        printf("\n");
    }
    printf("\n");
}

/// NAME: GenROOMDir
/// DESC: creates a new ROOMs directory with moderately secure permissions.
void GenROOMDir()
{
    char* staticDirName = "rubioj.rooms.";
    int pid = getpid();
    int ModeratlyecPermissionSetting = 0770;// rwxrwx---
    int unsecPermissionSetting = 0777; // rwxrwxrwx EXTREMELY UNSAFE

    memset(folderName,'\0',sizeof(folderName));
    sprintf(folderName,"%s%d",staticDirName,pid); // sets the new folder name.

    //printf("%s",folderName);
    mkdir(folderName,ModeratlyecPermissionSetting); // creates folder.
}


/// NAME: GenROOMFiles
/// DESC: copys ROOMlist struct and prints to a file.
void GenROOMFiles()
{
    FILE *ROOMFile;
    int i,j;
    char folderDIR[256];

    sprintf(folderDIR,"./rubioj.rooms.%d",getpid()); // gets the folders future directory.

    GenROOMDir(); // sets a folder.
    
    // prevents errors, checks if the folder exists or directory was changed.
    if(chdir(folderDIR) != 0){
        printf("DIR NOT CHANGED TO: %s\n",folderDIR);
        return;
    }

    //begins genning files
    for(i = 0; i < MAX_NUM_ROOMS; i++){
        //no conflicts should happen since the directory was 
        //dynamically generated.
        ROOMFile = fopen(ROOMList[i].Name,"w");
        
        //prints ROOM name then connections
        fprintf(ROOMFile,"ROOM NAME: %s\n",ROOMList[i].Name);
        for(j = 0;j < ROOMList[i].TotalConnections;j++){
            fprintf(ROOMFile,"CONNECTION %d: %s\n",j+1,ROOMList[i].Connections[j]->Name);
        }
        
        //checks the ROOM type and prints to file.
        if(ROOMList[i].RType == START_ROOM){
            fprintf(ROOMFile,"ROOM TYPE: %s\n","START_ROOM");
        }
        else if(ROOMList[i].RType == MID_ROOM){
            fprintf(ROOMFile,"ROOM TYPE: %s\n","MID_ROOM");
        }
        else if(ROOMList[i].RType == END_ROOM){
            fprintf(ROOMFile,"ROOM TYPE: %s\n","END_ROOM");
        }
        else{
            fprintf(ROOMFile,"ROOM TYPE: %s\n","NULL");
        }
        fclose(ROOMFile);
    }
}


int main(void)
{
    srand(time(NULL));
    FillROOMList();
  //  PrintROOMs_DEBUG();
    GenROOMFiles();
    return 0;
}