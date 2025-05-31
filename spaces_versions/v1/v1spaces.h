#ifndef SPACES_H_
#define SPACES_H_

#define MAX_CONNECTED_NODES 16

/* First version of Node (2025-05-29) */
typedef struct {
    char *node_name;
    char *node_file;
    char *date_created;
    char *connected_nodes[MAX_CONNECTED_NODES]; // MAX_CONNECTED_NODES is arbitrary
} V1Node;

void v1_spaces_initialize_node(V1Node *v1_node);
void v1_spaces_next_node(V1Node *v1_node, int connected_nodes_index);

#endif // SPACES_H_
