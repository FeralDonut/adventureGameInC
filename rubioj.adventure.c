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

    //makes sure the current directory was opened to not throw erros.
    if (directory != NULL) {
        while (directory_pointer= readdir(directory)) {// read all files.  
            if (strstr(directory_pointer->d_name,fd) != NULL){
                stat(directory_pointer->d_name, buffer);// read in stats of a folder
                last_modified = buffer->st_mtime; // give me the last last_modified date of a file (long int)

                if(last_modified > latest){ // if this folder is the newest set as the newest.
                    latest = last_modified;
                    strcpy(folder_name,directory_pointer->d_name);
                }
            }
        }
    }
    //printf("This is the newest: %s\n",folder_name);
}


/*
 NAME
    ReadMyRooms
 DESCRIPTION
    Finds the lastest folder modified in the current directory 
 RESOURCE
    http://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
*/
void ReadMyRooms()
{
    DIR *directory;
    struct dirent *current;
    int i, j, k;
    

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


/// NAME: FindRoomInArray
/// DESC: Helper function to find the integer position of a room.
int FindRoomInArray(char *Roomname)
{
    int RoomPos = -1;
    int i;

    for(i = 0;i < MAX_NUM_ROOMS; i++ ){
        if( strcmp(room_list[i].name,Roomname) == 0 ){ 
            return i;
        }
    }
    return RoomPos; // return -1 if nothing was found.
}


/// NAME: CleanLabelFromStr
/// DESC:  helper function, this returns a label of a file buffer, and value of the other half of it.
void CleanLabelFromStr(char *LabelStr, char *ValueStr)
{
    int EOLpos = 0;
    int i;

    strtok(LabelStr,":");// cut label from value at :
    strcpy(ValueStr,strtok(NULL,""));// set value from strtok.
    ValueStr[strlen(ValueStr) - 1] = '\0';//append end of line
    LabelStr[strlen(LabelStr) - 1] = '\0';

    for(i = 0;i < strlen(ValueStr);i++){ // remove space form value string.
        ValueStr[i] = ValueStr[i+1];
    }

    // printf("label: %s\n",LabelStr);
    // printf("value: %s\n",ValueStr);
}


/// NAME: ReCreateConnection
/// DESC: creates a connection FOR ONLY 1 of the structs.
void ReCreateConnection(int roomPos1,int roomPos2)
{
    int totCon1 = room_list[roomPos1].total_connection;

    room_list[roomPos1].Connections[totCon1] = &room_list[roomPos2]; // add address
    room_list[roomPos1].total_connection++; // inc connection.
}


/// NAME: ReCreateStructRooms
/// DESC: re create all structs from the files in the most recent directory.
void ReCreateStructRooms()
{
    char FileLineBuffer[256];
    char FileValueBuffer[256];

    FILE *RoomFile;// file pointer
    int i;

    ReadMyRooms(); // fill struct with file names
    chdir(folder_name); // change to the directory containing all the files.

    //dont need to check if file exists since we grabed it eariler
    for(i = 0;i < MAX_NUM_ROOMS;i++){
        RoomFile = fopen(room_list[i].name,"r");//OPEN FILE

        if(RoomFile == NULL){ // check if file was opened
            printf("%s file was not accessed\n",room_list[i].name);
            return;
        }

        memset(FileLineBuffer,'\0',sizeof(FileLineBuffer));
        memset(FileValueBuffer,'\0',sizeof(FileValueBuffer));

        // get each line from the file.
        while(fgets(FileLineBuffer,sizeof(FileLineBuffer),RoomFile) != NULL){

            //get the label and value from the line.
            CleanLabelFromStr(FileLineBuffer,FileValueBuffer);
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
            else if(strcmp(FileLineBuffer,"CONNECTION ") == 0){ // fill in connections.
                int conncRoomPos = FindRoomInArray(FileValueBuffer);
                ReCreateConnection(i,conncRoomPos);
                //printf("Room CONNECTION:%s,%d\n",FileValueBuffer,conncRoomPos);
            }
        }
        fclose(RoomFile);//END FILE
    }
    chdir(".."); // go back to main directory.
}



/// NAME: printStepPath
/// DESC: from a array of ints get each name and print their names.
void printStepPath(int *Path,int steps)
{
    int i;
    for(i = 0;i < steps + 1;i++){ // for amount of steps print a name.
        printf("%s\n",room_list[Path[i]].name);
    }
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


/// NAME: TimeThread
/// DESC: creates a seperate thread to write a file containing local time.
int TimeThread()
{
    pthread_t WriteTimeFile_Thread; // holder for the thread that will contain the function.
    pthread_mutex_lock(&time_mutex); // this thread cannot be used untill its done running.

    // if something went wrong dont continue.
    if(pthread_create(&WriteTimeFile_Thread,NULL,CreateCurrentTimeFile,NULL) != 0){ // begin running write file function.
        printf("Error from thread!");
        return FALSE;
    }

    //once done unlock the mutex.
    pthread_mutex_unlock(&time_mutex);
    // prevent runnaway processes.
    pthread_join(WriteTimeFile_Thread,NULL);
    return TRUE;
}

/// NAME: RunGame
/// DESC: singleton to run the game.
void RunGame()
{
    int step_count = 0;
    int step_tracker[1028];
    int i, j, k, current_position;
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
        current_position = step_tracker[step_count];
        current_room = room_list[current_position];

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

        //check if input matches the name of a room.
        for(k = 0; k < current_room.total_connection; k++)
        {
            //checking if user input matches a room connection
            if(strcmp(user_buffer,current_room.Connections[k]->name) == 0)
            {
                step_count++; 
                //keep track of the path taken by storing the room user entered in
                step_tracker[step_count] = FindRoomInArray(user_buffer); 
                //move rooms
                current_position = step_tracker[step_count]; 
                current_room = room_list[current_position];
              //  is_connected = TRUE; 
                if(current_room.type == "END_ROOM"){ // check if room is end room.
                    printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
                    printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n",step_count + 1);
                    printStepPath(step_tracker,step_count);
                    return;
                }
            }
        }

        //see if user is asking for the time, TIME, or Time
        if((strcmp(user_buffer,"time") == 0 || strcmp(user_buffer,"Time") == 0 || strcmp(user_buffer,"TIME") == 0) )
        {

            if( TimeThread() == TRUE)
            {
                ReadCurrentTimeFile(); 
            }
            
        }else
        {
            printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
        }
    }
    while(TRUE); //infinite loop until END_ROOM is found
}




int main(void)
{
    SelectFolder();
    ReCreateStructRooms();    
    RunGame();
    //PrintRooms_DEBUG();
}

/*
/// NAME: PrintRooms_DEBUG
/// DESC: DEBUG FUNCTION lets me check if a room_list is correct.
void PrintRooms_DEBUG()
{
    int i,j;
    for(i = 0; i < MAX_NUM_ROOMS;i++){
        printf("\n%d: ",i);
        printf("name: %s",room_list[i].name);
        printf("\ntotal_connection: %d",room_list[i].total_connection);
        if(room_list[i].total_connection > 0){
            printf("\n\tConnections:");
            for(j = 0;j < room_list[i].total_connection;j++){
                printf("\n\tC%d:%s",j,room_list[i].Connections[j]->name);
                //printf("\n\tC%d connectTotal: %d",j,room_list[i].Connections[j]->total_connection);
            }
        }

        if(room_list[i].type ==" START_R"OOM){
            printf("\nRoom Type: START_ROOM");
        }
        else if(room_list[i].type ==" END_ROO"M){
            printf("\nRoom Type: END_ROOM");
        }
        else{
            printf("\nRoom Type: MID_ROOM");
        }
        printf("\n");
    }
    printf("\n");
}

*/