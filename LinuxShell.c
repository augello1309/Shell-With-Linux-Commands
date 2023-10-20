// Initial code for shell along with header files which may be required for reference
# include <stdio.h>
# include <stdlib.h>   // used to execute subprocess and commands
# include <string.h>
# include <unistd.h>   // used for exit, getcwd,read, write, exec
# include <sys/wait.h>
# include <sys/types.h>
# include <dirent.h> // for ls
# include <errno.h>
# include <sys/stat.h>
# include <fcntl.h>  // used for open
# include <ctype.h>
# include <stdbool.h>
//char functions
char *read_line(char **argv);
char **parse(char *my_line, char **parsed);
char *Strcasestr(const char* buffer, const char* searchterm);

//void functions
void execute(char **parsed);
void cd(char **parsed);
void ls(char **parsed);
void funmkdir(char **parsed);
void funrmdir(char **parsed);
void execArgs(char **parsed);
void display();
void help();
void cat(char **parsed);
void touch(char **parsed);
void grep(char **parsed);

//int functions
int process(char **argv, char **parsed);
int cmds(char **parsed);

//global variables
char prevdir[100];
int wordcount = 0;
int main(int argc, char** argv)
{
    //show the startup display; NOT PART OF MAIN PROJECT, I just wanted a start up display screen.
    display();
    int fl = 0;
    //while loop to run commands until the command is exit
    while(1)
    {
        //keep a parsed double pointer array
        char **parsed[100];
        char buf[100];
        strcpy(buf, argv);
        //read a line from the user
        if(read_line(buf))
            continue;
        //If there is a command entered, the process function will return 1
        fl = process(buf, *parsed);
        //If there is a command entered, the execArgs function will fork the parsed command.
        if(fl == 1)
            execArgs(*parsed);


    }
    return 0;
}

//function to show a startup display for two seconds. Clears the screen before startup and after the display has shown for 2 seconds
void display()
{
    system("clear");
    printf("\033[0;36m");
    printf("\t\t**********Welcome to the Shell**********");
    printf("\n\n\t\t\t   Name: Alex Augello\n\n\t\t\t    RUID: 200000681");
    fflush(stdout);
    printf("\033[0m");
    sleep(2);
    system("clear");
}

// read command line for the input line in shell
char *read_line(char **str)
{
    //buffer and current working directory character arrays
    char buf[100];
    char cwd[100];
    printf("\033[0;32m");
    printf("%s >>> ", getenv("USER")); //prints before every command
    printf("\033[0m");
    fgets(buf, 100, stdin); //user input up to 100 characters
    buf[strlen(buf)-1] = '\0';
    //This if statement removes any extra whitespaces to avoid seg faults
    if(strlen(buf) != 0)
    {
       int i;
       int x;
       for(i = x = 0; buf[i]; i++)
        if(!isspace(buf[i]) || (i > 0 && !isspace(buf[i-1])))
            buf[x++] = buf[i];
        buf[x] = '\0';
        int index, j;
        index = -1;
        j = 0;
        while(buf[j] != '\0')
        {
            if(buf[j] != ' ' && buf[j] != '\t' && buf[j] != '\0')
            {
                index = j;
            }

            j++;
        }
        buf[index + 1] = '\0';
        //copy the command after removing extra whitespaces to the str parameter
        strcpy(str, buf);
        return 0;
    }

    else
        return (void *)1;

}

