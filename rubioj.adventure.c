/*
 name: Jose-Antonio D. Rubio
 OSUID: 932962915
 Class: 344-400
 Program 2 adventure.c
 Comment: A lot fo the references used in buildrooms were used here as well.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define MIN_ROOM_CONNECTIONS 3
#define MAX_ROOM_CONNECTIONS 6
#define MAX_NUM_ROOMS 7
#define TOTAL_NUM_ROOMS 10
#define TRUE 1
#define FALSE 0


char* room_types[3] = { "START_ROOM", "MID_ROOM", "END_ROOM" };

struct Room 
{
    int total_connection;
    struct Room* Connections[MAX_ROOM_CONNECTIONS];
    char* type;
    char name[256];
};



char* time_file = "currentTime.txt";
char folder_name[256];
struct Room room_list[MAX_NUM_ROOMS];
pthread_mutex_t time_mutex;



/*
 NAME
    SelectFolder
DESCRIPTION
    Finds the lastest folder modified in the current directory 
RESOURCE
    Lecture 2.4  https://oregonstate.instructure.com/courses/1692912/pages/2-dot-4-manipulating-directories   
*/
void SelectFolder()
{
    char *fd = "rubioj.rooms.";
    char current_directory[256];
    DIR *directory; 
    struct dirent *directory_pointer;
    struct stat *buffer;
    time_t last_modified;  
    time_t latest = 0;

    
    buffer = malloc(sizeof(struct stat));
    directory_pointer = malloc(sizeof(struct dirent));

    //clear arrays of any potential garbage data.
    memset(folder_name,'\0',sizeof(folder_name));
    memset(current_directory, '\0', sizeof(current_directory));

    getcwd(current_directory, sizeof(current_directory));
    directory = opendir(current_directory);

    //open current directory and parse through the files
    if (directory != NULL) {
        while (directory_pointer= readdir(directory)) 
        {  
            if (strstr(directory_pointer->d_name,fd) != NULL)
            {
                //read stats of folder in and find the latest modified one
                stat(directory_pointer->d_name, buffer);
                last_modified = buffer->st_mtime; 

                if(last_modified > latest)
                { 
                    latest = last_modified;
                    strcpy(folder_name,directory_pointer->d_name);
                }
            }
        }
    }
}


/*
 NAME
    ReadMyRooms
 DESCRIPTION
    Read in room struct info into room_list array
*/
void ReadMyRooms()
{
    DIR *directory;
    struct dirent *current;
    int i, j, k;
    

   //initialize room_list array to 0/NULL
   for(i = 0; i < MAX_NUM_ROOMS; i++)
   {
    //clear out garbage data in array
    memset(room_list[i].name,'\0',sizeof(room_list[i].name)); 
    room_list[i].total_connection = 0; 

        for(j = 0; j < MAX_ROOM_CONNECTIONS; j++)
        {
            //set connections to NULL
            room_list[i].Connections[j] = NULL;
        }
    }

    //parse through files and fill the room list array
    //check for error
    k=0;
    if ((directory = opendir (folder_name)) != NULL) 
    {    
        while ((current = readdir (directory)) != NULL) 
        {
            if(strlen(current->d_name) > 2)
            { 
                strcpy(room_list[k].name,current->d_name);
                k++;
            }
        }
    }
}


/*
 NAME
    FindRoomInArray
 DESCRIPTION
    searches room_list array for matching name and returns the location in room_list array
*/
int FindRoomInArray(char* current_room_name)
{
    int RoomPos = -1;
    int i;

    for(i = 0;i < MAX_NUM_ROOMS; i++ ){
        if( strcmp(room_list[i].name,current_room_name) == 0 ){ 
            return i;
        }
    }
}


/*
 NAME
    MakeMyRooms
 DECRIPTION
    Creates room structs from the files.  Calls ReadMyRooms()
 RESOURCE
    Lecture 2.5 Strings inC 
*/
void MakeMyRooms()
{
    char FileLineBuffer[256];
    char FileValueBuffer[256];

    FILE *room_file;
    int i, j, total_num_connections, connect_index;

    
    ReadMyRooms(); 

    chdir(folder_name);

    //dont need to check if file exists since we grabed it eariler
    for(i = 0;i < MAX_NUM_ROOMS;i++){
        room_file = fopen(room_list[i].name,"r");//OPEN FILE

        if(room_file == NULL){ // check if file was opened
            printf("%s file was not accessed\n",room_list[i].name);
            return;
        }

        memset(FileLineBuffer,'\0',sizeof(FileLineBuffer));
        memset(FileValueBuffer,'\0',sizeof(FileValueBuffer));

        // get each line from the file.
        while(fgets(FileLineBuffer,sizeof(FileLineBuffer),room_file) != NULL){

            //get the label and value from the line.
            
            strtok(FileLineBuffer,":");
            strcpy(FileValueBuffer,strtok(NULL,""));
            FileValueBuffer[strlen(FileValueBuffer) - 1] = '\0';
            FileLineBuffer[strlen(FileLineBuffer) - 1] = '\0';

            for(j = 0;j < strlen(FileValueBuffer);j++)
            { 
                FileValueBuffer[j] = FileValueBuffer[j+1];
            }

            if(strcmp(FileLineBuffer,"ROOM TYP") == 0)
            { 

                if(strcmp(FileValueBuffer,"START_ROOM") == 0){
                    room_list[i].type = "START_ROOM";
                }
                else if(strcmp(FileValueBuffer,"END_ROOM") == 0){
                    room_list[i].type = "END_ROOM";
                }
                else{
                    room_list[i].type = "MID_ROOM";
                }
                //printf("Room typ:%s\n",FileValueBuffer);
            }
            else if(strcmp(FileLineBuffer,"CONNECTION ") == 0)
            { 
                //add connections to rooms struct                
                connect_index = FindRoomInArray(FileValueBuffer);
                total_num_connections = room_list[i].total_connection;
                room_list[i].Connections[total_num_connections] = &room_list[connect_index]; 
                room_list[i].total_connection++; 

            }
        }
        fclose(room_file);
    }
    chdir(".."); 
}

