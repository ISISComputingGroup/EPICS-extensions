//Galil Motion Control
//Copyright 2006
//PCI driver for DMC-18xx.  printk output shows up in /var/log/messages

#include <linux/fs.h>         //struct file, struct inode
#include <linux/pci.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>      //copy_to_user(), copy_from_user()
#include <linux/interrupt.h>  //request_irq()

#define GALILNAME "galilpci"  //name shows up in /dev and /sys/bus/pci/drivers
#define BUFFERSIZE 512        //size of command and response buffer
#define MAX_DEVICES 4         //max number of Galil PCI boards in a single PC
#define MAX_INTERRUPTS 33     //the maximum number of interrupt status bytes that this driver will queue + 1.  Old status bytes will be discarded if the buffer is full.

struct galil_private //contains date private to each Galil board installed in the PC
{
  struct miscdevice  galilpci_miscdev;   //contains the minor number
  unsigned long MainFIFOAddress;         //base address N from BAR2.  This is 16 bytes of I/O ports.
  void __iomem *DualPortRAMAddress;      //from BAR0.  This is 512 bytes of I/O memory.
  unsigned long DualPortPhysicalAddress; //not used except for display
  char buffer[BUFFERSIZE];  //kernel buffer for holding controller commands and responses

  unsigned int  interrupt; //irq number (e.g. 11).  This is stored only to call free_irq upon rmmod
  unsigned char IRQByte[MAX_INTERRUPTS]; //interrupt status bytes read from N+8 (e.g. f0 results from UI0)
  int in;  //index into IRQByte[] where the interrupt handler will place the status byte
  int out; //index into IRQByte[] where ioctl will remove the status byte and pass it to user space
  wait_queue_head_t InterruptWaitQ; //this is a list of processes waiting for an UI or EI interrupt from the controller
};

static struct galil_private  galilcard[MAX_DEVICES]; //array of galil board structure that hold the IO address
static int card = -1;        //total galil boards found minus 1 (matching PLX vendor and device IDs)
/*
void PrintStatusByteQueue(char * message, struct galil_private* gp) //print the state of the circular buffer
{
   int i;
   
   printk("%s in %i, out %i, ", message, gp->in, gp->out);
   for(i = 0; i < MAX_INTERRUPTS; i++)
      printk("%02x ", gp->IRQByte[i]);
   printk("\n");
}*/

//___________________________________________________________________________________________
//returns 1 if there are characters in the read FIFO

static inline int canRead(struct galil_private *gp) 
{ 
   //18x2
//  unsigned long addr = (gp->MainFIFOAddress) + 1; //control register
//  return (inb(addr) & 0x20) == 0x0; // Bit5 0-read 1-no read

   //18x0 and 18x6
  unsigned long addr = (gp->MainFIFOAddress) + 4; //control register
  return (inb(addr) & 0x04) == 0x0; // Bit2, 0-read, 1-no read
}

//___________________________________________________________________________________________
//returns 1 if the write FIFO isn't full and we can write characters

static inline int canWrite(struct galil_private *gp) 
{ 
   //18x2
//  unsigned long addr = (gp->MainFIFOAddress) + 1; //control register
//  return (inb(addr) & 0x10) == 0x0; // Bit4 0 - not full 1-full

   //18x0 and 18x6
  unsigned long addr = (gp->MainFIFOAddress) + 4; //control register
  return (inb(addr) & 0x01) == 0x0; // Bit0, 0 - not full, 1-full
}

//___________________________________________________________________________________________
// Purpose  :  Handle all ioctrl 
// Returns  :  EINVAL if invalid ioctl

#include "pciioctl.h" //for the constants GALIL_PCI_CLEARFIFO...