// parse the input command
char **parse(char *my_line, char **parsed)
{
    wordcount = 0;
    for(int i = 0; i < 100; i++)
    {
        //separates each word after each space and stores them into a pointer called parsed
        parsed[i] = strsep(&my_line, " ");
        if (parsed[i] == NULL)
            break;
        if(strlen(parsed[i]) == 0)
            i--;
        wordcount++;
    }
    return 0;
}
//function to fork the command
void execArgs(char **parsed)
{
    // Forking a child
    pid_t pid = fork();

    if (pid == -1) {
        printf("\nFailed forking child..");
        return;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command..");
        }
        exit(0);
    } else {
        // waiting for child to terminate
        wait(NULL);
        return;
    }
}
//function to execute all commands
int cmds(char **parsed)
{
    int commands = 10;
    int switchstate = 0;
    char *commandlist[commands];
    commandlist[0] = "exit";
    commandlist[1] = "cd";
    commandlist[2] = "help";
    commandlist[3] = "ls";
    commandlist[4] = "mkdir";
    commandlist[5] = "rmdir";
    commandlist[6] = "pwd";
    commandlist[7] = "cat";
    commandlist[8] = "touch";
    commandlist[9] = "grep";


    //for loop to check which command was typed in. The value of switchstate is unique to each command
    for(int i = 0; i < commands; i++)
    {
        if(strcmp(parsed[0], commandlist[i]) == 0)
        {
            switchstate = i + 1;
            break;
        }


    }
    //Depending on the value of switchstate, execute the correct command
    switch(switchstate)
    {
        case 1:
            printf("\033[0;35m");
            printf("\nLogging out...\n");
            printf("\033[0m");
            exit(0);

        case 2:
            cd(parsed);
            return 1;

        case 3:
            help();
            return 1;

        case 4:
            ls(parsed);
            return 1;

        case 5:
            funmkdir(parsed);
            return 1;

        case 6:
            funrmdir(parsed);
            return 1;

        case 7:
            char cwd[256];
            if(getcwd(cwd, sizeof(cwd)) == NULL)
                perror("getcwd() error");
            else
                printf("%s\n", cwd);
            return 1;
        case 8:
            cat(parsed);
            return 1;
        case 9:
            touch(parsed);
            return 1;
        case 10:
            grep(parsed);
            return 1;
    default:
        break;
    }
    //if the command entered is not the same as any of the commands in commandlist, tell the user that the command was not found
    if(switchstate == 0)
    {
        printf("\033[0;31m");
        printf("Command Not Found!\n");
        printf("\033[0m");
    }


}
//help command shows all available commands in shell
void help()
{
    printf("The available commands are: \n"
    "1. exit (exit)\n"
    "2. pwd (pwd)\n"
    "3. cd (cd -, cd ~, cd ~/, cd, cd (directory(can include space)))\n"
    "4. help (help)\n"
    "5. ls (ls .., ls, ls (directory)\n"
    "6. mkdir (directory name(can include space))\n"
    "7. rmdir (directory name(can include space))\n"
    "8. cat (filename(can include spaces))\n"
    "9. touch (filename(can include spaces))\n"
    "10. grep <searchTerm> <filename> or grep <\"searchTerm\"> <filename> \n(filname can include spaces and search term can include spaces but only inside quotes)\n");

}

//if there is a command entered, then the value will return 0. If the value is 0, then the program will execute the forking process
int process(char **argv, char **parsed)
{
    parse(argv, parsed);
    if(cmds(parsed))
        return 0;
    else
        return 1;

}
//Function for the change directory command
void cd(char **parsed)
{
    char currentdir[100];
    char afterchange[100];
    char path[100];
    getcwd(currentdir, sizeof currentdir);
    char buf[100];
    char newbuf[100];
    newbuf[0] = '\0';
    //if there is input after cd store the characters after cd into a character buffer array
    if(parsed[1] != NULL)
    {
        for(int i = 1; i < wordcount; i++)
        {
            if(parsed != NULL)
            {
            strcpy(buf, parsed[i]);
            strcat(newbuf, buf);
            if(i == wordcount-1)
            {
                break;
            }
            strcat(newbuf, " ");
            }
        }
}
    int cdflag = 0;
    //if the command typed in is just cd then change the working directory to /home/user
    if(parsed[1] == NULL)
        {
            char *arg;
            arg = getenv("HOME");
            //checks to see if the cd command works
            int ch = chdir(arg);
            if(ch < 0)
            {
                printf("\033[0;31m");
                printf("Change not successful\n");
                printf("\033[0m");
                }
            else
            {
                printf("\033[0;34m");
                printf("Change successful\n");
                printf("\033[0m");
                }
        }
    //else if for cd -. cd - is not standard in the chdir command
    else if(strcmp(parsed[1], "-") == 0)
    {
        //checks if there was a previous directory
        if(prevdir[0] == '\0')
        {
            cdflag = 1;
            printf("\033[0;31m");
            printf("No previous directory\n");
            printf("\033[0m");
        }
        else
        {
            int ch = chdir(prevdir);
            if(ch < 0)
                {
                    printf("\033[0;31m");
                    printf("Change not successful\n");
                    printf("\033[0m");
                }
            else
                {
                    printf("\033[0;34m");
                    printf("Change successful\n");
                    printf("\033[0m");
                    printf("%s\n", getcwd(afterchange, sizeof afterchange));
                }
        }

    }
        //checks cd ~ and cd ~/. cd ~ and cd ~/ are not standard in the chdir command
        else if(strcmp(parsed[1], "~") == 0 || strcmp(parsed[1], "~/") == 0 )
        {
            char buf[100];
            strcpy(buf, parsed[1]);
            if(buf[strlen(buf)-1] == '/' || buf[strlen(buf)] == '\0')
            {
                //get the path of HOME and store it into path
                snprintf(path, sizeof path, "%s%s", getenv("HOME"), buf+1);
                for(int i = 0; i < sizeof path; i++)
                    buf[i] = path[i];
            }
            int ch = chdir(buf);
            if(ch < 0)
            {
                printf("\033[0;31m");
                printf("Change not successful\n");

                }
            else
            {
                printf("\033[0;34m");
                printf("Change successful\n");
                printf("\033[0m");
                }
        }
        else
        {
        int ch = chdir(newbuf);
        if(ch < 0)
        {
            printf("\033[0;31m");
            printf("Change not successful\n");
            printf("\033[0m");

            }
        else
        {
            printf("\033[0;34m");
            printf("Change successful\n");
            printf("\033[0m");

            }
        }
        //keep track of the previous directory for cd -.
        if(cdflag == 0)
            strcpy(prevdir, currentdir);
    }

