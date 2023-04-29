#include <xinu.h>
#include <fs.h>

extern fsystem_t* fsd;
extern filetable_t oft[NUM_FD];


/*
 * fs_close - Return SYSERR if the file is already closed
 *     otherwise, write the file's inode back to the block
 *     device and set the entry in the oft to FSTATE_CLOSED
 */
syscall fs_close(int fd) {

  if (oft[fd].state == FSTATE_CLOSED){
    // printf("\nFile already closed");
    return SYSERR;
  } else{
    bs_write(fsd->root_dir.entry[oft[fd].de].inode_block, 0, &oft[fd].in, sizeof(inode_t));
    oft[fd].state = FSTATE_CLOSED;
  }        
  return OK;
}