#include <xinu.h>
#include <fs.h>

extern fsystem_t* fsd;
extern filetable_t oft[NUM_FD];

/*
 * fs_open - Return SYSERR if the file is already open or does not exist
 *     otherwise, add a record to the oft in the first current FSTATE_CLOSED
 *     slot corresponding to the opened file and copy the inode from the block
 *     device to the oft record and return the index into the oft table for the
 *     newly opened file.
 */
int fs_open(char* filename, int flags) {

  int i,j;
  for (i=0; i<NUM_FD; i++){
      if(strcmp(fsd->root_dir.entry[oft[i].de].name, filename) == 0){
            if(oft[i].state == FSTATE_OPEN){
              //printf("\nFile already opened");
              return SYSERR;
            }
      }
  }
  
  /*
  for(i=0; i<NUM_FD; i++){
    if(oft[i].state == FSTATE_CLOSED){
      oft[i].state = FSTATE_OPEN;
      oft[i].fileptr = 0;
      for (j=0; j<fsd->root_dir.numentries; j++) {
        if(strcmp(filename,fsd->root_dir.entry[j].name) == 0){
            break;
        }
      } 
      oft[i].de = j;
      inode_t inode;
      bs_read(fsd->root_dir.entry[j].inode_block, 0, &inode, sizeof(inode_t));
      oft[i].in = inode;
      oft[i].flag = flags;
      return i;
    }
  }
  */

  for(int k = 0; k < DIR_SIZE; k++) {
    if (strcmp(fsd->root_dir.entry[k].name, filename) == 0) {

      // opening file
        for (i = 0; i < NUM_FD; i++) {
          if(oft[i].state == 0 || oft[i].state == FSTATE_CLOSED) {
            // i is the free index at oft available to open
            break;
          }
        }
        if (i == NUM_FD) {
          return SYSERR;
        }
        inode_t node;
        bs_read(fsd->root_dir.entry[k].inode_block, 0, &node, sizeof(inode_t));
        oft[i].state = FSTATE_OPEN;
        oft[i].fileptr = 0;
        oft[i].de = k;
        oft[i].flag = flags;
        oft[i].in = node;
        return i;
    }

  }


  return SYSERR;
}