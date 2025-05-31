/*

    S-TOOLS.C

    This file has the purpose of being able of 
    for example going back to the previous node
    through reading a save-state file. 

    how to use: 
        ./s back
        ./s back 2
        ./s back 999

        ./s reset

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* USER CUSTOMIZATION */

    /* Change to for example "vim" ("code"=vscode) */
    static bool user_copy_cmd_to_clipboard = true;      // default: true

/*  */

static const char *SPACES_ERROR = "[SPACES-ERROR]:"; // also defined in v1spaces.c

int find_start_end(FILE *save_fp, int lines, char *start_string, char *end_string) {
    // find start of save state
    fseek(save_fp, 0, SEEK_SET);
    char compare_buffer[256];
    int compare_line = -1;
    int l = 0;
    while (l < lines) {
        fgets(compare_buffer, 256, save_fp);
        //printf("1::%s", compare_buffer);
        if (strcmp(compare_buffer, start_string) == 0) {
            compare_line = l;
        }
        l++;
    }
    if (compare_line == -1) {
        printf("[SPACES-ERROR]: could not find a save state.\n");
        return -1;
    }
    
    // find end of save state (for safety)
    fseek(save_fp, 0, SEEK_SET);
    l = 0;
    compare_line = -1;
    while (l < lines) {
        fgets(compare_buffer, 256, save_fp);
        //printf(":    %s", compare_buffer);
        if (strcmp(compare_buffer, end_string) == 0) {
            compare_line = l;
        }
        l++;
    }
    if (compare_line == -1) {
        printf("[SPACES-ERROR]: a save state was corrupted.\n");
        return -1;
    }
    return 0;
}

int get_string_from_to(FILE *save_fp, int lines, char *destination, char *start_string, char *end_string) {
    fseek(save_fp, 0, SEEK_SET);
    char compare_buffer[256];
    int compare_line = -1;
    int f_offset = 0;
    int l = 0;
    while (l < lines) {
        fgets(compare_buffer, 256, save_fp);
        if (strcmp(compare_buffer, start_string) == 0) {
            compare_line = l;
            f_offset = ftell(save_fp);
        }
        l++;
    }
    l = 0;
    fseek(save_fp, f_offset, SEEK_SET);
    while (l < lines) {
         fgets(compare_buffer, 256, save_fp);
         if (strcmp(compare_buffer, end_string) == 0) {
            return 0;
        }
        destination = strcat(destination, compare_buffer);
    }
    return -1; // TODO: handle this
}


int main(int argc, char *argv[]) {

    if (strcmp(argv[1], "back") == 0) {
        FILE* save_fp;
            save_fp = fopen("save-states.txt", "r");
            if (save_fp == NULL) {
                printf("%s could not open save file.\n", SPACES_ERROR);
                return -1;
            }

            
            // count lines
            int ch = 0;
            int lines = 0;
            while ((ch = fgetc(save_fp)) != EOF) {
                if (ch == '\n')
                lines++;
            }

            // based on back-ammount (eg: go back 2 nodes), calculate total subtraction in lines from EOF
            int line_subtraction = 0;
            if (argc > 2) {
                for (int i = 0; i < atoi(argv[2])-1; i++) {
                    fseek(save_fp, 0, SEEK_SET);
                    char compare_buffer[256];
                    int compare_line = -1;
                    int l = 0;
                    while (l < lines) {
                        fgets(compare_buffer, 256, save_fp);
                        if (strcmp(compare_buffer, "SPACES-SAVE-STATE-BEGIN\n") == 0) {
                            compare_line = l;
                        }
                        l++;
                    }
                    line_subtraction += lines - compare_line;
                    if (compare_line == -1) {
                        printf("%s too few save states exsist.\n", SPACES_ERROR);
                        return -1;
                    }
                }
            }

            // check for start of save state
            if (find_start_end(save_fp, lines-line_subtraction, "SPACES-SAVE-STATE-BEGIN\n", "SPACES-SAVE-STATE-END\n") == -1) {
                return -1; 
            }

            // check for start of cmd and then extract it 
            if (find_start_end(save_fp, lines-line_subtraction, "SPACES-CMD-BEGIN\n", "SPACES-CMD-END\n")) {
                return -1;
            }
            char cmd[512];
            memset(cmd, 0, sizeof(cmd));
            get_string_from_to(save_fp, lines-line_subtraction, cmd, "SPACES-CMD-BEGIN\n", "SPACES-CMD-END\n");
            printf("%s\n", cmd);

            fclose(save_fp);

            if (user_copy_cmd_to_clipboard) {
                #if __APPLE__
                    FILE *p = popen("/usr/bin/pbcopy","w");
                    fprintf(p,"%s", cmd);
                    pclose(p);
                #endif

                #if _WIN32
                    // https://stackoverflow.com/questions/1264137/how-to-copy-string-to-clipboard-in-c
                #endif
            }
    }


    
    if (strcmp(argv[1], "reset") == 0) {

        printf("Are you sure you want to delete all save states? (Y/N)\n>");
        char answer = 'N';
        scanf("%c", &answer);

        if (answer == 'Y') {
            FILE* save_fp;
            save_fp = fopen("save-states.txt", "w");
            if (save_fp == NULL) {
                printf("%s could not open save file.\n", SPACES_ERROR);
                return -1;
            }
            fclose(save_fp);
        }   
    }

    return 0;
}