//function for the ls command
void ls(char **parsed)
{
    //DIR for a variable that points to a directory. Kind of like FILE
    DIR *d;
    DIR *de;
    //structure for dir
    struct dirent *dir;
    //opens directory
    d = opendir(".");
    int flag = 0;
    char buf[100];
    char newbuf[100];
    newbuf[0] = '\0';
    //checks to see if there are characters typed in after ls
    if(parsed[1] != NULL)
    {
        //if there are characters after ls, store them into newbuf
        for(int i = 1; i < wordcount; i++)
        {
            if(parsed != NULL)
            {
            strcpy(buf, parsed[i]);
            strcat(newbuf, buf);
            if(i == wordcount-1)
            {
                break;
            }
            strcat(newbuf, " ");
            }
        }
        de = opendir(newbuf);
        //while there are directories to read
        while((dir = readdir(d)) != NULL)
        {
            //if the directory thats in newbuf is detected, flag it
            struct stat buf;
            if(dir->d_type == DT_DIR)
                if(strcmp(newbuf, dir->d_name) == 0)
                    flag = 1;
        }
        //if the directory has been found, read the contents inside the directory. The called directory is in newbuf.
        if(flag == 1)
        {
            while((dir = readdir(de)) != NULL)
            {
                    printf("%s\n", dir->d_name);
            }
        closedir(d);
        }
        else
        {
            printf("Directory not found\n");
        }
    }

    //else if the command entered is just ls
    else if(parsed[1] == NULL)
    {
        //show all of the contents of the current directory
        while((dir = readdir(d)) != NULL)
        {
            printf("%s\n", dir->d_name);
        }
        closedir(d);
    }
    else
    {
        printf("Not a working command\n");

    }
}
//function to make a directory. Can only make one directory at a time
void funmkdir(char **parsed)
{
    char buf[100];
    char newbuf[100];
    newbuf[0] = '\0';
    //Stores the directory name into newbuf. The directory can have spaces.
    for(int i = 1; i < wordcount; i++)
    {
        if(parsed != NULL)
        {
            strcpy(buf, parsed[i]);
            strcat(newbuf, buf);
            if(i == wordcount-1)
            {
                break;
            }
            strcat(newbuf, " ");
        }
    }

    //if the command entered is just mkdir, tell the user there is no directory to be made.
    if(newbuf[0] == '\0')
    {
        fprintf(stderr, "Error: Must enter directory name\n");
    }
    else
    {
        //checks to see if directory can be created.
        int check = mkdir(newbuf, 0777);
        if (!check)
            printf("Directory created\n");
        else
        {
            printf("Unable to create directory. Directory may have same name.\n");
        }
    }
}
//function to remove a directory. Can only remove one directory at a time. Can remove directory with spaces
void funrmdir(char **parsed)
{
    char buf[100];
    char newbuf[100];
    newbuf[0] = '\0';
    //stores the directory that needs to be removed into newbuf
    for(int i = 1; i < wordcount; i++)
    {
        if(parsed != NULL)
        {
            strcpy(buf, parsed[i]);
            strcat(newbuf, buf);
            if(i == wordcount-1)
            {
                break;
            }
            strcat(newbuf, " ");

        }
    }
    //If the command entered is just rmdir, then tell the user that they must enter a directory to remove
    if(newbuf[0] == '\0')
    {
        fprintf(stderr, "Error: Must enter directory name\n");
    }
    else
    {
        //checks to see if directory exists
        int flag = 0;
        DIR* dir = opendir(newbuf);
        if(dir)
        {
            closedir(dir);
            flag = 1;
        }
        else if (ENOENT == errno)
            fprintf(stderr, "Error: Directory does not exist\n");

        //if the directory exists, then remove the directory. Also, check to see if the directory has been removed.
        if(flag == 1)
        {
        int check = rmdir(newbuf);

        if(check == 0)
            printf("Directory deleted successfully\n");
        else
            fprintf(stderr, "Error removing directory. Is the directory empty?\n");
        }
    }
}

