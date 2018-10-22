/*
 name: Jose-Antonio D. Rubio
 OSUID: 932962915
 Class: 344-400
 Program 2 buildrooms
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


char *room_names[TOTAL_NUM_ROOMS] = { "The_Great_Hall", "Room_Of_Requirement", "Library", "Headmasters_Office", 
                                      "Prefects_Bathroom", "Chamber_Of_Secrets", "Kitchen", 
                                      "Astronomy_Tower", "Greenhouse", "Trophy_Room" };
char* room_types[3] = { "START_ROOM", "MID_ROOM", "END_ROOM" };


struct Room 
{
    int total_connections;
    struct Room* Connections[MAX_ROOM_CONNECTIONS];
    char* type;
    char name[256];
};

struct Room room_list[MAX_NUM_ROOMS];
int  room_state[TOTAL_NUM_ROOMS];
char folder_name[256];


/*
 NAME
    IsGraphFull
 SYNPOSIS
    Takes in nothing and returns an integer
 DESCRIPTION
    Returns TRUE if all rooms have 3 to 6 outbound connections, FALSE otherwise
*/
int IsGraphFull()  
{

  int i;
  for(i=0; i< MAX_NUM_ROOMS; i++)
  {
  
    if (!(room_list[i].total_connections >= MIN_ROOM_CONNECTIONS))
    {
        return FALSE;
    }
  }

return TRUE;
}


/*
 NAME
    CanAddConnectionFrom
 SYNPOSIS
    Takes in a struct Room Type and returns an integer
 DESCRIPTION
    Returns TRUE if a connection can be added from ROOM x (< 6 outbound connections), FALSE otherwise
*/
int CanAddConnectionFrom(struct Room x) 
{
    if(x.total_connections == MAX_ROOM_CONNECTIONS)
    {
        return FALSE;
    }else
    {
      return TRUE;
    }
}

/*
 NAME
    GetRandomRoom
 SYNPOSIS
    Takes in nothing and returns a struct Room
 DESCRIPTION
    Returns a random ROOM, does NOT validate if connection can be added
*/
struct Room GetRandomRoom()
{
  int p;

    do
    {
        p = rand() % MAX_NUM_ROOMS + 0; 

    }while(CanAddConnectionFrom(room_list[p]) == FALSE && p <= MAX_NUM_ROOMS && p >= 0);
    // check if numbers are out of range.

    return room_list[p];
}


/*
 NAME
    ConnectionAlreadyExists
 SYNPOSIS
    Takes in Two struct Rooms and returns an integer
 DESCRIPTION
    Returns TRUE if a connection from ROOM x to ROOM y already exists, FALSE otherwise
*/
int ConnectionAlreadyExists(struct Room x, struct Room y)
{

  int i;
    //parse through list of connections
    for(i = 0; i < x.total_connections;i++)
    {
        //check to see if the Rooms are already connected
        if(strcmp(x.Connections[i]->name,y.name) == 0)
        {
            return TRUE; 
        }
    }
    return FALSE;
}

/*
 NAME
    ConnectRoom
 SYNPOSIS
    Two struct Rooms are passed in and returns nothing
 DESCRIPTION
    Connects ROOMs x and y together, does not check if this connection is valid
*/
void ConnectRoom(struct Room x, struct Room y) 
{
 
  int posx, posy,i;

  for(i=0; i<MAX_NUM_ROOMS; i++)
  {
    if(strcmp(room_list[i].name,x.name) == 0)
    {
        posx=i;
      
    }
  }

  for(i=0; i<MAX_NUM_ROOMS; i++)
  {
    if(strcmp(room_list[i].name,y.name) == 0)
    {
        posy=i;
       
    }
  }
    //connect ROOMs and increment their connection counts
    room_list[posx].Connections[x.total_connections] = &room_list[posy];
    room_list[posx].total_connections++;

}

/*
 NAME
    IsSameRoom
 SYNPOSIS
    Two struct Rooms are passed in and in Int is returned 
 DESCRIPTION
    Compares the names of the two rooms passed in
    Returns TRUE if ROOMs x and y are the same ROOM, FALSE otherwise
*/
int IsSameRoom(struct Room x, struct Room y) 

