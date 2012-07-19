/*
 *      drvG21X3.cc
 *      Motor record driver level support for Galil G21X3 motor controller.
 *
 *      Original Author: Mark Clift
 *      Date: 26-Sept-2005
 *
 * Modification Log:
 * -----------------
 *
 * Version 1-2 (latest 1-2 version only, post 20/12/07)
   Debugged position maintenance for Master/Slave applications (gantry mode)
   Added ssi connect flag
   Added encoder stall flag
   Version 1-3 (13/02/08)
   Broke code into smaller functions.
   Added G21X3AddThreadCode IocShell function
   Added G21X3StartCard IocShell function
   13/02/08 - Changed G21X3StartCard command.. replaced force_write with eeprom_write variable
   22/02/08 - changed ssidatabits to ssisingleturnbits
   	      Added ability to invert SSI encoders.
   18/04/08 - Made position maintenance min error define encoder deadbands for motor stop status declaration.
   24/04/08 - Added ability to specify encoder location for position maintenance.  For unusual encoder types.
   16/07/08 - Removed G21X3AddThreadCode IocShell function, G21X3StartCard enhanced to except user code
   Version 1-4 (27/06/08)
   30/11/11 - Always create G21X3Lock epics mutex even if DMCOpen fails.
 */

#include        <stdio.h>
#include        <string.h>
#include 	<ctype.h>
#include        <epicsThread.h>
#include        <epicsMutex.h>
#include        <epicsExport.h>
#include        <drvSup.h>
#include        "dmclnx.h"
#include        "motordrvCom.h"
/* Local data required for every motorRecord driver; see "motordrvComCode.h" */
#include        "motordrvComCode.h"
#include        "drvG21X3.h"
#include 	"GalilInterface.h"

#define STATIC static

