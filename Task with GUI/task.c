#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
/*FUNCTOINS ABS*/
void  listAllTheProcessesInTheSystem();
void  listAllTheProcessesGroupedByUser();
void  stopSpecificProcess();
void  runSpecificProcess();
void  sendSpecificSignalsToSpecificProcess();
char command[150] ,t;
int main(int argc , char *argv[])
{

    //MANUAL PAGE
    /*
    for the req manual page please follow the next steps on your terminal
    -   gedit task.1
    -   write the following text on the file task.1 "the manual page of the project"
    -   gzip task.1
    -   sudo cp task.1.gz /usr/share/man/ma1
    -   enter your password
    */

    /*THE INTERFACE*/

    
    while(1)//inf loop
    {

    int choise;
    printf("Enter a Number based on the following Menu:\n");
    /*1. list all the process
    2.List all the processes grouped by user
    3.run proccesses
    4.stop proccesses
    5.Send specific signals to specific process
    6.EXIT*/
    printf("\t1.List all the process \n\t2.List all the processes grouped by user\n\t3.run proccesses\n\t4.stop proccesses\n\t5.Send specific signals to specific process\n\t6.Display proccess ID\n\t7.EXIT\n");
    scanf(" %d",&choise);
    t = getchar();

    switch(choise)
    {
    case 1:
        listAllTheProcessesInTheSystem();
        break;
    case 2:
        listAllTheProcessesGroupedByUser();
        break;
    case 3:
        runSpecificProcess();
        break;
    case 4:
        stopSpecificProcess();
        break;
    case 5:
        sendSpecificSignalsToSpecificProcess();
        break;
    case 6:
        listAllTheProcessesInTheSystem();
        break;
    case 7:
        printf("we hope you have found what are you looking for.");
        exit(0);
    default:
        printf("wrong option/n");
        break;
    }

    }

    return 0;
}
 /*FUNCTIONS IMPLEMENTATION*/
    void runTheCommand(char c[], int n){
        system(c);// to run the command in the terminal
        strcpy(c, "");//to empty the command prompet each time
    }
    /*1*/
    /*to display all system processes with the ID*/
    void  listAllTheProcessesInTheSystem(){
        strcpy(command,"ps -A");
        runTheCommand(command, 150);
    }

    /*2*/
    /*TO LIST ALL PROCESSES FOR A SPECIFIEC USER*/
    void  listAllTheProcessesGroupedByUser(){
        char user[50];
    printf("Could you please enter the USER :");
    /*read user name*/
    fgets(user, sizeof(user), stdin);
    //set command line
    strcpy(command, "ps -u ");
    strcat(command, user);
    runTheCommand(command, 150);
    }

    /*3*/
    /*RUN PROCESSES*/
    void  runSpecificProcess(){
    printf("Please enter the command for the process to run: ");
    fgets(command, sizeof(command), stdin);
    command[strcspn(command, "\n")] = '\0'; // Remove trailing newline

    if (strlen(command) == 0) {
        printf("No command entered.\n");
        return;
    }

    // Check if '&' is already there, to avoid "command & &"
    if (strlen(command) > 0 && command[strlen(command)-1] != '&') {
         strcat(command, " &");
         runTheCommand(command, 150);
    } else if (strlen(command) == 0 && command[strlen(command)-1] == '&') {
        // User just typed "&", which is not a valid command start
        printf("Invalid command.\n");
        return;
    }

    
    printf("Attempted to launch '%s'.\n", command); // Removed 'in the background' as it might already be there
}

    /*4*/
    /*STOP PROCESSES*/
    void  stopSpecificProcess(){
    listAllTheProcessesInTheSystem();
    //get the process id
   char id_input[20];
    printf("Enter the process ID: ");
    fgets(id_input, sizeof(id_input), stdin);
    id_input[strcspn(id_input, "\n")] = '\0'; // remove newline
     int id = atoi(id_input);
    //set command line
    //strcpy(command, "kill ");
    kill(id, SIGSTOP);
   // strcat(command,id);
    //runTheCommand(command, 150);
    }

    /*5*/
    /*SEND SIGNAL TO PROCESS*/
    void  sendSpecificSignalsToSpecificProcess(){
    //specify the signal
    int signalnum;
    printf("choose a signal from the following list :");
    printf("\n\t1.SIGQUIT\n\t2.SIGUSR1\n\t3.SIGCONT\n\t4.SIGTERM\n");
    if (scanf("%d", &signalnum) != 1) {
        printf("Invalid input. Please enter a number.\n");
        while (getchar() != '\n'); // Clear input buffer
        return;
    }
    
    while (getchar() != '\n'); // Clear the newline
    
    listAllTheProcessesInTheSystem();
     // Specify the process ID
    char id_input[20];
    printf("Enter the process ID: ");
    fgets(id_input, sizeof(id_input), stdin);
    id_input[strcspn(id_input, "\n")] = '\0'; // remove newline
     int id = atoi(id_input);
     // Send the signal
    int signal;
    switch(signalnum) {
        case 1:
          signal = SIGQUIT;
         break;
        case 2: 
          signal = SIGUSR1; 
          break;
        case 3: 
          signal = SIGCONT; 
          break;
        case 4: 
          signal = SIGTERM; 
          break;
        default:
            printf("Invalid signal choice.\n");
            return;
    }

    if (kill(id, signal) == -1) {
        perror("Failed to send signal");
    } else {
        printf("Signal %d sent to process %d successfully.\n", signal, id);
    }
    }
