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
  int bytes_copied;
  int free_block;
  int i;
  int j = 0;

  if(fd < 0  || fd >= NUM_FD || oft[fd].state != FSTATE_OPEN || len < 0){
    return SYSERR;
  }
  
  int total = 0;

  int write_start_block = oft[fd].fileptr/fsd->device.blocksz;

  int write_head = oft[fd].fileptr%fsd->device.blocksz;

  int curr_block_data = fsd->device.blocksz-(write_head % fsd->device.blocksz ) ;

  
  while(len > 0){
    bytes_copied = 0;
    if(len < curr_block_data){
      bytes_copied = len;
    }else{
      bytes_copied = curr_block_data;
    }

    for(i = 2 ; i < MDEV_NUM_BLOCKS ; i++){
        if(fs_getmaskbit(i) == 0){
          free_block = i;
          break;
        }
    }

    fs_setmaskbit(free_block);

    oft[fd].in.blocks[write_start_block] = free_block;

    bs_write(free_block, write_head, buff, bytes_copied);

    buff += bytes_copied;
    write_start_block += bytes_copied;
    write_start_block %= fsd->device.blocksz;
    
    if(curr_block_data == 0){
      curr_block_data = write_start_block++;
      curr_block_data = fsd->device.blocksz;
    }

    
    len -= bytes_copied;
    total += bytes_copied;

  }
  
  oft[fd].fileptr += total;
  if(oft[fd].in.size < oft[fd].fileptr){
    oft[fd].in.size  = oft[fd].fileptr;
    // bs_write(fsd->root_dir.entry[oft[fd].de].inode_block, 0, &oft[fd].in, sizeof(inode_t));
  }

  // while(bytes_copied < len){
           
  //   int size = bytes_to_write > MDEV_BLOCK_SIZE ? MDEV_BLOCK_SIZE : bytes_to_write;

  //   if(bytes_copied < MDEV_BLOCK_SIZE){
  //     free_block = fsd->root_dir.entry[oft[fd].de].inode_block;
  //   }else{
  //     for(i = 2 ; i < MDEV_NUM_BLOCKS ; i++){
  //       if(fs_getmaskbit(i) == 0){
  //         free_block = i;
  //         break;
  //       }
  //     }
  //   }  
  //   bs_write(free_block, oft[fd].fileptr, buff, size);
  //   fs_setmaskbit(free_block);
  //   oft[fd].in.blocks[j] = free_block;
  //   oft[fd].in.size += size;
  //   bytes_copied += size; 
  //   buff += size; 
  //   bytes_to_write -= bytes_copied; 
  //   oft[fd].fileptr += size;
  //   j++;
  // }
  // // printf("\n size of oft %d",oft[fd].in.size);
  // // printf("\n returning bytes_copied : %d",bytes_copied);

  // char block[20];
  // bs_read(oft[fd].in.blocks[0], 0, &block, 20);
  // // printf("\nstring writren in block is %s",block);
  // // printf("\n given buff content is %s",buff);

  return total;
}