/*----------------debugging-----------------*/
#ifdef __GNUG__
    #ifdef	DEBUG
	volatile int drvG21X3Debug = 0;
	#define Debug(L, FMT, V...) { if(L <= drvG21X3Debug) \
                        { printf("%s(%d):",__FILE__,__LINE__); \
                          printf(FMT,##V); } }
	epicsExportAddress(int, drvG21X3Debug);
    #else
	#define Debug(l, f, args...)
    #endif
#else
    #define Debug()
#endif

/* --- Local data. --- */
int G21X3_num_cards = 0;
int G21X3_cards_started = 0;
static int *firstNameConfig;
static int *firstdigitalinput;
static char *card_thread_code;
static char *card_limit_code;
static char *card_digital_code;
static char *card_code;

/*----------------functions-----------------*/
STATIC int recv_mess(int, char *, int);
STATIC RTN_STATUS send_mess(int, const char *, char *);
STATIC void start_status(int card);
/*STATIC void G21X3Status(int card);*/
STATIC int set_status(int card, int signal);
static long report(int level);
static long init();
STATIC int motor_init();
STATIC void query_done(int, int, struct mess_node *);
STATIC void readG21X3Status(int card,int axis);
/*----------------functions-----------------*/

int galil_parse_userinput(struct G21X3controller *control,
			  struct G21X3axis         *cntrl,
			  int axis, 
			  unsigned *num_axis,
			  int gantrymode,
			  int limit_as_home,
			  char *stepper_pos_maintenance,
			  char *slavedata,
			  int axis_slavehomeallowed[],
			  int motortype,
			  int *ssiencoderinput,
			  int auxencoder,
			  int mainencoder,
			  int digitalhome,
			  int *digitalaway);
		      
void galil_store_master(struct G21X3axis *cntrl,
			double egu_after_limit,
			int motortype,
			double homevalue,
			int homeprogreg,
			int invert_coordinates,
			int ssitotalbits,
			int ssierrbits,
			int ssidataform);

void galil_store_motors_enable(struct G21X3controller *control,
			       struct G21X3axis *cntrl,
			       char *digitalhome_string,
			       int digitalaway);
			
void galil_store_slave(int i,
		       struct G21X3controller *control,
		       struct G21X3axis *cntrl,
		       struct G21X3axis *cntrl_slave,
		       int axis_slavehomeallowed[]);
			       
void galil_setup_motor_encoder(int i,
			       struct G21X3controller *control, 
			       struct G21X3axis *cntrl,
			       int ssiencoderinput,
			       int ssitotalbits,
			       int ssisingleturnbits,
			       int ssierrbits,
			       int ssitimecode,
			       int ssidataform);
			       
void galil_initialize_codegen(int i,
			      struct G21X3controller *control, 
			      struct G21X3axis *cntrl,
			      int digitalhome,
			      int digitalaway,
			      char *c,
			      char axis_thread_code[],
			      char axis_limit_code[],
			      char axis_dighome_code[]);
			         
void galil_gen_limitcode(int i,
			 char c,
			 struct G21X3controller *control, 
			 struct G21X3axis *cntrl,
			 char axis_thread_code[],
			 char axis_limit_code[]);
			 
void galil_gen_commoncode(int i,
			  char c,
			  struct G21X3controller *control,
			  struct G21X3axis *cntrl,
			  char axis_thread_code[]);
			 
void galil_gen_slavecode(int i,
			 char c,
			 struct G21X3controller *control, 
			 struct G21X3axis *cntrl,
			 int slavecmd,
			 char axis_thread_code[]);
			 
void galil_gen_pos_maintenance_code(int i,
				    char c,
				    struct G21X3controller *control,
				    struct G21X3axis *cntrl,
				    struct G21X3axis *cntrl_slave,
				    char axis_thread_code[]);

void galil_gen_digitalcode(int i,
			   char c,
			   struct G21X3controller *control,
			   struct G21X3axis *cntrl,
			   int digitalhome,
			   int digitalaway,
			   char axis_dighome_code[]);
			   
void galil_gen_homecode(int i,
			char c,
			struct G21X3controller *control,
			struct G21X3axis *cntrl,
			struct G21X3axis *cntrl_slave,
			char axis_thread_code[]);
			
void galil_gen_card_codeend(int card);

void galil_gen_motor_enables_code(struct G21X3controller *control);

void process_user_encoder_faults(struct G21X3controller *control,
				 struct G21X3axis *cntrl);

void galil_write_gen_codefile(int card);
void galil_read_codefile(int card, char *code_file);
void read_user_code(FILE *fp, char *user_code);

struct driver_table G21X3_access =
{
    motor_init,
    motor_send,
    motor_free,
    motor_card_info,
    motor_axis_info,
    &mess_queue,
    &queue_lock,
    &free_list,
    &freelist_lock,
    &motor_sem,
    &motor_state,
    &total_cards,
    &any_motor_in_motion,
    send_mess,
    recv_mess,
    set_status,
    query_done,
    start_status,
    &initialized,
    NULL
};


struct
{
    long number;
#ifdef __cplusplus
    long (*report) (int);
    long (*init) (void);
#else
    DRVSUPFUN report;
    DRVSUPFUN init;
#endif
} drvG21X3 = {2, report, init};

epicsExportAddress(drvet, drvG21X3);


STATIC struct thread_args targs = {SCAN_RATE, &G21X3_access};

/*********************************************************
 * Print out driver status report
 *********************************************************/

static long report(int level)   /*##########  G21X3 Version */
{
int card;
if (G21X3_num_cards <=0)
	errlogPrintf("    No G21X3 controllers configured.\n");
else 
	{
        for (card = 0; card < G21X3_num_cards; card++) 
		{
		struct controller *brdptr = motor_state[card];
		if (brdptr == NULL)
			errlogPrintf("    G21X3 controller %d connection failed.\n", card);
            	else 
			{
			struct G21X3controller *control;
			control = (struct G21X3controller *) brdptr->DevicePrivate;
			errlogPrintf("    G21X3 driver %d, axis:%s \n", card, brdptr->ident);
            		}
        	}
    	}
return (0);
}


static long init()
{
if (G21X3_num_cards <= 0)
	{
	Debug(1, "init(): G21X3 driver disabled. G21X3etup() missing from startup script.\n");
    	}
return ((long) 0);
}

STATIC void query_done(int card, int axis, struct mess_node *nodeptr)
{
}

/*********************************************************
 * Read the status and position of all motors on a card
 * start_status(int card)
 *            if card == -1 then start all cards
 *********************************************************/

STATIC void start_status(int card)
{
struct G21X3controller *control;
struct G21X3axis *cntrl;
int i;

if (card>=0)
	{
	control = (struct G21X3controller *) motor_state[card]->DevicePrivate;
	
	//scan through axis on this card, update any marked with status_update
	for (i=0; i<motor_state[card]->total_axis;i++)
		{
		cntrl = (struct G21X3axis *)&control->axis[i];
		if (strcmp(cntrl->axis_list,"")!=0)
			if (cntrl->status_update == 1 && cntrl->status_read != 1)
				{
				readG21X3Status(card,cntrl->axis_list[0]-65);
				cntrl->status_read = 1;
				}
		}
	}
    else 
        {   /*start all cards*/
        for (i = 0; i < total_cards; i++)
		if (motor_state[i])
			readG21X3Status(i,-1);
        }
}

/*****************************************************************
 * Read in the motion parameters such as velocity, Accel, Position etc
 *****************************************************************/

STATIC void readG21X3Status(int card, int axis_num)
{
    struct G21X3controller *control =
                (struct G21X3controller *) motor_state[card]->DevicePrivate;
    struct G21X3axis *cntrl;
    int G21X3_num_axes;
    int  i,c;
    char galil_cmd_str[G21X3_LINE_LENGTH]="\0";          /*holds the assembled Galil cmd string*/
    char buffer[G21X3_LINE_LENGTH]="\0";		  /*Holds the Galil response string*/
    int num_format1queries=2;
    char galil_cmds[G21X3_NUM_CMDS][3]={"DE","DP","TS","TV"};
    
    int loop_start,loop_end;

    struct mess_node *nodeptr; /* These are to print the DONE flag */
    register struct mess_info *motor_info;
    msta_field statusflags;
    
    G21X3_num_axes = motor_state[card]->total_axis;
    Debug(2, "G21X3:readG21X3Status card=%d num_axes=%d\n",card,G21X3_num_axes);
    
    if (axis_num != -1)
    	{
	loop_start = axis_num; 
	loop_end = axis_num + 1;
	}
    else
    	{
    	loop_start = 0;
	loop_end = G21X3_num_axes;
	}

    /* Take a lock so that only 1 thread can be talking to the Galil G21X3
    */
    if (epicsMutexLock(control->G21X3Lock)==epicsMutexLockOK)
    	{
	for (i=loop_start; i<loop_end;i++)
		{
		motor_info = &(motor_state[card]->motor_info[i]); /*To print DONE flag*/
		nodeptr = motor_info->motor_motion;
		statusflags.All = motor_info->status.All;
	
		Debug(9, "G21X3:readG21X3Status RA_DONE=%d, RA_MOVING=%d, "\
			"RA_PROBLEM=%d\n",\
		statusflags.Bits.RA_DONE, statusflags.Bits.RA_MOVING,\
		statusflags.Bits.RA_PROBLEM);
	
		control = (struct G21X3controller *) motor_state[card]->DevicePrivate;
		cntrl = (struct G21X3axis *)&control->axis[i];
		
		if (strcmp(cntrl->axis_list,"")!=0)				/*has this motor been mentioned in a Nameconfig (st.cmd), ie, is it setup ?*/
			{
			Debug(2, "G21X3:readG21X3Status card=%d axis=%d sock=%d gp=%s\n",card,i,\
			cntrl->socket,cntrl->groupname);
			/*poll for data*/
			for (c=0;c<G21X3_NUM_CMDS;c++)
				{
				if ((c==1 && (cntrl->encoder_valid==1 || cntrl->encoder_valid==2)) || c==0 || c==2)  /*mr does not implement velocity yet, so remove with simple if statement to reduce cpu load*/
					{				        /*dont read encoder unless needed, to reduce CPU load*/
					strcpy(galil_cmd_str,"");
					cntrl->status = OK;
					if (c<num_format1queries)
						sprintf(galil_cmd_str,"%s%c=?",galil_cmds[c],cntrl->axis_list[0]);
					else
						sprintf(galil_cmd_str,"%s%c",galil_cmds[c],cntrl->axis_list[0]);
					
					if (galil_send_cmd("readG21X3Status",control,galil_cmd_str,buffer)==DMCNOERROR)
						{
						switch (c)
							{
						case 0: cntrl->motor_posi=(double)galil_conv_str_val(buffer); /*This is the aux encoder*/
							break;
						case 1: cntrl->encoder_posi=(double)galil_conv_str_val(buffer); /*This is the main encoder*/
							break;
						case 2: cntrl->switches=(unsigned)galil_conv_str_val(buffer);
							break;
						case 3: cntrl->velocity=(epicsInt32)galil_conv_str_val(buffer);  /*in encoder counts*/
							break;
						default: break;
							}
						}
					else
						cntrl->status = G21X3_COMM_ERR;
					}
				if (c==1 && cntrl->encoder_valid==0)
					cntrl->encoder_posi = 0;
				
				//process user encoder, if user encoder is set up correctly
				//no need to test all slaves, as a slave user encoder fault
				//will cause the master motor user encoder to go off (& stop) also, refer 
				//encoder_statechg_parse (MOVING section) in devG21x3.cc
				if (cntrl->uenc_settings_valid == 1)
					set_uenc_reading_valid_flag(control,cntrl->axis_list[0],0);
					
				//Update position value to the latest read from controller
				//invert motor/encoder values as instructed
				//done here instead of set_status, to ensure encoder, motor values are properly inverted
				//for all axis read by the readstatus function.  This is done as readstatus may read
				//multiple axis, and set_status may not be called for all axis read by readstatus,
				//immediately after the read status call.  In fact other functions may get called such
				//as build_trans (eg. STUP command, and start_status start all cards).
				
				if (cntrl->invert_coordinates & INVERT_MOTOR)
					cntrl->motor_posi=(epicsInt32)cntrl->motor_posi*-1;	       /*invert step count/aux encoder position*/
					
				if ((cntrl->invert_coordinates & INVERT_ENCODER) && (cntrl->encoder_valid!=2))
					cntrl->encoder_posi=(epicsInt32)cntrl->encoder_posi*-1;        /*invert quadrature encoder position*/
					
				if ((cntrl->invert_coordinates & INVERT_ENCODER) && (cntrl->encoder_valid==2))   /*invert ssi encoder*/
					cntrl->encoder_posi = (powerof(2,(cntrl->ssitotalbits - cntrl->ssierrbits)) - 1) - cntrl->encoder_posi;
				}
			}
		}
	/* Free the lock */
    	epicsMutexUnlock(control->G21X3Lock);
	}
}

/**************************************************************
 * Parse status and position as read by readG21X3status for a card and signal (axis)
 * set_status()  One card and signal combo, is one axis.
 ************************************************************/

STATIC int set_status(int card, int signal)
{
struct G21X3controller *control;
struct G21X3axis         *cntrl,*slave_cntrl;
struct mess_node *nodeptr;
register struct mess_info *motor_info;
int rtn_state=0;			/*return value, used in the query_axis function in the motordrvCom.cc file*/
int stop_time;			/*number of milliseconds before declaring the motor as stopped*/
bool ls_active = false;
msta_field status;
double nomovedelay;
unsigned index=0;			/*used to check if a master has slaves still moving after a master/slave move.  Possible due to position maintenance*/
unsigned slave_moving=0;		/*used to determine if the master or any of its slaves are still moving*/
int i;				/*General purpose counter*/
char galil_cmd_str[G21X3_LINE_LENGTH]="\0";          /*holds the assembled Galil cmd string*/
char buffer[G21X3_LINE_LENGTH]="\0";		  /*Holds the Galil response string*/
int motor_move,enc_move,uenc_move;	/*motor move, encoder move, user encoder move status 0=no move, 1=move*/
int resp;				/*integer galil response value, used to complete homing*/

control = (struct G21X3controller *) motor_state[card]->DevicePrivate;
cntrl = (struct G21X3axis *)&control->axis[signal];
motor_info = &(motor_state[card]->motor_info[signal]);
nodeptr = motor_info->motor_motion;
status.All = motor_info->status.All;

stop_time = (cntrl->stepper_pos_maintenance == 1) ? 250 * strlen(cntrl->axis_list) : 0;

//adjust stop time if doing a home
if ((cntrl->homming_now == 1) && (stop_time == 0))
	stop_time = 250;

/*has this motor been mentioned in a Nameconfig (st.cmd), ie, is it setup ?*/
if (strcmp(cntrl->axis_list,"")!=0)				
	{
	Debug(2, "G21X3:set_status entry: card=%d, signal=%d\n", card, signal);
	
	status.Bits.RA_MINUS_LS=0;	/*Re init limit status*/
	status.Bits.RA_PLUS_LS=0;
	status.Bits.RA_HOME=0;
	status.Bits.EA_HOME=0;	
	
	ls_active=false;
	
	motor_info->position=(epicsInt32)cntrl->motor_posi;                    /*pass step count/aux encoder info to motorRecord*/
	
	if (cntrl->uenc_state == 0)
		motor_info->encoder_position=(epicsInt32)cntrl->encoder_posi;          /*pass encoder info to motorRecord*/
	else
		motor_info->encoder_position=(epicsInt32)cntrl->uencoder_posi;         /*pass user encoder info to motorRecord*/
	
	//work out motor, encoder, and user encoder move status
	motor_move = (cntrl->oldpositionstep!=cntrl->motor_posi) ? 1 : 0;
	
	enc_move = 0;
	uenc_move = 0;
	if (cntrl->encoder_valid != 0)
		{
		if (cntrl->oldpositionenc>(cntrl->encoder_posi+(cntrl->pos_maintenance_min_error/cntrl->neres)))
			enc_move = 1;
		if (cntrl->oldpositionenc<(cntrl->encoder_posi-(cntrl->pos_maintenance_min_error/cntrl->neres)))
			enc_move = 1;
		}
	if (cntrl->uenc_reading_valid == 1)
		{
		if (cntrl->oldpositionuenc>(cntrl->uencoder_posi+(cntrl->pos_maintenance_min_error/cntrl->ueres)))
			uenc_move = 1;
		if (cntrl->oldpositionuenc<(cntrl->uencoder_posi-(cntrl->pos_maintenance_min_error/cntrl->ueres)))
			uenc_move = 1;
		}
	
	//reset stalled encoder flag, and encoder no move counter when moving ok
	//flag only reset when all active encoders are moving properly.
	if (((motor_move == 1) && (enc_move == 1) && (cntrl->encoder_valid != 0)))
		{
	 	if (((cntrl->uenc_reading_valid == 1) && (motor_move == 1) && (uenc_move == 1)) || (cntrl->uenc_reading_valid == 0))
			{
			status.Bits.EA_SLIP_STALL = 0;
			cntrl->encoder_stall = 0;
			cntrl->enc_no_move_count = 0;
			}
		}
		
	//reset encoder no move counter when we are stopped
	if (((cntrl->encoder_valid != 0) && (motor_move == 0) && (enc_move == 0)))
		{
		if (((cntrl->uenc_reading_valid == 1) && (motor_move == 0) && (uenc_move == 0)) || cntrl->uenc_reading_valid == 0)
			cntrl->enc_no_move_count = 0;
		}
			
	//check for ssi encoder connect status, set ssi connect flag
	if (cntrl->encoder_valid == 2)
		set_ssi_connectflag(cntrl);
			
	//check for stalled encoders, whilst we are moving
	//stop motors with stalled encoders.
	if (((cntrl->encoder_valid != 0) && (motor_move == 1) && (enc_move == 0)) || ((cntrl->uenc_reading_valid == 1) && (motor_move == 1) && (uenc_move == 0)))
		{
		cntrl->enc_no_move_count++;
		status.Bits.EA_SLIP_STALL = 0;
		//Calculate time encoder has been frozen for
		nomovedelay=(1.0/targs.motor_scan_rate)*1000.0*cntrl->enc_no_move_count;
		//stop axis with frozen encoders
		if ((nomovedelay>(350.0+(1.0/(cntrl->mtr_enc_scale/cntrl->mtr_enc_multiplier)))))
			{
			if (epicsMutexLock(control->G21X3Lock)==epicsMutexLockOK)
				{
				status.Bits.EA_SLIP_STALL = 1;
				cntrl->encoder_stall = 1;
				//stop the position maintenance	
				if (cntrl->stepper_pos_maintenance == 1)
					{
					sprintf(galil_cmd_str,"inps%c=0",cntrl->axis_list[0]);
					if (galil_send_cmd("set_status",control,galil_cmd_str,buffer)!=DMCNOERROR)
						errlogPrintf("Error stopping pos maintenance axis %c\n",cntrl->axis_list[0]);
					}
					
				//stop the master causing the slave with broken encoder to move.
				if ((cntrl->is_slave == 1) && (cntrl->axis_gear[0]!=0.0000))
					{
					sprintf(galil_cmd_str,"MG _BG%c",cntrl->axis_list[1]);
					galil_send_cmd("set_status",control,galil_cmd_str,buffer);
	
					if (((unsigned)galil_conv_str_val(buffer)) == 1.0000)
						{
						if (cntrl->stepper_pos_maintenance == 1)
							{
							//stop the master position maintenance
							sprintf(galil_cmd_str,"inps%c=0",cntrl->axis_list[1]);
							galil_send_cmd("set_status",control,galil_cmd_str,buffer);
							}

						//stop the master
						sprintf(galil_cmd_str,"ST%c",cntrl->axis_list[1]);
						galil_send_cmd("set_status",control,galil_cmd_str,buffer);
						}
					}
						
				//stop the motor
				sprintf(galil_cmd_str,"ST%c",cntrl->axis_list[0]);
				galil_send_cmd("set_status",control,galil_cmd_str,buffer);
				epicsMutexUnlock(control->G21X3Lock);
				}
			}
		}
	
	/*Test if the axis is moving, and set status flags appropriately*/
	if (((cntrl->switches & AXIS_INMOTION) || (motor_move == 1) || (enc_move == 1) || (uenc_move == 1)) && (cntrl->dev_init!=0))
		{
		cntrl->moving=1;               /*set flag for moving*/
		status.Bits.RA_DONE=0;         /*set flag for motion not complete*/
		status.Bits.RA_MOVING=1;	/*set mR flag for moving*/
		motor_info->no_motion_count=0; /*Set value for motion*/
  		cntrl->no_motion_count=0;
		if (cntrl->oldpositionstep-cntrl->motor_posi<0 || cntrl->oldpositionenc-cntrl->encoder_posi<0 || cntrl->oldpositionuenc-cntrl->uencoder_posi<0)	/*moving forward*/
			status.Bits.RA_DIRECTION=1;				/*set direction while we are in here*/
		else		
			{			
			if (cntrl->oldpositionstep-cntrl->motor_posi>0 || cntrl->oldpositionenc-cntrl->encoder_posi>0 || cntrl->oldpositionuenc-cntrl->uencoder_posi>0)	/*moving backward*/
				status.Bits.RA_DIRECTION=0;	
			else
				status.Bits.RA_DIRECTION=cntrl->olddirection;	/*We cannot detect movement, leave direction alone*/
			}
		/*report only limits we are heading away from whilst moving, else motor record stops polling controller*/
		if (status.Bits.RA_DIRECTION==1)		/*moving forward*/
			{
			if (cntrl->homming_now == 0)
				{
				if (!(cntrl->invert_coordinates & INVERT_SWITCHES) && !(cntrl->switches & AXIS_REVLIMIT_INACTIVE))
					status.Bits.RA_MINUS_LS=1;
				if ((cntrl->invert_coordinates & INVERT_SWITCHES) && !(cntrl->switches & AXIS_FORLIMIT_INACTIVE))
					status.Bits.RA_MINUS_LS=1;
				}
			}
		else
			{
			/*moving backward*/
			if (cntrl->homming_now == 0)
				{
				if (!(cntrl->invert_coordinates & INVERT_SWITCHES) && !(cntrl->switches & AXIS_FORLIMIT_INACTIVE))
					status.Bits.RA_PLUS_LS=1;
				if ((cntrl->invert_coordinates & INVERT_SWITCHES) && !(cntrl->switches & AXIS_REVLIMIT_INACTIVE))
					status.Bits.RA_PLUS_LS=1;
				}
			}
		}
	else
		{
		//Test if any slaves attached to this motor are doing an independant correction
		slave_moving=0;
		if (cntrl->stepper_pos_maintenance==1)
			{
			index = 1;
			slave_moving=0;
			while (cntrl->axis_list[index]!='\0')
				{
				slave_cntrl = (struct G21X3axis *)&control->axis[(cntrl->axis_list[index]-65)];
				index++;
				
				//work out slave motor and encoder move status	
				motor_move = (slave_cntrl->oldpositionstep!=slave_cntrl->motor_posi) ? 1 : 0;
				enc_move = 0;
				uenc_move = 0;
				if (cntrl->encoder_valid != 0)
					{
					if (slave_cntrl->oldpositionenc>(slave_cntrl->encoder_posi+(cntrl->pos_maintenance_min_error/cntrl->neres)))
						enc_move = 1;
					if (slave_cntrl->oldpositionenc<(slave_cntrl->encoder_posi-(cntrl->pos_maintenance_min_error/cntrl->neres)))
						enc_move = 1;
					}
				if (cntrl->uenc_reading_valid == 1)
					{
					if (slave_cntrl->oldpositionuenc>(slave_cntrl->uencoder_posi+(cntrl->pos_maintenance_min_error/cntrl->ueres)))
						uenc_move = 1;
					if (slave_cntrl->oldpositionuenc<(slave_cntrl->uencoder_posi-(cntrl->pos_maintenance_min_error/cntrl->ueres)))
						uenc_move = 1;
					}
					
				if ((slave_cntrl->switches & AXIS_INMOTION) || (motor_move == 1) || (enc_move == 1) || (uenc_move == 1))
					if (slave_cntrl->is_slave==1)
						slave_moving=1;
				}
			}
		//Count as no motion only if master and all slaves are stopped
		if (slave_moving==0)
			{
			motor_info->no_motion_count++;  /*count number of cycles without movement, mR stops scanning when motor is stopped*/
			cntrl->no_motion_count++;	/*device support uses this var, mR uses the motor_info variable instead*/
			}
		else
			{
			motor_info->no_motion_count=0;  
			cntrl->no_motion_count=0;	
			}
			
		//Calculate time master and all slaves have been stopped for
		nomovedelay=(1.0/targs.motor_scan_rate)*1000.0*cntrl->no_motion_count;
		
		//If we have been stopped longer than stop_time ms, set no motion flags as appropriate.
		if (nomovedelay>=stop_time || cntrl->init_update==1 || cntrl->status_update==1)
			{
			//test if home complete, set flags
			if (cntrl->homming_now == 1)
				{
				/*check if home complete on controller*/
				sprintf(galil_cmd_str,"home%c=?\n",cntrl->axis_list[0]);
				if (galil_send_cmd("G21X3StartCard",control,galil_cmd_str,buffer)==DMCNOERROR)
					{
					resp = (epicsInt32)galil_conv_str_val(buffer);
					if (resp != 1)
						{
						cntrl->homming_now = 0;
						//reset limdc for velo here
						}
					}
				}
	
			//Update motor record if homming not in progress
			if (cntrl->homming_now == 0)
				{
				//Update motor record
				cntrl->moving=0;             /*Set flag for no motion*/
				status.Bits.RA_MOVING=0;	/*set mR flag for moving*/
				status.Bits.RA_DONE=1;       /*Set flag for motion complete*/	
				if (!(cntrl->switches & AXIS_REVLIMIT_INACTIVE))	/*report any limit switch activation now we are stopped*/
					if (cntrl->invert_coordinates & INVERT_SWITCHES)
						status.Bits.RA_PLUS_LS=1;
					else
						status.Bits.RA_MINUS_LS=1;
				if (!(cntrl->switches & AXIS_FORLIMIT_INACTIVE))
					if (cntrl->invert_coordinates & INVERT_SWITCHES)
						status.Bits.RA_MINUS_LS=1;
					else
						status.Bits.RA_PLUS_LS=1;
					
				//turn off position maintenance after move complete
				if (cntrl->stepper_pos_maintenance==1 && cntrl->pm_state==1 && cntrl->init_update!=1 && cntrl->status_update!=1)
					{
					if (epicsMutexLock(control->G21X3Lock)==epicsMutexLockOK)
						{
						cntrl->pm_state=0;
						if (cntrl->is_slave==0)
							{
							//do master and all slaves
							for (i=0;i<(int)strlen(cntrl->axis_list);i++)
								{
								sprintf(galil_cmd_str,"inps%c=0",cntrl->axis_list[i]);
								galil_send_cmd("set_status",control,galil_cmd_str,buffer);
								}
							}
						else
							{
							//do slave only
							sprintf(galil_cmd_str,"inps%c=0",cntrl->axis_list[0]);
							galil_send_cmd("set_status",control,galil_cmd_str,buffer);
							}
						epicsMutexUnlock(control->G21X3Lock);
						}
					}	
				}	
			
			if (cntrl->init_update==1)
				cntrl->init_update=0;			/*we may only do an init update once*/
			if (cntrl->status_update==1)
				cntrl->status_update=0;			/*we may only do one status update at a time*/
			}
		else
			{				/*Tell mR to keep scanning*/
			status.Bits.RA_MOVING=1;	/*set mR flag for moving*/
			status.Bits.RA_DONE=0;          /*Set flag for motion in progress*/	
			}
		}
		
	process_user_encoder_faults(control,cntrl);

	/*if Home switch is on and we are using it, set the appropriate flag*/
	if ((cntrl->switches & AXIS_HOMENOT_MASK)==AXIS_HOMENOT_MASK && cntrl->limit_as_home==0)
		{
		status.Bits.RA_HOME=1;
		status.Bits.EA_HOME=1;
		}

	/*if Rev switch is on and we are using it as a home, set the appropriate flag*/
	if ((!(cntrl->switches & AXIS_REVLIMIT_INACTIVE)) && cntrl->limit_as_home>0)
		{
		status.Bits.RA_HOME=1;
		status.Bits.EA_HOME=1;
		}
	/*if Rev switch is on and we are using it as a home, set the appropriate flag*/
	if ((!(cntrl->switches & AXIS_FORLIMIT_INACTIVE)) && cntrl->limit_as_home>0)
		{
		status.Bits.RA_HOME=1;
		status.Bits.EA_HOME=1;
		}
	
	/*Test for controller comms error*/
	if (cntrl->status == G21X3_COMM_ERR)
		status.Bits.CNTRL_COMM_ERR=1;
	else
		status.Bits.CNTRL_COMM_ERR=0;

	/*Save current position for next cycle*/	
	cntrl->oldpositionstep=motor_info->position;
	cntrl->oldpositionenc=(epicsInt32)cntrl->encoder_posi;
	cntrl->oldpositionuenc=(epicsInt32)cntrl->uencoder_posi;
	/*Save current direction for next cycle*/	
	cntrl->olddirection=status.Bits.RA_DIRECTION;
	}

    rtn_state = (!motor_info->no_motion_count || ls_active==true || (status.Bits.RA_DONE | status.Bits.RA_PROBLEM)) ? 1 : 0;  /*return value looked at within motorRecord*/

    Debug(1, "--------set_status rtn_state=%d  \n",rtn_state);

    motor_info->status.All = status.All;

    //Force this thread to sleep for short time, so others can get mutex if needed
    epicsThreadSleep(.001);
    return(rtn_state);                    /*return value looked at within motorRecord*/
}

/*****************************************************/
/* send a message to the XPS board                  */
/* send_mess()                                       */
/*****************************************************/
STATIC RTN_STATUS send_mess(int card, char const *com, char *name)
{
    /* This is a no-op for the XPS, but must be present */
    return (OK);
}


/*****************************************************/
/* receive a message from the XPS board           */
/* recv_mess()                                       */
/*****************************************************/
STATIC int recv_mess(int card, char *com, int flag)
{
    /* This is a no-op for the XPS, but must be present */
    /* Set the returned message to a null string */
    *com = '\0';
    return (0);
}



/*****************************************************/
/* Setup system configuration                        */
/* G21X3Setup()                                      */
/*****************************************************/
RTN_STATUS G21X3Setup(int num_cards,   /* number of controllers in system.  */
                      int scan_rate)   /* I think this is for the epicsthread */
{
    Debug(1, "G21X3Setup: Controllers=%d Scan Rate=%d\n",num_cards,scan_rate);
    int itera;
    if (num_cards > G21X3_NUM_CONTROLLERS)
        errlogPrintf(" Error in setup too many channels\n");

    if (num_cards < 1)
        G21X3_num_cards = 1;
    else
        G21X3_num_cards = num_cards;

/*     Set motor polling task rate */
    if (scan_rate >= 1 && scan_rate <= 60)
        targs.motor_scan_rate = scan_rate;
    else
        targs.motor_scan_rate = SCAN_RATE;

   /* Allocate space for pointers to motor_state structures. */
    motor_state = (struct controller **) calloc(G21X3_num_cards, sizeof(struct controller *));
    
    /*allocate flags that are used in NameConfig*/
    firstNameConfig=(int *)calloc(G21X3_num_cards, sizeof(int));
    firstdigitalinput=(int *)calloc(G21X3_num_cards, sizeof(int));

    for (itera=0;itera<G21X3_num_cards;itera++)			/*init flags that are used in NameConfig*/
	{
	firstNameConfig[itera]=1;
	firstdigitalinput[itera]=1;
	}

    for (itera = 0; itera < G21X3_num_cards; itera++)
        motor_state[itera] = (struct controller *) NULL;

    DMCInitLibrary();    /*Initialize Galil Driver Lib*/
    return (OK);
}


/*****************************************************/
/* Configure a Galil 21X3 controller one command   */
/* For each controller                              */
/* G21X3Config()                                    */
/*****************************************************/
RTN_STATUS G21X3Config(int card,        /* Controller number */
                       char *ip,        /* Galil IP address*/
            int totalaxesused,          /* Number of axis/positioners used*/
	    int model_num,	        /* Galil controller model*/
	    int limit_type,		/*-1 for normally closed, 1 for normally open limit switches*/
	    int home_type,	  	/*-1 for normally closed, 1 for normally open home switches*/
	    int cntrl_home_dir) 	/*-1 for reverse, 1 for forward*/
{
    struct G21X3controller *control;
    struct G21X3axis         *cntrl;
    HANDLEDMC hdmc=-1;		    	/*Galil connection handle*/
    CONTROLLERINFO controllerinfo;  	/*Galil Controller information structure */
    int axis;				/*loop counter only*/
    char buffer[G21X3_LINE_LENGTH];	/*holds the galil controller response*/
    char galil_cmd_str[G21X3_LINE_LENGTH]="\0";	/*Holds assembled command or query for the Galil controller*/
    int rc;				/*Galil function return code*/
    char RV[] ={0x12,0x16,'\r',0x0};    /*Galil command string for model and firmware version query*/
    int connectok=1;
    int i;				//general purpose variable
    
    /*Error Checking*/
    if ((totalaxesused>G21X3_NUM_CHANNELS) || (totalaxesused<0))
	{
	errlogPrintf("Total Avail axis specified is too high or too low\n");
	return(ERROR);
	}
  
    /*Allocation*/
    motor_state[card] = (struct controller *) calloc(1,
                                                     sizeof(struct controller));
    motor_state[card]->DevicePrivate = calloc(1,
                                              sizeof(struct G21X3controller));
    control = (struct G21X3controller *) motor_state[card]->DevicePrivate;

    motor_state[card]->total_axis = totalaxesused;			/*calculate how big the main buffers need to be*/

    /*allocate buffers used to store generated G21X3 code */
    card_thread_code=(char *)calloc(G21X3_num_cards*G21X3_NUM_CHANNELS*(THREAD_CODE_LEN),sizeof(char));	
    card_limit_code=(char *)calloc(G21X3_num_cards*G21X3_NUM_CHANNELS*(LIMIT_CODE_LEN),sizeof(char));
    card_digital_code=(char *)calloc(G21X3_num_cards*G21X3_NUM_CHANNELS*(INP_CODE_LEN),sizeof(char));
    card_code=(char *)calloc(G21X3_num_cards*G21X3_NUM_CHANNELS*(THREAD_CODE_LEN+LIMIT_CODE_LEN+INP_CODE_LEN),sizeof(char));					 

    /*Setup Galil Connection Parameters*/
    memset(&controllerinfo, '\0', sizeof(controllerinfo));
    controllerinfo.cbSize = sizeof(controllerinfo);
    controllerinfo.usModelID = model_num;
    controllerinfo.fControllerType = ControllerTypeEthernet;
    if (totalaxesused>0)
    	{
	controllerinfo.ulTimeout = 2000;
	controllerinfo.ulDelay = 10;
    	}
    else
    	{
	controllerinfo.ulTimeout = 100;
	controllerinfo.ulDelay = 10;
	}
    strcpy(controllerinfo.hardwareinfo.socketinfo.szIPAddress, ip);
    controllerinfo.hardwareinfo.socketinfo.fProtocol = EthernetProtocolTCP;
    control->G21X3Lock = epicsMutexCreate();

    rc = DMCOpen(&controllerinfo, &hdmc);
    
    /* Open the connection to the Galil*/
    if (rc!=DMCNOERROR)
    	{
	connectok=0;			
	errlogPrintf("\n\nError Connecting to Card %d, %d Controller\n\n",card,model_num);
	}
    else
    	{
	errlogPrintf("Connection Established to Galil at %s\n",ip);
	
	/*place controller wide settings in G21X3 controller structure*/
	control->limit_type=limit_type;
	control->home_type=home_type;
	control->hdmc=hdmc;                                  /*store connection handle */
	strcpy(control->ip,ip);				/*store ip address of controller*/
	control->connectok=connectok;		       /*store connection status flag*/
	control->model_num=model_num;			/*store the model number*/
	control->card=card;				/*store the card number*/

	//Stop all threads running on motor controller
	for (i=0;i<G21X3_NUM_CHANNELS;i++)
		{
		sprintf(galil_cmd_str,"HX%d",i);
		galil_send_cmd("G21X3Config",control,galil_cmd_str,buffer);
		}
	
	/* Test for Device Response to Command.. Get Serial Number*/
    	if (galil_send_cmd("G21X3Config",control,"MG _BN",buffer)!=DMCNOERROR)
		errlogPrintf("Error Obtaining Galil Serial Number\n");
    	else
    		errlogPrintf("Galil DMC Serial Num : %s\n", buffer);
		
	/*Get Controller Model and Firmware numbers, and display to user*/
	if (galil_send_cmd("G21X3Config",control,RV,buffer)!=DMCNOERROR)
		errlogPrintf("Error obtaining Galil model, and firmware numbers\n");
	else
		errlogPrintf("Galil Model : %s\n",buffer);
		
	if (totalaxesused != 0)
		{
		/* Read how many axis the device has*/
		if (galil_send_cmd("G21X3Config",control,"MG _BV",buffer)==DMCNOERROR)
			control->num_axis=(int)galil_conv_str_val(buffer);	
		}
	else
		control->num_axis = 0;

	/*Parse the user input, putting settings into format understood by G21X3*/
	limit_type=(limit_type>0) ? -1 : 1;

	/*Setup home and input line parameters*/
	sprintf(galil_cmd_str,"CN %d,1,-1,0,1",limit_type);
	galil_send_cmd("G21X3Config",control,galil_cmd_str,buffer);

	for (axis=0; axis<totalaxesused; axis++)
		{
		/*retrieve axis structure*/
		cntrl = (struct G21X3axis *)&control->axis[axis];	
		/*initialize the axis list associated with this axis (ie. master/slave(s) e-gear)*/
		strcpy(cntrl->axis_list,"");
		//Determine correct home switch operation*/
		if (home_type == 1)
			{			
			cntrl->homesw_act = 1;
			cntrl->homesw_inact = 0;
			}	
		else
			{
			cntrl->homesw_act = 0;
			cntrl->homesw_inact = 1;	
			}
		}
		
	/*Set initialize flag for user code, we are starting initialization*/
	sprintf(galil_cmd_str,"init=0");
	galil_send_cmd("G21X3Config",control,galil_cmd_str,buffer);
	}

    Debug(11, "G21X3Config: End\n");
    return (OK);
}

/*********************************************************/
/*    Pass the group and positioner names                */
/*     Call this for each axis                           */
/*                                                       */
/*********************************************************/
RTN_STATUS G21X3NameConfig(int card,      /*specify which controller 0-up*/
                           char *axisname,      /*axis number A-H*/
			   int motortype,      /*motor type on this axis*/
			   int home_direction,		/*-1 reverse home or 1 forward home*/
			   int invert_coordinates,	/*invert the galil native coordinates?*/
			   double homevalue,	/*home position in EGU*/
			   int homeprogreg,	/*Program home register once home 0=No, 1=Yes*/
			   int mainencoder,    /*Main encoder setup param*/
			   int auxencoder,     /*aux encoder setup param*/
			   int ssiencoderinput,  /*ssi input is 0=not on, 1=main Enc Inp, 2=Aux Enc Input*/
			   int ssitotalbits,     /*Total number of encoder bits*/
			   int ssisingleturnbits, /*Number of bits that represent a single turn*/
			   int ssierrbits,       /*Number of bits that represent err*/
			   int ssitimecode,      /*provides timming information to Galil about the Enc comms*/
			   int ssidataform,      /*1=binary, 2=gray code*/
			   int limit_as_home,		/*0=no, 1=yes. Using a limit switch as home*/
			   double egu_after_limit,	/*egu travelled after Limit & home switch activated*/
			   char *slavedata,		/*electronic gear slave's axis names formatted like "A=1.0000,0,B=.2500,1"*/
			   int slavecmd,		/*slave cmd source, master cmd 0, or master main enc 1*/
			   int gantrymode,		/*enable gantry mode b/n master and slaves (master limit swtch only) 0=off, 1=on*/
			   char *digitalhome_string,	/*digital input(s) to use to send home, or interlock motor if -ve*/
			   int digitalaway,		/*digital input to use to send away from home*/
			   char *stepper_pos_maintenance)   /*Does the user want position maintenance code loaded to controller for this stepper*/
{
	struct G21X3controller *control;
	struct G21X3axis         *cntrl;
	struct G21X3axis         *cntrl_slave;
	unsigned i;
	char c;
	unsigned  num_axis;			/*axis number count, inc slaves*/
	int axis;				/*the Master axis number*/
	char axis_limit_code[LIMIT_CODE_LEN];   /*The LIMSWI code buffer*/
	char axis_dighome_code[INP_CODE_LEN];	     /*The ININT code buffer*/
	char axis_thread_code[THREAD_CODE_LEN+LIMIT_CODE_LEN+INP_CODE_LEN];	     /*The main thread code buffer*/
	int axis_slavehomeallowed[G21X3_NUM_CHANNELS+1];
	
	int digitalhome;
	
	//Store first digital port specified in digitalhome
	sscanf(digitalhome_string,"%d,",&digitalhome);
	
	control = (struct G21X3controller *)motor_state[card]->DevicePrivate;
	
	/*if we connected ok to galil in config function*/
	if (control->connectok)
		{
		/*A is 65 in ASCII*/
		axis = axisname[0]-65;
		
		cntrl = (struct G21X3axis *)&control->axis[axis];	
		cntrl_slave = NULL;
		
		/*Parse user input and insert axis names (master and all slaves) into cntrl->axis_list*/
		/*Stores some master settings for later use */
		galil_parse_userinput(control,
		                      cntrl,
				      axis,
				      &num_axis,
				      gantrymode,
				      limit_as_home,
				      stepper_pos_maintenance,
				      slavedata,
				      axis_slavehomeallowed,
				      motortype,
				      &ssiencoderinput,
				      auxencoder,
				      mainencoder,
				      digitalhome,
				      &digitalaway);
	
		//initialize the homming_now variable
		cntrl->homming_now = 0;

		/*Store master settings for later use*/
		galil_store_master(cntrl,
				   egu_after_limit,
				   motortype,
				   homevalue,
				   homeprogreg,
				   invert_coordinates,
				   ssitotalbits,
				   ssierrbits,
				   ssidataform);

		/*store the motor enable/disable digital IO setup */
		if (digitalhome>-9 && digitalhome<0)
			galil_store_motors_enable(control, cntrl, digitalhome_string, digitalaway);

		/*Generate Galil code given user settings for this axis including slave motors, and store in code buffer for transfer to the Galil*/ 
		for (i=0;i<num_axis;i++)
			{
			if (i!=0)	
				{
				/*workout slave axis number from its name*/
				axis = cntrl->axis_list[i]-65;
				/*retrieve slave axis cntrl info*/
				cntrl_slave = (struct G21X3axis *)&control->axis[axis];
				/*store slave settings for later use*/				
				galil_store_slave(i, control, cntrl, cntrl_slave, axis_slavehomeallowed);
				}
				
			galil_setup_motor_encoder(i,
						  control,
						  cntrl,
						  ssiencoderinput,
						  ssitotalbits,
						  ssisingleturnbits,
						  ssierrbits,
						  ssitimecode,
						  ssidataform);

			galil_initialize_codegen(i,
						 control, 
			      			 cntrl,
						 digitalhome,
						 digitalaway,
						 &c,
						 axis_thread_code,
						 axis_limit_code,
						 axis_dighome_code);

			galil_gen_limitcode(i,
					    c,
					    control,
					    cntrl,
					    axis_thread_code,
					    axis_limit_code);

			galil_gen_commoncode(i,
					     c,
					     control,
					     cntrl,
					     axis_thread_code);

			galil_gen_slavecode(i,
					    c,
					    control,
					    cntrl,
					    slavecmd,
					    axis_thread_code);

			galil_gen_pos_maintenance_code(i,
						       c,
						       control,
						       cntrl,
						       cntrl_slave,
						       axis_thread_code);

			galil_gen_digitalcode(i,
					      c,
					      control,
					      cntrl,
					      digitalhome,
					      digitalaway,
					      axis_dighome_code);

			galil_gen_homecode(i,
					   c,
					   control,
					   cntrl,
					   cntrl_slave,
					   axis_thread_code);

			/* insert motor interlock code into thread A */
			if (c=='A')
				{
				sprintf(axis_thread_code,"%sIF (mlock=1)\n",axis_thread_code);
				sprintf(axis_thread_code,"%sII ,,dpon,dvalues\nENDIF\n",axis_thread_code);
				}

			//Insert the final jump statement for the current thread code
			sprintf(axis_thread_code,"%sJP #THREAD%c\n",axis_thread_code,c);
			//Copy this axis code to the card code buffer
			strcat(&card_thread_code[card*G21X3_NUM_CHANNELS*THREAD_CODE_LEN],axis_thread_code);
			strcat(&card_limit_code[card*G21X3_NUM_CHANNELS*LIMIT_CODE_LEN],axis_limit_code);
			strcat(&card_digital_code[card*G21X3_NUM_CHANNELS*INP_CODE_LEN],axis_dighome_code);
			}
	}	/*end connectok if statement*/
    return (OK);
}

/*--------------------------------------------------------------*/
/* Start the card requested by user   */
/*--------------------------------------------------------------*/

RTN_STATUS G21X3StartCard(int card, char *code_file, int eeprom_write,int display_code)
{
	struct G21X3controller *control;
	unsigned i,index;
	int start_ok=1;					//Have the controller threads started ok
	char c;						/*used when printing the uploaded and generated code*/
	char galil_cmd_str[G21X3_LINE_LENGTH]="\0";	/*holds the assembled Galil cmd string*/
	char buffer[G21X3_LINE_LENGTH] = "";		/* Response from Galil controller, used here to store command params */
	long rc=0;					/*Galil Command return code*/
	char current_code[(G21X3_NUM_CHANNELS*(THREAD_CODE_LEN+LIMIT_CODE_LEN+INP_CODE_LEN))];
	
	control = (struct G21X3controller *)motor_state[card]->DevicePrivate;
	
	/*First add termination code to end of code generated for this card*/
	galil_gen_card_codeend(card);
	
	/*Assemble the sections of generated code for this card */
	strcat(&card_code[card*G21X3_NUM_CHANNELS*(THREAD_CODE_LEN+LIMIT_CODE_LEN+INP_CODE_LEN)],&card_thread_code[card*G21X3_NUM_CHANNELS*THREAD_CODE_LEN]);
	strcat(&card_code[card*G21X3_NUM_CHANNELS*(THREAD_CODE_LEN+LIMIT_CODE_LEN+INP_CODE_LEN)],&card_limit_code[card*G21X3_NUM_CHANNELS*LIMIT_CODE_LEN]);
	strcat(&card_code[card*G21X3_NUM_CHANNELS*(THREAD_CODE_LEN+LIMIT_CODE_LEN+INP_CODE_LEN)],&card_digital_code[card*G21X3_NUM_CHANNELS*INP_CODE_LEN]);
	
	//Dump generated code to file
	galil_write_gen_codefile(card);

	//load up code specified by user (ie. generated, or generated & user edited, or complete user code)
	galil_read_codefile(card,code_file);

	/*print out the generated/user code for the G21X3 card*/
	if ((display_code == 1) || (display_code == 3))
		{
		errlogPrintf("\nGenerated/User code is\n\n");
		for (i=0;i<strlen(&card_code[card*G21X3_NUM_CHANNELS*(THREAD_CODE_LEN+LIMIT_CODE_LEN+INP_CODE_LEN)]);i++)
			{
			c=card_code[card*G21X3_NUM_CHANNELS*(THREAD_CODE_LEN+LIMIT_CODE_LEN+INP_CODE_LEN)+i];
			errlogPrintf("%c",c);
			}
		}

	/*Upload code currently in controller for comparison to generated code */
	if ((rc=DMCUploadToBuffer(control->hdmc,current_code,(G21X3_NUM_CHANNELS*(THREAD_CODE_LEN+LIMIT_CODE_LEN+INP_CODE_LEN))))!=DMCNOERROR)
		errlogPrintf("\n\nError uploading code from controller rc %ld card %d\n",rc,card);

	//Filter out \r characters in uploaded program
	index = 0;
	for (i=0;i<strlen(current_code);i++)
		{
		if (current_code[i]!='\r')
			{
			current_code[index] = current_code[i];
			index++;
			}
		}
	current_code[index] = '\0';

	if ((display_code == 2) || (display_code == 3))
		{
		/*print out the uploaded code from the G21X3 card*/
		errlogPrintf("\nUploaded code is\n\n");
		for (i=0;i<strlen(current_code);i++)
			{
			c=current_code[i];
			errlogPrintf("%c",c);
			}
		}

	/*If generated code differs from controller current code then download generated code*/
	if ((strcmp(current_code,&card_code[card*G21X3_NUM_CHANNELS*(THREAD_CODE_LEN+LIMIT_CODE_LEN+INP_CODE_LEN)])!=0) &&
		strcmp(&card_code[card*G21X3_NUM_CHANNELS*(THREAD_CODE_LEN+LIMIT_CODE_LEN+INP_CODE_LEN)],"") !=0)
		{
		errlogPrintf("\n\nTransferring code to model %d, card %d\n",control->model_num,card);
		if ((rc=DMCDownloadFromBuffer(control->hdmc,&card_code[card*G21X3_NUM_CHANNELS*(THREAD_CODE_LEN+LIMIT_CODE_LEN+INP_CODE_LEN)],""))!=DMCNOERROR)
			{
			errlogPrintf("\n\nError downloading code model %d, card %d rc %d\n",control->model_num,card,rc);
			errlogPrintf("This error is fatal for this driver.!  Exiting\n\n");
			exit(0);
			}
		else
			{	
			errlogPrintf("Code transfer successful to model %d, card %d\n",control->model_num,card);
				
			/*burn code to eeprom if eeprom_write is 1*/
			if (eeprom_write == 1)
				{
				/*Burn program to EEPROM*/
				sprintf(galil_cmd_str,"BP");
				if (galil_send_cmd("G21X3StartCard",control,galil_cmd_str,buffer)!=DMCNOERROR)
					errlogPrintf("Error burning software to EEPROM, card %d\n",card);
				else
					errlogPrintf("Burning code to EEPROM, card %d\n",card);
						
				epicsThreadSleep(3);
					
				/*Burn parameters to EEPROM*/
				sprintf(galil_cmd_str,"BN");
				if (galil_send_cmd("G21X3StartCard",control,galil_cmd_str,buffer)!=DMCNOERROR)
					errlogPrintf("Error burning parameters to EEPROM, card %d\n",card);
				else
					errlogPrintf("Burning parameters to EEPROM, card %d\n",card);
						
				epicsThreadSleep(3);
					
				/*Burn variables to EEPROM*/
				sprintf(galil_cmd_str,"BV");
				if (galil_send_cmd("G21X3StartCard",control,galil_cmd_str,buffer)!=DMCNOERROR)
					errlogPrintf("Error burning variables to EEPROM, card %d\n",card);
				else
					errlogPrintf("Burning variables to EEPROM, card %d\n",card);
					
				epicsThreadSleep(3);
				}
			}
		}
		
	//if axis defined start the code that was delivered to controller
	if ((control->num_axis == 0 && strcmp(code_file,"") != 0) || (control->num_axis >0))
		{
		//last thing, we start thread 0 on the G21X3
		//Note that thread 0 starts any other required threads on the motor controller
		sprintf(galil_cmd_str,"XQ 0,0");
		if (galil_send_cmd("G21X3StartCard",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Thread 0 failed to start on card %d, rc was %ld\n\n",card,rc);
					
		epicsThreadSleep(1);
		
		if (control->num_axis>0)
			{
			for (i=0;i<(unsigned)motor_state[card]->total_axis;i++)
				{		
				/*check that our internal G21X3 code is running*/
				sprintf(galil_cmd_str,"MG _XQ%d\n",i);
				if (galil_send_cmd("G21X3StartCard",control,galil_cmd_str,buffer)==DMCNOERROR)
					{
					if (((epicsInt32)galil_conv_str_val(buffer))==-1)
						{
						start_ok = 0;
						errlogPrintf("\nThread %d failed to start on card %d\n",i,card);
						}
					}
				}
			}
		else
			{
			/*check that thread 0 of our custom G21X3 code is running*/
			sprintf(galil_cmd_str,"MG _XQ0\n");
			if (galil_send_cmd("G21X3StartCard",control,galil_cmd_str,buffer)==DMCNOERROR)
				{
				if (((epicsInt32)galil_conv_str_val(buffer))==-1)
					{
					start_ok = 0;
					errlogPrintf("\nThread %d failed to start on card %d\n",i,card);
					}
				}
			}
		
		if (start_ok == 0)
			{
			/*stop all motors on the crashed controller*/
			sprintf(galil_cmd_str,"AB 1\n");
			if (galil_send_cmd("G21X3StartCard",control,galil_cmd_str,buffer)!=DMCNOERROR)
				errlogPrintf("\nError aborting all motion on controller\n");
			else	
				errlogPrintf("\nStopped all motion on crashed controller\n");
			exit(0);
			}
		else
			errlogPrintf("\nCode started successfully, card %d\n\n",card);
		}
		
	G21X3_cards_started++;
	if (G21X3_cards_started == G21X3_num_cards)
		{
		/*if we have started all specified cards*/
		/*free all the RAM we used*/
		free(card_thread_code);
		free(card_limit_code);
		free(card_digital_code);
		free(card_code);
		}
	return (OK);
}


/*****************************************************/
/* initialize all software and hardware              */
/* This is called from the init()                    */
/* motor_init()                                      */
/*****************************************************/
STATIC int motor_init()
{

    struct controller *brdptr;
    struct G21X3controller *control;
    struct G21X3axis         *cntrl;
    struct G21X3axis         *cntrl_two;
    int card_index, motor_index;
    int totalaxes;
    char galil_cmd_str[G21X3_LINE_LENGTH]="\0";          /*holds the assembled Galil cmd string*/
    char galil_cmd_str2[G21X3_LINE_LENGTH]="\0";          /*holds the assembled Galil cmd string*/
    char buffer[G21X3_LINE_LENGTH] = ""; /* Response from Galil controller, used here to store command params */
    int switches=AXIS_INMOTION;
    int i, motor_count=0;
    int uenc_ok = 1;
    
    initialized = true;   /* Indicate that driver is initialized. */

    for (card_index = 0; card_index < G21X3_num_cards; card_index++)		/*scan thru every controller*/
    	{
        totalaxes = motor_state[card_index]->total_axis;
        Debug(5, "G21X3:motor_init: Card init loop card_index=%d\n",card_index);
        brdptr = motor_state[card_index];
        total_cards = card_index + 1;
        Debug(5, "G21X3:motor_init: Above control def card_index=%d\n",\
              card_index);
        control = (struct G21X3controller *) brdptr->DevicePrivate;
        for (motor_index = 0; motor_index < totalaxes; motor_index++)	  /*scan thru every axis on current controller*/
	    	{
		motor_count++;
		cntrl = (struct G21X3axis *)&control->axis[motor_index];
		struct mess_info *motor_info = &brdptr->motor_info[motor_index];
		motor_info->status.All = 0;
		motor_info->no_motion_count = 0;
		if (control->connectok)
			{
			brdptr->localaddr = (char *) NULL;
			brdptr->motor_in_motion = 0;
			brdptr->motor_info[motor_index].motor_motion = NULL;
			strcpy(brdptr->ident, "Galil_controller");
		
			/*Setup default values for the below parameters*/
			motor_info->status.Bits.RA_PROBLEM=0;    /*Signify driver is still polling*/
			motor_info->status.Bits.RA_HOME=0;       /*home signal is off*/
			/* encoder status */
			motor_info->status.Bits.EA_SLIP=0;       /*encoder slip disable*/
			motor_info->status.Bits.EA_SLIP_STALL=0; /*No slip or stall detected*/
			motor_info->status.Bits.EA_HOME=0;       /*encoder home signal is off*/

			/*Set gain support options for the current axis based on user set motor type*/
					
			switch (cntrl->motortype)
				{
				case 0:
				case 1:
				/*Motor is servo, enable PID, encoder, and position maintenance*/
				motor_info->status.Bits.GAIN_SUPPORT=1;
				motor_info->pid_present = YES;
				motor_info->encoder_present=YES;
				motor_info->status.Bits.EA_PRESENT=1;
				motor_info->status.Bits.EA_POSITION=1;
				break;
				case 2:
				case 3:
				case 4:
				case 5:
				/*Motor is Stepper, disable PID, enable position maintenance and encoder only if the user has indicated that the galil is running position maintenance software*/
				motor_info->status.Bits.GAIN_SUPPORT=0;
				motor_info->pid_present = NO;
				//check user encoder settings
				for (i=0;i<(int)strlen(cntrl->axis_list);i++)
					{
					//retrieve control structure for axis in master list
					cntrl_two = (struct G21X3axis *)&control->axis[cntrl->axis_list[i]-65];	
					if (cntrl_two->uenc_settings_valid == 0)
						uenc_ok = 0;
					}
				if (cntrl->encoder_valid==1 || cntrl->encoder_valid==2 || uenc_ok == 1)
					{
					motor_info->encoder_present=YES;    	             /*i believe this is redundant.. refer EA_PRESENT*/
					motor_info->status.Bits.EA_PRESENT=1;
					if (cntrl->stepper_pos_maintenance==1)
						motor_info->status.Bits.EA_POSITION=1;
					else
						motor_info->status.Bits.EA_POSITION=0;
					}	
				else
					{
					motor_info->status.Bits.EA_PRESENT=0;
					motor_info->encoder_present=NO;
					}
					break;
				default: break;
				}

			/*Get switches info from Galil ie. forward limit,reverse limit ,moving, home switch*/
			sprintf(galil_cmd_str,"TS %c\n",cntrl->axis_list[0]);
			if (galil_send_cmd("motor_init",control,galil_cmd_str,buffer)!=DMCNOERROR)
				errlogPrintf("Error reading status switches at startup\n");
			else
				cntrl->switches=(epicsInt32)galil_conv_str_val(buffer);
		
			/*Test if the axis is moving, if it is, stop it.*/
			if (cntrl->switches & AXIS_INMOTION)
				{
				errlogPrintf("Stopping Axis %c\n",cntrl->axis_list[0]);
				sprintf(galil_cmd_str2,"ST %c",cntrl->axis_list[0]);
				if (galil_send_cmd("motor_init",control,galil_cmd_str2,buffer)!=DMCNOERROR)
					errlogPrintf("Error stopping moving axis during initialization\n");
				else
					{
					while (switches & AXIS_INMOTION)          /*keep reading inputs until the motor stops*/
						{
						galil_send_cmd("motor_init",control,galil_cmd_str,buffer);
						switches=(epicsInt32)galil_conv_str_val(buffer);
						}
					}
				}
			/*Update position now that all motors are stopped*/
			/*start status reads status and posi info for specified card by calling readG21X3status.
			if card ==-1 then the status and posi for all cards are read*/
			start_status(card_index);
			/* Read status of each motor */
			Debug(5, " G21X3:motor_init: calling set_status for motor %d\n",\
			motor_index);
			/*read status from hardware, update internal variables*/
			set_status(card_index, motor_index);
			/*set init flag for user code, we have finished init*/
			sprintf(galil_cmd_str,"init=1");
			if (galil_send_cmd("motor_init",control,galil_cmd_str,buffer)!=DMCNOERROR)
				errlogPrintf("Error setting initialize flag\n");
			}
		else
			{
			errlogPrintf("Galil G21X3 Controller Not Found\n");
			/*exit(0);*/
			/*motor_state[card_index] = (struct controller *) NULL;*/ /*This was the original code caused segmentation fault when controller wasnt found*/
			}
		}
   	 }
    Debug(5, "G21X3:motor_init: done with start_status and set_status\n");

    any_motor_in_motion = 0;

    mess_queue.head = (struct mess_node *) NULL;
    mess_queue.tail = (struct mess_node *) NULL;

    free_list.head = (struct mess_node *) NULL;
    free_list.tail = (struct mess_node *) NULL;

    Debug(5, "G21X3:motor_init: spawning G21X3_motor task\n");
    
    if (motor_count>0)
    	{
	epicsThreadCreate((char *) "G21X3_motor",epicsThreadPriorityMedium,epicsThreadGetStackSize(epicsThreadStackMedium),
			(EPICSTHREADFUNC) motor_task, (void *) &targs);
	}

    return (OK);
}

/*--------------------------------------------------------------------------------*/
/* Parse user input to G21X3NameConfig command */

int galil_parse_userinput(struct G21X3controller *control,
		      struct G21X3axis         *cntrl,
		      int axis, 
		      unsigned *num_axis,
		      int gantrymode,
		      int limit_as_home,
		      char *stepper_pos_maintenance,
		      char *slavedata,
		      int axis_slavehomeallowed[],
		      int motortype,
		      int *ssiencoderinput,
		      int auxencoder,
		      int mainencoder,
		      int digitalhome,
		      int *digitalaway)
{
	struct G21X3axis *cntrl_two;
	double slaveaxisgears[G21X3_NUM_CHANNELS];
	int slaveallowedtohome[G21X3_NUM_CHANNELS];
	char slaveaxisnames[G21X3_NUM_CHANNELS];
	char slaveencoderstr[G21X3_NUM_CHANNELS][G21X3_LINE_LENGTH];  //user defined encoder string
	char slaveencoderflag[G21X3_NUM_CHANNELS][G21X3_LINE_LENGTH]; //user defined encoder valid flags (variables)
	
	double ueres[G21X3_NUM_CHANNELS];                      //user defined encoder resolution
	double ueoff[G21X3_NUM_CHANNELS];		       //user defined encoder offset for user coordinate sys
							       
	char temp[G21X3_LINE_LENGTH];			       //temp buffer for workings.
	bool inc_slave;
	unsigned index,i,indexi;
	int uenc_ok=1;						//are the user encoder settings ok ?
	
	strcpy(slaveaxisnames,"");
	
	cntrl->gantry_mode = (gantrymode > 0) ? 1 : 0;
	cntrl->limit_as_home = (limit_as_home > 0) ? 1 : 0;

	if (digitalhome<0)
		*digitalaway = (*digitalaway == -1) ? 1 : 0;
	
	/*check user specified axis is not higher than the total number of user specified axis on this card/controller*/
	if (axis > (motor_state[control->card]->total_axis-1) || axis<0)
		{
		errlogPrintf("\n\nG21X3NameConfig: Axis Name should be A to %c\n\n",(motor_state[control->card]->total_axis-1)+65);
		return(ERROR);
		}
	
	/*Store axis name in axis struct*/
	cntrl->axis_list[0]=(char)(axis+65);
	cntrl->axis_list[1]='\0';
	*num_axis=1;
	
	//extract user encoder details for master
	//we use ueres and ueoff for convenience here
	sscanf(stepper_pos_maintenance,"%[^,],%lf,%lf,%s",temp,&ueres[0],&ueoff[0],cntrl->uenc_valid_flag);
	strcpy(stepper_pos_maintenance,temp);
		
	//set default for master encoder string
	strcpy(cntrl->uenc_str,"");
	
	//check user encoder & position maintenance settings for master
	if (strcmp(stepper_pos_maintenance,"")!=0 && stepper_pos_maintenance[0]!=48)
		{
		cntrl->stepper_pos_maintenance = 1;
		if ((strncmp(stepper_pos_maintenance,"uenc",4)==0) && 
		     stepper_pos_maintenance[4] == cntrl->axis_list[0] &&
		     stepper_pos_maintenance[5]== '=' &&
		     ueres[0] != 0.0 &&
		     strlen(cntrl->uenc_valid_flag) <= 6 &&
		     strlen(cntrl->uenc_valid_flag) >= 2)
		     	{
			cntrl->uenc_settings_valid = 1;
			cntrl->ueres = ueres[0];
			cntrl->ueoff = ueoff[0];
			strcpy(cntrl->uenc_str,stepper_pos_maintenance);
			}
		else
			{
			cntrl->uenc_settings_valid = 0;
			cntrl->ueres = 0.00025;
			cntrl->ueoff = 0;
			uenc_ok = 0;
			strcpy(cntrl->uenc_str,"");
			}
		}
	else
		{
		cntrl->stepper_pos_maintenance = 0;
		uenc_ok = 0;
		}
	
	/*put user input re slaves axis into the appropriate strings*/
	sscanf(slavedata,"%c=%lf,%d,%[^,],%lf,%lf,%s,%c=%lf,%d,%[^,],%lf,%lf,%s,%c=%lf,%d,%[^,],%lf,%lf,%s,%c=%lf,%d,%[^,],%lf,%lf,%s,%c=%lf,%d,%[^,],%lf,%lf,%s,%c=%lf,%d,%[^,],%lf,%lf,%s,%c=%lf,%d,%[^,],%lf,%lf,%s",&slaveaxisnames[0],&slaveaxisgears[0],&slaveallowedtohome[0],&slaveencoderstr[0][0],&ueres[0],&ueoff[0],&slaveencoderflag[0][0],&slaveaxisnames[1],&slaveaxisgears[1],&slaveallowedtohome[1],&slaveencoderstr[1][0],&ueres[1],&ueoff[1],&slaveencoderflag[1][0],&slaveaxisnames[2],&slaveaxisgears[2],&slaveallowedtohome[2],&slaveencoderstr[2][0],&ueres[2],&ueoff[2],&slaveencoderflag[2][0],&slaveaxisnames[3],&slaveaxisgears[3],&slaveallowedtohome[3],&slaveencoderstr[3][0],&ueres[3],&ueoff[3],&slaveencoderflag[3][0],&slaveaxisnames[4],&slaveaxisgears[4],&slaveallowedtohome[4],&slaveencoderstr[4][0],&ueres[4],&ueoff[4],&slaveencoderflag[4][0],&slaveaxisnames[5],&slaveaxisgears[5],&slaveallowedtohome[5],&slaveencoderstr[5][0],&ueres[5],&ueoff[5],&slaveencoderflag[5][0],&slaveaxisnames[6],&slaveaxisgears[6],&slaveallowedtohome[6],&slaveencoderstr[6][0],&ueres[6],&ueoff[6],&slaveencoderflag[6][0]);
	for (index=0;index<G21X3_NUM_CHANNELS;index++)
		{
		if (slaveaxisnames[index]-65<0 || slaveaxisnames[index]-65>7)  /*axis name not recognized, reject from here on*/
			slaveaxisnames[index]='\0';
		if (slaveaxisgears[index]>127.0 || slaveaxisgears[index]<-127.0)  /*gear ratio to high or too low,reject*/
			slaveaxisnames[index]='\0';
		if (slaveallowedtohome[index]!=0 && slaveallowedtohome[index]!=1)	/*parse slave allowed to home user input*/
			slaveaxisnames[index]='\0';
		}
	
	/*Parse specified slaves and insert into cntrl->axis_list*/
	if (slaveaxisnames!=NULL)
		for (index=0;index<strlen(slaveaxisnames);index++)
			{
			slaveaxisnames[index]=toupper(slaveaxisnames[index]);
			inc_slave=true;
			if (((slaveaxisnames[index]-65)>motor_state[control->card]->total_axis-1) || ((slaveaxisnames[index]-65)<0))
				{
				/*check user specified slaves*/
				errlogPrintf("\n\nG21X3NameConfig: Slave specified %c is Not Valid\n\n",slaveaxisnames[index]);
				inc_slave=false;
				}
			if (axis==(slaveaxisnames[index]-65))
				{
				errlogPrintf("\n\nG21X3NameConfig: Main axis %c and slave %c cannot be the same\n\n",(axis+65),slaveaxisnames[index]);
				inc_slave=false;
				}
				
			/*check if another master also has claimed this axis as a slave, if so, dont use the slave */
			for (i=0;i<(unsigned)axis;i++)
				{
				cntrl_two = (struct G21X3axis *)&control->axis[i];
				for (indexi=0;indexi<strlen(cntrl_two->axis_list);indexi++)
					if (cntrl_two->axis_list[indexi]==slaveaxisnames[index])
						{
						errlogPrintf("\n\nG21X3NameConfig: Slave specified %c is in Use, Master %c\n\n",slaveaxisnames[index],cntrl_two->axis_list[0]);
						inc_slave=false;
						}
				}
			/*Have we claimed this slave already, if so, dont use the slave for a second time*/
			for (i=0;i<index;i++)
				{
				for (indexi=0;indexi<strlen(cntrl->axis_list);indexi++)
					if (cntrl->axis_list[indexi]==slaveaxisnames[index])
						{
						errlogPrintf("\n\nG21X3NameConfig: Slave claimed by this axis already!\n");
						inc_slave=false;
						}
				}
			if (inc_slave==true)
				{
				//check slave encoder string
				cntrl_two = (struct G21X3axis *)&control->axis[slaveaxisnames[index]-65];
				if ((strncmp(slaveencoderstr[index],"uenc",4)==0) && 
				     slaveencoderstr[index][4] == slaveaxisnames[index] &&
				     slaveencoderstr[index][5] == '=' &&
				     ueres[index] != 0.0 &&
				      strlen(slaveencoderflag[index]) <= 6 &&
		     		      strlen(slaveencoderflag[index]) >= 2)
					{
					cntrl_two->uenc_settings_valid = 1;
					cntrl_two->ueres = ueres[index];
					cntrl_two->ueoff = ueoff[index];
					strcpy(cntrl_two->uenc_valid_flag,slaveencoderflag[index]);
					strcpy(cntrl_two->uenc_str,slaveencoderstr[index]);
					}
				else
					{
					cntrl_two->uenc_settings_valid = 0;
					cntrl_two->ueres = 0.00025;
					cntrl_two->ueoff = 0;
					uenc_ok = 0;
					strcpy(cntrl_two->uenc_str,"");
					}

				//save slave details in control structure
				*num_axis=*num_axis + 1;
				cntrl->axis_list[*num_axis-1]=slaveaxisnames[index];
				cntrl->axis_gear[*num_axis-1]=slaveaxisgears[index];
				if (slaveallowedtohome[index]==1 && gantrymode==0)
					axis_slavehomeallowed[*num_axis-1]=slaveallowedtohome[index];
				else
					axis_slavehomeallowed[*num_axis-1]=0;
					
				if (gantrymode==1 && slaveallowedtohome[index]==1)
					errlogPrintf("Cannot allow slave homming in Gantry mode, slave home has been turned off\n");
				cntrl->axis_list[*num_axis]='\0';
				}
			}
	
	/*Protect against using Aux Enc input while using a stepper motor on the axis*/
	/*Aux Enc Input is step count register when using steppers on the Galil*/
	if (motortype>=2 && *ssiencoderinput==2)
		{
		errlogPrintf("\n\nCannot Use Aux Enc Input for Encoder when using a stepper\n\n");
		errlogPrintf("\n\nSetting encoder to use Main Enc Input Instead\n\n");
		*ssiencoderinput=1;
		}
	
	/*Sort out encoder use and stepper_pos_maintenance use, from user settings*/
	if (motortype < 2)		/*motor is a servo*/
		{
		cntrl->stepper_pos_maintenance=0;
		if ((auxencoder>-1 || mainencoder>-1 || *ssiencoderinput>0))	
			{
			if (*ssiencoderinput>0)
				cntrl->encoder_valid=2;	/*SSI encoder type.  Encoder or main register*/
			else	
				cntrl->encoder_valid=1;	/*Quadrature encoder.  Encoder register type*/
			auxencoder=(auxencoder<0) ? 0 : auxencoder;
			mainencoder=(mainencoder<0) ? 0 : mainencoder;
			cntrl->encoder_setting=auxencoder+mainencoder;
			}
		else
			{
			errlogPrintf("\n\nUsing a Servo without an encoder is not allowed\n");
			return(ERROR);
			}
		}
	else					/*motor is a stepper*/
		{
		if ((mainencoder>-1) || (*ssiencoderinput==1) || (uenc_ok == 1))
			{
			if (*ssiencoderinput==1)
				{
				cntrl->encoder_valid=2;		/*SSI encoder type.  Encoder or main register*/
				cntrl->encoder_setting=0;
				}
			if (mainencoder>-1)
				{
				cntrl->encoder_setting=mainencoder;
				cntrl->encoder_valid=1;        /*Quadrature encoder.  Encoder register type*/
				}
			if (uenc_ok == 1)
				cntrl->encoder_setting=0;	/*User encoder.  Allows stepper_pos_maintenance = 1*/
			}					/*when using only user encoder*/
		else
			{
			cntrl->encoder_valid=0;				/*encoder settings were not valid*/
			cntrl->stepper_pos_maintenance=0;		/*cant do pos maintenance with no encoder*/
			cntrl->encoder_setting=0;
			}
		}
		
	return(OK);	
}

/*--------------------------------------------------------------------------------*/
/* Store axis setting in master data struct */

void galil_store_master(struct G21X3axis *cntrl,
			double egu_after_limit,
			int motortype,
			double homevalue,
			int homeprogreg,
			int invert_coordinates,
			int ssitotalbits,
			int ssierrbits,
			int ssidataform)
{
	/*Store master settings for later use*/
	cntrl->egu_after_limit=egu_after_limit;
	cntrl->motortype=motortype;
	cntrl->is_slave=0;
	cntrl->status_update=0;
	cntrl->init_update=1;
	cntrl->home_value=homevalue;
	cntrl->home_prog_reg=homeprogreg;	
	cntrl->invert_coordinates=invert_coordinates;
	cntrl->mtr_enc_scale = 1.0;
	cntrl->encoder_stall = 0;
	cntrl->ssi_connect = 0;
	cntrl->uenc_state = 0;
	cntrl->uenc_requested_state = 0;
	cntrl->ssitotalbits = ssitotalbits;
	cntrl->ssierrbits = ssierrbits;
	cntrl->ssidataform = ssidataform;
	cntrl->status_update = 1;
	cntrl->dev_init = 0;
	cntrl->stop_state = 0;
}

/*--------------------------------------------------------------------------------*/
/* Store motor enable settings for controller in main control structure */

void galil_store_motors_enable(struct G21X3controller *control,
			       struct G21X3axis *cntrl,
			       char *digitalhome_string,
			       int digitalaway)
{
	/*int digport = digitalhome * -1;		 The digital port we are dealing with now */
	int motors_index;			/* The current index for the motor array */
	int i,j,k;				/* General loop variable */
	char motor;				/* The motor we are adding to the list */
	char state = digitalaway;		/* The state we are adding to the list */
	struct G21X3motor_enables *motor_enables;
	bool done=false;
	int digport[8];
	int add_motor;				/* Add motor to list flag */
	
	//Provide default values for digport
	for (i=0;i<8;i++)
		digport[i] = 9;
	
	//retrieve port numbers from user specified parameter
	sscanf(digitalhome_string,"%d,%d,%d,%d,%d,%d,%d,%d",&digport[0],&digport[1],&digport[2],&digport[3],&digport[4],&digport[5],&digport[6],&digport[7]);
	
	k = 0;
	//Cycle through port list provided by user
	while (done == false)
		{
		if (digport[k] == 9)
			done = true;
		else
			{
			//Bring port specified back into +ve numbers
			if (digport[k]<0)
				digport[k] = digport[k] * -1;
				
			//Retrieve port structure
			motor_enables = (G21X3motor_enables *)&control->motor_enables[digport[k] - 1];
			motors_index = strlen(motor_enables->motors);
			
			//Add motors, and digital IO states provided into motor_enables structure
			for (i=0;i<(int)strlen(cntrl->axis_list);i++)
				{
				motor = cntrl->axis_list[i];
				add_motor = 1;
				// Check to make sure motor is not already in list
				for (j=0;j<motors_index;j++)
					{
					if (motor_enables->motors[j] == motor)
						add_motor = 0;
					}
				if (add_motor)
					{
					motor_enables->motors[motors_index] = motor;
					motor_enables->motors[motors_index + 1] = '\0';
					//Interrupt for digital IO can only be programmed to occur for one state
					//So we only pickup the last specified state for this digital in port
					motor_enables->disablestates[motors_index] = state;
					motor_enables->disablestates[motors_index + 1] = '\0';
					}
				}
			}
		k++;
		}
		
	//Print ports specified
	/*
	for (i=0;i<8;i++)
		{
		printf("Port %d\n",i);
		motor_enables = (G21X3motor_enables *)&control->motor_enables[i];
		for (j=0;j<8;j++)
			{
			printf("%d%d\n",motor_enables->motors[j],motor_enables->disablestates[j]);
			}
		}	*/
}

/*--------------------------------------------------------------------------------*/
/* Store axis setting in slave data struct */

void galil_store_slave(int i,
		       struct G21X3controller *control,
		       struct G21X3axis *cntrl,
		       struct G21X3axis *cntrl_slave,
		       int axis_slavehomeallowed[])
{
	/*if the current axis is not a master, set its cntrl variables up for mR use.  Master vars done already.. except home_direction_match*/
	
	cntrl_slave->axis_list[0]=cntrl->axis_list[i];		/*put slave name in slave cntrl axis list in 1st pos*/
	cntrl_slave->axis_list[1]=cntrl->axis_list[0];		/*put master axis name in slave axis list in 2nd pos*/	
	cntrl_slave->axis_list[2]='\0';
	cntrl_slave->axis_gear[0]=cntrl->axis_gear[i];		/*store slave gear ratio incase we need in another func*/
	cntrl_slave->home_value=cntrl->home_value;
	cntrl_slave->home_prog_reg=cntrl->home_prog_reg;
	cntrl_slave->limit_as_home=cntrl->limit_as_home;
	cntrl_slave->egu_after_limit=cntrl->egu_after_limit;
	cntrl_slave->motortype=cntrl->motortype;
	cntrl_slave->stepper_pos_maintenance=cntrl->stepper_pos_maintenance;
	cntrl_slave->encoder_valid=cntrl->encoder_valid;
	cntrl_slave->gantry_mode=cntrl->gantry_mode;
	cntrl_slave->is_slave=1;					/*indicate this is a slave, used in readstatus*/
	cntrl_slave->slaveallowedtohome=axis_slavehomeallowed[i];
	cntrl_slave->status_update=0;
	cntrl_slave->init_update=1;
	cntrl_slave->invert_coordinates=cntrl->invert_coordinates;
	cntrl_slave->mtr_enc_scale = 1.0;
	cntrl_slave->eres = .0025;
	cntrl_slave->encoder_stall = 0;
	cntrl_slave->ssi_connect = 0;
	cntrl_slave->uenc_state = 0;
	cntrl_slave->uenc_requested_state = 0;
	cntrl_slave->ssitotalbits = cntrl->ssitotalbits;
	cntrl_slave->ssierrbits = cntrl->ssierrbits;
	cntrl_slave->ssidataform = cntrl->ssidataform;
	cntrl_slave->status_update = 1;
	cntrl_slave->dev_init = 0;
	cntrl_slave->stop_state = 0;
}

/*--------------------------------------------------------------------------------*/
/* Setup motor and encoder settings on galil */

void galil_setup_motor_encoder(int i,
			       struct G21X3controller *control,
			       struct G21X3axis *cntrl,
			       int ssiencoderinput,
			       int ssitotalbits,
			       int ssisingleturnbits,
			       int ssierrbits,
			       int ssitimecode,
			       int ssidataform)
{
	char galil_cmd_str[G21X3_LINE_LENGTH]="\0";	/*holds the assembled Galil cmd string*/
	char buffer[G21X3_LINE_LENGTH] = "";		/* Response from Galil controller, used here to store command params */
	
	/*Assemble Motor type command str for Galil*/  /*setup motors*/
	switch (cntrl->motortype)
		{
		case 0: strcpy(buffer,"1");
			break;
		case 1: strcpy(buffer,"-1");
			break;
		case 2: strcpy(buffer,"-2");
			break;
		case 3: strcpy(buffer,"2");
			break;
		case 4: strcpy(buffer,"-2.5");
			break;
		case 5: strcpy(buffer,"2.5");
			break;
		default:break;
		}
		
	/*Set motor type*/
	sprintf(galil_cmd_str,"MT%c=%s",cntrl->axis_list[i],buffer);
	if (galil_send_cmd("galil_setup_motor_encoder",control,galil_cmd_str,buffer)!=DMCNOERROR)
		errlogPrintf("Error setting motor type.  Check galil jumper setting\n");
					
	/*Assemble Encoder setup command*/  /*setup encoders*/
	sprintf(galil_cmd_str,"CE%c=%d",cntrl->axis_list[i],cntrl->encoder_setting);
	if (galil_send_cmd("galil_setup_motor_encoder",control,galil_cmd_str,buffer)!=DMCNOERROR)
		errlogPrintf("Error setting encoder type\n");
				
	/*If using SSI, setup and issue command str to galil*/
	if (ssiencoderinput>0)
		{
		sprintf(galil_cmd_str,"SI%c=%d,%d,%d,%d<%d>%d",cntrl->axis_list[i],ssiencoderinput,ssitotalbits,ssisingleturnbits,ssierrbits,ssitimecode,ssidataform);
		if (galil_send_cmd("galil_setup_motor_encoder",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error setting up SSI encoder\n");
		}
	
	/*Turn the "Off on Error" function off for this motor/axis, includes slaves*/
	sprintf(galil_cmd_str,"OE%c=0",cntrl->axis_list[i]);
	if (galil_send_cmd("galil_setup_motor_encoder",control,galil_cmd_str,buffer)!=DMCNOERROR)
		errlogPrintf("Error turning off 'Off On Error'\n");
	
	/*finally assemble string to enable the specified motor on the Galil Controller*/
	sprintf(galil_cmd_str,"SH %c",cntrl->axis_list[i]);
	if (galil_send_cmd("galil_setup_motor_encoder",control,galil_cmd_str,buffer)!=DMCNOERROR)
		errlogPrintf("Error enabling motor\n");
}

/*--------------------------------------------------------------------------------*/
/* Initialize code buffers, insert program labels, set generator variables */


void galil_initialize_codegen(int i,
			      struct G21X3controller *control, 
			      struct G21X3axis *cntrl,
			      int digitalhome,
			      int digitalaway,
			      char *c,
			      char axis_thread_code[],
			      char axis_limit_code[],
			      char axis_dighome_code[])
{
	char axis_dighome_label[10]="#ININT\n";
	
	if (firstNameConfig[control->card]==1)	
		{
		firstNameConfig[control->card]=0;
				
		//setup #AUTO label
		strcpy(&card_code[control->card*G21X3_NUM_CHANNELS*(THREAD_CODE_LEN+LIMIT_CODE_LEN+INP_CODE_LEN)],"#AUTO\n");
		
		//setup #LIMSWI label	 
		strcpy(&card_limit_code[control->card*G21X3_NUM_CHANNELS*LIMIT_CODE_LEN],"#LIMSWI\n");
	
		//zero card buffers
		strcpy(&card_digital_code[control->card*G21X3_NUM_CHANNELS*INP_CODE_LEN],"");/*empty code buffers for axis limit/home and digital home progs*/
		strcpy(&card_thread_code[control->card*G21X3_NUM_CHANNELS*THREAD_CODE_LEN],"");
		}
	
	/*Empty code buffers for this axis*/
	strcpy(axis_thread_code,"");
	strcpy(axis_limit_code,"");
		
	//Insert code to start motor thread that will be constucted
	//thread 0 (motor A) is auto starting
	if (cntrl->axis_list[i]!='A')
		sprintf(&card_code[control->card*G21X3_NUM_CHANNELS*(THREAD_CODE_LEN+LIMIT_CODE_LEN+INP_CODE_LEN)],"%sXQ #THREAD%c,%d\n",&card_code[control->card*G21X3_NUM_CHANNELS*(THREAD_CODE_LEN+LIMIT_CODE_LEN+INP_CODE_LEN)],cntrl->axis_list[i],cntrl->axis_list[i]-65);
	
	//Insert label for motor thread we are constructing	
	sprintf(axis_thread_code,"%s#THREAD%c\n",axis_thread_code,cntrl->axis_list[i]);

	if (firstdigitalinput[control->card]==1 && digitalhome!=0 && digitalaway>=0 && i<1)	/*Setup ININT program label.*/
		{
		firstdigitalinput[control->card]=0;
		strcpy(axis_dighome_code,axis_dighome_label);
		// Insert code to initialize dpoff (digital ports off) used for motor interlocks management
		if (digitalhome<0)
			sprintf(axis_dighome_code,"%sdpoff=dpon\n",axis_dighome_code);
		}
	else
		strcpy(axis_dighome_code,"");
		
	*c = cntrl->axis_list[i];
}

/*--------------------------------------------------------------------------------*/
/* Generate the required limit code for this axis */

void galil_gen_limitcode(int i,
			 char c,
			 struct G21X3controller *control, 
			 struct G21X3axis *cntrl,
			 char axis_thread_code[],
			 char axis_limit_code[])
{
	char galil_cmd_str[G21X3_LINE_LENGTH]="\0";	/*holds the assembled Galil cmd string*/
	char buffer[G21X3_LINE_LENGTH] = "";		/* Response from Galil controller, used here to store command params */
	
	/*Setup thread 0 code needed to support LIMSWI routine*/
	/*code to reset deceleration after limit emergency decel*/
	sprintf(axis_thread_code,"%sIF ((rdecel%c=1) & (_BG%c=0) & (home%c=0))\nDC%c=oldecel%c;rdecel%c=0\nENDIF\n",axis_thread_code,c,c,c,c,c,c);
						
	/*setup code to reset limit active flags, if no limit is active on the specified axis*/
	/*see limit code below for reasoning behind limit flags*/
			
	sprintf(axis_thread_code,"%sIF ((_LR%c=1) & (_LF%c=1))\nlim%c=0\nENDIF\n",axis_thread_code,c,c,c);
	
	/*Setup the LIMSWI interrupt routine. The Galil Code Below, is called once per limit activate on ANY axis*/
	sprintf(axis_limit_code,"%sIF (((_LR%c=0) | (_LF%c=0)) & (hjog%c=0) & (lim%c=0))\noldecel%c=_DC%c;DC%c=limdc%c;ST%c;lim%c=1;rdecel%c=1\n",axis_limit_code,c,c,c,c,c,c,c,c,c,c,c);
	if (cntrl->stepper_pos_maintenance==1)
		{
		/*code for pos maintenance turn off when limits active*/
		if (!(cntrl->invert_coordinates & INVERT_POSMAIN))
			sprintf(axis_limit_code,"%sIF (posci%c=1)\nDP%c=(_DP%c/scale%c)*multi%c\nENDIF\ninps%c=0;posci%c=0\nENDIF\n",axis_limit_code,c,c,c,c,c,c,c);
		else
			sprintf(axis_limit_code,"%sIF (posci%c=1)\nDP%c=((_DP%c/scale%c)*multi%c)*-1\nENDIF\ninps%c=0;posci%c=0\nENDIF\n",axis_limit_code,c,c,c,c,c,c,c);
		}
	else
		sprintf(axis_limit_code,"%sENDIF\n",axis_limit_code);
	
	/*provide sensible default for limdc (limit deceleration) value*/
	sprintf(galil_cmd_str,"limdc%c=67107840\n",cntrl->axis_list[i]);
	if (galil_send_cmd("galil_gen_limitcode",control,galil_cmd_str,buffer)!=DMCNOERROR)
		errlogPrintf("Error telling controller our limit/home deceleration default \n");
	
	/*provide sensible default for lim (limit active flag) value*/
	sprintf(galil_cmd_str,"lim%c=0\n",cntrl->axis_list[i]);
	if (galil_send_cmd("galil_gen_limitcode",control,galil_cmd_str,buffer)!=DMCNOERROR)
		errlogPrintf("Error telling controller to set limit flag to zero\n");
				
	/*provide sensible default for rdecel (reset deceleration flag)*/
	sprintf(galil_cmd_str,"rdecel%c=0\n",cntrl->axis_list[i]);
	if (galil_send_cmd("galil_gen_limitcode",control,galil_cmd_str,buffer)!=DMCNOERROR)
		errlogPrintf("Error telling controller that a decel reset is not required\n");
				
	/*initialise homejogoff variable*/
	sprintf(galil_cmd_str,"hjog%c=0\n",cntrl->axis_list[i]);
	if (galil_send_cmd("galil_gen_limitcode",control,galil_cmd_str,buffer)!=DMCNOERROR)
		errlogPrintf("Error telling controller we are not doing a home jogoff\n");
}

/*--------------------------------------------------------------------------------*/
/* Generate code that is common to master and slave axis */


void galil_gen_commoncode(int i,
			  char c,
			  struct G21X3controller *control,
			  struct G21X3axis *cntrl,
			  char axis_thread_code[])
{
	char galil_cmd_str[G21X3_LINE_LENGTH]="\0";	/*holds the assembled Galil cmd string*/
	char buffer[G21X3_LINE_LENGTH] = "";		/* Response from Galil controller, used here to store command params */
	
	if ((i>0 && cntrl->gantry_mode==1) || (i>0 && cntrl->stepper_pos_maintenance==1) || (i==0 && cntrl->stepper_pos_maintenance==1))
		{
		/*setup code to determine direction*/
		/*initialise oldpos, dir, stop count, and backlash distance*/
		sprintf(galil_cmd_str,"oldpos%c=0\n",cntrl->axis_list[i]);
		if (galil_send_cmd("galil_gen_commoncode",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error telling controller previous position of axis\n");
				
		sprintf(galil_cmd_str,"dir%c=0\n",cntrl->axis_list[i]);
		if (galil_send_cmd("galil_gen_commoncode",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error telling controller initial axis travel direction\n");
				
		sprintf(galil_cmd_str,"scnt%c=0\n",cntrl->axis_list[i]);
		if (galil_send_cmd("galil_gen_commoncode",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error telling num cycles this axis has been stopped for\n");
			
		if (cntrl->stepper_pos_maintenance==1)
			{
			sprintf(galil_cmd_str,"bd%c=0\n",cntrl->axis_list[i]);
			if (galil_send_cmd("galil_gen_commoncode",control,galil_cmd_str,buffer)!=DMCNOERROR)
				errlogPrintf("Error telling the backlash distance\n");
					
			sprintf(galil_cmd_str,"bmovst%c=0\n",cntrl->axis_list[i]);
			if (galil_send_cmd("galil_gen_commoncode",control,galil_cmd_str,buffer)!=DMCNOERROR)
				errlogPrintf("Error telling controller our start position value\n");
						
			sprintf(galil_cmd_str,"bmovfin%c=0\n",cntrl->axis_list[i]);
			if (galil_send_cmd("galil_gen_commoncode",control,galil_cmd_str,buffer)!=DMCNOERROR)
				errlogPrintf("Error telling controller our end position value\n");
						
			sprintf(galil_cmd_str,"bldm%c=0\n",cntrl->axis_list[i]);
			if (galil_send_cmd("galil_gen_commoncode",control,galil_cmd_str,buffer)!=DMCNOERROR)
				errlogPrintf("Error setting backlash, last move direction match flag\n");
				
			sprintf(galil_cmd_str,"lbd%c=0\n",cntrl->axis_list[i]);
			if (galil_send_cmd("galil_gen_commoncode",control,galil_cmd_str,buffer)!=DMCNOERROR)
				errlogPrintf("Error setting last moved backlash distance\n");
			}
		
		/*setup code to determine direction*/
		sprintf(axis_thread_code,"%sIF ((oldpos%c-5)>_DE%c)\ndir%c=-1;scnt%c=0\nENDIF\n",axis_thread_code,c,c,c,c);
		sprintf(axis_thread_code,"%sIF ((oldpos%c+5)<_DE%c)\ndir%c=1;scnt%c=0\nENDIF\n",axis_thread_code,c,c,c,c);
									
		sprintf(axis_thread_code,"%sIF (oldpos%c=_DE%c)\nscnt%c=scnt%c+1\nENDIF\n",axis_thread_code,c,c,c,c);
		sprintf(axis_thread_code,"%soldpos%c=_DE%c\n",axis_thread_code,c,c);
		
		if (cntrl->stepper_pos_maintenance==1)
			{
			//backlash direction and last direction match flag
			sprintf(axis_thread_code,"%sIF (((dir%c>0) & (bd%c>0)) | ((dir%c<0) & (bd%c<0)))\n",axis_thread_code,c,c,c,c);
			sprintf(axis_thread_code,"%sbldm%c=1\n",axis_thread_code,c);
			sprintf(axis_thread_code,"%sELSE\n",axis_thread_code);
			sprintf(axis_thread_code,"%sIF (dir%c<>0)\n",axis_thread_code,c);
			sprintf(axis_thread_code,"%sbldm%c=0\n",axis_thread_code,c);
			sprintf(axis_thread_code,"%sENDIF\nENDIF\n",axis_thread_code);
					
			//Has the readback settled after stop ?
			//code to stop from slave limits depends on this
			sprintf(axis_thread_code,"%sIF ((oldpos%c=_DE%c) & (scnt%c>2) & (dir%c<>0))\ndir%c=0;bmovst%c=bmovfin%c;bmovfin%c=_DE%c\n",axis_thread_code,c,c,c,c,c,c,c,c,c);
					
			//store last move distance if backlash and last move directions match
			sprintf(axis_thread_code,"%sIF (bldm%c=1)\nlbd%c=lbd%c+(bmovfin%c-bmovst%c)\nELSE\n",axis_thread_code,c,c,c,c,c);
			sprintf(axis_thread_code,"%slbd%c=0\nENDIF\nENDIF\n",axis_thread_code,c);
			}
		else
			{
			//Has the readback settled after stop ?
			//code to stop from slave limits depends on this
			sprintf(axis_thread_code,"%sIF ((oldpos%c=_DE%c) & (scnt%c>2) & (dir%c<>0))\ndir%c=0\nENDIF\n",axis_thread_code,c,c,c,c,c);
			}
		}
}

/*--------------------------------------------------------------------------------*/
/* Generate the required code for this slave axis */

void galil_gen_slavecode(int i,
			 char c,
			 struct G21X3controller *control, 
			 struct G21X3axis *cntrl,
			 int slavecmd,
			 char axis_thread_code[])
{
	char galil_cmd_str[G21X3_LINE_LENGTH]="\0";	/*holds the assembled Galil cmd string*/
	char buffer[G21X3_LINE_LENGTH] = "";		/* Response from Galil controller, used here to store command params */
	int z;						/*general counter*/
	
	/*setup the slave axis on the G21X3*/
	if (i>0)
		{
		/*setup slave axis*/
		if (slavecmd==0)
			sprintf(galil_cmd_str,"GA%c=C%c",cntrl->axis_list[i],cntrl->axis_list[0]);  /*slave cmd is the master's cmded position*/
		else
			sprintf(galil_cmd_str,"GA%c=%c",cntrl->axis_list[i],cntrl->axis_list[0]);  /*slave cmd is the master's encoder*/
		if (galil_send_cmd("galil_gen_slavecode",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error Setting up slave axis\n");
	
		/*setup gear ratio*/
		sprintf(galil_cmd_str,"GR%c=%lf",cntrl->axis_list[i],cntrl->axis_gear[i]);
		if (galil_send_cmd("galil_gen_slavecode",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error Setting up gear ratio on slave axis\n");
					
		/*setup gantry mode*/
		if (cntrl->gantry_mode==1)
			{
			/*setup gantry mode*/
			sprintf(galil_cmd_str,"GM%c=%d",cntrl->axis_list[i],cntrl->gantry_mode);
			if (galil_send_cmd("galil_gen_slavecode",control,galil_cmd_str,buffer)!=DMCNOERROR)
				errlogPrintf("Error Setting up gear ratio on slave axis\n");
				
			//Insert code to stop master and thus all slaves, if this slave has an active limit  
			//and the slave is still heading toward the active limit.
			//After all, in gantry mode our master and slaves are tightly/mechanically coupled and cannot ignore limits
			sprintf(axis_thread_code,"%sIF (((_LR%c=0) & (dir%c=-1)) | ((_LF%c=0) & (dir%c=1)))\nST%c",axis_thread_code,c,c,c,c,cntrl->axis_list[0]);
			if (cntrl->stepper_pos_maintenance == 1)
				{
				for (z=0;z<(int)strlen(cntrl->axis_list);z++)
					sprintf(axis_thread_code,"%s;inps%c=0",axis_thread_code,cntrl->axis_list[z]);
				}
				
			sprintf(axis_thread_code,"%s\nENDIF\n",axis_thread_code);
			}
		}
}

/*--------------------------------------------------------------------------------*/
/* Generate position maintenance code for master and slave axis */

void galil_gen_pos_maintenance_code(int i,
				    char c,
				    struct G21X3controller *control,
				    struct G21X3axis *cntrl,
				    struct G21X3axis *cntrl_slave,
				    char axis_thread_code[])
{
	char galil_cmd_str[G21X3_LINE_LENGTH]="\0";	/*holds the assembled Galil cmd string*/
	char buffer[G21X3_LINE_LENGTH] = "";		/* Response from Galil controller, used here to store command params */
	unsigned index;
	
	if (cntrl->stepper_pos_maintenance == 1)
		{
		/*initialise variable*/
		sprintf(galil_cmd_str,"posci%c=0\n",c);
		if (galil_send_cmd("galil_gen_pos_maintenance_code",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error telling controller that we are have not issued a correction cmd\n");
			
		//initialize position maintenance state	
		sprintf(galil_cmd_str,"inps%c=0\n",c);
		if (galil_send_cmd("galil_gen_pos_maintenance_code",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error telling controller that we want position maintenance disabled for startup init\n");
				
		/*setup dummy scale for init, real scale gets set when set_enc in devG21X3 gets called later in init process*/
		/*the value set is based on mres and eres set in the database*/
		sprintf(galil_cmd_str,"scale%c=1.0000\n",c);
		if (galil_send_cmd("galil_gen_pos_maintenance_code",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error telling controller our motor/enc scaling ratio\n");
			
		//set initial value for scale multiplier.  This is needed to maintain accuracy
		//given G21x3 can only handle four decimal places on variables.
		sprintf(galil_cmd_str,"multi%c=1.0000\n",c);
		if (galil_send_cmd("galil_gen_pos_maintenance_code",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error telling controller our motor/enc multiplier value\n");
	
		/*setup dummy mnerr and mxerr, so G21X3 code doesnt crash when we start it*/
		/*They are calculated for real in set_enc in devG21X3 using mres from db*/
		sprintf(galil_cmd_str,"mnerr%c=5\n",c);
		if (galil_send_cmd("galil_gen_pos_maintenance_code",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error telling controller our min error for pos maintenance\n");
	
		sprintf(galil_cmd_str,"mxerr%c=10\n",c);
		if (galil_send_cmd("galil_gen_pos_maintenance_code",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error telling controller our max error for pos maintenance\n");
			
		//setup dummy position target
		sprintf(galil_cmd_str,"targ%c=999999\n",c);
		if (galil_send_cmd("galil_gen_pos_maintenance_code",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error telling controller our target\n");
					
		//setup backlash error direction match flag
		sprintf(galil_cmd_str,"bedm%c=0\n",c);
		if (galil_send_cmd("galil_gen_pos_maintenance_code",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error setting backlash error direction match flag\n");
			
		//generate code that gets readback from encoder
		if (((i == 0) && (cntrl->uenc_settings_valid == 0)) || ((i > 0) && (cntrl_slave->uenc_settings_valid == 0)))
			{
			//Do we need to invert SSI reading for pos maintenance ?
			if ((cntrl->encoder_valid == 2) && (cntrl->invert_coordinates & INVERT_ENCODER))
				sprintf(axis_thread_code,"%senpos%c=%ld - _DP%c\n",axis_thread_code,c,(long)(powerof(2,(cntrl->ssitotalbits - cntrl->ssierrbits)) - 1),c);
			else
				sprintf(axis_thread_code,"%senpos%c=_DP%c\n",axis_thread_code,c,c);
			}
		else
			{
			//user has requested custom encoder string
			sprintf(galil_cmd_str,"uencs%c=0\n",c);
			if (galil_send_cmd("galil_gen_pos_maintenance_code",control,galil_cmd_str,buffer)!=DMCNOERROR)
				errlogPrintf("Error setting user encoder state flag\n");
			sprintf(galil_cmd_str,"uencv%c=0\n",c);
			if (galil_send_cmd("galil_gen_pos_maintenance_code",control,galil_cmd_str,buffer)!=DMCNOERROR)
				errlogPrintf("Error setting user encoder validate flag\n");	
			if (i==0)
				{
				sprintf(axis_thread_code,"%s%s\n",axis_thread_code,cntrl->uenc_str);
				sprintf(axis_thread_code,"%sIF ((%s=1) & (uencs%c=1))\n",axis_thread_code,cntrl->uenc_valid_flag,c);
				sprintf(axis_thread_code,"%senpos%c=uenc%c\nELSE\n",axis_thread_code,c,c);
				//Do we need to invert SSI reading for pos maintenance ?
				if ((cntrl->encoder_valid == 2) && (cntrl->invert_coordinates & INVERT_ENCODER))
					sprintf(axis_thread_code,"%senpos%c=%ld - _DP%c\nENDIF\n",axis_thread_code,c,(long)(powerof(2,(cntrl->ssitotalbits - cntrl->ssierrbits)) - 1),c);
				else
					sprintf(axis_thread_code,"%senpos%c=_DP%c\nENDIF\n",axis_thread_code,c,c);
				}
			else
				{
				sprintf(axis_thread_code,"%s%s\n",axis_thread_code,cntrl_slave->uenc_str);
				sprintf(axis_thread_code,"%sIF ((%s=1) & (uencs%c=1))\n",axis_thread_code,cntrl_slave->uenc_valid_flag,c);
				sprintf(axis_thread_code,"%senpos%c=uenc%c\nELSE\n",axis_thread_code,c,c);
				//Do we need to invert SSI reading for pos maintenance ?
				if ((cntrl->encoder_valid == 2) && (cntrl->invert_coordinates & INVERT_ENCODER))
					sprintf(axis_thread_code,"%senpos%c=%ld - _DP%c\nENDIF\n",axis_thread_code,c,(long)(powerof(2,(cntrl_slave->ssitotalbits - cntrl_slave->ssierrbits)) - 1),c);
				else
					sprintf(axis_thread_code,"%senpos%c=_DP%c\nENDIF\n",axis_thread_code,c,c);
				}
			}
			
		//calculate the total error		
		if (!(cntrl->invert_coordinates & INVERT_POSMAIN))
			{
			sprintf(axis_thread_code,"%stoterr%c=(((enpos%c/scale%c)*multi%c)-_DE%c)*-1\nIF (@ABS[toterr%c]>mxerr%c)\ninps%c=0;posci%c=0\nENDIF\n",axis_thread_code,c,c,c,c,c,c,c,c,c);
			}
		else
			{
			sprintf(axis_thread_code,"%stoterr%c=(((enpos%c/scale%c)*multi%c)-(_DE%c*-1))\nIF (@ABS[toterr%c]>mxerr%c)\ninps%c=0;posci%c=0\nENDIF\n",axis_thread_code,c,c,c,c,c,c,c,c,c);
			}
					
		if (!(cntrl->invert_coordinates & INVERT_POSMAIN))
			sprintf(axis_thread_code,"%serr%c=@INT[((((enpos%c/scale%c)*multi%c)-targ%c)*-1)]\n",axis_thread_code,c,c,c,c,c);
		else
			sprintf(axis_thread_code,"%serr%c=@INT[(((enpos%c/scale%c)*multi%c)-(targ%c*-1))]\n",axis_thread_code,c,c,c,c,c);
			
		//Code to establish motor steps and encoder count correlation.
		//Needed for position maintenance to engage at all, else toterr prevents algorithm engaging.
		if (!(cntrl->invert_coordinates & INVERT_POSMAIN))
			sprintf(axis_thread_code,"%sIF ((dir%c=0) & (scnt%c=3) & (_BG%c=0))\nDP%c=(enpos%c/scale%c)*multi%c\nENDIF\n",axis_thread_code,c,c,c,c,c,c,c);
		else
			sprintf(axis_thread_code,"%sIF ((dir%c=0) & (scnt%c=3) & (_BG%c=0))\nDP%c=((enpos%c/scale%c)*multi%c)*-1\nENDIF\n",axis_thread_code,c,c,c,c,c,c,c);
			
		//code to enable slave gear ratio if pos maintenance stopped by user pressing the stop button
		//in such circumstances, the error will still be larger than the mnerr
		if ((strlen(cntrl->axis_list)>1) && (i==0))
			{
			index = 1;
			sprintf(axis_thread_code,"%sIF ((inps%c=0) & (@ABS[err%c]>mnerr%c) & (_BG%c=0))\n",axis_thread_code,c,c,c,c);
			while (i==0 && cntrl->axis_list[index]!='\0')
				{
				sprintf(axis_thread_code,"%sGR%c=oldgr%c\n",axis_thread_code,cntrl->axis_list[index],cntrl->axis_list[index]);
				index++;
				}
			sprintf(axis_thread_code,"%sENDIF\n",axis_thread_code);
			}
	
		sprintf(axis_thread_code,"%sIF ((inps%c<>0))\n",axis_thread_code,c);
			
		sprintf(axis_thread_code,"%sIF ((_BG%c=0) & (posci%c=0) & (_LR%c=1) & (_LF%c=1) & (dir%c=0))\n",axis_thread_code,c,c,c,c,c);
					
		if (i>0)
			sprintf(axis_thread_code,"%sIF (((_BG%c=0) & (_GR%c<>0.0)) | (_GR%c=0))\n",axis_thread_code,cntrl_slave->axis_list[1],c,c);
					
		sprintf(axis_thread_code,"%sIF ((@ABS[err%c]>mnerr%c) & (@ABS[err%c]<=mxerr%c))\n",axis_thread_code,c,c,c,c);
				
		//code to include backlash in any pos maintenance correction
		//Error equal to/greater than backlash distance and same sign, or error is 0, dont include backlash
		//Else include backlash (error = error - backlash)
		sprintf(axis_thread_code,"%sIF (((err%c>0) & (bd%c>0)) | ((err%c<0) & (bd%c<0)))\n",axis_thread_code,c,c,c,c);
		//backlash direction and error direction match flag
		sprintf(axis_thread_code,"%sbedm%c=1\n",axis_thread_code,c);
		sprintf(axis_thread_code,"%sELSE\n",axis_thread_code);
		sprintf(axis_thread_code,"%sbedm%c=0\nENDIF\n",axis_thread_code,c);
		
		sprintf(axis_thread_code,"%sIF (((@ABS[err%c]+@ABS[lbd%c])>=(@ABS[bd%c]-mnerr%c)) & (bedm%c=1) | (err%c=0))\n",axis_thread_code,c,c,c,c,c,c);
		sprintf(axis_thread_code,"%serr%c=err%c\n",axis_thread_code,c,c);
		sprintf(axis_thread_code,"%sELSE\n",axis_thread_code);
		sprintf(axis_thread_code,"%serr%c=err%c-bd%c\nENDIF\n",axis_thread_code,c,c,c);
					
		/*code to disable slave gear ratio before starting correction move*/
		/*for all slaves attached to this master*/
		index = 1;
		while (i==0 && cntrl->axis_list[index]!='\0')
			{
			sprintf(axis_thread_code,"%soldgr%c=_GR%c;GR%c=0.0\n",axis_thread_code,cntrl->axis_list[index],cntrl->axis_list[index],cntrl->axis_list[index]);
			index++;
			}
			
		sprintf(axis_thread_code,"%sIP%c=err%c;posci%c=1;inps%c=inps%c+1\nENDIF\nENDIF\n",axis_thread_code,c,c,c,c,c);
					
		if (i>0)
			sprintf(axis_thread_code,"%sENDIF\n",axis_thread_code);
				
		sprintf(axis_thread_code,"%sIF ((_BG%c=0) & (posci%c<>0))\nposci%c=0\n",axis_thread_code,c,c,c);
				
		/*code to enable slave gear ratio after correction move finished*/
		/*for all slaves attached to this master*/
		index = 1;
		while (i==0 && cntrl->axis_list[index]!='\0')
			{
			sprintf(axis_thread_code,"%sGR%c=oldgr%c",axis_thread_code,cntrl->axis_list[index],cntrl->axis_list[index]);
			if (index==strlen(cntrl->axis_list)-1)
				sprintf(axis_thread_code,"%s\n",axis_thread_code);
			else
				sprintf(axis_thread_code,"%s;",axis_thread_code);
			index++;
			}
				
		sprintf(axis_thread_code,"%sENDIF\n",axis_thread_code);
					
		sprintf(axis_thread_code,"%sENDIF\n",axis_thread_code);
		}
}

/*--------------------------------------------------------------------------------*/
/* setup digital inputs i.e. input interrupt automatic subroutine #ININT*/
/* Functions are 
	1.  EPS home and away.

   See also galil_gen_motor_enables_code, it too generates #ININT code
*/

void galil_gen_digitalcode(int i,
			   char c,
			   struct G21X3controller *control,
			   struct G21X3axis *cntrl,
			   int digitalhome,
			   int digitalaway,
			   char axis_dighome_code[])
{
	char galil_cmd_str[G21X3_LINE_LENGTH]="\0";	/*holds the assembled Galil cmd string*/
	char buffer[G21X3_LINE_LENGTH] = "";		/* Response from Galil controller, used here to store command params */

	if (digitalhome>=1 && digitalhome<=8 && digitalaway>=1 && digitalaway<=8 && digitalaway!=digitalhome && i<1)
		{		
		/* Generate digital home and away code.  EPS function */	
		/* initialise digsp%c variable for this axis with sensible default */
		sprintf(galil_cmd_str,"digsp%c=1\n",cntrl->axis_list[i]);
		if (galil_send_cmd("galil_gen_digitalcode",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error telling controller the default dighome speed\n");
		// go away (mirror in), forward 
		sprintf(axis_dighome_code,"%sIF ((@IN[%d]=0) & (@IN[%d]=1) & (_LF%c=1))\nJG%c=digsp%c\nBG%c\nENDIF\n",axis_dighome_code,digitalhome,digitalaway,c,c,c,c);
		// go home (mirror out), reverse
		sprintf(axis_dighome_code,"%sIF ((@IN[%d]=1) & (@IN[%d]=0) & (_LR%c=1))\nJG%c=digsp%c*(-1)\nBG%c\nENDIF\n",axis_dighome_code,digitalhome,digitalaway,c,c,c,c);
		}
}

/*--------------------------------------------------------------------------------*/
/* Generate home code.*/

void galil_gen_homecode(int i,
			char c,
			struct G21X3controller *control,
			struct G21X3axis *cntrl,
			struct G21X3axis *cntrl_slave,
			char axis_thread_code[])
{
	char galil_cmd_str[G21X3_LINE_LENGTH]="\0";	/*holds the assembled Galil cmd string*/
	char buffer[G21X3_LINE_LENGTH] = "";		/* Response from Galil controller, used here to store command params */
	unsigned index;
	
	/*Setup normal home code*/
	/*dont insert limit home jog code for slave axis, unless home is allowed*/
	if ((i<1 && cntrl->encoder_valid!=2) || (i>0 && cntrl_slave->slaveallowedtohome==1 && cntrl->encoder_valid!=2))
		{
		if (cntrl->limit_as_home==1)
			{
			/*hjog%c=1 we have found limit switch outer edge*/
			/*hjog%c=2 we have found limit switch inner edge*/
			/*hjog%c=3 we have found our final home pos*/
			sprintf(axis_thread_code,"%sIF ((home%c=1))\n",axis_thread_code,c);
			sprintf(axis_thread_code,"%sIF ((hjog%c=0) & (_BG%c=0) & ((_LR%c=0) | (_LF%c=0)))\nspeed%c=_SP%c;DC%c=hjgdc%c;JG%c=hjgsp%c;BG%c;hjog%c=1\nENDIF\n",axis_thread_code,c,c,c,c,c,c,c,c,c,c,c,c);
			sprintf(axis_thread_code,"%sIF ((_LR%c=1) & (_LF%c=1) & (hjog%c=1) & (_BG%c=1))\nST%c\nENDIF\n",axis_thread_code,c,c,c,c,c);
			if (cntrl->encoder_valid==1)
				{
				sprintf(axis_thread_code,"%sIF ((_LR%c=1) & (hjog%c=1) & (_BG%c=0) & (_LF%c=1))\nSP%c=speed%c;DC%c=67107840;FI%c;BG%c;hjog%c=2\nENDIF\n",axis_thread_code,c,c,c,c,c,c,c,c,c,c);
				sprintf(axis_thread_code,"%sIF ((hjog%c=2) & (_BG%c=0))\nhjog%c=3\nENDIF\n",axis_thread_code,c,c,c);
				sprintf(axis_thread_code,"%sIF ((_LR%c=1) & (_LF%c=1) & (hjog%c=3) & (_BG%c=0))\nWT10;hjog%c=0;home%c=0",axis_thread_code,c,c,c,c,c,c);
				if (cntrl->home_prog_reg==1)
					sprintf(axis_thread_code,"%s;DE%c=enhmval%c;DP%c=mrhmval%c",axis_thread_code,c,c,c,c);
				}
			else
				{
				sprintf(axis_thread_code,"%sIF ((_LR%c=1) & (_LF%c=1) & (hjog%c=1) & (_BG%c=0))\nSP%c=speed%c;hjog%c=3\nENDIF\n",axis_thread_code,c,c,c,c,c,c,c);
				sprintf(axis_thread_code,"%sIF ((_LR%c=1) & (_LF%c=1) & (hjog%c=3) & (_BG%c=0))\nWT10;hjog%c=0;home%c=0",axis_thread_code,c,c,c,c,c,c);
				if (cntrl->home_prog_reg==1)
					sprintf(axis_thread_code,"%s;DP%c=mrhmval%c",axis_thread_code,c,c);
				}
			}	
		else
			{
			sprintf(axis_thread_code,"%sIF ((home%c=1))\n",axis_thread_code,c);
			sprintf(axis_thread_code,"%sIF ((_HM%c=%d) & (hjog%c=0) & (_BG%c=1))\nST%c\nENDIF\n",axis_thread_code,c,cntrl->homesw_act,c,c,c);
			
			sprintf(axis_thread_code,"%sIF ((_HM%c=%d) & (hjog%c=0) & (_BG%c=0))\noldecel%c=_DC%c;speed%c=_SP%c;DC%c=hjgdc%c;JG%c=hjgsp%c;BG%c;hjog%c=1\nENDIF\n",axis_thread_code,c,cntrl->homesw_act,c,c,c,c,c,c,c,c,c,c,c,c);
			sprintf(axis_thread_code,"%sIF ((_HM%c=%d) & (hjog%c=1) & (_BG%c=1))\nST%c\nENDIF\n",axis_thread_code,c,cntrl->homesw_inact,c,c,c);
			
			if (cntrl->encoder_valid==1)
				{
				sprintf(axis_thread_code,"%sIF ((_HM%c=%d) & (hjog%c=1) & (_BG%c=0))\nDC%c=67107840;SP%c=speed%c;FI%c;BG%c;hjog%c=2\nENDIF\n",axis_thread_code,c,cntrl->homesw_inact,c,c,c,c,c,c,c,c);
				sprintf(axis_thread_code,"%sIF ((hjog%c=2) & (_BG%c=0))\nDC%c=oldecel%c;hjog%c=3\nENDIF\n",axis_thread_code,c,c,c,c,c);
				sprintf(axis_thread_code,"%sIF ((_HM%c=%d) & (hjog%c=3) & (_BG%c=0))\nWT10;hjog%c=0;home%c=0",axis_thread_code,c,cntrl->homesw_inact,c,c,c,c);
				if (cntrl->home_prog_reg==1)
					sprintf(axis_thread_code,"%s;DE%c=enhmval%c;DP%c=mrhmval%c",axis_thread_code,c,c,c,c);
				}
			else
				{
				sprintf(axis_thread_code,"%sIF ((_HM%c=%d) & (hjog%c=1) & (_BG%c=0))\nDC%c=oldecel%c;SP%c=speed%c;hjog%c=3\nENDIF\n",axis_thread_code,c,cntrl->homesw_inact,c,c,c,c,c,c,c);
				sprintf(axis_thread_code,"%sIF ((_HM%c=%d) & (hjog%c=3) & (_BG%c=0))\nWT10;hjog%c=0;home%c=0",axis_thread_code,c,cntrl->homesw_inact,c,c,c,c);
				if (cntrl->home_prog_reg==1)
					sprintf(axis_thread_code,"%s;DP%c=mrhmval%c",axis_thread_code,c,c);
				}
			}
		sprintf(axis_thread_code,"%s\nENDIF\n",axis_thread_code);
		sprintf(axis_thread_code,"%sENDIF\n",axis_thread_code);
		}
		
	/*initialise home variable for this axis, set to not homming just yet.  Set to homming only when doing a home*/
	sprintf(galil_cmd_str,"home%c=0\n",cntrl->axis_list[i]);
	if (galil_send_cmd("galil_gen_homecode",control,galil_cmd_str,buffer)!=DMCNOERROR)
		errlogPrintf("Error telling controller that we are not yet doing a limit home\n");
				
	/*initialise motor home value variable, with sensible default.  Real val is set in set_enc in devG21x3*/
	sprintf(galil_cmd_str,"mrhmval%c=0\n",cntrl->axis_list[i]);
	if (galil_send_cmd("galil_gen_homecode",control,galil_cmd_str,buffer)!=DMCNOERROR)
		errlogPrintf("Error telling controller our encoder home value\n");
				
	/*initialise encoder home value variable, with sensible default.  Real val is set in set_enc in devG21x3*/
	if (cntrl->encoder_valid==1)
		{
		sprintf(galil_cmd_str,"enhmval%c=0\n",cntrl->axis_list[i]);
		if (galil_send_cmd("galil_gen_homecode",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error telling controller our encoder home value\n");
		}
				
	/*Setup SSI home code*/
	/*dont insert limit home jog code for slave axis, unless home is allowed*/
	if ((i<1 && cntrl->encoder_valid==2) || (i>0 && cntrl_slave->slaveallowedtohome==1 && cntrl->encoder_valid==2))
		{
		if (i==0)
			{
			//Code for programming master step registers once home
			sprintf(axis_thread_code,"%sIF ((home%c=1) & (_BG%c=0) & (_LR%c=1) & (_LF%c=1))\n",axis_thread_code,c,c,c,c);
			
			if (cntrl->stepper_pos_maintenance == 1)
				sprintf(axis_thread_code,"%sIF ((@ABS[err%c]<=mnerr%c))\n",axis_thread_code,c,c);
				
			if (cntrl->home_prog_reg == 1)
				sprintf(axis_thread_code,"%sDP%c=mrhmval%c\n",axis_thread_code,c,c);
				
			//Code for programming slave step registers once home
			if (strlen(cntrl->axis_list)>1)
				{
				//Code for programming slave step registers once home
				if (cntrl->stepper_pos_maintenance == 1)
					{
					index = 1;
					while (i==0 && cntrl->axis_list[index]!='\0')
						{
						sprintf(axis_thread_code,"%sIF ((@ABS[err%c]<=mnerr%c) & (_BG%c=0))\n",axis_thread_code,cntrl->axis_list[index],cntrl->axis_list[index],cntrl->axis_list[index]);
						index++;
						}
					}
					
				index = 1;
				while (i==0 && cntrl->axis_list[index]!='\0')
					{
					if (cntrl->home_prog_reg==1)
						{
						sprintf(axis_thread_code,"%sDP%c=_DP%c/scale%c\n",axis_thread_code,cntrl->axis_list[index],cntrl->axis_list[index],cntrl->axis_list[index]);
						}
					if (index==strlen(cntrl->axis_list)-1)
						sprintf(axis_thread_code,"%shome%c=0\n",axis_thread_code,c);
					index++;
					}
				
				if (cntrl->stepper_pos_maintenance == 1)
					{
					index = 1;
					while (i==0 && cntrl->axis_list[index]!='\0')
						{
						sprintf(axis_thread_code,"%sENDIF\n",axis_thread_code);
						index++;
						}
					}
				}	
			else
				sprintf(axis_thread_code,"%shome%c=0\n",axis_thread_code,c);
			if (cntrl->stepper_pos_maintenance == 1)
				sprintf(axis_thread_code,"%sENDIF\n",axis_thread_code);
			sprintf(axis_thread_code,"%sENDIF\n",axis_thread_code);
			}
			
		if (i>0)
			{
			//Code for programming slave step registers once independantly hommed
			sprintf(axis_thread_code,"%sIF ((home%c=1) & (_BG%c=0) & (_LR%c=1) & (_LF%c=1))\n",axis_thread_code,c,c,c,c);
			if (cntrl->stepper_pos_maintenance == 1)
				sprintf(axis_thread_code,"%sIF ((@ABS[err%c]<=mnerr%c))\n",axis_thread_code,c,c);
			if (cntrl->home_prog_reg==1)
				sprintf(axis_thread_code,"%sDP%c=mrhmval%c\n",axis_thread_code,c,c);
			sprintf(axis_thread_code,"%shome%c=0\n",axis_thread_code,c);
			if (cntrl->stepper_pos_maintenance == 1)
				sprintf(axis_thread_code,"%sENDIF\n",axis_thread_code);
			sprintf(axis_thread_code,"%sENDIF\n",axis_thread_code);
			}
		}
			
	//Add code that counts cpu cycles through thread 0
	if (cntrl->axis_list[0] == 'A')
		{
		sprintf(axis_thread_code,"%scounter=counter+1\n",axis_thread_code);
		
		//initialise counter variable
		sprintf(galil_cmd_str,"counter=0\n");
		if (galil_send_cmd("galil_gen_homecode",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error initializing counter variable\n");
		}
}


/*--------------------------------------------------------------------------------*/
/* Generate code end, and controller wide code eg. error recovery*/

void galil_gen_card_codeend(int card)
{
	struct G21X3controller *control;
	int digports=0,digport,digvalues=0;
	char galil_cmd_str[G21X3_LINE_LENGTH]="\0";	/*holds the assembled Galil cmd string*/
	char buffer[G21X3_LINE_LENGTH] = "";		/*Response from Galil controller, used here to store command params */
	int i;
	struct G21X3motor_enables *motor_enables;
	
	control = (struct G21X3controller *)motor_state[card]->DevicePrivate;

	/*Ensure motor interlock function is initially disabled*/
	sprintf(galil_cmd_str,"mlock=0\n");
	if (galil_send_cmd("galil_gen_card_codeend",control,galil_cmd_str,buffer)!=DMCNOERROR)
		errlogPrintf("Error disabling motor interlock function\n");

	/* Calculate the digports and digvalues required for motor interlock function */
	for (i=0;i<8;i++)
		{
		motor_enables = (G21X3motor_enables *)&control->motor_enables[i];
		if (strlen(motor_enables->motors) > 0)
			{
			digport = i + 1;  	//digital port number counting from 1 to 8
			digports = digports | (1 << (digport-1)); 
			digvalues = digvalues | (motor_enables->disablestates[0] << (digport-1));
			}
		}
	
	/* Activate input interrupts.  Two cases are EPS home, away function AND motor interlock function */
	if (firstdigitalinput[card]==0)
		{
		if (digports==0)
			{
			/* EPS home and away function */
			strcat(&card_code[card*G21X3_NUM_CHANNELS*(THREAD_CODE_LEN+LIMIT_CODE_LEN+INP_CODE_LEN)],"II 1,8\n");		/*code to enable dig input interrupt must be internal to G21X3*/
			}
		else
			{
			/*motor interlock.  output variable values to activate code embedded in thread A*/
			sprintf(galil_cmd_str,"dpon=%d;dvalues=%d;mlock=1\n",digports,digvalues);
			if (galil_send_cmd("galil_gen_card_codeend",control,galil_cmd_str,buffer)!=DMCNOERROR)
				errlogPrintf("Error enabling motor interlock function\n");
			}
		}
		
	
	//generate code end, only if axis are defined	
	if (control->num_axis != 0)
		{
		// Add galil program termination code
		if (firstdigitalinput[card]==0)
			{
			strcat(&card_limit_code[card*G21X3_NUM_CHANNELS*LIMIT_CODE_LEN],"RE 1\n");	/*we have written limit code, and we are done with LIMSWI but not prog end*/
			//Add controller wide motor interlock code to #ININT
			if (digports!=0)
				galil_gen_motor_enables_code(control);
	
			// Add code to end digital port interrupt routine, and end the prog
			strcat(&card_digital_code[card*G21X3_NUM_CHANNELS*INP_CODE_LEN],"RI 1\nEN\n");	
			}
		else
			strcat(&card_limit_code[card*G21X3_NUM_CHANNELS*LIMIT_CODE_LEN],"RE 1\nEN\n");   /*we have written limit code, and we are done with LIMSWI and is prog end*/
					
		/*Set unsolicited messages port to the ethernet port*/
		sprintf(galil_cmd_str,"CF A");
		if (galil_send_cmd("galil_gen_card_codeend",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error setting port for unsolicited messages\n");
					
		//Add command error handler
		strcat(&card_thread_code[card*G21X3_NUM_CHANNELS*THREAD_CODE_LEN],"#CMDERR\n");
		strcat(&card_thread_code[card*G21X3_NUM_CHANNELS*THREAD_CODE_LEN],"errstr=_ED\n");
		strcat(&card_thread_code[card*G21X3_NUM_CHANNELS*THREAD_CODE_LEN],"errcde=_TC\n");
		strcat(&card_thread_code[card*G21X3_NUM_CHANNELS*THREAD_CODE_LEN],"cmderr=cmderr+1\n");
		//restart crashed thread, skipping the faulty line that caused the fault
		strcat(&card_thread_code[card*G21X3_NUM_CHANNELS*THREAD_CODE_LEN],"XQ _ED3,_ED1,1\n");
		strcat(&card_thread_code[card*G21X3_NUM_CHANNELS*THREAD_CODE_LEN],"EN\n");
					
		/*Set cmderr counter to 0*/
		sprintf(galil_cmd_str,"cmderr=0");
		if (galil_send_cmd("galil_gen_card_codeend",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error setting cmd error counter to 0\n");
		}
}

/*--------------------------------------------------------------------------------*/
/* Generate code to stop motors if an disabled via digital IO transition*/
/* Generates code for #ININT */ 
/*
   See also galil_gen_digitalcode, it too generates #ININT code
*/

void galil_gen_motor_enables_code(struct G21X3controller *control)
{
	struct G21X3motor_enables *motor_enables;
	struct G21X3axis *cntrl;
	int i,j,k;
	int loop_end;
	int any = 0;
	
	for (i=0;i<8;i++)
		{
		// Load motor_enables structure for this digital port
		motor_enables = (G21X3motor_enables *)&control->motor_enables[i];
		// Generate if statement for this digital port
		if (strlen(motor_enables->motors)>0)
			{
			any = 1;
			sprintf(&card_digital_code[control->card*G21X3_NUM_CHANNELS*INP_CODE_LEN],"%sIF ((@IN[%d]=%d)\n",&card_digital_code[control->card*G21X3_NUM_CHANNELS*INP_CODE_LEN],(i + 1),(int)motor_enables->disablestates[0]);
			// Scan through all motors associated with the port
			for (j=0;j<(int)strlen(motor_enables->motors);j++)
				{
				cntrl = (struct G21X3axis *)&control->axis[motor_enables->motors[j]-65];
	
				//cycle through axis list.  The list can be from masters and slaves
				//slaves specify their master in position 1 in the list.  So we must guard against
				//processing axis_list positions greater than 0 for slaves
				if (cntrl->is_slave == 0)
					loop_end = (int)strlen(cntrl->axis_list);
				else
					loop_end = 1;
	
				// Scan through motor axis list (slaves)
				for (k=0;k<loop_end;k++)
					{
					if (k == (loop_end - 1))
						sprintf(&card_digital_code[control->card*G21X3_NUM_CHANNELS*INP_CODE_LEN],"%sST%c\n",&card_digital_code[control->card*G21X3_NUM_CHANNELS*INP_CODE_LEN],cntrl->axis_list[k]);
					else
						sprintf(&card_digital_code[control->card*G21X3_NUM_CHANNELS*INP_CODE_LEN],"%sST%c;\n",&card_digital_code[control->card*G21X3_NUM_CHANNELS*INP_CODE_LEN],cntrl->axis_list[k]);
					}
				}
			// Manipulate interrupt flag to turn off the interrupt on this port for one threadA cycle
			sprintf(&card_digital_code[control->card*G21X3_NUM_CHANNELS*INP_CODE_LEN],"%sdpoff=dpoff-%d\nENDIF\n",&card_digital_code[control->card*G21X3_NUM_CHANNELS*INP_CODE_LEN],(int)powerof(2,i));
			}
		}
	/* Re-enable input interrupt for all except the digital port(s) just serviced during interrupt routine*/
	/* ThreadA will re-enable the interrupt for the serviced digital ports(s) after 1 cycle */
	if (any != 0)
		strcat(&card_digital_code[control->card*G21X3_NUM_CHANNELS*INP_CODE_LEN],"II ,,dpoff,dvalues\n");
}

/*--------------------------------------------------------------------------------*/
/* React to user encoder faults on motors (eg. stop moving motors), set uenc_requested_state accordingly*/
/* enforces user encoder state rules for de-activated/faulty encoders, similar to UESTATE commands */

void process_user_encoder_faults(struct G21X3controller *control,
				 struct G21X3axis *cntrl)
{
	struct G21X3axis *cntrl_two,*cntrl_three;	/*working axis cntrl structures*/
	char galil_cmd_str[G21X3_LINE_LENGTH]="\0";          /*holds the assembled Galil cmd string*/	
	char buffer[G21X3_LINE_LENGTH]="\0";		    /*Holds the Galil response string*/
	int i;
	//an encoder state change has been requested
	//stop the motor so the step count register can be programmed for the new encoder
	//The state change will not be honoured by encoder_state_change until the motor stops
	if (cntrl->uenc_requested_state != cntrl->uenc_state)
		{		
		if (epicsMutexLock(control->G21X3Lock)==epicsMutexLockOK)
    			{		
			//if motor is a slave, stop the master if it moving and there is a non zero gear ratio
			//request a user encoder state change for the master encoder also if the state
			//requested for the slave is off
			if (cntrl->is_slave==1)
				{
				if (cntrl->moving == 1)
					{
					//retrieve control structure for master axis
					cntrl_two = (struct G21X3axis *)&control->axis[cntrl->axis_list[1]-65];
					if (cntrl_two->moving==1 && cntrl->axis_gear[0]!=0.0000)
						{
						//disable position maintenance
						//disable the master, and all slaves in the master list with non zero gear
						for (i=0;i<(int)strlen(cntrl_two->axis_list);i++)
							{
							//retrieve control structure for axis in master list
							cntrl_three = (struct G21X3axis *)&control->axis[cntrl_two->axis_list[i]-65];
							if ((i==0) || ((i>0) && (cntrl_three->axis_gear[0]!=0.0000)))
								{
								//disable position maintenance
								sprintf(galil_cmd_str,"inps%c=0",cntrl_three->axis_list[0]);
								if (galil_send_cmd("G21X3_build_trans",control,galil_cmd_str,buffer)!=DMCNOERROR)
									errlogPrintf("Error stopping position maintenance\n");
								}
							}
							
						//stop the motor
						//disable the master, and all slaves in the master list with non zero gear
						for (i=0;i<(int)strlen(cntrl_two->axis_list);i++)
							{
							//retrieve control structure for axis in master list
							cntrl_three = (struct G21X3axis *)&control->axis[cntrl_two->axis_list[i]-65];
							if ((i==0) || ((i>0) && (cntrl_three->axis_gear[0]!=0.0000)))
								{
								//stop the motor
								sprintf(galil_cmd_str,"ST %c",cntrl_three->axis_list[0]);
								if (galil_send_cmd("G21X3_build_trans",control,galil_cmd_str,buffer)!=DMCNOERROR)
									errlogPrintf("Error stopping position maintenance\n");
								}
							}
							
						//set uenc requested state to disabled
						//disable the master, and all slaves in the master list with non zero gear
						for (i=0;i<(int)strlen(cntrl_two->axis_list);i++)
							{
							//retrieve control structure for axis in master list
							cntrl_three = (struct G21X3axis *)&control->axis[cntrl_two->axis_list[i]-65];
							if ((i==0) || ((i>0) && (cntrl_three->axis_gear[0]!=0.0000)))
								{
								//it is not safe to arbitarily set the master user encoder requested state to 
								//enabled (ie. cntrl_two->uenc_requested_state = 1) without validity check
								//However, it is safe to set master user encoder to disabled here
								if (cntrl->uenc_requested_state == 0)
									cntrl_three->uenc_requested_state = cntrl->uenc_requested_state;
								}
							}
						}
					else
						{
						//the slave is moving independantly
						if (cntrl->stepper_pos_maintenance == 1)
							{
							//disable position maintenance
							sprintf(galil_cmd_str,"inps%c=0",cntrl->axis_list[0]);
							if (galil_send_cmd("G21X3_build_trans",control,galil_cmd_str,buffer)!=DMCNOERROR)
								errlogPrintf("Error stopping position maintenance\n");
							}
							
						//stop this motor
						sprintf(galil_cmd_str,"ST %c",cntrl->axis_list[0]);
						if (galil_send_cmd("G21X3_build_trans",control,galil_cmd_str,buffer)!=DMCNOERROR)
							errlogPrintf("Error stopping axis after user encoder state change whilst moving\n");
						}
					}
				else
					{
					//retrieve control structure for master axis
					cntrl_two = (struct G21X3axis *)&control->axis[cntrl->axis_list[1]-65];
					if (cntrl->axis_gear[0]!=0.0000)
						{
						//set uenc requested state to disabled
						//disable the master, and all slaves in the master list with non zero gear
						for (i=0;i<(int)strlen(cntrl_two->axis_list);i++)
							{
							//retrieve control structure for axis in master list
							cntrl_three = (struct G21X3axis *)&control->axis[cntrl_two->axis_list[i]-65];
							if ((i==0) || ((i>0) && (cntrl_three->axis_gear[0]!=0.0000)))
								{
								//it is not safe to arbitarily set the master user encoder requested state to 
								//enabled (ie. cntrl_two->uenc_requested_state = 1) without validity check
								//However, it is safe to set master user encoder to disabled here
								if (cntrl->uenc_requested_state == 0)
									cntrl_three->uenc_requested_state = cntrl->uenc_requested_state;
								
								}
							}
						}
					}
				}
			else
				{
				//Stop the master and all slaves with a non zero gear ratio
				//Request user encoder disable for all slaves with non zero gear
				if (cntrl->moving == 1)
					{
					for (i=0;i<(int)strlen(cntrl->axis_list);i++)
						{
						cntrl_three = (struct G21X3axis *)&control->axis[cntrl->axis_list[i]-65];
						if ((i==0) || ((i>0) && (cntrl->moving==1) && (cntrl_three->axis_gear[0]!=0.0000)))
							{
							//disable position maintenance
							sprintf(galil_cmd_str,"inps%c=0",cntrl_three->axis_list[i]);
							if (galil_send_cmd("G21X3_build_trans",control,galil_cmd_str,buffer)!=DMCNOERROR)
								errlogPrintf("Error stopping position maintenance\n");
										
							//stop the master and its slaves
							sprintf(galil_cmd_str,"ST %c",cntrl_three->axis_list[i]);
							if (galil_send_cmd("G21X3_build_trans",control,galil_cmd_str,buffer)!=DMCNOERROR)
								errlogPrintf("Error stopping axis after user encoder state change whilst moving\n");
							//it is not safe to arbitarily set the slave user encoder requested state to 
							//enabled (ie. cntrl_two->uenc_requested_state = 1) without validity check
							//However, it is safe to set master user encoder to disabled here
							if (cntrl->uenc_requested_state == 0)
								cntrl_three->uenc_requested_state = 0;
							}
						}
					}
				else
					{
					for (i=0;i<(int)strlen(cntrl->axis_list);i++)
						{
						cntrl_three = (struct G21X3axis *)&control->axis[cntrl->axis_list[i]-65];
						if ((i==0) || ((i>0) && (cntrl_three->axis_gear[0]!=0.0000)))
							{
							//it is not safe to arbitarily set the slave user encoder requested state to 
							//enabled (ie. cntrl_two->uenc_requested_state = 1) without validity check
							//However, it is safe to set master user encoder to disabled here
							
							if (cntrl->uenc_requested_state == 0)
								cntrl_three->uenc_requested_state = 0;
							}
						}
					}
				
				}
				
			/* Free the lock */
    			epicsMutexUnlock(control->G21X3Lock);
			}
		}
}


/*-----------------------------------------------------------------------------------*/
/*  Dump galil code generated by this driver (based on user settings) to file
*/

void galil_write_gen_codefile(int card)
{
	FILE *fp;
	int i=0;
	
	fp = fopen("./G21X3_generated.gmc","w");

	if (fp != NULL)
		{
		while (card_code[i+(card*G21X3_NUM_CHANNELS*(THREAD_CODE_LEN+LIMIT_CODE_LEN+INP_CODE_LEN))]!='\0')
			{
			fputc(card_code[i+(card*G21X3_NUM_CHANNELS*(THREAD_CODE_LEN+LIMIT_CODE_LEN+INP_CODE_LEN))],fp);
			i++;
			}
		fclose(fp);
		}
	else
		printf("Could not open ./G21X3_generated.gmc");
}

/*-----------------------------------------------------------------------------------*/
/*  Load the galil code specified for the card number specified
*/

void galil_read_codefile(int card,char *code_file)
{
	int i = 0;
	char user_code[G21X3_NUM_CHANNELS*(THREAD_CODE_LEN+LIMIT_CODE_LEN+INP_CODE_LEN)];
	char file[256];
	FILE *fp;

	if (strcmp(code_file,"")!=0)
		{
		strcpy(file,code_file);

		fp = fopen(file,"r");

		if (fp != NULL)
			{
			//Read the specified galil code
			while (!feof(fp))
				{
				user_code[i] = fgetc(fp);
				i++;
				}
			fclose(fp);
			user_code[i] = '\0';
	
			//Filter code
			for (i=0;i<(int)strlen(user_code);i++)
				{
				//Filter out any REM lines
				if (user_code[i]=='R' && user_code[i+1]=='E' && user_code[i+2]=='M')
					{
					while (user_code[i]!='\n' && user_code[i]!=EOF)
						i++;
					}
				}
	
			//Terminate the code buffer, we dont want the EOF character
			user_code[i-1] = '\0';
	
			strcpy(&card_code[card*G21X3_NUM_CHANNELS*(THREAD_CODE_LEN+LIMIT_CODE_LEN+INP_CODE_LEN)],user_code);
			}
		else
			errlogPrintf("\ngalil_read_codefile: Can't open user code file, using generated code\n\n");
		}
}

/*-----------------------------------------------------------------------------------*/
/*  The Galil puts a space in front of every return string, this function removes the space, and converts buffer to double
*/

double galil_conv_str_val(char *buffer)
{
	int index=0;
	double val=0;
	while (buffer[index]!='\0')
		{
		buffer[index]=buffer[index+1];
		index++;
		}
	val=atof(buffer);
	return(val);
}

/*-----------------------------------------------------------------------------------*/
/*  Sends command string to the galil controller
*/

long galil_send_cmd(char callingfunction[], struct G21X3controller *control, char command[], char resp[])
{
	int retry_counter=0;		/*retry counter*/
	int retry_max=200;		/*maximum allowed retries*/
	long rc=DMCERROR_COMMAND;	/*Galil Library return code*/
	resp[1] = '?';
	resp[2] = '\0';
	
	//Controller sometimes responds with ? to a valid command
	//We use below while loop to provide greater robustness
	while (rc==DMCERROR_COMMAND && rc!=DMCERROR_TIMEOUT && resp[1]=='?' && retry_counter<retry_max)
		{
		rc = DMCCommand(control->hdmc, command, resp, G21X3_LINE_LENGTH);
		if (rc!=DMCNOERROR || resp[1]=='?')
			{
			//special case where motor type jumper in hardware is wrong for user motor type selection
			//stop send retry.
			if ((resp[1] == '?') && (strncmp(command,"MT",2) == 0))
				retry_counter = retry_max;
			}
		if (rc==DMCERROR_COMMAND && rc!=DMCERROR_TIMEOUT && resp[1]=='?' && retry_counter<retry_max)
			retry_counter++;
		}
		
	//report any errors
	if ((rc!=DMCNOERROR && strncmp(command,"BP",2)!=0) && strncmp(command,"MG _BN",4)!=0)	//burn program command always gives timeout response
		{
		errlogPrintf("galil_send_cmd: Calling function %s\n",callingfunction);
		errlogPrintf("galil_send_cmd: Error sending: %s rc: %ld\n",command,rc);
		errlogPrintf("galil_send_cmd: Card response string: %s\n",resp);
		if (strncmp(command,"MT",2) == 0)
			errlogPrintf("galil_send_cmd: Check motor jumper settings on controller\n");
		}
	if ((rc!=DMCNOERROR && strncmp(command,"BP",2)==0))	//burn program command always gives timeout response
		rc = DMCNOERROR;
	
	return(rc);
}


/*------------------------------------------------------------------------------------------*/
/* Power of function*/
double powerof(int base, int power)
{
	double i;
	double result=1.0;
	for (i=0;i<power;i++)
		result *= base;
	return (result);
}

/*-----------------------------------------------------------------------------------*/
/* Checks ssi encoder connect status and sets connect flag
*/

double set_ssi_connectflag(struct G21X3axis *cntrl)
{
	double disconnect_val = 0.0;
	long disconnect_valtmp = 0;
	int i;
	//work out the value recieved when encoder disconnected
	if (cntrl->ssidataform == 2)
		{
		//First we do gray code encoders
		for (i=0;i<(cntrl->ssitotalbits - cntrl->ssierrbits);i++)
			{
			if (i % 2)
				disconnect_valtmp |= (long)powerof(2,i);
				
			}
		if (!(cntrl->invert_coordinates & INVERT_ENCODER))
			disconnect_val = (double)disconnect_valtmp;
		else
			disconnect_val = (powerof(2,cntrl->ssitotalbits - cntrl->ssierrbits) - 1) - disconnect_valtmp;
		}
	else
		{
		//last we do binary code encoders
		if (!(cntrl->invert_coordinates & INVERT_ENCODER))
			disconnect_val = (powerof(2,cntrl->ssitotalbits - cntrl->ssierrbits) - 1);
		else
			disconnect_val = 0;
		}
		
	//check if cntrl->encoder_posi == value recieved when encoder disconnected
	//set connect flag accordingly
	if ((cntrl->encoder_posi == disconnect_val))
		cntrl->ssi_connect = 0;
	else
		cntrl->ssi_connect = 1;
	return(disconnect_val);
}

/*-----------------------------------------------------------------------------------*/
/* Checks user encoder validity.  Mutex required before calling this function.
*/

int set_uenc_reading_valid_flag(struct G21X3controller *control,char axis_name, int test_slaves)
{
	int loop_end;
	int i;
	int any_invalid = 0;
	int all_valid;
	struct G21X3axis *cntrl;
	struct G21X3axis *cntrl_two;
	
	char galil_cmd_str[G21X3_LINE_LENGTH],buffer[G21X3_LINE_LENGTH];
	
	//retrieve motor control structure
	cntrl = (struct G21X3axis *)&control->axis[axis_name-65];
			
	//cycle through axis list.  The list can be from masters and slaves
	//slaves specify their master in position 1 in the list.  So we must guard against
	//processing axis_list positions greater than 0 for slaves 
	if (test_slaves == 1)
		loop_end = strlen(cntrl->axis_list);	
	else
		loop_end = 1;
		
	if (cntrl->is_slave == 1)
		loop_end = 1;

	for (i=0;i<loop_end;i++)
		{
		//retrieve control structure for the next axis in the specified axis's list of slaves
		cntrl_two = (struct G21X3axis *)&control->axis[cntrl->axis_list[i]-65];
		//process only slaves that have a non zero gear ratio in the specified axis list
		//process masters, and independant slave queries
		if ((cntrl_two->is_slave==1 && cntrl_two->axis_gear[0] != 0.0000) || (i==0))
			{
			//process user encoder, if user encoder is set up correctly
			if (cntrl_two->uenc_settings_valid == 1)
				{
				/*query controller for user encoder validity*/
				sprintf(galil_cmd_str,"%s=?",cntrl_two->uenc_valid_flag);
				galil_send_cmd("set_uenc_reading_valid_flag",control,galil_cmd_str,buffer);
				//if user encoder measurement is valid, read it.
				if (galil_conv_str_val(buffer) == 1.0000)
					{
					/*read user encoder*/
					sprintf(galil_cmd_str,"uenc%c=?",cntrl_two->axis_list[0]);
					galil_send_cmd("set_uenc_reading_valid_flag",control,galil_cmd_str,buffer);
					cntrl_two->uencoder_posi = (double)galil_conv_str_val(buffer);
					cntrl_two->uenc_reading_valid = 1;
					}
				else
					cntrl_two->uenc_reading_valid = 0;
									
				//if reading not valid, set user encoder state to off
				if (cntrl_two->uenc_reading_valid == 0)
					{
					sprintf(galil_cmd_str,"uencs%c=0",cntrl_two->axis_list[0]);
					galil_send_cmd("set_uenc_reading_valid_flag",control,galil_cmd_str,buffer);
					cntrl_two->uenc_requested_state = 0;
					any_invalid = 1;
					}
				}
			else
				{
				cntrl_two->uencoder_posi = 0;
				cntrl_two->uenc_reading_valid = 0;
				any_invalid = 1;
				}
			}
		}
	
	all_valid = (any_invalid == 1) ? 0 : 1;
	
	//disable user encoder for this axis and all its slaves as at least
	//one axis has a bad user encoder
	if (all_valid == 0)
		for (i=0;i<loop_end;i++)
			{
			//retrieve control structure for the next axis in the specified axis's list of slaves
			cntrl_two = (struct G21X3axis *)&control->axis[cntrl->axis_list[i]-65];
			cntrl_two->uenc_requested_state = 0;
			}
	return(all_valid);
}



/*-------------------------------------------------------------------------------------------*/
/*
 *      GalilSanityCheck()
 *      Check that addressing is valid,  and that we are connected to the specified controller
 *
 */

int GalilSanityCheck(unsigned short card, unsigned short signal,char parm[])
{
	struct G21X3controller *control;		
	/*
	*  Card specified should be within range known in the driver
	*/
	if (card >= G21X3_num_cards)
		{
		errlogPrintf("Galil: Invalid Card (C:) value: %d\n", card);
		return -1;
		}
		
	control=(struct G21X3controller *) motor_state[card]->DevicePrivate;
	
	/*
	*  Signal specified should be within range known in the driver
	*/
	if (signal>=motor_state[card]->total_axis)
		{
		if (control->num_axis != 0)
			{
			errlogPrintf("Galil: Invalid Axis (S:) value: %d\n", signal);
			return -1;
			}
		}
		
	if (control->connectok!=1)
		return -1;
	//Gear ratio monitor/setpoint
	if (strcmp(parm,"GR")==0)
		return(0);
	//digital input port monitor
	if (strcmp(parm,"DIGREAD")==0)
		return(0);
	//digital output port setpoint
	if (strcmp(parm,"DIGWRITE")==0)
		return(0);
	//analog input port monitor
	if (strcmp(parm,"AIREAD")==0)
		return(0);
	//analog output port setpoint
	if (strcmp(parm,"AOWRITE")==0)
		return(0);
	//jog setpoint
	if (strcmp(parm,"JOG")==0)
		return(0);
	//encoder connection status monitor
	if (strcmp(parm,"SCONN")==0)
		return(0);
	//encoder stall status monitor
	if (strcmp(parm,"ESTALL")==0)
		return(0);
	//variable monitor/setpoint
	if (strncmp(parm,"VAR",3)==0)
		return(0);
	//user encoder reading valid monitor
	if (strcmp(parm,"UEVALID")==0)
		return(0);
	//user encoder raw reading monitor
	if (strcmp(parm,"UERAW")==0)
		return(0);
	//user encoder dial reading monitor
	if (strcmp(parm,"UEDIAL")==0)
		return(0);
	//user encoder user reading monitor
	if (strcmp(parm,"UEUSER")==0)
		return(0);
	//user encoder validate command
	if (strcmp(parm,"UEVALIDATE")==0)
		return(0);
	//user encoder state command
	if (strcmp(parm,"UESTATE")==0)
		return(0);
	//encoder raw coordinate
	if (strcmp(parm,"NERAW")==0)
		return(0);
	//encoder dial coordinate
	if (strcmp(parm,"NEDIAL")==0)
		return(0);
	//encoder user coordinate
	if (strcmp(parm,"NEUSER")==0)
		return(0);
	//Motor stop that doesnt also stop motor record processing.
	if (strcmp(parm,"MTRSTOP")==0)
		return(0);
	//User defined command/monitor
	if (strncmp(parm,"CMD",3)==0)
		return(0);
	if (strcmp(parm,"MTRON")==0)
		return(0);
	if (strcmp(parm,"STEPSMOOTH")==0)
		return(0);
	if (strcmp(parm,"POSMAXERR")==0)
		return(0);
	if (strcmp(parm,"POSMINERR")==0)
		return(0);
	if (strcmp(parm,"MCONN")==0)
		return(0);
	return(-1);
}

/*-------------------------------------------------------------------------------------------*/
/*
 *      GalilReadReal()
 *      Read real value from controller that corresponds to the argument parm
 *
 */

int GalilReadReal(unsigned short card, unsigned short signal, char parm[],double *value)
{
	struct G21X3controller *control;		
	struct G21X3axis *cntrl;
	char galil_cmd_str[G21X3_LINE_LENGTH]="\0";          /*holds the assembled Galil cmd string*/	
	char buffer[G21X3_LINE_LENGTH]="\0";		    /*Holds the Galil response string*/
	char axis_name;
	int dig_byte1,dig_byte2;			    /*Galil digital input bytes*/
	int proceed = 0;				    /*used to catch comms errors*/
	int got_badcomms=0;
	int i;
	/*retrieve controller structure*/
	control=(struct G21X3controller *) motor_state[card]->DevicePrivate;
	if (control->connectok)
		{
		/*retrieve axis structure*/
		cntrl = (struct G21X3axis *)&control->axis[signal];
		/*get the axis name*/
		axis_name=cntrl->axis_list[0];
		
		while (proceed==0)
			{
			if (epicsMutexLock(control->G21X3Lock)==epicsMutexLockOK)
				proceed = 1;
			else
				epicsThreadSleep(.1);
			}
			
		/* Test for Device Response to Command.. Get Serial Number*/
		if (proceed==1)
			{
			strcpy(galil_cmd_str,"MG _BN");
			if (galil_send_cmd("GalilReadReal",control,galil_cmd_str,buffer)!=DMCNOERROR)
				{
				proceed = 0;
				epicsMutexUnlock(control->G21X3Lock);
				}
			}
			
		if (proceed == 1)
			{
			/*assemble command string based on parm*/	
			if (strcmp(parm,"GR")==0)
				{
				sprintf(galil_cmd_str,"GR%c=?",axis_name);
				if (galil_send_cmd("GalilReadReal",control,galil_cmd_str,buffer)==DMCNOERROR)
					{
					*value=(double)galil_conv_str_val(buffer);
					//update gear ratio in slave data structure
					cntrl->axis_gear[0] = *value;
					}
				else
					*value = cntrl->axis_gear[0];
				}
			if (strcmp(parm,"DIGREAD")==0)
				{
				if (control->model_num == 1410 || control->model_num == 1411 || control->model_num == 1412 || control->model_num == 1414 || control->model_num == 1417 || control->model_num == 1415 || control->model_num == 1416 || control->model_num == 1425)
					{
					/*read first byte*/
					sprintf(galil_cmd_str,"TI");
					if (galil_send_cmd("GalilReadReal",control,galil_cmd_str,buffer)==DMCNOERROR)
						{
						*value=(double)galil_conv_str_val(buffer);
						control->digital_in = (int)*value;
						}
					else
						*value = control->digital_in;
					}
				else
					{
					/*read first byte*/
					sprintf(galil_cmd_str,"TI0");
					got_badcomms=(galil_send_cmd("GalilReadReal",control,galil_cmd_str,buffer)==DMCNOERROR) ? 0 : 1;
					if (got_badcomms==0)
						{
						*value=(double)galil_conv_str_val(buffer);
						control->digital_in = (int)*value;
						}
					else
						*value = control->digital_in;
					
					dig_byte1 = (int)*value;
					if ((control->num_axis>4) || (control->num_axis == 0))
						{
						/*only if we have more than 4 axis do these BI's (DI's) actually exist*/
						/*or if device is Galil RIO_47200 PLC */
						/*read second byte*/
						sprintf(galil_cmd_str,"TI1");
						if (galil_send_cmd("GalilReadReal",control,galil_cmd_str,buffer)==DMCNOERROR)
							{
							*value=(double)galil_conv_str_val(buffer);
							
							dig_byte2 = (int)*value;
							
							if (got_badcomms==0)
								{
								*value = dig_byte1 + (dig_byte2 << 8);
								control->digital_in = (int)*value;
								}
							else
								*value = control->digital_in;
							}
						}
					}
				}
			if (strcmp(parm,"AIREAD")==0)
				{
				sprintf(galil_cmd_str,"MG @AN[%d]",signal);
				if (galil_send_cmd("GalilReadReal",control,galil_cmd_str,buffer)==DMCNOERROR)
					{
					*value=(double)galil_conv_str_val(buffer);
					//update analog in value in control structure
					control->analog_in[signal] = *value;
					}
				else
					*value = control->analog_in[signal];
				}
			if (strcmp(parm,"SCONN")==0)
				{
				*value = cntrl->ssi_connect;
				}
			if (strcmp(parm,"ESTALL")==0)
				{
				*value = cntrl->encoder_stall;
				}
			if (strncmp(parm,"VAR",3)==0)
				{
				strcpy(galil_cmd_str,"");
				for (i=3;i<(int)strlen(parm);i++)
					galil_cmd_str[i-3] = parm[i];
				galil_cmd_str[i-3] = '\0';
				sprintf(galil_cmd_str,"%s=?",galil_cmd_str);
				if (galil_send_cmd("GalilReadReal",control,galil_cmd_str,buffer)==DMCNOERROR)
					*value=(double)galil_conv_str_val(buffer);
				}
			//user encoder reading valid monitor
			if (strcmp(parm,"UEVALID")==0)
				{
				*value = cntrl->uenc_reading_valid;
				}
			//user encoder raw reading monitor
			if (strcmp(parm,"UERAW")==0)
				{
				*value = cntrl->uencoder_posi;
				}
			//user encoder dial reading monitor
			if (strcmp(parm,"UEDIAL")==0)
				{
				*value = cntrl->uencoder_posi * cntrl->ueres;
				}
			//user encoder user reading monitor
			if (strcmp(parm,"UEUSER")==0)
				{
				*value = (cntrl->uencoder_posi * cntrl->ueres) + cntrl->ueoff;
				}
			//user encoder state command
			if (strcmp(parm,"UESTATE")==0)
				{
				*value = cntrl->uenc_state;
				}
			//encoder raw reading monitor
			if (strcmp(parm,"NERAW")==0)
				{
				*value = cntrl->encoder_posi;
				}
			//encoder dial reading monitor
			if (strcmp(parm,"NEDIAL")==0)
				{
				*value = cntrl->encoder_posi * cntrl->neres;
				}
			//encoder user reading monitor
			if (strcmp(parm,"NEUSER")==0)
				{
				*value = (cntrl->encoder_posi * cntrl->neres) + cntrl->neoff;
				}	
			//user defined command/monitor
			if (strncmp(parm,"CMD",3)==0)
				{
				strcpy(galil_cmd_str,"");
				for (i=3;i<(int)strlen(parm);i++)
					galil_cmd_str[i-3] = parm[i];
				galil_cmd_str[i-3] = '\0';
				if (galil_send_cmd("GalilReadReal",control,galil_cmd_str,buffer)==DMCNOERROR)
					*value=(double)galil_conv_str_val(buffer);
				}
			//motor stop state
			if (strcmp(parm,"MTRSTOP")==0)
				{
				*value = cntrl->stop_state;
				}
			//motor on query
			if (strcmp(parm,"MTRON")==0)
				{
				sprintf(galil_cmd_str,"MG _MO%c",axis_name);
				if (galil_send_cmd("GalilReadReal",control,galil_cmd_str,buffer)==DMCNOERROR)
					{
					*value = (double)galil_conv_str_val(buffer);
					*value = ((int)*value == 0) ? 1 : 0;
					cntrl->on_state = (int)*value;
					}
				}
			//Stepper smoothing query
			if (strcmp(parm,"STEPSMOOTH")==0)
				{
				sprintf(galil_cmd_str,"MG _KS%c",axis_name);
				if (galil_send_cmd("GalilReadReal",control,galil_cmd_str,buffer)==DMCNOERROR)
					{
					*value = (double)galil_conv_str_val(buffer);
					cntrl->step_smooth = *value;
					}
				}

			if (strcmp(parm,"MCONN")==0)
				{
				sprintf(galil_cmd_str,"TS%c",axis_name);
				if (galil_send_cmd("GalilReadReal",control,galil_cmd_str,buffer)==DMCNOERROR)
					{
					*value = (double)galil_conv_str_val(buffer);
					unsigned state = (int)*value;
					//are both limits active
					if ((state & AXIS_REVLIMIT_INACTIVE) && (state & AXIS_FORLIMIT_INACTIVE))
						cntrl->motor_connected = 0;
					else
						cntrl->motor_connected = 1;
					*value = cntrl->motor_connected;
					}
				}
			epicsMutexUnlock(control->G21X3Lock);
			//Force this thread to sleep for short time, so others can get mutex if needed
    			epicsThreadSleep(.001);
			}
		else
			{
			if (strcmp(parm,"GR")==0)
				{
				*value=(double)cntrl->axis_gear[0];
				}
			if (strcmp(parm,"DIGREAD")==0)
				{
				*value = (int)control->digital_in;
				}
			if (strcmp(parm,"AIREAD")==0)
				{
				*value = (int)control->analog_in[signal];
				}
			if (strcmp(parm,"SCONN")==0)
				{
				*value = cntrl->ssi_connect;
				}
			if (strcmp(parm,"ESTALL")==0)
				{
				*value = cntrl->encoder_stall;
				}
			//user encoder valid monitor
			if (strcmp(parm,"UEVALID")==0)
				{
				*value = cntrl->uenc_reading_valid;
				}
			//user encoder raw readback monitor
			if (strcmp(parm,"UERAW")==0)
				{
				*value = cntrl->uencoder_posi;
				}
			//user encoder dial readback monitor
			if (strcmp(parm,"UEDIAL")==0)
				{
				*value = cntrl->uencoder_posi * cntrl->ueres;
				}
			//user encoder user readback monitor
			if (strcmp(parm,"UEUSER")==0)
				{
				*value = (cntrl->uencoder_posi * cntrl->ueres) + cntrl->ueoff;
				}
			//user encoder state command
			if (strcmp(parm,"UESTATE")==0)
				{
				*value = cntrl->uenc_state;
				}
			//encoder raw readback monitor
			if (strcmp(parm,"NERAW")==0)
				{
				*value = cntrl->encoder_posi;
				}
			//encoder dial readback monitor
			if (strcmp(parm,"NEDIAL")==0)
				{
				*value = cntrl->encoder_posi * cntrl->neres;
				}
			//encoder user readback monitor
			if (strcmp(parm,"NEUSER")==0)
				{
				*value = (cntrl->encoder_posi * cntrl->neres) + cntrl->neoff;
				}
			//motor stop state
			if (strcmp(parm,"MTRSTOP")==0)
				{
				*value = cntrl->stop_state;
				}
			//motor on query
			if (strcmp(parm,"MTRON")==0)
				{
				*value = cntrl->on_state;
				}
			//Step smooth query
			if (strcmp(parm,"STEPSMOOTH")==0)
				{
				*value = cntrl->step_smooth;
				}
			//Motor connected query
			if (strcmp(parm,"MCONN")==0)
				{
				*value = cntrl->motor_connected;
				}
			}
		//Below functions query only control structure
		
		//Position maintenance max error
		//Read from control structure to get value in egu
		if (strcmp(parm,"POSMAXERR")==0)
			{
			*value = cntrl->pos_maintenance_max_error;
			}
	
		//Position maintenance min error
		//Read from control structure to get value in egu
		if (strcmp(parm,"POSMINERR")==0)
			{
			*value = cntrl->pos_maintenance_min_error;
			}
		}
	return 2;	/*dont convert*/
}

/*-------------------------------------------------------------------------------------------*/
/*
 *      GalilWriteReal()
 *      Write real value to controller that corresponds to the argument parm
 *
 */

int GalilWriteReal(unsigned short card, unsigned short signal, char parm[], double value)
{
	struct G21X3controller *control;		
	struct G21X3axis *cntrl;				/*control struct of axis we are processing*/
	struct G21X3axis *cntrl_two;			    /*temporary control struct*/
	char galil_cmd_str[G21X3_LINE_LENGTH]="\0";          /*holds the assembled Galil cmd string*/	
	char buffer[G21X3_LINE_LENGTH]="\0";		    /*Holds the Galil response string*/
	char axis_name;
	int temp;					    /*temporary integer store of value*/
	int temp2;					    /*temporary integer store of value*/
	int proceed = 0;				    /*used to control function flow*/
	int i;
	int uenc_all_valid;		/*flag indicating axis and all slaves have valid uenc reading*/
	int loop_end;			/*used to guard against processing slave axis_list positions > 0*/
	
	/*retrieve controller structure*/
	control=(struct G21X3controller *) motor_state[card]->DevicePrivate;
	/*retrieve axis structure*/
	if (control->connectok)
		{
		cntrl = (struct G21X3axis *)&control->axis[signal];
		/*get the axis name*/
		axis_name=cntrl->axis_list[0];
		
		while (proceed==0)
			{
			if (epicsMutexLock(control->G21X3Lock)==epicsMutexLockOK)
				proceed = 1;
			else
				epicsThreadSleep(.1);
			}
			
		//Below functions set only control structure
		//Mutex required, but comms to controller not required
		//Controller MINERR (mnerr), MAXERR (mxerr) are set in devG21X3.cc
	
		//Position maintenance max error
		//Read from control structure to get value in egu
		if (strcmp(parm,"POSMAXERR")==0)
			cntrl->pos_maintenance_max_error = value;
	
		//Position maintenance min error
		//Read from control structure to get value in egu
		if (strcmp(parm,"POSMINERR")==0)
			cntrl->pos_maintenance_min_error = value;
			
		/* Test for Device Response to Command.. Get Serial Number*/
		if (proceed==1)
			{
			strcpy(galil_cmd_str,"MG _BN");
			if (galil_send_cmd("GalilWriteReal",control,galil_cmd_str,buffer)!=DMCNOERROR)
				{
				proceed = 0;
				epicsMutexUnlock(control->G21X3Lock);
				}
			}
		
		if (proceed == 1)
			{
			if (strcmp(parm,"GR")==0)
				{
				sprintf(galil_cmd_str,"GR%c=%lf",axis_name,value);
				//update gear ratio in slave data structure
				if (galil_send_cmd("GalilWriteReal",control,galil_cmd_str,buffer)==DMCNOERROR)
					cntrl->axis_gear[0] = value;
				if (cntrl->stepper_pos_maintenance == 1)
					{
					//update old gear ratio variable used for pos maintenance
					sprintf(galil_cmd_str,"oldgr%c=%lf",axis_name,value);
					galil_send_cmd("GalilWriteReal",control,galil_cmd_str,buffer);
					}
				}
			if (strcmp(parm,"DIGWRITE")==0)
				{
				temp = (int) value;
				if (control->model_num == 1410 || control->model_num == 1411 || control->model_num == 1412 || control->model_num == 1414 || control->model_num == 1417 || control->model_num == 1415 || control->model_num == 1416 || control->model_num == 1425)
					{
					temp = temp & MODEL_1400_DO_MASK;
					if (control->model_num == 1415 || control->model_num == 1416 || control->model_num == 1425)
						sprintf(galil_cmd_str,"OP %d,3",temp);
					else
						sprintf(galil_cmd_str,"OP %d",temp);
					if (galil_send_cmd("GalilWriteReal",control,galil_cmd_str,buffer)!=DMCNOERROR)
						errlogPrintf("Failure setting digital output\n");
					}
				else
					{ 
					/*21X3 models and others*/
					if (control->num_axis != 0)
						{
						if ((control->num_axis > 4)) 
							temp = temp & MODEL_8AXIS_DO_MASK;
						else
							temp = temp & MODEL_4AXIS_DO_MASK;
						sprintf(galil_cmd_str,"OP %d",temp);
						if (galil_send_cmd("GalilWriteReal",control,galil_cmd_str,buffer)!=DMCNOERROR)
							errlogPrintf("Failure setting digital output\n");
						}
					/*RIO and others*/
					if (control->num_axis == 0)
						{
						//mask out all bits except 0-7
						temp = temp & 255;
						//mask out all bits except 8-15
						temp2 = (int)value & 65280;
						//Down shift to get second byte value to output
						temp2 = temp2 >> 8;
						sprintf(galil_cmd_str,"OP %d,%d",temp,temp2);
						if (galil_send_cmd("GalilWriteReal",control,galil_cmd_str,buffer)!=DMCNOERROR)
							errlogPrintf("Failure setting digital output\n");
						}
					}
				}
			if (strcmp(parm,"AOWRITE")==0)
				{
				sprintf(galil_cmd_str,"AO %d,%f",signal,value);
				if (galil_send_cmd("GalilWriteReal",control,galil_cmd_str,buffer)!=DMCNOERROR)
					errlogPrintf("Failure setting analog output port %d\n",signal);
				}
			if (strcmp(parm,"JOG")==0)
				{
				if (value != 0.0000)
					{
					sprintf(galil_cmd_str,"JG%c=%lf",axis_name,value);
					if (galil_send_cmd("GalilWriteReal",control,galil_cmd_str,buffer)!=DMCNOERROR)
						errlogPrintf("Failure setting jog value\n");
						
					sprintf(galil_cmd_str,"BG%c",axis_name);
					if (galil_send_cmd("GalilWriteReal",control,galil_cmd_str,buffer)!=DMCNOERROR)
						errlogPrintf("Failure starting jog\n");
					}
				else	
					{
					sprintf(galil_cmd_str,"ST%c",axis_name);
					if (galil_send_cmd("GalilWriteReal",control,galil_cmd_str,buffer)!=DMCNOERROR)
						errlogPrintf("Failure stopping motor\n");
					}
				}
			if (strncmp(parm,"VAR",3)==0)
				{
				strcpy(galil_cmd_str,"");
				for (i=3;i<(int)strlen(parm);i++)
					galil_cmd_str[i-3] = parm[i];
				galil_cmd_str[i-3] = '\0';
				sprintf(galil_cmd_str,"%s=%lf",galil_cmd_str,value);
				if (galil_send_cmd("GalilWriteReal",control,galil_cmd_str,buffer)!=DMCNOERROR)
					errlogPrintf("Failure setting user variable\n");
				}
				
			//user encoder validate command
			if (strcmp(parm,"UEVALIDATE")==0 && cntrl->stop_state == 0)
				{
				if (cntrl->uenc_settings_valid==1)
					{
					if (value == 1)
						{
						if (cntrl->uenc_settings_valid == 1)
							{
							sprintf(galil_cmd_str,"uencv%c=1",axis_name);
							if (galil_send_cmd("GalilWriteReal",control,galil_cmd_str,buffer)!=DMCNOERROR)
								errlogPrintf("Failure issuing user encoder validate cmd\n");
							}
						}
					}
				}	
				
			//user encoder state command
			if (strcmp(parm,"UESTATE")==0)
				{
				//process user encoder, if user encoder is set up correctly	
				if (cntrl->uenc_settings_valid==1)
					{
					uenc_all_valid = set_uenc_reading_valid_flag(control,axis_name,1);
					//set user encoder state based on user command, and reading validity
					//we do all slaves with non zero gear ratio for the specified motor also
					
					//cycle through axis list.  The list can be from masters and slaves
					//slaves specify their master in position 1 in the list.  So we must guard against
					//processing axis_list positions greater than 0 for slaves 
					if (cntrl->is_slave == 0)
						loop_end = strlen(cntrl->axis_list);
					else
						loop_end = 1;	
					if (cntrl->moving == 0)
						for (i=0;i<loop_end;i++)
							{
							//retrieve control structure for the next axis in the specified axis's list of slaves
							cntrl_two = (struct G21X3axis *)&control->axis[cntrl->axis_list[i]-65];
							
							if ((cntrl_two->is_slave==1 && cntrl_two->axis_gear[0] != 0.0000) || (i==0))
								{
								if (value == 0 || value == 1)
									{
									if (value == 1 && uenc_all_valid == 0)
										value = 0;
									sprintf(galil_cmd_str,"uencs%c=%lf",cntrl->axis_list[i],value);
									if (galil_send_cmd("GalilWriteReal",control,galil_cmd_str,buffer)!=DMCNOERROR)
										errlogPrintf("Failure setting user encoder state\n");
			
									cntrl_two->uenc_requested_state = (int)value;
									}
								}
							}
					}
				}
			//motor stop command
			if (strcmp(parm,"MTRSTOP")==0)
				{
				if (value != 0.0000)
					{
					//turn off position maintenance, cancel hjog, home and posci (pos correction)
					for (i=0;i<(int)strlen(cntrl->axis_list);i++)
						{
						//retrieve control structure for the next axis in the specified axis's list of slaves
						cntrl_two = (struct G21X3axis *)&control->axis[cntrl->axis_list[i]-65];
						
						if ((cntrl_two->is_slave==1 && cntrl_two->axis_gear[0] != 0.0000) || (i==0))
							{
							if (cntrl->stepper_pos_maintenance==1)
								{
								sprintf(galil_cmd_str,"inps%c=0\n",cntrl->axis_list[i]);
								if (galil_send_cmd("STOP_AXIS",control,galil_cmd_str,buffer)!=DMCNOERROR)
									errlogPrintf("Error telling controller that we want position maintenance disabled\n");
								sprintf(galil_cmd_str,"posci%c=0\n",cntrl->axis_list[i]);
								if (galil_send_cmd("STOP_AXIS",control,galil_cmd_str,buffer)!=DMCNOERROR)
									errlogPrintf("Error telling controller that we want to cancel any position maintenance correction move\n");
								}
							
							/*cancel any home operations that may be underway*/
							sprintf(galil_cmd_str,"home%c=0\n",cntrl->axis_list[i]);
							if (galil_send_cmd("STOP_AXIS",control,galil_cmd_str,buffer)!=DMCNOERROR)
								errlogPrintf("Error telling controller to cancel any home procedures\n");
												
							/*cancel any home switch jog off operations that may be underway*/
							sprintf(galil_cmd_str,"hjog%c=0\n",cntrl->axis_list[i]);
							if (galil_send_cmd("STOP_AXIS",control,galil_cmd_str,buffer)!=DMCNOERROR)
								errlogPrintf("Error telling controller to cancel any home switch jog off procedures\n");
							}
						}
							
					/*Stop the axis, and its slaves*/
					sprintf(galil_cmd_str,"ST %c",cntrl->axis_list[0]);
					if (galil_send_cmd("STOP_AXIS",control,galil_cmd_str,buffer)!=DMCNOERROR)
						errlogPrintf("Error stopping axis\n");
						
					cntrl->stop_state = 1;
					}
				else
					cntrl->stop_state = 0;
				}
				
			//user defined command/monitor
			if (strncmp(parm,"CMD",3)==0)
				{
				strcpy(galil_cmd_str,"");
				for (i=3;i<(int)strlen(parm);i++)
					galil_cmd_str[i-3] = parm[i];
				galil_cmd_str[i-3] = '\0';
				galil_send_cmd("GalilWriteReal",control,galil_cmd_str,buffer);
				}
	
			//motor on command
			if (strcmp(parm,"MTRON")==0)
				{
				if ((int)value == 0)
					sprintf(galil_cmd_str,"MO%c",axis_name);
				else
					sprintf(galil_cmd_str,"SH%c",axis_name);
				if (galil_send_cmd("MTRON",control,galil_cmd_str,buffer)!=DMCNOERROR)
						errlogPrintf("Error changing motor on/off status\n");
				}
			//Step smooth command
			if (strcmp(parm,"STEPSMOOTH")==0)
				{
				sprintf(galil_cmd_str,"KS%c=%lf",axis_name,value);
				if (galil_send_cmd("STEPSMOOTH",control,galil_cmd_str,buffer)!=DMCNOERROR)
						errlogPrintf("Error changing motor step smoothing\n");
				}	
			epicsMutexUnlock(control->G21X3Lock);
			//Force this thread to sleep for short time, so others can get mutex if needed
    			epicsThreadSleep(.001);
			}
		}
	return 2;		/*dont convert*/
}
