#include "cmds.h"

//Program Driver
int main(void)
{
    int commandNum = -1; //Command number for switch
    char* temp; //temp for parsing clobber

	//initialize procs/cwds/minode refcounts
	init();
    // mount the root
    mount_root();

    printf("\n\n-----------------------------------------------\n");
    printf("-          NShephard's FILE SYSTEM            -\n");
    printf("-         ENTER MENU TO SEE COMMANDS          -\n");
    printf("-----------------------------------------------\n\n");
    while(1)
    {
        strcpy(input, " "); //Reset input+pathname+command
        input[0] = 0;
        strcpy(pathname, " ");
        strcpy(command, " ");
        printf("Enter Command: ");
        scanf(" %[^\n]", input); //get command from input
        strcpy(command, strtok(input, " "));

        if ((temp = strtok('\0', "\n")) != '\0') //if pathname entered retrieve it
        {
            strcpy(pathname, temp);
		}
        else //no pathname entered
        {
            pathname[0] = 0;
		}
        printf("Command: %s  Pathname: %s\n", command, pathname);
       
        //Valid Command Check
        if ((commandNum = get_index(command)) == -1 )
        {
            printf("ERROR: Invalid Command!\n");
		}
		//Quit Check
        if (strcmp(command, "quit") == 0)
        {
            break;
		}
        else //Execute Command
        {
            execute_cmd(commandNum, pathname);
        }
    }
    return 0;
}