int galilpci_ioctl(struct inode*  inode, struct file*  filep, unsigned int cmd, unsigned long arg)
{
   struct galil_private *gp = filep->private_data;
   unsigned long CONTROL_REG = gp->MainFIFOAddress + 4; //control register
   //unsigned long bytesRead = 0;

   switch(cmd)
   {
      case GALIL_PCI_CLEARFIFO:  //return ClearFIFO( pDevice );                //returns 0
         outb(0x06, gp->MainFIFOAddress + 8); //clear both read (bit 1) and write (bit 2) FIFOs
         return 0;

      case GALIL_PCI_CHARAVAIL:  //return IsCharAvailable( pDevice );          //returns boolean 0 or 1
         return canRead(gp);

      case GALIL_PCI_GETFIFODR:  //return GetFIFODR( pDevice, (char*)arg );    //returns 0 or -1.  Returns data record via pointer argument
         //DUAL PORT RAM
         outb(0x10 | inb(CONTROL_REG), CONTROL_REG); //set freeze bit
         while(0x08 & inb(CONTROL_REG)) {} //wait if controller is busy updating 2nd FIFO

         memcpy_fromio(gp->buffer, gp->DualPortRAMAddress, DATA_RECORD_SIZE); //copy DATA_RECORD_SIZE bytes from the dual port RAM (up to 512)

         //for(bytesRead = 0; bytesRead < DATA_RECORD_SIZE; bytesRead++)
            //gp->buffer[bytesRead] = ioread8(gp->DualPortRAMAddress + bytesRead);  //read DPRAM and stuff the buffer until no more data

         if(copy_to_user((char*) arg, gp->buffer, DATA_RECORD_SIZE)) //get the data read in gp->buffer and copy to user space in arg
            {
               printk("galilpci_ioctl copy_to_user failed\n");
               return -EFAULT;
            }

         outb(~0x10 & inb(CONTROL_REG), CONTROL_REG);   //Clear freeze bit
         return 0;
/*
       //2ND FIFO
         if(!(0x80 & inb(CONTROL_REG))) //if 2nd FIFO has data (isn't empty)
         {
            outb(0x10 | inb(CONTROL_REG), CONTROL_REG); //set freeze bit

            if(!(0x08 & inb(CONTROL_REG))) //if controller isn't busy updating 2nd FIFO
            {
               while (!(0x80 & inb(CONTROL_REG)))  //while 2nd FIFO has data (isn't empty)
                 gp->buffer[bytesRead++] = inb(gp->MainFIFOAddress + 0xC);  //read address N+C and stuff the buffer until no more data

               if(copy_to_user((char*) arg, gp->buffer, bytesRead)) //get the data read in gp->buffer and copy to user space in arg
               {
                  printk("galilpci_ioctl copy_to_user failed\n");
                  return -EFAULT;
               }

               outb(~0x10 & inb(CONTROL_REG), CONTROL_REG);   //Clear freeze bit
               return 0; //success
            }
            else //controller is busy updating 2nd FIFO
            {
               outb(~0x10 & inb(CONTROL_REG), CONTROL_REG);   //Clear freeze bit
               return 2; //return to user space and let them try again
            }
         }
         else
            return 1; //2nd FIFO was empty.  Nothing to read.  Return to user space and let them try again
*/
      case GALIL_PCI_INTSTATUS:  //return GetIntStatus( pDevice, (int*)arg );  //returns 0.  Returns status byte via pointer argument
         if(0 == *((int*)arg)) //user passed 0.  Sleep until we get an interrupt
         {
            if(0 == wait_event_interruptible(gp->InterruptWaitQ, gp->in != gp->out)) //wait for the IRQ byte to be set by the IRQ handler (wait for the circular buffer to be non-empty)
            {
               *((int*)arg) = gp->IRQByte[gp->out]; //return the status byte retrieved in the interrupt handler
               gp->out = (gp->out + 1) % MAX_INTERRUPTS; //increment the read pointer
               //PrintStatusByteQueue("ioctl", gp);
               return 0;
            }
            else //wait_event was interrupted by a signal (e.g. ctrl-c)
               return -ERESTARTSYS;
         } 
         else //user passed in 1.  Clear the status byte queue
         {
            gp->out = gp->in; //setting the out pointer equal to the in pointer makes the queue empty
            return 0;
         }
            
//      case GALIL_PCI_VALIDATESN: //return ValidateSN( pDevice, (int*)arg );    //returns boolean 0 or 1.  Takes serial number as argument.
   }

  return -EINVAL; //invalid ioctl code (switch fell thru).  POSIX says we should return -ENOTTY
}

//___________________________________________________________________________________________
//called when the user process calls read().  Never fails.  Returns 0 if no data to read.
//Otherwise, returns number of bytes read. 