void cat(char **parsed)
{
    char buf[100];
    char newbuf[100];
    newbuf[0] = '\0';
    //Stores the file name into newbuf. The file name can have spaces.
    for(int i = 1; i < wordcount; i++)
    {
        if(parsed != NULL)
        {
            strcpy(buf, parsed[i]);
            strcat(newbuf, buf);
            if(i == wordcount-1)
            {
                break;
            }
            strcat(newbuf, " ");

        }
    }
    char buffer[500];
    int textsize = 1;
    int flag = 0;
    struct stat buffers;
    int exists = stat(newbuf, &buffers);
    FILE *textfile;
    //if the file doesn't exist, return an error
    if (exists == -1)
    {
        printf("\033[0;31m");
        printf("File not found. Input a correct file.\n");
        printf("\033[0m");
    }
    //else, print out all the text in the file including their line number
    else
    {
    textfile = fopen(newbuf, "r");
    printf("\033[0;34m");
    printf("Here are the lines of the file: \n");
    printf("\033[0m");
    while(fgets(buffer, sizeof(buffer), textfile) != NULL)
    {
        printf("line %d: %s", textsize, buffer);
        textsize++;
    }

    fclose(textfile);
    }


}


void touch(char **parsed)
{
    char buf[100];
    char newbuf[100];
    newbuf[0] = '\0';
    //Stores the file name into newbuf. The file name can have spaces.
    for(int i = 1; i < wordcount; i++)
    {
        if(parsed != NULL)
        {
            strcpy(buf, parsed[i]);
            strcat(newbuf, buf);
            if(i == wordcount-1)
            {
                break;
            }
            strcat(newbuf, " ");

        }
    }
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    char response[2];
    int flag = 0;
    //checks to see if the file exists
    while((dir = readdir(d)) != NULL)
    {
        if(strcmp(dir->d_name, newbuf) == 0)
        {
            printf("\033[0;34m");
            printf("File already exists. Would you like to continue? (Y/N)\n");
            printf("\033[0m");
            gets(response);
            while(strlen(response) > 2)
            {
                printf("\033[0;31m");
                printf("Invalid response\n");
                printf("\033[0m");
                gets(response);
            }
            response[2] = '\0';
            response[0] = toupper(response[0]);
            //if the user wants to make the file anyway, make the file and flag that the file has been made.
            if(strcmp(response, "Y") == 0)
            {
                FILE *fp = fopen(newbuf, "w");
                fclose(fp);
                printf("\033[0;34m");
                printf("File has been made.\n");
                printf("\033[0m");
                flag = 1;
            }
            //if the user doesn't want to make the file, exit out of the command and flag that the command has been done.
            else
            {
                printf("\033[0;34m");
                printf("File has not been made.\n");
                printf("\033[0m");
                flag = 1;
            }
        }
    }
    //if the file is unique make the desired file
    if(flag == 0)
    {
        FILE *fp = fopen(newbuf, "w");
        fclose(fp);
        printf("\033[0;34m");
        printf("File has been made.\n");
        printf("\033[0m");
    }

    closedir(d);
}



//searches for case insensitive substring in the character buffer
char* Strcasestr(const char* buffer, const char* searchterm) {
    if (!searchterm[0]) return (char*) buffer;
    for (size_t i = 0; buffer[i]; i++) {
        bool found = true;
        for (size_t j = 0; searchterm[j]; j++) {
            if (!buffer[i + j]) return NULL;
            if (tolower((unsigned char)searchterm[j]) !=
                tolower((unsigned char)buffer[i + j])) {
                found = false;
                break;
            }
        }
        if (found) return (char *)(buffer + i);
    }
    return NULL;
}

