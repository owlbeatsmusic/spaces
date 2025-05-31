#include <stdio.h>
#include <stdbool.h>

#include "v1spaces.h"


/* USER CUSTOMIZATION */

    /* Change to for example "vim" ("code"->vscode) */
    static char *user_editor_cmd = "code";              // default: "code"
    static bool user_print_editor_cmd = true;           // default: true
    static bool user_print_run_and_compile_cmd = true;  // default: true
    static bool user_copy_cmd_to_clipboard = true;      // default: true

/*  */

static const char *SPACES_ERROR = "[SPACES-ERROR]:"; // also defined in s-tools.c

/* First version of Node (2025-05-29) */
void v1_spaces_initialize_node(V1Node *v1_node) {
    // construct the open-editor command for self
    char editor_cmd[256];
    if (user_print_editor_cmd) {
        sprintf(editor_cmd, "%s %s.c", user_editor_cmd, v1_node->node_file);
    }
    
    // construct the run-and-compile command for self
    char compile_and_run_cmd[256];
    if (user_print_run_and_compile_cmd) {
        sprintf(compile_and_run_cmd, "cc -o %s %s.c spaces_versions/v1/v1spaces.c && ./%s", v1_node->node_file, v1_node->node_file,v1_node->node_file);
    }

    // add save state to text file
    FILE* save_fp;
    save_fp = fopen("save-states.txt", "a");
    if (save_fp == NULL) {
        printf("%s could not write to save file.\n", SPACES_ERROR);
        return;
    }
    fprintf(save_fp, "SPACES-SAVE-STATE-BEGIN\nSPACES-CMD-BEGIN\n%s\n%s\nSPACES-CMD-END\nSPACES-SAVE-STATE-END\n\n", 
        editor_cmd, compile_and_run_cmd);
    fclose(save_fp);

    printf("\n.\n| [\"%s\" (V1 NODE) : CREATED %s]\n| \n", v1_node->node_name, v1_node->date_created);
}



void v1_spaces_next_node(V1Node *v1_node, int connected_nodes_index) {

    if (connected_nodes_index+1 > MAX_CONNECTED_NODES) {
        printf("");
    }

    // construct the open-editor command for next node
    char editor_cmd[256];
    if (user_print_editor_cmd) {
        sprintf(editor_cmd, "%s %s.c", user_editor_cmd, v1_node->connected_nodes[connected_nodes_index]);
    }
    
    // construct the run-and-compile command for next node
    char compile_and_run_cmd[256];
    if (user_print_run_and_compile_cmd) {
        sprintf(compile_and_run_cmd, "cc -o %s %s.c spaces_versions/v1/v1spaces.c && ./%s", 
            v1_node->connected_nodes[connected_nodes_index], 
            v1_node->connected_nodes[connected_nodes_index],
            v1_node->connected_nodes[connected_nodes_index]
        );
    }

    // copy to user clipboard if enabled
    if (user_copy_cmd_to_clipboard) {
        #if __APPLE__
            FILE *p = popen("/usr/bin/pbcopy","w");
            fprintf(p,"%s\n%s", editor_cmd, compile_and_run_cmd);
            pclose(p);
        #endif

        #if _WIN32
            // https://stackoverflow.com/questions/1264137/how-to-copy-string-to-clipboard-in-c
        #endif
    }

    // final output
    printf("\n\n[NEXT NODE]:\n%s\n%s\n\n", editor_cmd, compile_and_run_cmd);
}