ssize_t galil_read(struct file *filep, char *buf, size_t count, loff_t *ppos) 
{
   int maxBytes = min((int) count, (int) BUFFERSIZE);  //read no more than 512 bytes (the size of gp->buffer)
   unsigned long bytesRead = 0;
   struct galil_private *gp = filep->private_data;

   if (canRead(gp)) 
   {
      while(canRead(gp) && bytesRead < maxBytes) 
         gp->buffer[bytesRead++] = inb(gp->MainFIFOAddress);	//read address N and stuff the buffer until no more data

      if(copy_to_user(buf, gp->buffer, bytesRead))	//get the data read in gp->buffer and copy to user space in buf
      {
         printk("galil_read copy_to_user failed\n");
         return -EFAULT;
      }
   }

//   printk("galil_read {bytesRead %lu} {count %u}\n", bytesRead, count);

   return bytesRead;
}

//___________________________________________________________________________________________
//called when the user process calls write().

ssize_t galil_write(struct file *filep, const char *buf, size_t count, loff_t *ppos) 
{
   int maxBytes = min((int) count, (int) BUFFERSIZE);  //write no more than 512 bytes (the size of gp->buffer)
   unsigned long bytesWritten = 0;
   struct galil_private *gp = filep->private_data;

   if(copy_from_user(gp->buffer, buf, maxBytes))	//get data from user space in buf (e.g. "TPX") and copy to kernel buffer (gp->buffer)
      {
         printk("galil_write copy_from_user failed\n");
         return -EFAULT;
      }

   while(canWrite(gp) && bytesWritten < maxBytes) 
   {
      outb(gp->buffer[bytesWritten], gp->MainFIFOAddress); //write to address N at BAR2
      bytesWritten++;
   }

//   printk("galil_write {bytesWritten %lu} {count %u}\n", bytesWritten, count);
/*  for(i = 0; i < bytesWritten; i++)
  	printk("%02x ", gp->buffer[i]);
  	
  printk("\n");
  */
   return bytesWritten;
}
//___________________________________________________________________________________________
//called when the open() system call is invoked from user space

int galil_open(struct inode *inode, struct file *filep) 
{
   int i = 0;

   printk("galil_open %i %i\n", imajor(inode), iminor(inode));
   
   //search for the minor number
   for(i = 0; i <= card; i++)
      if(galilcard[i].galilpci_miscdev.minor == iminor(inode))
         break; //found it

   filep->private_data = &galilcard[i];	//initialize the private data for this file descriptor.  This will be accessed in read() and write()

   return 0;
}
//___________________________________________________________________________________________
//called when the close() system call is invoked from user space

int galil_release(struct inode *inode, struct file *filep) 
{
  return 0;
}

//___________________________________________________________________________________________
//file operations structure

static struct file_operations  galilpci_fops = {
   .owner      = THIS_MODULE,
   .read       = galil_read,
   .write      = galil_write,
   .open       = galil_open,
   .release    = galil_release,
   .ioctl      = galilpci_ioctl, 
};

//___________________________________________________________________________________________
//the interrupt handler.  Runs after UI or EI commands are send to the controller.
//This function will run when any device sharing one of the Galil controller's IRQ lines fires.

int interrupts = 0;

static irqreturn_t galilpci_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
   struct galil_private *gp = dev_id; //get a pointer to the device.  This was passed to request_irq()

//   if(inb(gp->MainFIFOAddress + 4) & 0x20) //if the Galil controller generated an interrupt
   if(0x60 == (inb(gp->MainFIFOAddress + 4) & 0x60)) //if the Galil controller generated an interrupt and interrupts were enabled on the controller
   {
      interrupts++; //count the number of interrupts for debugging
      
      if(gp->out == ((gp->in + 1) % MAX_INTERRUPTS)) //check for full
         gp->out = (gp->out + 1) % MAX_INTERRUPTS; //toss out the oldest element

      gp->IRQByte[gp->in] = inb(gp->MainFIFOAddress + 8); //read the status byte and store it in the device structure
      //printk("galilpci_interrupt IRQ %i control register %x status %x\n", irq, inb(gp->MainFIFOAddress + 4), gp->IRQByte[gp->in]);
      gp->in = (gp->in + 1) % MAX_INTERRUPTS; //increment the write pointer
      //PrintStatusByteQueue("galilpci_interrupt", gp);
      
      outb(0x20 | inb(gp->MainFIFOAddress + 4), gp->MainFIFOAddress + 4); //acknowledge the interrupt (else the controller will keep interrupting)
      wake_up_interruptible(&(gp->InterruptWaitQ)); //wake up ioctl

      return IRQ_HANDLED;
   }
   else //another device sharing the IRQ generated the interrupt
      return IRQ_NONE;
}