//searches for a desired phrase in a file. Returns the lines with that phrase.
void grep(char **parsed)
{
    char buf[100];
    char newbuf[100];
    newbuf[0] = '\0';
    //Stores the file name and text into newbuf. The file name can have spaces.
    for(int i = 1; i < wordcount; i++)
    {
        if(parsed != NULL)
        {
            strcpy(buf, parsed[i]);
            strcat(newbuf, buf);
            if(i == wordcount-1)
            {
                break;
            }
            strcat(newbuf, " ");

        }
    }
    //if there was nothing entered after grep, print out grep's usage
    if (newbuf[0] == '\0')
        {
        printf("\033[0;31m");
        printf("USAGE: grep <searchTerm> <filename> or grep <\"searchTerm\"> <filename>\n");
        printf("\033[0m");
        return 1;
        }
    int flag = 0;
    char searchterm[40] = {0};
    char filename[40] = {0};
    const char *searchterm1;
    char filename1[40];
    int i = 0;
    int fileflag;
    //checks if the search term has quotes surrounding it. This is to check for input of multiple words.
    if(newbuf[0] == '\"')
    {
        flag = 1;
        fileflag = 1;
        while(flag = 1)
        {
            //Checks for an end quote. If there isn't an end quote, print back an error.
            if(newbuf[i+1] == '\"')
            {
                flag = 0;
                break;
            }
            else if(newbuf[i+1] == '\0')
            {
                printf("\033[0;31m");
                printf("Error: Missing ending quote\n");
                printf("\033[0m");
                return 1;
            }
            searchterm[i] = newbuf[i + 1];
            i++;
        }
        int j = strlen(searchterm) + 3;
        int k = 0;
        //checks if a file was entered
        if(newbuf[strlen(searchterm)+2] == '\0')
        {
            printf("\033[0;31m");
            printf("Error: Must enter file name.\n");
            printf("\033[0m");
            return 1;
        }
        while(newbuf[j] != '\0')
        {
            filename[k] = newbuf[j];
            k++;
            j++;
        }
    }
    //perform the grep command
    else
    {
        fileflag = 2;
        char *token = strtok(newbuf, " ");
        searchterm1 = token;
        token = strtok(NULL, " ");
        if(token == NULL)
        {
            printf("\033[0;31m");
            printf("Error: Must enter a file name\n");
            printf("\033[0m");
            return 1;
        }
        else
        {
            while(token != NULL)
            {
                strcat(filename1, token);
                strcat(filename1, " ");
                token = strtok(NULL, " ");
            }

            filename1[strlen(filename1)-1] = '\0';
        }
    }

    FILE *fp;
    char filebuffer[4000];
    bool foundflag = false;
    int substringflag2 = 0;
    int linecount = 1;
    //opens the file based on if the search term had quotes or not(fileflag)
    if(fileflag == 1)
    {
        fp = fopen(filename, "r");
        for(int g = 0; g < strlen(filename); g++)
            filename[g] = NULL;
        if(!fp)
        {
            printf("\033[0;31m");
            printf("File not found.\n");
            printf("\033[0m");
            return 1;
        }
        //prints out the lines that have the search term, if there are any.
        while(fgets(filebuffer, sizeof(filebuffer), fp) != NULL)
        {
            if(Strcasestr(filebuffer, searchterm))
            {
                substringflag2++;
                if(substringflag2 == 1)
                {
                    printf("\033[0;34m");
                    printf("Here are the lines with this search term:\n");
                    printf("\033[0m");
                }

                printf("%d. %s", linecount, filebuffer);
                foundflag = true;
            }
            linecount++;
        }
        //checks if substring has not been found
        if (foundflag == false)
        {
            printf("\033[0;31m");
            printf("Substring not found.\n");
            printf("\033[0m");
        }

        fclose(fp);
        return 1;
    }

    FILE *fp1;
    const char filebuffer2[1000];
    bool substringfound = false;
    int substringflag = 0;
    int linenumber = 1;
    if(fileflag == 2)
    {
        fp1 = fopen(filename1, "r");
        for(int g = 0; g < strlen(filename1); g++)
            filename1[g] = NULL;
        if(!fp1)
        {
            printf("\033[0;31m");
            printf("File not found.\n");
            printf("\033[0m");
            return 1;
        }

        while(fgets(filebuffer2, sizeof(filebuffer2), fp1) != NULL)
        {

            if(Strcasestr(filebuffer2, searchterm1))
            {
                substringflag++;
                if(substringflag == 1)
                {
                    printf("\033[0;34m");
                    printf("Here are the lines with this search term:\n");
                    printf("\033[0m");
                }
                printf("%d. %s", linenumber, filebuffer2);
                substringfound = true;
            }
            linenumber++;

        }
        if(substringfound == false)
        {
            printf("\033[0;31m");
            printf("Substring not found.\n");
            printf("\033[0m");
        }
        fclose(fp1);
        return 1;
    }

}











