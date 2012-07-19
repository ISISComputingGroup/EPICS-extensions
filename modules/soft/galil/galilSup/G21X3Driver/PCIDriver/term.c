/*simple Galil terminal for DMC-18xx
[root@localhost PCIDriver]# gcc term.c -o term
[root@localhost PCIDriver]# ./term
Ctrl-C exits.  Connected to  1.0000
:DMC1886 Rev 1.0
:
[root@localhost PCIDriver]#
*/
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>		// open 
#include <termios.h>
#include <string.h> //for strcat()

#define STDIN  0  //file descriptor for standard input (keyboard).   Passed to read() and write().
#define STDOUT 1  //file descriptor for standard output (screen).    Passed to read() and write().
char GALIL_FILE[100] = "/dev/galilpci\0"; //default device file is /dev/galilpci0.  The null character will be overwritten.

int main(int argc, char *argv[]) //./a.out X connects to /dev/galilpciX.  No arg connects to /dev/galilpci0
{
   char acBuffer[80] = {0}; //holds characters read from controller, and characters to write to controller
   int iBytesRead = 0; 
   static struct termios termattr, save_termattr; //structures that hold terminal settings
   int file_desc = open((argc == 2 ? strcat(GALIL_FILE, argv[1]) : strcat(GALIL_FILE, "0")), O_RDWR);  //open the Galil PCI driver for read and write
   
   if (file_desc < 0) 
   {
      printf("Can't open device file %s\n", GALIL_FILE);
      exit(-1);
   }

   //put terminal in raw mode, so all characters (including control characters) can be read)
   //see man page for termios and stty.  Also http://www.pwilson.net/kbhit.html
   tcgetattr (STDIN, &termattr); //get the current terminal addributes
   save_termattr = termattr;     //save these attributes for restoring later

   //local settings:  don't echo input characters, disable erase ^?, kill ^U, werase ^W, and rprnt ^R special characters
   //disable interrupt ^C, quit ^\, and suspend ^Z special characters
   termattr.c_lflag &= ~(ECHO | ICANON | ISIG); 
   termattr.c_iflag &= ~(ICRNL | IXON); //input settings:  don't translate carriage return to newline, disable XON ^Q XOFF ^S flow control
   
   //The two settings below cause read(STDIN) to not block.  See http://www.unixwiz.net/techtips/termios-vmin-vtime.html
   termattr.c_cc[VMIN] = 0;  //Minimum amount of characters to satisfy read().  Let read return if no characters.
   termattr.c_cc[VTIME] = 0; //Intercharacter timeout.  Don't wait for characters coming from stdin.
   tcsetattr (STDIN, TCSANOW, &termattr); //make the new settings active NOW

   write(STDOUT, "Ctrl-C exits.  Connected to ", 28); 
   write(file_desc, "\x12\x16\r", 3); //write ^R^V to get e.g. DMC1886 rev 1.0

   while(1) //ctrl-C will exit the infinite loop 
   {
      //FROM CONTROLLER
      iBytesRead = read(file_desc, acBuffer, sizeof(acBuffer)); //read some characters from the controller.  Returns 0 if none.
      if(iBytesRead)
      {
         write(STDOUT, acBuffer, iBytesRead); //write to standard out
         //for(i = 0; i < iBytesRead; i++) printf("%02x ", acBuffer[i]); //print characters read from the controller
      }

      //FROM KEYBOARD
      if(read(STDIN, acBuffer, 1)) //read a character from standard input.  Returns 0 if no characters.
      {
         //printf("%02x\n", acBuffer[0]); //print the character typed
  
         if(0x1b == acBuffer[0]) //filter out escape sequences (such as arrow keys, function keys, alt)
            while(read(STDIN, acBuffer, 1)) {} //read all characters following escape
         else
         {
            if (0x7f == acBuffer[0])      //backspace results in DEL
            {
               acBuffer[0] = 8; //change to backspace
               write(STDOUT, "\x08 ", 2); //backspace, space, and backspace will be sent again below
            }
            
            if (0x03 == acBuffer[0])      //control-C exits program
            {
               write(STDOUT, "\n", 1);  //write new line so unix prompt is left justified
               tcsetattr (STDIN, TCSAFLUSH, &save_termattr); //restore original terminal attributes
               exit(0);          // and get out 
            }

            //change upper case to lower and vica versa
            if('A' <= acBuffer[0] && acBuffer[0] <= 'Z') //upper case
               acBuffer[0] += 32; //change to lower case
            else if('a' <= acBuffer[0] && acBuffer[0] <= 'z') //lower case
               acBuffer[0] -= 32; //change to upper case

            write(file_desc, acBuffer, 1); //write the character to the controller
           
            write(STDOUT, "\033[32m", 5); //turn on green text.  See http://kb.iu.edu/data/adzf.html
            write(STDOUT, acBuffer, 1);   //write character to standard output
            write(STDOUT, "\033[0m", 4);  //return to normal black text
            
            if('\r' == acBuffer[0])    //user hit Enter key.
               write(STDOUT, "\n", 1); //write newline and carraige return to standard out (else display is incorrect)
         }  
      }
   }
}
