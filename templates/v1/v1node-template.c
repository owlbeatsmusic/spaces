#include <stdio.h>
#include "../spaces_versions/v1/v1spaces.h"

int main(void) {
    
    V1Node self = {
        .node_name = "Template Node",
        .node_file = "mainspace/template",
        .date_created = "2025-05-30",
        .connected_nodes = {"mainspace/next-node"}
    };

    v1_spaces_initialize_node(&self);

     // CODE HERE
     //
     // printf(">");
     // scanf(".");


    v1_spaces_next_node(&self, 0); 

    return 0;
}

