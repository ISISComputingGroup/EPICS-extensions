/*Driver global functions.. services to device support*/

double galil_conv_str_val(char *buffer);     							/*removes space in front of string retrned by Galil, and convert str to double*/
long galil_send_cmd(char callingfunction[], struct G21X3controller *control, char command[], char resp[]);		/*sends commands to controller*/
double powerof(int base, int power);								/*returns base to the power of number*/
double set_ssi_connectflag(struct G21X3axis *cntrl);						/*sets ssi connect flag, based on ssi encoder details*/
int set_uenc_reading_valid_flag(struct G21X3controller *control,char axis_name,int test_slaves);               /*determine if user encoder reading is valid, set uenc state to off if reading is invalid.*/
int GalilSanityCheck(unsigned short card,unsigned short signal,char parm[]);			/*parses user input card, signal and parm, checks controller connection status*/
int GalilReadReal(unsigned short card, unsigned short signal, char parm[],double *value); 	/*Reads a double type value from the controller corresponding to parm*/
int GalilWriteReal(unsigned short card, unsigned short signal, char parm[], double value);	/*Writes a double type value from the controller corresponding to parm*/
