#include <xinu.h>

#define TY_ESC   0x1b
#define TY_BRACE 0x5b
#define TY_A     0x41

#define SKIP     0
#define DO_PRINT 1

local void echo(char ch, struct ttycblk* typtr, struct uart_csreg* csrptr);
local void clearline(struct ttycblk* typtr, struct uart_csreg* csrptr);

/*
 * ttydiscipline - Handles special character interactions.
 * 
 * Returns - Whether to print character (SKIP or DO_PRINT)
 */
int ttydiscipline(char ch,
		  struct ttycblk *typtr,
		  struct uart_csreg* csrptr) {

  if (ch == TY_NEWLINE || ch == TY_RETURN) {
    /* 
     * Copy the contents of the 'tyibuff' buffer from the 'tyihead' through 'tyitail'
     *     into the 'typrev' buffer.
     */
    
    char *temp;
    temp = typtr->tyihead;

    char *prev_temp;
    prev_temp = &typtr->typrev[0];
    printf("address of prev is %u",prev_temp);

    while(temp != typtr->tyitail){
      *prev_temp = *temp;
      prev_temp++;
      temp++;
      // try : *typtr->typrev++ = *temp++;
      if (temp>&typtr->tyibuff[TY_IBUFLEN]) {
          temp = typtr->tyibuff;       
      }
    }
    *prev_temp = *temp;  // to store character of tyitail itself   
    return DO_PRINT;
  }

  /*
   * Check if the up key was pressed:
   * Use 'tycommand' to indicate if a command is in progress and the previous character
   *     If the characters appear in the sequence TY_ESC, then TY_BRACE, then TY_A
   *     the up key was sent
   */

  if(ch == TY_ESC){
    typtr->tycommand = ch;
    return SKIP;
  }else if(typtr->tycommand == TY_ESC && ch == TY_BRACE){
    typtr->tycommand = ch;
    return SKIP;
  }else if(typtr->tycommand == TY_BRACE && ch == TY_A){
  // If the up key is detected (as above)
        /*
        * Clear the current input with the 'clearline' function and copy the contents of 
        *     'typrev' buffer into the 'tyibuff' buffer
        *     remember to reset the 'tyicursor' as well
        *  Call 'echo' on each character to display it to the screen
        */

        clearline(typtr,csrptr);

        char *prev;
        prev = &typtr->typrev[0];

        while(*prev != 0){
          if (typtr->tyitail>=&typtr->tyibuff[TY_IBUFLEN]) {
             typtr->tyitail = typtr->tyibuff;
          }
          *typtr->tyitail = *prev;
          echo(*typtr->tyitail,typtr,csrptr);
          typtr->tyitail++;
          prev++;
        }
    return SKIP;    
  }  
  return DO_PRINT;
}

local void echo(char ch, struct ttycblk* typtr, struct uart_csreg* csrptr) {
  *typtr->tyetail++ = ch;

  /* Wrap around buffer, if needed */
  if (typtr->tyetail >= &typtr->tyebuff[TY_EBUFLEN])
    typtr->tyetail = typtr->tyebuff;
  ttykickout(csrptr);
  return;
}

local void clearline(struct ttycblk* typtr, struct uart_csreg* csrptr) {
  int i, j;
  while (typtr->tyitail != typtr->tyihead)
    if ((--typtr->tyitail) < typtr->tyibuff)
      typtr->tyitail += TY_IBUFLEN;

  for (i=0; i < typtr->tyicursor; i++) {
    echo(TY_BACKSP, typtr, csrptr);
    echo(TY_BLANK, typtr, csrptr);
    echo(TY_BACKSP, typtr, csrptr);
    for (j=0; j<3; j++)
      ttyhandle_out(typtr, csrptr);
  }
  typtr->tyicursor = 0;
}