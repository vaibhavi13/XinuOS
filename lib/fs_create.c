#include <xinu.h>
#include <fs.h>

extern fsystem_t* fsd;

/*
 * fs_create - Return SYSERR if not enough space is available
 *     in the directory or the filename already exists.
 *     Otherwise:
 *        1. Find an available block on the block store
 *        2. Create an inode_t for the new file
 *        3. Update the root directory
 *        4. Write the inode and free bitmask back to the block device
 */
syscall fs_create(char* filename) {
  

  intmask mask;
  mask = disable();

  int i;
  int next_free_block_index;
  for(i = 2 ; i < MDEV_NUM_BLOCKS ; i++){
    // to check if free size is available
    if(fs_getmaskbit(i) == 0){
      next_free_block_index = i;
      break;
    }
  }
  if(i == MDEV_NUM_BLOCKS){
    return SYSERR;
  }
  
  // to check if filename already exists 
  
  for (i=0; i<fsd->root_dir.numentries; i++) {
     if(strcmp(filename,fsd->root_dir.entry[i].name) == 0){
        printf("\nFile already exists");
        return SYSERR;
     }
  }  

  // update root directory
  fsd->root_dir.entry[fsd->root_dir.numentries].inode_block = next_free_block_index;
  strcpy(fsd->root_dir.entry[fsd->root_dir.numentries].name, filename);
  
  inode_t inode;
  inode.id = next_free_block_index;
  inode.size = 0;

  bs_write(next_free_block_index,0,&inode,sizeof(inode_t));
  fs_setmaskbit(next_free_block_index);

  fsd->root_dir.numentries++;
  restore(mask);
  return OK;
}