/// NAME: CreateCurrentTimeFile
/// DESC: Creates a time file in current diretory.
/// SOURCE: http://stackoverflow.com/questions/5141960/get-the-current-time-in-c
void* CreateCurrentTimeFile()
{
    char TimeStr[256];
    time_t CurrTime;
    struct tm * TimeInfo;
    FILE *TimeFile;

    memset(TimeStr,'\0',sizeof(TimeStr)); // clear time string of garbage.

    time(&CurrTime); // get current time.
    TimeInfo = localtime(&CurrTime); // put time into an easily accessable struct.
    strftime(TimeStr,256, "%I:%M%P %A, %B %d, %Y", TimeInfo); // format string.
    //printf("\n%s\n\n",TimeStr);

    TimeFile = fopen(time_file,"w");//Will create or overwrite a file
    fprintf(TimeFile,"%s\n",TimeStr); // print time to file.
    fclose(TimeFile);

    return NULL;
}

/// NAME: ReadCurrentTimeFile
/// DESC: reads in a file and display the current time.
void ReadCurrentTimeFile()
{
    char Buffer[256];
    FILE *TimeFile;

    memset(Buffer,'\0',sizeof(Buffer)); // clear buffer of garbage.

    TimeFile = fopen(time_file,"r"); // readin a file.
    if(TimeFile == NULL){// check if the file exists.
        printf("%s was not accessed.\n", time_file);
        return;
    }

    //read in each line in the file (there should only be one.)
    while(fgets(Buffer,256,TimeFile) != NULL){
        printf("%s\n",Buffer); // print the line.
    }
    fclose(TimeFile);
}


/*
 NAME 
    TimeThread
 DESCRIPTION
    creates a seperate thread to write a file containing local time.
 RESOURCE
    Lecture 2.3
*/
int TimeThread()
{
    pthread_t time_thread; 
    //lock
    pthread_mutex_lock(&time_mutex);

    // if something went wrong dont continue.
    if(pthread_create(&time_thread,NULL,CreateCurrentTimeFile,NULL) != 0){ // begin running write file function.
        printf("Error from thread!");
        return FALSE;
    }

    //once done unlock the mutex.
    pthread_mutex_unlock(&time_mutex);
   
    pthread_join(time_thread,NULL);
    return TRUE;
}

/*
 NAME
    ReadMyRooms
 DESCRIPTION
    Function that drives the game - finds start_room, displays info for each room checks users inputs and will 
    moves accordingly, keeps track of steps taken, keeps track of path,, prints end path and steps and displays time
*/
void RunGame()
{
    int step_count = 0;
    int step_tracker[1028];
    int i, j, k, n, current_index;
    int is_connected = FALSE;
    struct Room current_room;
    char user_buffer[256];

    //find the START_ROOM
    for(i =0;i< MAX_NUM_ROOMS; i++)
    { 
        if(room_list[i].type == "START_ROOM")
        { 
            step_tracker[step_count] = i;
        }
    }
    
    //display room info to user
    do{        
        current_index = step_tracker[step_count];
        current_room = room_list[current_index];

        printf("CURRENT LOCATION: %s\n",current_room.name);

        printf("POSSIBLE CONNECTIONS:");
        for(j = 0; j < current_room.total_connection - 1; j++)
        {
            printf(" %s,",current_room.Connections[j]->name);
        }
        //needed so last connection is displayed
        printf(" %s.\n",current_room.Connections[j]->name);

        //clear user_buffer of any garbage data
        memset(user_buffer,'\0',sizeof(user_buffer));
        printf("WHERE TO? >");

        //read in user input and prevent buffer overrun
        //source: https://stackoverflow.com/questions/15813408/using-the-scanf-function
        scanf("%255s",user_buffer);
        printf("\n");

        //reset connection checker
        is_connected = FALSE;

        //check if input matches the name of a room.
        for(k = 0; k < current_room.total_connection; k++)
        {
            //checking if user input matches a roo
            if(strcmp(user_buffer,current_room.Connections[k]->name) == 0)
            { 
                step_count++; 
                //keep track of the path taken by storing the room user entered in
                step_tracker[step_count] = FindRoomInArray(user_buffer); 
                //move index to next room
                current_index = step_tracker[step_count]; 
                current_room = room_list[current_index];
                //keeps HUH? message from being thrown later
                is_connected = TRUE;
                //check to see if END_ROOM is found
                if(current_room.type == "END_ROOM")
                { 
                    printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
                    printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n",step_count);

                    //print the path n = 1 so start room is not displayed
                    for(n = 1; n < step_count + 1; n++)
                    { // for amount of steps print a name.
                        printf("%s\n",room_list[step_tracker[n]].name);
                    }
                    return;
                }             
            }
        }

        //see if user is asking for the time, TIME, or Time
        if((strcmp(user_buffer,"time") == 0 || strcmp(user_buffer,"Time") == 0 || strcmp(user_buffer,"TIME") == 0) ){

            if( TimeThread() == TRUE){
                ReadCurrentTimeFile(); 
            }
            
        }
        // error message to user.
        else if(is_connected == FALSE){
            printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
        }
    }
    while(TRUE);
}




int main(void)
{
    SelectFolder();
    MakeMyRooms();    
    RunGame();

}

