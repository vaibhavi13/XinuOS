#include <xinu.h>
#include <fs.h>

extern fsystem_t* fsd;
extern filetable_t oft[NUM_FD];

/*
 * Copy the data from the block device into the 'buff' argument
 *    Use the fileptr field of the oft to track the location of the next read/write
 *    operation.
 *    - Determine the current block and offset from the fileptr
 *    - Copy bytes into the 'buff' until you have exhausted the block or copied 'len' bytes
 *    - Repeat until 'len' bytes are copied
 *    - Update the fileptr field
 *    - Return the number of bytes read
 */
int fs_read(int fd, char* buff, int len) {

  int bytes_to_read = len; 
  int bytes_read = 0;
  int j = 0;
  int size;
  oft[fd].fileptr = 0;
  while(bytes_read < len){ 
      size = bytes_to_read > MDEV_BLOCK_SIZE ? MDEV_BLOCK_SIZE : bytes_to_read;     
      if(oft[fd].in.blocks[j] != 0){
       // printf("\ninside if of read");
        bs_read(oft[fd].in.blocks[j], oft[fd].fileptr, buff, size);
       // printf("\n oft fileptr : %d",oft[fd].fileptr);
        bytes_read += size;
        buff += size;
        bytes_to_read -= bytes_read;
        oft[fd].fileptr += size;
        //printf("\n oft fileptr : %d",oft[fd].fileptr);
        j++;
      } 
  }

  return bytes_read;
}