//___________________________________________________________________________________________
//a misc devices has a major number of 10.  See http://www.linuxjournal.com/node/2920/print
/*
static struct miscdevice  galilpci_miscdev = {
        .minor  = MISC_DYNAMIC_MINOR, //assign the minor number dynamically (ask for a free one).  This field will eventually contain the actual minor number.
        .name   = GALILNAME,		  //the name for this device, meant for human consumption: users will find the name in the /proc/misc file.
        .fops   = &galilpci_fops,	  //the file operations
};
*/
//___________________________________________________________________________________________
//PCI device ID structure.  pcigalil_card_init will be called for each board that matches one the of the following
//sets of vendor ID, device ID, subsystem vendor ID, and subsystem device ID.  This is a filter.

static struct pci_device_id pcigalil_pci_tbl[] __devinitdata = {
   {.vendor = 0x10B5, .device = 0x9050, .subvendor = 0x1079, .subdevice = 0x1600},  //DMC-1600  could set .subdevice to PCI_ANY_ID
   {.vendor = 0x10B5, .device = 0x9050, .subvendor = 0x1079, .subdevice = 0x1800},  //DMC-1800
   {.vendor = 0x10B5, .device = 0x9050, .subvendor = 0x1079, .subdevice = 0x1806},  //DMC-18x6
//  { PCI_DEVICE(0x10B5, 0x9050) },  //PLX vendor ID, device ID.  Should also check for subvendor
  { 0 },
};

MODULE_DEVICE_TABLE(pci, pcigalil_pci_tbl); //let module loading system know what module works with what hardware devices

//___________________________________________________________________________________________
//PCI "probe" function called when the kernel finds a Galil board (matching pcigalil_pci_tbl).
//0 is normal return.  Called once per board.  If the function doesn't want to control the device
//or an error occurs, returns a negative value.  This function stores the minor number 
//in galilpci_miscdev.minor so that open() can later figure out which board is being opened,
//and set the correct filep->private_data (so that read() and write() know which ioaddr to access).

static int __devinit pcigalil_card_init(struct pci_dev  *pdev, const struct pci_device_id  *ent)
{
   int ret = -EIO;
   u16 uModel;    //will contain galil model number (e.g. 0x1802)
   char name[10]; //will contain name in /dev/ (e.g. galilpci0)
   card++;        //incremented each time be find a galil board
   
   if (card >= MAX_DEVICES) {
      printk("pcigalil_card_init This driver only supports %i devices\n", MAX_DEVICES);
      card--; //update the card count
      return -ENODEV;
   }
   
   if (pci_enable_device(pdev)) {  //wake up the device
      printk("pcigalil_card_init Not possible to enable PCI Device\n");
      card--; //update the card count
      return -ENODEV;
   }

   //Mark all PCI regions associated with PCI device pdev as being reserved by owner res_name.
   //Do not access any address inside the PCI regions unless this call returns successfully.
   //This will reverve both i/o ports and memory regions, and shows up in /proc/ioports and /proc/iomem
   if (pci_request_regions(pdev, GALILNAME)) {
      printk("pcigalil_card_init I/O address (0x%04x) already in use\n", (unsigned int)galilcard[card].MainFIFOAddress);
      //ret = -EIO;
      goto err_out_disable_device;
   }

   //I/O ADDRESSES
   galilcard[card].MainFIFOAddress         = pci_resource_start(pdev, /*BAR*/ 2); //get base address N from BAR2
   galilcard[card].DualPortPhysicalAddress = pci_resource_start(pdev, /*BAR*/ 0); //get dual port RAM address from BAR0 (unused except for printing below)
//   galilcard[card].DualPortRAMAddress      = ioremap(galilcard[card].DualPortPhysicalAddress, 512); //get virtual address
   galilcard[card].DualPortRAMAddress = pci_iomap(pdev, /*BAR*/ 0, 512 /*bytes*/); //performs ioremap(pci_resource_start(pdev, 0), 512);

   if (galilcard[card].MainFIFOAddress == 0x0000) { //check base address.
      printk("pcigalil_card_init No Main I/O-Address for card detected\n");
      ret = -ENODEV;
      goto err_out_disable_device;
   }

   //INTERRUPTS
   galilcard[card].in = galilcard[card].out = 0; //initialize the irq byte circular buffer pointers
   galilcard[card].interrupt = pdev->irq; //store the IRQ number so we can call free-irq in the cleanup function when the module us removed with rmmod
   if (request_irq(galilcard[card].interrupt, galilpci_interrupt, SA_SHIRQ, GALILNAME, &galilcard[card])) //register the interrupt handler.  This should happen before we enable interrupts on the controller.
   {
      printk( KERN_ERR "IRQ %x is not free\n", galilcard[card].interrupt);
      goto err_out_irq;
   }
   outb(0x40 | inb(galilcard[card].MainFIFOAddress + 4), galilcard[card].MainFIFOAddress + 4); //enable interrupts on the controller.  They will stay enabled until the module is unloaded (or power is shut off)
   init_waitqueue_head(&galilcard[card].InterruptWaitQ); //initialize the list of processes waiting for the interrupt

  //register the device under major number 10.  Minor number will show up in /proc/misc
   galilcard[card].galilpci_miscdev.minor  = MISC_DYNAMIC_MINOR, //assign the minor number dynamically (ask for a free one).  This field will eventually contain the actual minor number.
   sprintf(name, GALILNAME "%i", card);
   galilcard[card].galilpci_miscdev.name   = name,               //the name for this device, meant for human consumption: users will find the name in the /proc/misc file.
   galilcard[card].galilpci_miscdev.fops   = &galilpci_fops,     //the file operations

   ret = misc_register (&(galilcard[card].galilpci_miscdev));
   if (ret) {
      printk ("pcigalil_card_init cannot register miscdev (err=%d)\n", ret);
      goto err_out_misc;
   }

   pci_read_config_word(pdev, PCI_SUBSYSTEM_ID, &uModel);    //read galil model (e.g. 0x1806)
   printk("pcigalil_card_init I/O address (0x%lx), DPRAM Physical Address (0x%lx), DPRAM Virtual Address (0x%p), IRQ %u, Model %x, minor number %d\n",
      galilcard[card].MainFIFOAddress, galilcard[card].DualPortPhysicalAddress, galilcard[card].DualPortRAMAddress, galilcard[card].interrupt, uModel, galilcard[card].galilpci_miscdev.minor);

   return 0;

   err_out_misc:
      //misc_deregister(&(galilcard[card].galilpci_miscdev));
      free_irq(galilcard[card].interrupt, &galilcard[card]);
   err_out_irq:
      pci_release_regions(pdev);
   err_out_disable_device:
      pci_disable_device(pdev);

   card--; //update the card count

   return ret;
}

