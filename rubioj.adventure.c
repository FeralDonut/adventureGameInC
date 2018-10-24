

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
    free(buffer);
    free(directory_pointer);
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
    char line_from_file[256];
    char data_from_line[256];

    FILE *room_file;
    int i, j, total_num_connections, connect_index;

    
    ReadMyRooms(); 

    chdir(folder_name);

    //dont need to check if file exists since we grabed it eariler
    for(i = 0;i < MAX_NUM_ROOMS;i++){
        room_file = fopen(room_list[i].name,"r");//OPEN FILE

        memset(line_from_file,'\0',sizeof(line_from_file));
        memset(data_from_line,'\0',sizeof(data_from_line));

        // get each line from the file.
        while(fgets(line_from_file,sizeof(line_from_file),room_file) != NULL){

            //get the label and value from the line.
            
            strtok(line_from_file,":");
            strcpy(data_from_line,strtok(NULL,""));
            data_from_line[strlen(data_from_line) - 1] = '\0';
            line_from_file[strlen(line_from_file) - 1] = '\0';

            for(j = 0;j < strlen(data_from_line);j++)
            { 
                data_from_line[j] = data_from_line[j+1];
            }

            if(strcmp(line_from_file,"ROOM TYP") == 0)
            { 

                if(strcmp(data_from_line,"START_ROOM") == 0){
                    room_list[i].type = "START_ROOM";
                }
                else if(strcmp(data_from_line,"END_ROOM") == 0){
                    room_list[i].type = "END_ROOM";
                }
                else{
                    room_list[i].type = "MID_ROOM";
                }
                //printf("Room typ:%s\n",data_from_line);
            }
            else if(strcmp(line_from_file,"CONNECTION ") == 0)
            { 
                //add connections to rooms struct                
                connect_index = FindRoomInArray(data_from_line);
                total_num_connections = room_list[i].total_connection;
                room_list[i].Connections[total_num_connections] = &room_list[connect_index]; 
                room_list[i].total_connection++; 

            }
        }
        fclose(room_file);
    }
    chdir(".."); 
}

/*
 NAME
    CurrentTimeFile()
 DESCRIPTION
    reads the current time, creates a folder and writes the current time to it
 SOURCE
   http://stackoverflow.com/questions/5141960/get-the-current-time-in-c
*/
void* CurrentTimeFile()
{
    char time_holder[256];
    time_t raw_time;
    struct tm * time_info;
    FILE *time_file_write;

    // clear time_holder array of garbage data
    memset(time_holder,'\0',sizeof(time_holder)); 

    time(&raw_time); 
    time_info = localtime(&raw_time); 
    strftime(time_holder,256, "%I:%M%P %A, %B %d, %Y", time_info); 
  
    //create a file to write to, write to it
    time_file_write = fopen(time_file,"w");
    fprintf(time_file_write,"%s\n",time_holder); 
    fclose(time_file_write);
}

/*
 NAME 
    DisplayTime
 DESCRIPTION
    creates a seperate thread, calls CurrentTimeFile function and reads what is writen to a time file
 RESOURCE
    Lecture 2.3
    https://www.geeksforgeeks.org/multithreading-c-2/
*/
void DisplayTime()
{
    char buffer[256];
    FILE* time_file_read;
    pthread_t time_thread; 
    // clear buffer array of garbage data
    memset(buffer,'\0',sizeof(buffer)); 
    
    //lock to run pthread_create and then unlock
    pthread_mutex_lock(&time_mutex);
    pthread_create(&time_thread,NULL,CurrentTimeFile,NULL);
    pthread_mutex_unlock(&time_mutex);
   
    pthread_join(time_thread,NULL);

    //open the file to read
    time_file_read = fopen(time_file,"r"); 

    //read and print the line in the file
    while(fgets(buffer,256,time_file_read) != NULL){
        printf("%s\n",buffer); 
    }
    fclose(time_file_read);
}



/*
 NAME
    RunGame()
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
    //loop until END ROOM is found     
      while(TRUE)
      {
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
        if((strcmp(user_buffer,"time") == 0 || strcmp(user_buffer,"Time") == 0 || strcmp(user_buffer,"TIME") == 0) )
        {
                DisplayTime();
        }
        // error message to user.
        else if(is_connected == FALSE){
            printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
        }
    }
}




int main(void)
{
    SelectFolder();
    MakeMyRooms();    
    RunGame();

return 0;
}

