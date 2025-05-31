# SPACES

Spaces is more an interactive art project than a useful program. It is a bunch of programs that are linked togheter to create an explorable "space" of programs(nodes). Spaces is also supposed to be used with an editor so that you can edit the code of the node you are in. All of this is my attempt to create a creative canvas for code.

It is reccomended to create a folder in this directory called for example `mainspace/` and put all nodes in that folder(then you can create more folder for separate spaces).

## Main Structure Rules

Altough Spaces is supposed to be decentralized - as in there is no fixed center point for all programs, they just call each other - it has some very basic rules. 

 - The file name of nodes are not allowed to be descriptive of its content (preferred is `node34.c`). This is because spaces are supposed to be explored though the nodes.

 - Each node has to have a description of what it does. 

 - Nodes are not allowed to share custom libraries with other nodes (other than the spaces header and for example OpenGL). This is to ensure programs are self-contained and the system is descentralized.

## Version Rules

And on top of this, depending of which verison of spaces you use (it is possible to create custom versions) there will additional rules for that version.

**For version rules: [Documentation for spaces versions](docs/versions/versions.md)**

There are also some rules of what a version must do (altough they are not as strict but are still reccomended):

 - Write run command for initialized node to `save-states.txt` with correct formatting. See [save-states.md](docs/save-states.md).


## External Tools

Because all nodes are separate, cross-node actions are not possible. For this there exsist some tools separated from the node-to-node process. 

The `save-states.txt` is a file that stores the history of visited files and is implemented in some verions of spaces which makes it a sort of center point but it is not necceary for the node-to-node process, mostly used by external tools.

### S-TOOLS

S-Tools is are a set of basic actions.

 - **BACK**. You can go back x ammount of nodes This is done by extracting build and run commands from the `save-states.txt`:
    ```shell
    ./s back
    ./s back 2  
    ```

 - **RESET**. You can also clear the `save-states.txt` file with s-tools:

    ```shell
    ./s reset 
    ```
