#include <xinu.h>
#include <fs.h>

extern fsystem_t* fsd;
extern filetable_t oft[NUM_FD];

/*
 * Copy 'len' bytes from 'buff' into the blocks associated with the file 'fd'
 *   - Until 'len' bytes are copied...
 *   - Check if a new block is needed (find free block and add to the file's blocks if necessary)
 *   - Write bytes from the 'buff' into the block starting at the fileptr
 *   - Write 'len' bytes or fill the block, whichever is less
 *   - Repeat until write is complete
 *   - Update inode state to reflect changes
 *   - Return the number of bytes written
 */
int fs_write(int fd, char* buff, int len) {
 
  int bytes_to_write = len; 
  int bytes_copied = 0;
  int free_block;
  int i;
  int j = 0;
  while(bytes_copied < len){
           
    int size = bytes_to_write > MDEV_BLOCK_SIZE ? MDEV_BLOCK_SIZE : bytes_to_write;

    if(bytes_copied < MDEV_BLOCK_SIZE){
      free_block = fsd->root_dir.entry[oft[fd].de].inode_block;
    }else{
      for(i = 2 ; i < MDEV_NUM_BLOCKS ; i++){
        if(fs_getmaskbit(i) == 0){
          free_block = i;
          break;
        }
      }
    }  
    bs_write(free_block, oft[fd].fileptr, buff, size);
    fs_setmaskbit(free_block);
    oft[fd].in.blocks[j] = free_block;
    oft[fd].in.size += size;
    bytes_copied += size; 
    buff += size; 
    bytes_to_write -= bytes_copied; 
    oft[fd].fileptr += size;
    j++;
  }
  // printf("\n size of oft %d",oft[fd].in.size);
  // printf("\n returning bytes_copied : %d",bytes_copied);

  char block[20];
  bs_read(oft[fd].in.blocks[0], 0, &block, 20);
  // printf("\nstring writren in block is %s",block);
  // printf("\n given buff content is %s",buff);

  return bytes_copied;
}