{   

   if(strcmp(x.name, y.name)== 0)
  {
    return TRUE;
  }else
  {
    return FALSE;
  }
} 

/*
 NAME
   AddRandomConnection
 DESCRIPTION
   Adds a random, valid outbound connection from a ROOM to another ROOM
*/ 
void AddRandomConnection()  
{
  struct Room A;  // Maybe a struct, maybe global arrays of ints
  struct Room B;

  
  while(TRUE)
  {
    A = GetRandomRoom();

    if (CanAddConnectionFrom(A) == TRUE)
      break;
  }

  do
  {
    B = GetRandomRoom();
  }
  while(CanAddConnectionFrom(B) == FALSE || IsSameRoom(A, B) == TRUE || ConnectionAlreadyExists(A, B) == TRUE);

  ConnectRoom(A, B);  // TODO: Add this connection to the real variables, 
  ConnectRoom(B, A);  //  because this A and B will be destroyed when this function terminates
}





/*
 NAME
   MakeMyRooms
 DESCRIPTION
   Will select 7 rooms - set the total connections to null and then randomly select from the list of 10
   to connect.
*/
void MakeMyRooms()
{
     int i;   
    
    //room_state used to control which rooms will be used and connected
    for(i = 0; i < MAX_NUM_ROOMS;i++){
        room_state[i] = FALSE;
    }

   
    for(i = 0; i < MAX_NUM_ROOMS;i++){
        room_list[i].total_connections = 0;

        //set all Room connections to NULL
        int j;
        for(j = 0; j < MAX_ROOM_CONNECTIONS; j++)
        {
            room_list[i].Connections[j] = NULL;
        }

        while(TRUE){
            int random_room = rand() % TOTAL_NUM_ROOMS; 
          
            if(room_state[random_room] == FALSE)
            {  
                // change room state to TRUE so as not to cbe chosen again
                room_state[random_room] = TRUE;

                //remove garbage data from ROOM arrray
                memset(room_list[i].name,'\0',sizeof(room_list[i].name));
                strcpy(room_list[i].name,room_names[random_room]);

                room_list[i].type = room_types[1];
                break;
            }
        }
    }

    //since rooms are randomly chosen assigning first room to be START ROOM 
    //and last room in array to be END ROOM
    room_list[0].type = room_types[0];
    room_list[MAX_NUM_ROOMS - 1].type = room_types[2];


    while(IsGraphFull() == FALSE)
    {
      AddRandomConnection();
    } 
}



/*
 NAME
   MakeFileDirectory
 DESCRIPTION
   Makes a file directory using the current pid and sets the permissions
*/
void MakeFileDirectory()
{
    char* directory_name = "rubioj.rooms.";
    int pid = getpid();

    //clear out array for folder name of any garbage data
    memset(folder_name,'\0',sizeof(folder_name));
    //set folder name to as specs require
    sprintf(folder_name,"%s%d",directory_name,pid);

    //set the permissions to read and write  
    mkdir(folder_name,0770); 
}



/*
 NAME
   MakeMyFile
 DESCRIPTION
   Calls MakeFileDirectory and saves rooms files in the made directory
*/
void MakeMyFiles()
{
    FILE *Room_File;
    int i,j;
    char folder_directory[256];

    sprintf(folder_directory,"./rubioj.rooms.%d",getpid()); 

    MakeFileDirectory(); // sets a folder.
    
    //check to see if folder already exists and prevents error if it does
    if(chdir(folder_directory) != 0){
        printf("Directory has been changed: %s\n",folder_directory);
        return;
    }

   //parse through the room_list and make a file for each
    for(i = 0; i < MAX_NUM_ROOMS; i++)
    {
       Room_File = fopen(room_list[i].name,"w");
        
        fprintf(Room_File,"ROOM NAME: %s\n",room_list[i].name);
        //pare through each room struct connection to print to file
        for(j = 0;j < room_list[i].total_connections;j++)
        {
            fprintf(Room_File,"CONNECTION %d: %s\n",j+1,room_list[i].Connections[j]->name);
        }
        
        fprintf(Room_File,"ROOM TYPE: %s\n",room_list[i].type);
        fclose(Room_File);
    }
}


int main(void)
{
    srand(time(NULL));
    MakeMyRooms();
    MakeMyFiles();
    return 0;
}