//___________________________________________________________________________________________
//Called once per board upon rmmod

static void __devexit pcigalil_card_exit(struct pci_dev *pdev)
{
   printk("pcigalil_card_exit\n");

   misc_deregister(&(galilcard[card].galilpci_miscdev));  //unregister the device with major number 10

   outb(~0x40 & inb(galilcard[card].MainFIFOAddress + 4), galilcard[card].MainFIFOAddress + 4); //disable interrupts on the controller.  They will stay disabled until the module is loaded again.
   free_irq(galilcard[card].interrupt, &galilcard[card]); //should happen after interrupts are disabled on the controller

   //iounmap(galilcard[card].DualPortRAMAddress); //release the virtual address
   //pci_iounmap(pdev, galilcard[card].DualPortRAMAddress); 
   pci_release_regions(pdev);				 //relase the I/O ports and memory regions
   pci_disable_device(pdev);
   card--;
}
//___________________________________________________________________________________________
//PCI driver structure

static struct pci_driver   pcigalil_driver = 
{
  .name     = GALILNAME,                       //name of driver (must be unique among all PCI drivers).  Shows up under /sys/bus/pci/drivers
  .id_table = pcigalil_pci_tbl,				     //list of PCI IDs this driver supports
  .probe    = pcigalil_card_init,				  //called when a board is found
  .remove   = __devexit_p(pcigalil_card_exit), //called on exit
};

//___________________________________________________________________________________________
//called when module is inserted into the kernel (insmod)

static int __init galil_init(void)
{
   printk("galil_init\n");
   return pci_register_driver(&pcigalil_driver); //calls pcigalil_card_init.  pci_module_init() is obselete
}

//___________________________________________________________________________________________
//called when module is removed from kernel (rmmod)
static void __exit galil_exit(void)
{
   printk("galil_exit, interrupts %i\n", interrupts);
   pci_unregister_driver(&pcigalil_driver); //calls pcigalil_card_exit
}
//___________________________________________________________________________________________

module_init(galil_init);
module_exit(galil_exit);

