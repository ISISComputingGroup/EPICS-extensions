/* File: devG21X3.cc                     */

/* Device Support Routines for motor record for Galil G21X3 Controller */
/*
 *      Original Author: Mark Clift
 *
 */


#define VERSION 1.4

#include 	<math.h>
#include        <string.h>
#include        <epicsMutex.h>
#include        <epicsExport.h>
#include        <epicsThread.h>
#include        "motorRecord.h"
#include        "motordevCom.h"
#include        "dmclnx.h"
#include        "drvG21X3.h"
#include 	"GalilInterface.h"
#include        <errlog.h>



#define STATIC static
extern struct driver_table G21X3_access;

#define NINT(f) (long)((f)>0 ? (f)+0.5 : (f)-0.5)
#define ABS(f) ((f)>0 ? (f) : -(f))

#define MOVING 1

/*----------------debugging-----------------*/
#ifdef __GNUG__
    #ifdef	DEBUG
	volatile int devG21X3Debug = 0;
	#define Debug(L, FMT, V...) { if(L <= devG21X3Debug) \
			{ printf("%s(%d):",__FILE__,__LINE__); \
			  printf(FMT,##V); } }
	epicsExportAddress(int, devG21X3Debug);
    #else
	#define Debug(L, FMT, V...)
    #endif
#else
    #define Debug()
#endif

/* Debugging levels:
 *      devG21X3Debug >= 3  Print new part of command and command string so far
 *                          at the end of G21X3_build_trans
 */


/* ----------------Create the dsets for devG21X3----------------- */
STATIC struct driver_table *drvtabptr;
STATIC long G21X3_init(int);
STATIC long G21X3_init_record(struct motorRecord *);
STATIC long G21X3_start_trans(struct motorRecord *);
STATIC RTN_STATUS G21X3_build_trans(motor_cmnd, double *, struct motorRecord *);
STATIC RTN_STATUS G21X3_end_trans(struct motorRecord *);

struct motor_dset devG21X3 =
{
    {8, NULL, (DEVSUPFUN)G21X3_init, (DEVSUPFUN)G21X3_init_record, NULL},
    motor_update_values,
    G21X3_start_trans,
    G21X3_build_trans,
    G21X3_end_trans
};
epicsExportAddress(dset,devG21X3);


/* --------------------------- program data --------------------- */
/* This table is used to define the command types */

static msg_types G21X3_table[] = {
    MOTION,     /* MOVE_ABS */
    MOTION,     /* MOVE_REL */
    MOTION,     /* HOME_FOR */
    MOTION,     /* HOME_REV */
    IMMEDIATE,  /* LOAD_POS */
    IMMEDIATE,  /* SET_VEL_BASE */
    IMMEDIATE,  /* SET_VELOCITY */
    IMMEDIATE,  /* SET_ACCEL */
    IMMEDIATE,  /* GO */
    IMMEDIATE,  /* SET_ENC_RATIO */
    INFO,       /* GET_INFO */
    MOVE_TERM,  /* STOP_AXIS */
    VELOCITY,   /* JOG */
    IMMEDIATE,  /* SET_PGAIN */
    IMMEDIATE,  /* SET_IGAIN */
    IMMEDIATE,  /* SET_DGAIN */
    IMMEDIATE,  /* ENABLE_TORQUE */
    IMMEDIATE,  /* DISABL_TORQUE */
    IMMEDIATE,  /* PRIMITIVE */
    IMMEDIATE,  /* SET_HIGH_LIMIT */
    IMMEDIATE   /* SET_LOW_LIMIT */
};

void up_scale_mtr_enc_scale(struct G21X3axis *cntrl); /*upscale enc/mtr scale factor in attempt to maintain resolution*/

int check_uenc_states(struct G21X3controller *control, struct G21X3axis *cntrl); 
						/*check user encoder states for the axis recieving go cmds*/
						/*make sure all axis involved in move have user encoder in same state*/ 
void basic_motion_parse(struct motorRecord *mr, struct G21X3controller *control, struct G21X3axis *cntrl,int command);
						//check for changes in velo, hvel, bdst, and mres
						//update controller variables accordingly
void encoder_state_change(struct motorRecord *mr, struct G21X3controller *control, struct G21X3axis *cntrl);
						     //check for changes in encoder state, eres, and off
						     //update any runtime variables/mr accordingly
void pos_maintenance_parse(struct motorRecord *mr, struct G21X3controller *control, struct G21X3axis *cntrl);
						     //check for changes in encoder state, mres, and bdst
						     //update any controller variables accordingly 
void check_readbacks_setpoints(struct motorRecord *mr, 
			       struct G21X3controller *control, 
			       struct G21X3axis *cntrl,
			       motor_cmnd command);
			       //when stopped, val is set to rbv
			       //dval is set to drbv
			       //not used anywhere
void sync_mr_cntrl_variables(struct motorRecord *mr, struct G21X3axis *cntrl);
						//synchronize mr and cntrl structure variables
						//helps reduce comms traffic

int motor_interlock_ok(struct G21X3controller *control, struct G21X3axis *cntrl, double digital_port_value); 
						//Check interlock of motor and all slaves before move begins

static struct board_stat **G21X3_cards;
static int move_type;				/*used to calculate position targets for stepper rel and ssi home moves*/
static double rel_dval;		  		/*stores relative move distance for next move */

/* --------------------------- program data --------------------- */



/* initialize device support for G21X3 stepper motor */
STATIC long G21X3_init(int after)
{
    long rtnval;
    Debug(1, "G21X3_init, after=%d\n", after);
    if (after == 0) {
        drvtabptr = &G21X3_access;
        Debug(1, "G21X3_init, calling driver initialization\n");
        (drvtabptr->init)();
    }

    rtnval = motor_init_com(after, *drvtabptr->cardcnt_ptr, drvtabptr,
                            &G21X3_cards);
    Debug(1, "G21X3_init, end of function\n");
    return(rtnval);
}


/* initialize a record instance */
STATIC long G21X3_init_record(struct motorRecord *mr)
{
    long rtnval;
    struct motor_trans *trans;
    struct mess_node *motor_call;
    struct controller *brdptr;
    struct G21X3controller *control;
    struct G21X3axis         *cntrl;
    int card, signal;
    double reg_step,reg_enc;
    double motor_enc_scale;
    char galil_cmd_str[G21X3_LINE_LENGTH]="\0";          /*holds the assembled Galil cmd string*/
    char buffer[G21X3_LINE_LENGTH]="\0";		        /*Holds the Galil response string*/

    Debug(1, "--------G21X3_init_record \n");
    rtnval = motor_init_record_com(mr, *drvtabptr->cardcnt_ptr,
                                   drvtabptr, G21X3_cards);
    /* We have a logic problem here.  motor_init has read in the motor
     * position in engineering units, but it did not yet know the resolution
     * to convert to steps. It thus did a divide by zero.
     * We have to wait till after calling
     * motor_init_record_com for mr->dpvt to be initialized, and then set
     * the rmp field of the record again */
    trans = (struct motor_trans *) mr->dpvt;
    motor_call = &(trans->motor_call);
    card = mr->out.value.vmeio.card;
    signal = mr->out.value.vmeio.signal;
    brdptr = (*trans->tabptr->card_array)[card];
    control = (struct G21X3controller *) brdptr->DevicePrivate;
    cntrl = (struct G21X3axis *)&control->axis[signal];
    
    /*Check home velocity setting*/
    mr->hvel=(mr->hvel==0.000000) ? mr->velo : mr->hvel;

    /*Setup initial values of rmp and rep*/
    mr->rmp=NINT(cntrl->motor_posi);		
    mr->rep=NINT(cntrl->encoder_posi);          
    
    //store initial motor record settings that are required later
    //these variables are used to track runtime changes to native encoder
    //resolution and offset.  User encoder resolution and offset already initialized
    cntrl->neres = mr->eres;
    cntrl->neoff = mr->off;
    
    //Use SSI/user encoder to calculate corresponding step value
    if ((cntrl->encoder_valid==2 && control->connectok==1 && cntrl->motortype>1) ||
        (cntrl->uenc_state==1 && control->connectok==1 && cntrl->motortype>1))
	{
	//set correct string to read encoder, based on active encoder type
	if (cntrl->encoder_valid==2 && cntrl->uenc_state==0)
		{
		sprintf(galil_cmd_str,"DP%c=?",cntrl->axis_list[0]);	
		motor_enc_scale=(double)(mr->mres/cntrl->neres);
		}
	else
		{
		sprintf(galil_cmd_str,"uenc%c=?",cntrl->axis_list[0]);
		motor_enc_scale=(double)(mr->mres/cntrl->ueres);
		//load motor record with user encoder resolution and offset
		mr->eres = cntrl->ueres;
		mr->off = cntrl->ueoff;
		}
		
	if (galil_send_cmd("G21X3_init_record",control,galil_cmd_str,buffer)!=DMCNOERROR)
		errlogPrintf("Error reading encoder\n");
	else
		{
		reg_enc=(double)galil_conv_str_val(buffer);
		
		/*invert ssi encoder*/
		if (cntrl->uenc_state==0)
			if (cntrl->invert_coordinates & INVERT_ENCODER)
				reg_enc = (double)(powerof(2,(cntrl->ssitotalbits - cntrl->ssierrbits)) - 1) - reg_enc;
			
		reg_step = (reg_enc / motor_enc_scale);
		
		//load motor counts, dial coordinate system
		cntrl->motor_posi = reg_step;
		mr->rmp=NINT(cntrl->motor_posi); 
		//load encoder counts, dial coordinate system
		if (cntrl->uenc_state == 1)
			cntrl->uencoder_posi = reg_enc;
		else
			cntrl->encoder_posi = reg_enc;
		mr->rep=NINT(reg_enc);
		
		/*output step count register value to galil controller*/
		sprintf(galil_cmd_str,"DP%c=%lf",cntrl->axis_list[0],reg_step);	
		if (galil_send_cmd("G21X3_init_record",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error setting step count register to encoder value\n");
			
		if (cntrl->stepper_pos_maintenance==1)
			{
			sprintf(galil_cmd_str,"targ%c=%d",cntrl->axis_list[0],mr->rmp);	
			if (galil_send_cmd("G21X3_init_record",control,galil_cmd_str,buffer)!=DMCNOERROR)
				errlogPrintf("Error setting position target\n");
				
			//we must reset lbd now that step count reg is loaded
			sprintf(galil_cmd_str,"lbd%c=0\n",cntrl->axis_list[0]);
			if (galil_send_cmd("G21X3_init_record",control,galil_cmd_str,buffer)!=DMCNOERROR)
				errlogPrintf("Error setting last moved backlash distance\n");
			}
		}
	}
    
    /*Restore any autosave data regarding motor positions*/
    /*only restore if motor step and enc register are 0 on hardware, and the initial dval (restored value) is not zero and if we connect ok to the controller*/
    if ((cntrl->motor_posi==0 && cntrl->encoder_posi==0 && mr->dval!=0 && control->connectok==1 && cntrl->uenc_state==0) ||
        (cntrl->motor_posi==0 && cntrl->uencoder_posi==0 && mr->dval!=0 && control->connectok==1 && cntrl->uenc_state==1))
	{
	//user encoder may be incremental type
	if (cntrl->uenc_state == 1)
		reg_enc = mr->dval/cntrl->ueres;
	else
		reg_enc = mr->dval/cntrl->neres;
		
	reg_step = mr->dval/mr->mres;
	
	if (cntrl->invert_coordinates & INVERT_MOTOR)
		reg_step=reg_step*-1;
		
	if ((cntrl->invert_coordinates & INVERT_ENCODER) && (cntrl->uenc_state == 0))
		reg_enc=reg_enc*-1;
		
	/*alter readbacks as we will be changing the register these readbacks are derived from*/
	cntrl->motor_posi = reg_step;
	if (cntrl->uenc_state == 1)
		cntrl->uencoder_posi = reg_enc;
	else
		cntrl->encoder_posi = reg_enc;
	mr->rmp = NINT(reg_step);
	mr->rep = NINT(reg_enc);
	
	if (cntrl->motortype>1 && cntrl->encoder_valid!=2)	/*For steppers*/
		{	
		/*output encoder register value to galil controller*/
		sprintf(galil_cmd_str,"DE%c=%lf",cntrl->axis_list[0],reg_enc);	
		if (galil_send_cmd("G21X3_init_record",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error setting step count register to restored value\n");
		
		/*output step count register value to galil controller*/
		sprintf(galil_cmd_str,"DP%c=%lf",cntrl->axis_list[0],reg_step);	
		if (galil_send_cmd("G21X3_init_record",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error setting step count register to restored value\n");
		}
	else					/*For servos*/
		{
		if (cntrl->encoder_valid==1)		/*Program encoder register only if quadrature encoder is used*/
			{
			/*output encoder register value to galil controller*/
			sprintf(galil_cmd_str,"DE%c=%lf",cntrl->axis_list[0],reg_enc);	
			if (galil_send_cmd("G21X3_init_record",control,galil_cmd_str,buffer)!=DMCNOERROR)
				errlogPrintf("Error setting step count register to restored value\n");
			
			}
		}
	}
    return(rtnval);
}


/* start building a transaction */
STATIC long G21X3_start_trans(struct motorRecord *mr)
{
    Debug(1, "--------G21X3_start_trans\n");
    long rtnval;
    rtnval = motor_start_trans_com(mr, G21X3_cards);
    return(rtnval);
}


/* end building a transaction */
STATIC RTN_STATUS G21X3_end_trans(struct motorRecord *mr)
{
    Debug(1, "--------G21X3_end_trans\n");
    RTN_STATUS rtnval;
    rtnval = motor_end_trans_com(mr, drvtabptr);
    return(rtnval);

}


/* add a part to the transaction */
STATIC RTN_STATUS G21X3_build_trans(motor_cmnd command, double *parms, struct motorRecord *mr)
{
	struct motor_trans *trans = (struct motor_trans *) mr->dpvt;
	struct mess_node *motor_call;
	struct controller *brdptr;
	struct G21X3controller *control;
	struct G21X3axis         *cntrl;                /*axis control structure for master/slave motors*/
	struct G21X3axis         *cntrl_slave;          /*axis control structure for slave motors*/
	double temp_calc,temp_calc2;			/*used for numerous calculations*/
	char galil_cmd_str[G21X3_LINE_LENGTH]="\0";      /*holds the assembled Galil cmd string*/
	char buffer[G21X3_LINE_LENGTH]="\0";		/*Holds the Galil response string*/
    	int i;		       				/*for loop indices*/
	int proceed=0;					/*are the required motor record fields set,
							can we comms to device ok ?*/
	bool ssi_ok = true;				/*blocks moves when ssi encoder is disconnected, includes slaves*/
	int uenc_ok = 1;				/*blocks moves when uenc_ok = 0, includes slaves */
	int loop_end;					/*Used to control cycling through axis list*/
							  
	double dval=0.0;
	long enc_disconnect_val, enc_position;
					
	double mr_motor_err=0, slave_err[G21X3_NUM_CHANNELS];/*keeps track of position errors*/
	int ival=0;
	RTN_STATUS rtnval=OK;
	int card, signal;
	int lock;				/*was the epics mutex lock successful ? */
	double digital_port_value;		/* digital port value read */
	
	if (parms != NULL)
		{
        	dval = parms[0]; /* I assume this is the record DVAL which you set*/
                         /* to move e.g. dval = 10mm and command = MOVE_ABS*/
        	ival = NINT(parms[0]);
    		}
	motor_call = &(trans->motor_call);
	card = motor_call->card;
	signal = motor_call->signal;
	brdptr = (*trans->tabptr->card_array)[card];

	Debug(11, "G21X3_build_trans: After brdptr command\n");

    	if (brdptr == NULL)
        	return(rtnval = ERROR);

    	control = (struct G21X3controller *) brdptr->DevicePrivate;
    	cntrl = (struct G21X3axis *)&control->axis[signal];
	cntrl_slave = NULL;
	
	Debug(1, "G21X3_build_trans: card=%d, signal=%d, command=%d, ival=%d"\
		" dval=%f, steps=%f\n",\
		card, signal, command, ival, dval, steps);

	Debug(1, "G21X3_build_trans: resolution=%f\n",resolution);
	if (G21X3_table[command] > motor_call->type)
		motor_call->type = G21X3_table[command];
	Debug(11, "G21X3_build_trans: After cntrl command\n");
	if (trans->state != BUILD_STATE)
		return(rtnval = ERROR);
	Debug(11, "G21X3_build_trans: After cntrl command\n");
	
	Debug(1, "build_trans: Top Of Switch command=%d, cntrl->moving=%d"\
		" GStat=%d-\n", command,cntrl->moving,cntrl->groupstatus);
	
	/*are the required motor record fields set*/
	if (mr->mres!=0.000000 && mr->velo!=0.000000)
		{
		if (cntrl->stepper_pos_maintenance==1 && mr->eres!=0.000000)
			proceed=1;
		else if (cntrl->stepper_pos_maintenance==0)
			proceed=1;
		}
		
	/*Obtain lock so we are the only process talking to the motor controller*/
	if ((lock=epicsMutexLock(control->G21X3Lock))!=epicsMutexLockOK)
		proceed = 0;
		
	/* Test for Device Response to Command.. read digital IO*/
	sprintf(galil_cmd_str,"TI");
	if (galil_send_cmd("G21X3_build_trans",control,galil_cmd_str,buffer)!=DMCNOERROR)
		{
		/*comms error, mark the appropriate msta bit to signify the error*/
		cntrl->status = G21X3_COMM_ERR;
		mr->msta = mr->msta | 4096;
		proceed = 0;
		}
	else
		{
		cntrl->status = OK;
		digital_port_value=(double)galil_conv_str_val(buffer);
		}
	
	if (proceed==1)
		{	
		
		//check for runtime encoder setting changes
		encoder_state_change(mr, control, cntrl);
		
		//check for runtime hvel, velo and mres setting changes
		basic_motion_parse(mr, control, cntrl, command);
		
		//check for setting changes that effect position maintenance variables
		pos_maintenance_parse(mr, control, cntrl);
		
		//when stopped, force val equal to rbv and dval equal to drbv
		//check_readbacks_setpoints(mr, control, cntrl, command);
		
		//synchronize mr and cntrl structure variables.
		sync_mr_cntrl_variables(mr, cntrl);
	
		switch (command) 
			{
			case MOVE_ABS:  /* command 0*/
					/*Assemble galil Abs move command*/
					if (cntrl->moving!=MOVING)
						{
						/*invert motor coordinates if instructed*/
						if (cntrl->invert_coordinates & INVERT_MOTOR)	
							dval=dval*-1.0;
						sprintf(galil_cmd_str,"PA%c=%lf",cntrl->axis_list[0],dval);
						if (galil_send_cmd("MOVE_ABS",control,galil_cmd_str,buffer)!=DMCNOERROR)
							errlogPrintf("Error issuing absolute move\n");
							
						move_type = MOVE_ABS;

						//Ensure home flag is 0
						sprintf(galil_cmd_str,"home%c=0",cntrl->axis_list[0]);
						if (galil_send_cmd("MOVE_REL",control,galil_cmd_str,buffer)!=DMCNOERROR)
							errlogPrintf("Setting home to 0 MOVE_REL\n");	
						}
					break;
			
			case MOVE_REL:  /* command 1*/
					/*Assemble galil Rel move command*/
					if (cntrl->moving!=MOVING)
						{
						/*invert motor coordinates if instructed*/
						if (cntrl->invert_coordinates & INVERT_MOTOR)	
							dval=dval*-1.0;
							
						//store dval from mR cmd to calculate position targets for the
						//motors just prior to starting move.  See GO command.
						rel_dval = dval;
						
						//must check user encoder validity if its on, before issuing move.
						//must check all slaves here also.
						if (cntrl->uenc_settings_valid == 1)
							set_uenc_reading_valid_flag(control,cntrl->axis_list[0],1);
					
						/*if we have pos maintenance on, we want it on for normal move*/
						if (cntrl->stepper_pos_maintenance==1)
							{
							//Set flag to activate position maintenance on controller after 
							//move has begun
							cntrl->pm_state = 1;
							//set flag to indicate we are processing a relative move
							move_type = MOVE_REL;
							}
							
						sprintf(galil_cmd_str,"PR%c=%lf",cntrl->axis_list[0],rel_dval);
						if (galil_send_cmd("MOVE_REL",control,galil_cmd_str,buffer)!=DMCNOERROR)
							errlogPrintf("Error issuing relative move\n");

						//Ensure home flag is 0
						sprintf(galil_cmd_str,"home%c=0",cntrl->axis_list[0]);
						if (galil_send_cmd("MOVE_REL",control,galil_cmd_str,buffer)!=DMCNOERROR)
							errlogPrintf("Setting home to 0 MOVE_REL\n");						}
					break;
			
			case HOME_FOR: 	
			case HOME_REV:/* command 2 and 3*/
					/*Assemble galil Home command*/
					if (cntrl->is_slave==0 || (cntrl->is_slave==1 && cntrl->slaveallowedtohome==1))
						{
						if (cntrl->moving!=MOVING)
							{
							if (cntrl->encoder_valid!=2)	/*If we are not using SSI*/
								{
								//define home parameters used in galil code for homing
								int dir = (mr->dir == 0) ? 1 : -1;
								int home_direction = (command == HOME_REV) ? 1 : -1;
								double hjgsp;				/*home jog speed */
								double hjgdc;				/*home jog decel */
								double mrhmval;				/*motor home value*/
								double enhmval;				/*encoder home value*/
							
								//Calculate home jog off speed
								hjgsp = ((mr->hvel/mr->mres)/10.0) * home_direction;
								//Calculate home decel required to take 1 second
								hjgdc = ((mr->hvel/mr->mres)/10.0) * home_direction;
								//Calculate the encoder home value and mtr home value each in cnt	
								if (cntrl->home_value != 0.0000)
									{
									enhmval = (double)((cntrl->home_value-cntrl->eoff)/mr->eres) * dir;
									mrhmval = (double)((cntrl->home_value-cntrl->eoff)/mr->mres) * dir;
									}
								else
									{
									enhmval = 0.0;
									mrhmval = 0.0;
									}

								//Update motor record val fields so mr doesnt think a new move has arrived
								//due to home process updating position
								mr->val = mr->lval = cntrl->home_value * home_direction;
								
								//Write home parameters to controller
								sprintf(galil_cmd_str,"hjgdc%c=%lf",cntrl->axis_list[0],hjgdc);
								if (galil_send_cmd("G21X3_build_trans",control,galil_cmd_str,buffer)!=DMCNOERROR)
									errlogPrintf("Error setting home jog deceleration\n");
									
								sprintf(galil_cmd_str,"hjgsp%c=%lf",cntrl->axis_list[0],hjgsp);
								if (galil_send_cmd("G21X3_build_trans",control,galil_cmd_str,buffer)!=DMCNOERROR)
									errlogPrintf("Error setting home jog speed\n");

								sprintf(galil_cmd_str,"mrhmval%c=%lf\n",cntrl->axis_list[0],rint(mrhmval));
								if (galil_send_cmd("G21X3_build_trans",control,galil_cmd_str,buffer)!=DMCNOERROR)
									errlogPrintf("Error telling controller motor home value\n");

								sprintf(galil_cmd_str,"enhmval%c=%lf\n",cntrl->axis_list[0],rint(enhmval));
								if (galil_send_cmd("G21X3_build_trans",control,galil_cmd_str,buffer)!=DMCNOERROR)
									errlogPrintf("Error telling controller encoder home value\n");

								/*calculate home velocity speed in motor steps per sec*/
								/*we need to do this because we use jog command*/
								/*SP command does not affect jog*/
								temp_calc2=(mr->hvel/mr->mres) * home_direction * -1;
									
								sprintf(galil_cmd_str,"JG%c=%lf",cntrl->axis_list[0],temp_calc2);
								if (galil_send_cmd("HOME_REV",control,galil_cmd_str,buffer)!=DMCNOERROR)
									errlogPrintf("Error Jogging home\n");
				
								/*recalculate limit deceleration given hvel (instead of velo) and allowed steps after home/limit activation*/
								temp_calc=(cntrl->egu_after_limit < mr->mres) ? mr->mres : cntrl->egu_after_limit; 
								/*suvat equation for acceleration*/
								temp_calc=(temp_calc2*temp_calc2)/((temp_calc/mr->mres)*2.0);
								sprintf(galil_cmd_str,"limdc%c=%lf\n",cntrl->axis_list[0],rint(temp_calc));
								if (galil_send_cmd("HOME_REV",control,galil_cmd_str,buffer)!=DMCNOERROR)
									errlogPrintf("Error telling controller our limit deceleration\n");
				
								/*set decel for home activated stop*/
								sprintf(galil_cmd_str,"DC%c=%lf",cntrl->axis_list[0],rint(temp_calc));
								if (galil_send_cmd("HOME_REV",control,galil_cmd_str,buffer)!=DMCNOERROR)
									errlogPrintf("Error Setting Home Decel\n");
				
								/*calculate acceleration */
								/*Moving the motor sets accel, homing doesn't so we do it ourselves based on hvel*/
								temp_calc=(mr->hvel/mr->mres)/mr->accl;
								sprintf(galil_cmd_str,"AC%c=%lf",cntrl->axis_list[0],temp_calc);
								if (galil_send_cmd("HOME_REV",control,galil_cmd_str,buffer)!=DMCNOERROR)
									errlogPrintf("Error setting home acceleration\n");
				
								/*tell controller which axis we are doing a home on*/ 
								/*We do this last as controller may reset jog speed once home%c is set to 1*/
								/*Sitting on the home limit is one example of this*/
								sprintf(galil_cmd_str,"home%c=1\n",cntrl->axis_list[0]);
								if (galil_send_cmd("HOME_REV",control,galil_cmd_str,buffer)!=DMCNOERROR)
									errlogPrintf("Error telling controller we are doing a home\n");

								//flag that we are doing a home
								cntrl->homming_now = 1;
								}
							}
						}
					break;
			
			case LOAD_POS: /*Command 4*/
					/*Load Galil with user set position*/
					/*Is dval in encoder or motor steps*/
					/*temp_calc is for step register, temp_calc2 is for encoder register*/
					if (cntrl->moving!=MOVING)
						{
						/*dval is in motor steps.*/
						temp_calc = dval;
						if (cntrl->invert_coordinates & INVERT_MOTOR)
							temp_calc = temp_calc * -1;
						/*output step count register value to galil controller*/
						sprintf(galil_cmd_str,"DP%c=%lf",cntrl->axis_list[0],temp_calc);	
						if (galil_send_cmd("LOAD_POS",control,galil_cmd_str,buffer)!=DMCNOERROR)
							errlogPrintf("Error setting step count register to user value\n");
						if (cntrl->encoder_valid == 1)
							{
							temp_calc = (dval*mr->mres)/mr->eres;
							/*output encoder count register value to galil controller*/
							sprintf(galil_cmd_str,"DE%c=%lf",cntrl->axis_list[0],temp_calc);	
							if (galil_send_cmd("LOAD_POS",control,galil_cmd_str,buffer)!=DMCNOERROR)
								errlogPrintf("Error setting encoder count register to user value\n");
							}		
						}
					break;
			
			case SET_VEL_BASE: 
					break;      /* The Galil G21X3 does not have a Base velocity, so ignore command with a break*/
			case SET_VELOCITY: /*Command 5 and 6*/
					/*Assemble Galil set velocity string*/
					//cycle through axis list.  The list can be from masters and slaves
					//slaves specify their master in position 1 in the list.  So we must guard against
					//processing axis_list positions greater than 0 for slaves 
					if (cntrl->is_slave == 0)
						{
						if ((cntrl->stepper_pos_maintenance == 1) && (cntrl->gantry_mode == 1))
							loop_end = strlen(cntrl->axis_list);
						else
							loop_end = 1;
						}
					else
						loop_end = 1;
					for (i=0;i<loop_end;i++)
						{
						//retrieve control structure for the next axis in the specified axis's list of slaves
						cntrl_slave = (struct G21X3axis *)&control->axis[cntrl->axis_list[i]-65];
						if (i==0)
							sprintf(galil_cmd_str,"SP%c=%lf",cntrl_slave->axis_list[0],dval);
						else
							sprintf(galil_cmd_str,"SP%c=%lf",cntrl_slave->axis_list[0],dval*cntrl_slave->axis_gear[0]);
							
						if (galil_send_cmd("SET_VELOCITY",control,galil_cmd_str,buffer)!=DMCNOERROR)
								errlogPrintf("Error setting velocity\n");
						}
					break;
			
			case SET_ACCEL:    /* command 7 */
					/*Issue galil accel and decel commands*/
					if (cntrl->moving!=MOVING)
						{
						for (i=0;i<2;i++)
							{
							if (i==0)
								sprintf(galil_cmd_str,"AC%c=%lf",cntrl->axis_list[0],dval);
							else
								sprintf(galil_cmd_str,"DC%c=%lf",cntrl->axis_list[0],dval);
							if (galil_send_cmd("SET_ACCEL",control,galil_cmd_str,buffer)!=DMCNOERROR)
								errlogPrintf("Error Setting Accel/Decel\n");
							}
						}
					break;
			case GO:           /* command 8 */
					if (cntrl->moving!=MOVING && cntrl->stop_state == 0)
						{		
						//check that all slaves to this motor have user encoder
						//in the same state.  Doesnt matter what the state is on/off
						//abort move if this condition doesnt hold
						if (check_uenc_states(control,cntrl)==0)
							uenc_ok = 0;

						//must set position targets for relative moves before move begins		
						if ((cntrl->stepper_pos_maintenance==1 && cntrl->pm_state==1 && move_type==MOVE_REL && cntrl->uenc_state==0) ||
						    (cntrl->stepper_pos_maintenance==1 && cntrl->pm_state==1 && move_type==MOVE_REL && cntrl->uenc_state == 1 && uenc_ok == 1))
							{
							//cycle through axis list.  The list can be from masters and slaves
							//slaves specify their master in position 1 in the list.  So we must guard against
							//processing axis_list positions greater than 0 for slaves 
							for (i=0;i<(int)strlen(cntrl->axis_list);i++)
								{	
								if (i == 0)
									{/*mR motor target position, maybe a master or slave motor*/
									/*query controller for position error in motor steps*/
									sprintf(galil_cmd_str,"err%c=?",cntrl->axis_list[0]);
									if (galil_send_cmd("GO",control,galil_cmd_str,buffer)!=DMCNOERROR)
										errlogPrintf("Error reading position error\n");
									
									mr_motor_err = (double)galil_conv_str_val(buffer);
									
									//Did position maintenance reduce error below or equal to mnerr last time ?
									//if not dont include error, it could be a huge value. (eg. motor lost power)
									if (ABS(mr_motor_err)>((double)cntrl->pos_maintenance_min_error/mr->mres))
										mr_motor_err = 0;
									
									//Calculate target
									//Mr takes error in master position into account for rel moves
									//except when the move is backlash takeout
									//For backlash takeout move, we adjust for the position error here
									//For this to work, a driver usage rule is required
									//A rule for this driver is: 
									//minimum backlash distance used (bdst) = real device bdst + min allowed error for pos main
									//backlash distance used (bdst) must be smaller than max allowed error for pos main
									
									if (mr->mip==MIP_MOVE_BL)
										temp_calc = (mr->drbv/mr->mres)+rel_dval+mr_motor_err;
									else
										temp_calc = (mr->drbv/mr->mres)+rel_dval;
									}
								else
									{
									//Process the mR motor slaves
									//Dont process slave axis list
									if (cntrl->is_slave==0)
										{
										//retrieve slave axis data structure. It has updated gear ratios.
										cntrl_slave = (struct G21X3axis *)&control->axis[(cntrl->axis_list[i]-65)];
										//dont process slaves with a zero gear ratio
										if (cntrl_slave->axis_gear[0] != 0.0000)
											{
											/*slave target position*/
											/*query controller for position error in motor steps*/
											sprintf(galil_cmd_str,"err%c=?",cntrl->axis_list[i]);
											if (galil_send_cmd("GO",control,galil_cmd_str,buffer)!=DMCNOERROR)
												errlogPrintf("Error reading position error\n");
							
											slave_err[i] = (double)galil_conv_str_val(buffer);
										
											//Did position maintenance reduce error below or equal to mnerr last time ?
											//if not dont include error, it could be a huge value.
											if (ABS(slave_err[i])>((double)cntrl->pos_maintenance_min_error/mr->mres))
												slave_err[i] = 0;
							
											/*scale master distance to slave distance.  Slave gear ratio is the scaling factor*/
											/*subtract current slave position error */
											/*add current master position error if move is backlash takeout, coz mR doesnt do it*/
											if (mr->mip==MIP_MOVE_BL)
												temp_calc = (rel_dval * cntrl_slave->axis_gear[0]) + (mr_motor_err * cntrl_slave->axis_gear[0]) + (((mr_motor_err * cntrl_slave->axis_gear[0]) - slave_err[i])*-1.0000);
											else
												temp_calc = (rel_dval * cntrl_slave->axis_gear[0]) + (((mr_motor_err * cntrl_slave->axis_gear[0]) - slave_err[i])*-1.0000);
											
											/*query controller for slave real position by reading encoder*/
											/*we use position maintenance variable for this*/
											/*we use this variable for conveniance, as it's source data*/
											/*changes depending on native or user encoder*/
											/*set_status maintains seperate variables for native and user encoder*/
											sprintf(galil_cmd_str,"enpos%c=?",cntrl->axis_list[i]);
										
											galil_send_cmd("GO",control,galil_cmd_str,buffer);
												
											temp_calc2 = (double)galil_conv_str_val(buffer);
										
											/*Add slave move distance to slave current position*/ 
											/*to obtain slave target in motor steps*/
											temp_calc = (double)(temp_calc2/(cntrl->mtr_enc_scale/cntrl->mtr_enc_multiplier)) + temp_calc;
											}
										}
									}
								//process masters and all slaves that have a gear ratio that is not zero
								//Guard against processing slave axis_list positions greater than 0, only process the slave itself.
								if ((i>0 && cntrl->is_slave==0 && cntrl_slave->axis_gear[0]!=0.0000) || i==0)
									{
									/*Tell controller the target we are after*/
									sprintf(galil_cmd_str,"targ%c=%lf\n",cntrl->axis_list[i],temp_calc);
									if (galil_send_cmd("GO",control,galil_cmd_str,buffer)!=DMCNOERROR)
										errlogPrintf("Error telling controller the destination target\n");
										
									}
								}
							}
						
						//must set position targets for ssi home before move begins
						if (cntrl->stepper_pos_maintenance==1 && cntrl->pm_state==1 && move_type==HOME_REV && cntrl->encoder_valid==2)
							{
							if (cntrl->is_slave==0 || (cntrl->is_slave==1 && cntrl->slaveallowedtohome==1))
								{
								for (i=0;i<(int)strlen(cntrl->axis_list);i++)
									{
									/*Calculate target for position maintenance in motor steps*/
									if (i == 0)   /*Mr motor*/
										temp_calc = (double)(cntrl->home_value)/mr->mres;
									else
										{     /*Any slaves the mR motor has*/
										if (cntrl->is_slave == 0)
											{
											//retrieve slave axis data structure. It has updated gear ratios.
											cntrl_slave = (struct G21X3axis *)&control->axis[(cntrl->axis_list[i]-65)];
											if (cntrl_slave->axis_gear[0] !=0.0000)
												{
												/*query controller for position error in motor steps*/
												sprintf(galil_cmd_str,"err%c=?",cntrl->axis_list[i]);
												if (galil_send_cmd("GO",control,galil_cmd_str,buffer)!=DMCNOERROR)
													errlogPrintf("Error reading position error\n");
												
												temp_calc2 = galil_conv_str_val(buffer);
										
												//Did position maintenance reduce error below or equal to mnerr last time ?
												//if not dont include error, it could be a huge value.
												if (ABS(temp_calc2)>((double)cntrl->pos_maintenance_min_error/mr->mres))
													temp_calc2 = 0; 
											
												/*calculate the distance in motor steps that master will go through*/
												temp_calc = mr->rval - temp_calc; 
												/*scale master distance to slave move distance, scale factor is the slave gear ratio*/
												/*also add current position error*/
												temp_calc2 = (temp_calc * cntrl_slave->axis_gear[0])+temp_calc2;
												/*query controller for slave position in motor steps*/
												sprintf(galil_cmd_str,"enpos%c=?",cntrl->axis_list[i]);
												if (galil_send_cmd("GO",control,galil_cmd_str,buffer)!=DMCNOERROR)
													errlogPrintf("Error reading slave encoder position\n");
												temp_calc = galil_conv_str_val(buffer);
												
												/*subtract slave move distance from slave current position*/ 
												/*to obtain slave target in motor steps*/
										
												temp_calc = temp_calc/(cntrl->mtr_enc_scale/cntrl->mtr_enc_multiplier) - temp_calc2;
												}
											}
										}
									//process masters and all slaves that have a gear ratio that is not zero
									//Guard against processing slave axis_list positions greater than 0, only process the slave itself.
									if ((i>0 && cntrl->is_slave==0 && cntrl_slave->axis_gear[0]!=0.0000) || i==0)
										{
										/*Tell controller the target we are after for this motor*/
										sprintf(galil_cmd_str,"targ%c=%lf\n",cntrl->axis_list[i],temp_calc);
										if (galil_send_cmd("GO",control,galil_cmd_str,buffer)!=DMCNOERROR)
											errlogPrintf("Error telling controller the destination target\n");
										}
									}
								}
							}
							
						//check for disconnected ssi encoders
						//must check all slaves of any motor, readStatus version of this code doesnt need/have
						//to check all slaves like this code does.
						//this code is also necessary to check ssi connect status before move
						//as STUP field and thus readStatus MAY not run regularly when motor is not moving
						if (cntrl->encoder_valid == 2)
							{
							if (cntrl->is_slave == 0)
								for (i=0;i<(int)strlen(cntrl->axis_list);i++)
									{
									//retrieve slave axis data structure. It has updated gear ratios.
									if (i>0)
										cntrl_slave = (struct G21X3axis *)&control->axis[(cntrl->axis_list[i]-65)];
										
									sprintf(galil_cmd_str,"DP%c=?",cntrl->axis_list[i]);
									if (galil_send_cmd("GO",control,galil_cmd_str,buffer)!=DMCNOERROR)
										errlogPrintf("Error reading encoder position\n");
										
									if (i>0)
										enc_disconnect_val = (long)set_ssi_connectflag(cntrl_slave);
									else
										enc_disconnect_val = (long)set_ssi_connectflag(cntrl);
									enc_position = (long)galil_conv_str_val(buffer);
									
									/*invert SSI encoder if instructed*/
									if (cntrl->invert_coordinates & INVERT_ENCODER)   
										enc_position = (long)(powerof(2,(cntrl->ssitotalbits - cntrl->ssierrbits)) - 1) - enc_position;
										
									if (enc_disconnect_val == enc_position)
										{
										if (i>0)
											{
										 	if ((cntrl_slave->axis_gear[0] != 0.0000))
												ssi_ok = false;
											}
										else
											ssi_ok = false;
										}
									}
							else
								{
								sprintf(galil_cmd_str,"DP%c=?",cntrl->axis_list[0]);
								if (galil_send_cmd("GO",control,galil_cmd_str,buffer)!=DMCNOERROR)
									errlogPrintf("Error reading encoder position\n");
									
								enc_disconnect_val = (long)set_ssi_connectflag(cntrl);
								enc_position = (long)galil_conv_str_val(buffer);
								
								/*invert SSI encoder if instructed*/
								if (cntrl->invert_coordinates & INVERT_ENCODER)   /*invert ssi encoder if instructed*/
										enc_position = (long)(powerof(2,(cntrl->ssitotalbits - cntrl->ssierrbits)) - 1) - enc_position;
								
								if (enc_disconnect_val == enc_position)
									ssi_ok = false;
								}
							}
							
						//issue move command if 
						//not using an encoder
						//using quadrature encoder
						//using ssi, and ssi encoder is ok
						//using user encoder, and user encoder is valid and on
						if ((cntrl->encoder_valid == 2) && (ssi_ok == true) && (cntrl->uenc_state==0) ||
						     (cntrl->encoder_valid != 2) && (cntrl->uenc_state == 0) ||
						     (cntrl->uenc_state == 1) && (uenc_ok == 1) ||
						     (cntrl->uenc_state == 0) && (uenc_ok == 1))
							{
							if (motor_interlock_ok(control, cntrl, digital_port_value))
								{
								sprintf(galil_cmd_str,"BG %c",cntrl->axis_list[0]);
								if (galil_send_cmd("GO",control,galil_cmd_str,buffer)!=DMCNOERROR)
									errlogPrintf("Error Issuing GO\n");
								}
							else	
								errlogPrintf("Motor, or a slave of this motor is interlocked via digital IO\n");
							}
							
						//must enable position maintenance after issuing move command	
						if (cntrl->stepper_pos_maintenance==1 && cntrl->pm_state==1)
							{
							if (((cntrl->encoder_valid == 2) && (ssi_ok == true)) || (cntrl->encoder_valid != 2))
								{
								if (cntrl->is_slave==0)
									for (i=0;i<(int)strlen(cntrl->axis_list);i++)
										{
										sprintf(galil_cmd_str,"inps%c=1",cntrl->axis_list[i]);
										if (galil_send_cmd("GO",control,galil_cmd_str,buffer)!=DMCNOERROR)
											errlogPrintf("Error telling controller position maintenance is on\n");
										}
								else
									{
									sprintf(galil_cmd_str,"inps%c=1",cntrl->axis_list[0]);
									if (galil_send_cmd("GO",control,galil_cmd_str,buffer)!=DMCNOERROR)
										errlogPrintf("Error telling controller position maintenance is on\n");
									}
								}
							}
						}
					break; 
			case SET_ENC_RATIO: /*setup home position value for the encoder register*/
						if (cntrl->home_value!=0.0000)
							{
							if (cntrl->invert_coordinates & INVERT_ENCODER)		
								temp_calc=(double)((cntrl->home_value-cntrl->eoff)/mr->eres)*-1.0;
							else
								temp_calc=(double)(cntrl->home_value-cntrl->eoff)/mr->eres;
							}
						else
							temp_calc=0.0;
						sprintf(galil_cmd_str,"enhmval%c=%lf\n",cntrl->axis_list[0],rint(temp_calc));
						if (galil_send_cmd("SET_ENC_RATIO",control,galil_cmd_str,buffer)!=DMCNOERROR)
							errlogPrintf("Error telling controller encoder home value\n");
						break;
			
			case GET_INFO:   cntrl->status_update=1;	/*inform our axis we want a status update for limits and we want to ignore the no motion timeout period*/
					 cntrl->status_read=0;   	/*flag to indicate we havent called readG21X3status for this axis yet*/
					break;	
			
			case STOP_AXIS: /*Command 11*/
					//motor stop command
					//turn off position maintenance, cancel hjog, home and posci (pos correction)
					for (i=0;i<(int)strlen(cntrl->axis_list);i++)
						{
						//retrieve control structure for the next axis in the specified axis's list of slaves
						//note these are not always slaves here, contrary to the variable name.
						cntrl_slave = (struct G21X3axis *)&control->axis[cntrl->axis_list[i]-65];
						
						if ((cntrl_slave->is_slave==1 && cntrl_slave->axis_gear[0] != 0.0000) || (i==0))
							{
							if (cntrl->stepper_pos_maintenance==1 && mr->spmg!=2 && mr->spmg!=3)  /*dont turn off pos maintenance if spmg is still on go or move*/
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
					
					break;
			case JOG:         /*Command 12*/
					/*Assemble Galil jog command*/
					if (cntrl->invert_coordinates & INVERT_MOTOR)
						dval=dval*-1.0;	
					sprintf(galil_cmd_str,"JG%c=%lf",cntrl->axis_list[0],dval);
					if (galil_send_cmd("JOG",control,galil_cmd_str,buffer)!=DMCNOERROR)
							errlogPrintf("Error setting jog mode\n");
					/*if we have pos maintenance on, we want it off for this move*/
					if (cntrl->stepper_pos_maintenance==1)
						{
						for (i=0;i<(int)strlen(cntrl->axis_list);i++)
							{
							if (cntrl->is_slave==0)
								{
								sprintf(galil_cmd_str,"posci%c=0\n",cntrl->axis_list[i]);
								if (galil_send_cmd("JOG",control,galil_cmd_str,buffer)!=DMCNOERROR)
									errlogPrintf("Error telling controller that we want to cancel any correction move in progress\n");	
								}
							}
						}
					//Ensure home flag is 0
					sprintf(galil_cmd_str,"home%c=0",cntrl->axis_list[0]);
					if (galil_send_cmd("MOVE_REL",control,galil_cmd_str,buffer)!=DMCNOERROR)
						errlogPrintf("Setting home to 0 MOVE_REL\n");	
					
					/*start jogging*/
					sprintf(galil_cmd_str,"BG %c",cntrl->axis_list[0]);
					if (galil_send_cmd("JOG",control,galil_cmd_str,buffer)!=DMCNOERROR)
							errlogPrintf("Error Issuing GO for jog\n");
					break;
			
			case SET_PGAIN:  /*Command 13*/
					/*Assemble Galil Set KP command*/
					sprintf(galil_cmd_str,"KP%c=%lf",cntrl->axis_list[0],(double)(dval*KPMAX));
					if (galil_send_cmd("SET_PGAIN",control,galil_cmd_str,buffer)!=DMCNOERROR)
							errlogPrintf("Error setting P gain\n");
					break;
			
			case SET_IGAIN:  /*Command 14*/
					/*Assemble Galil Set KI command*/
					sprintf(galil_cmd_str,"KI%c=%lf",cntrl->axis_list[0],(double)(dval*KIMAX));
					if (galil_send_cmd("SET_IGAIN",control,galil_cmd_str,buffer)!=DMCNOERROR)
							errlogPrintf("Error setting I gain\n");
					break;
			
			case SET_DGAIN:  /*Command 15*/
					/*Assemble Galil Set KD command*/
					sprintf(galil_cmd_str,"KD%c=%lf",cntrl->axis_list[0],(double)(dval*KDMAX));
					if (galil_send_cmd("SET_DGAIN",control,galil_cmd_str,buffer)!=DMCNOERROR)
							errlogPrintf("Error setting D gain\n");
					break;
			
			case ENABLE_TORQUE: /*Command 16*/
			case DISABL_TORQUE: /*Command 17*/
						
					break;
			
			case SET_HIGH_LIMIT: /*Command 19*/
						/*this gets called at init for every mR*/
						/*Assemble Galil Set High Limit, forward limit in Galil language*/
						dval = (dval > 2147483647.0) ? 2147483647.0 : dval; 
						if (cntrl->invert_coordinates & INVERT_MOTOR)
							{
							sprintf(galil_cmd_str,"BL%c=%lf",cntrl->axis_list[0],(double)((dval+1)*-1));
							if (galil_send_cmd("SET_HIGH_LIMIT",control,galil_cmd_str,buffer)!=DMCNOERROR)
								errlogPrintf("Error setting reverse software limit\n");
							}	
						else
							{
							sprintf(galil_cmd_str,"FL%c=%lf",cntrl->axis_list[0],(double)(dval));
							if (galil_send_cmd("SET_HIGH_LIMIT",control,galil_cmd_str,buffer)!=DMCNOERROR)
								errlogPrintf("Error setting forward software limit\n");
							}	
							
						break;
			
			case SET_LOW_LIMIT:  /*Command 20*/
						/*Assemble Galil Set Low Limit, we cater for forward and reverse home types*/
						dval = (dval < -2147483648.0) ? -2147483648.0 : dval;
						if (cntrl->invert_coordinates & INVERT_MOTOR)
							{
							sprintf(galil_cmd_str,"FL%c=%lf",cntrl->axis_list[0],(double)((dval+1)*-1));
							if (galil_send_cmd("SET_LOW_LIMIT",control,galil_cmd_str,buffer)!=DMCNOERROR)
								errlogPrintf("Error setting forward software limit\n");
							}
						else
							{
							sprintf(galil_cmd_str,"BL%c=%lf",cntrl->axis_list[0],(double)(dval));
							if (galil_send_cmd("SET_LOW_LIMIT",control,galil_cmd_str,buffer)!=DMCNOERROR)
								errlogPrintf("Error setting reverse software limit\n");
							}
							
						break;
			default:             errlogPrintf("Default\n");
						rtnval = ERROR;
						break;
			}
		}
	else
		{
		if (cntrl->status == OK && lock==epicsMutexLockOK)
			{
			errlogPrintf("MRES, VELO and maybe ERES need to be set to non zero value before processing is allowed\n");
			errlogPrintf("It is Strongly advised that you restart your IOC with these database values set\n");
			}
		if (lock!=epicsMutexLockOK)
			errlogPrintf("Cannot obtain mutex to sent command, card %d\n",card);
		}
    if (lock==epicsMutexLockOK)
	epicsMutexUnlock(control->G21X3Lock);
    Debug(1, "End Of Build_trans after Switch\n");
    return (rtnval);
}

/*----------------------------------------------------------------------------------*/
/* Upscale enc/mtr scale factor in attempt to maintain precision
*/

void up_scale_mtr_enc_scale(struct G21X3axis *cntrl)
{
	float multiplier=1.0;
	while ((cntrl->mtr_enc_scale*multiplier)<100.0)
		{
		multiplier *= 10.0;
		cntrl->mtr_enc_multiplier = multiplier;
		}
	cntrl->mtr_enc_scale *= multiplier;
}

/*--------------------------------------------------------------------------------------*/
/* Check that user encoder state is consistent amongst the specified axis, and all its slaves
*/

int check_uenc_states(struct G21X3controller *control, struct G21X3axis *cntrl)
{
	struct G21X3axis *cntrl_two;
	int i;
	int loop_end;
	int any_mismatch = 0;
	int all_match;
	
	//cycle through axis list.  The list can be from masters and slaves
	//slaves specify their master in position 1 in the list.  So we must guard against
	//processing axis_list positions greater than 0 for slaves 
	if (cntrl->is_slave == 0)
		loop_end = strlen(cntrl->axis_list);
	else
		loop_end = 1;
	for (i=0;i<loop_end;i++)
		{
		//retrieve control structure for the next axis in the specified axis's list of slaves
		cntrl_two = (struct G21X3axis *)&control->axis[cntrl->axis_list[i]-65];
	
		if (cntrl_two->uenc_state != cntrl->uenc_state)
			{
			any_mismatch = 1;
			}
		}
	all_match = (any_mismatch==0) ? 1 : 0;
	return(all_match);
}

/*--------------------------------------------------------------------------------------*/
/* Check for changes in velo, hvel, bdst and mres
   Update controller variables accordingly
*/

void basic_motion_parse(struct motorRecord *mr, struct G21X3controller *control, struct G21X3axis *cntrl, int command)
{
	char galil_cmd_str[G21X3_LINE_LENGTH]="\0";     /*holds the assembled Galil cmd string*/
	char buffer[G21X3_LINE_LENGTH]="\0";		/*Holds the Galil response string*/
	double digsp;					/*digital control speed */
	double limdc;					/*decel in position feedback counts*/
	double soft_limit;				/*forward and reverse software limit*/
	int dir = (mr->dir == 0) ? 1 : -1;		/*direction based on motorrecord dir field*/
	
	//Calculate dig speed in cnt/sec from hvel, mres, and dir
	digsp = mr->hvel/mr->mres * dir;
	
	if ((cntrl->mres != mr->mres) || (cntrl->velo != mr->velo) || (cntrl->dev_init < 2))
		{
		/*Calculate our required Home/Limit Deceleration rate given the velocity, and allowed number of steps after Home/Limit*/
		/*activation*/
		/*suvat equation for constant acceleration*/
		limdc=(double)((mr->velo/mr->mres)*(mr->velo/mr->mres))/((cntrl->egu_after_limit/mr->mres)*2.0);
			
		if (limdc>67107840.0)
			limdc = 67107840.0;
				
		sprintf(galil_cmd_str,"limdc%c=%lf\n",cntrl->axis_list[0],rint(limdc));
		if (galil_send_cmd("G21X3_build_trans",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error telling controller our limit/home deceleration\n");
		}
		
	if ((cntrl->mres != mr->mres) || (cntrl->hvel != mr->hvel) || (cntrl->dev_init < 2))
		{	
		/*recalculate hjgsp, hjgdc, mrhmval and digsp incase db values have been changed by user*/	
		/*hjgdc is set for a slow deceleration given what speed hjgsp is so we are cleanly off the switch*/
		/*encoder reference homing is harded coded in galil to stop at max decel*/
				
		sprintf(galil_cmd_str,"digsp%c=%lf\n",cntrl->axis_list[0],rint(digsp));
		if (galil_send_cmd("G21X3_build_trans",control,galil_cmd_str,buffer)!=DMCNOERROR)
			errlogPrintf("Error telling controller dig home speed\n");
		}
		
	if ((cntrl->mres != mr->mres))
		{
		//rewrite our software limits
		//first is forward software limit
		soft_limit = mr->dhlm/mr->mres;
		soft_limit = (soft_limit > 2147483647.0) ? 2147483647.0 : soft_limit; 
		if (cntrl->invert_coordinates & INVERT_MOTOR)
			{
			sprintf(galil_cmd_str,"BL%c=%lf",cntrl->axis_list[0],(double)((soft_limit+1)*-1));
			if (galil_send_cmd("SET_HIGH_LIMIT",control,galil_cmd_str,buffer)!=DMCNOERROR)
				errlogPrintf("Error setting reverse software limit\n");
			}	
		else
			{
			sprintf(galil_cmd_str,"FL%c=%lf",cntrl->axis_list[0],(double)(soft_limit));
			if (galil_send_cmd("SET_HIGH_LIMIT",control,galil_cmd_str,buffer)!=DMCNOERROR)
				errlogPrintf("Error setting forward software limit\n");
			}
		//Second is reverse software limit
		soft_limit = mr->dllm/mr->mres;
		soft_limit = (soft_limit < -2147483648.0) ? -2147483648.0 : soft_limit;
		if (cntrl->invert_coordinates & INVERT_MOTOR)
			{
			sprintf(galil_cmd_str,"FL%c=%lf",cntrl->axis_list[0],(double)((soft_limit+1)*-1));
			if (galil_send_cmd("SET_LOW_LIMIT",control,galil_cmd_str,buffer)!=DMCNOERROR)
				errlogPrintf("Error setting forward software limit\n");
			}
		else
			{
			sprintf(galil_cmd_str,"BL%c=%lf",cntrl->axis_list[0],(double)(soft_limit));
			if (galil_send_cmd("SET_LOW_LIMIT",control,galil_cmd_str,buffer)!=DMCNOERROR)
				errlogPrintf("Error setting reverse software limit\n");
			}
		}
}

/*--------------------------------------------------------------------------------------*/
/* Check for changes in encoder runtime variables
   Includes encoder changeover, resolution and offset changes
   Updates controller, mr, and axis control structure variables
*/

void encoder_state_change(struct motorRecord *mr, struct G21X3controller *control, struct G21X3axis *cntrl)
{	
	if (cntrl->moving!=MOVING)
		{
		//an encoder state change has not occurred
		//save any changes user has made to eres, ueres, off, ueoff at runtime.
		if (cntrl->uenc_state == 0 && cntrl->uenc_requested_state == cntrl->uenc_state)
			{
			cntrl->neres = mr->eres;
			cntrl->neoff = mr->off;
			}
					
		if (cntrl->uenc_state == 1 && cntrl->uenc_requested_state == cntrl->uenc_state)
			{
			cntrl->ueres = mr->eres;
			cntrl->ueoff = mr->off;
			}
			
		//an encoder state change has been requested
		//honour state change as the motor is stopped
		//save any changes to the resolution and offset factors made at runtime
		//for the encoder we are disabling
		if (cntrl->uenc_state == 1 && cntrl->uenc_requested_state != cntrl->uenc_state)
			{
			//save any changes to user encoder eres and off
			cntrl->ueres = mr->eres;
			cntrl->ueoff = mr->off;
			//place native encoder off and res into motor record
			mr->off = cntrl->neoff;
			mr->eres = cntrl->neres;
			//load native encoder raw counts into motor record
			mr->rep = (epicsInt32)cntrl->encoder_posi;
			//load val and rbv that corresponds to native encoder
			mr->rbv = mr->val = (cntrl->encoder_posi * mr->eres) + mr->off;
			//load dval and drbv that corresponds to native encoder
			mr->drbv = mr->dval = (cntrl->encoder_posi * mr->eres);
				
			//acknowledge encoder state change
			cntrl->uenc_state = cntrl->uenc_requested_state;
			}
			
		if (cntrl->uenc_state == 0 && cntrl->uenc_requested_state != cntrl->uenc_state)
			{
			//save any changes to native encoder eres and off
			cntrl->neres = mr->eres;
			cntrl->neoff = mr->off;
			//place user encoder off and res into motor record
			mr->off = cntrl->ueoff;
			mr->eres = cntrl->ueres;
			//load user encoder raw counts into motor record
			mr->rep = (epicsInt32)cntrl->uencoder_posi;
			//load val and rbv that corresponds to user encoder
			mr->rbv = mr->val = (cntrl->uencoder_posi * mr->eres) + mr->off;
			//load dval and drbv that corresponds to user encoder
			mr->drbv = mr->dval = (cntrl->uencoder_posi * mr->eres);
			
			//acknowledge encoder state change
			cntrl->uenc_state = cntrl->uenc_requested_state;
			}	
		}	
}

/*--------------------------------------------------------------------------------------*/
/* Check for changes in encoder state, mres, and bdst
   Update controller variables accordingly
*/

void pos_maintenance_parse(struct motorRecord *mr, struct G21X3controller *control, struct G21X3axis *cntrl)
{
	struct G21X3axis *cntrl2;
	char galil_cmd_str[G21X3_LINE_LENGTH]="\0";     /*holds the assembled Galil cmd string*/
	char buffer[G21X3_LINE_LENGTH]="\0";		/*Holds the Galil response string*/
	int i, loop_end;
	
	double temp_calc;
	
	if (cntrl->moving!=MOVING)
		{
		/*setup pos maintenance variables*/
		if (cntrl->stepper_pos_maintenance==1)
			{	
			if ((cntrl->mres != mr->mres) || (cntrl->eres != mr->eres) || 
			    (cntrl->dev_init < 2))
				{
				//calculate position in motor steps from selected encoder
				cntrl->mtr_enc_scale=(double)(mr->mres/mr->eres);
				cntrl->motor_posi = mr->rep/cntrl->mtr_enc_scale;
				//load motor record with motor step number that corresponds
				//to user encoder position
				mr->rmp = NINT(cntrl->motor_posi);
				//Set motor controller step register to value calculated
				sprintf(galil_cmd_str,"DP%c=%d",cntrl->axis_list[0],mr->rmp);
				if (galil_send_cmd("G21X3_build_trans",control,galil_cmd_str,buffer)!=DMCNOERROR)
					errlogPrintf("Error setting new step count for changed encoder\n");
	
				//set pos maintenance target in motor steps	
				sprintf(galil_cmd_str,"targ%c=%d",cntrl->axis_list[0],mr->rmp);	
				if (galil_send_cmd("G21X3_init_record",control,galil_cmd_str,buffer)!=DMCNOERROR)
					errlogPrintf("Error setting position target to changed encoder\n");
				
				//upscale mtr_enc_scale to maintain resolution with controller math
				//that is used in position maintenance code made by the code generator
				//all use of mtr_enc_scale from here on must be cntrl->mtr_enc_scale/cntrl->mtr_enc_multiplier
				//to get the right mtr_enc_scale for math
				up_scale_mtr_enc_scale(cntrl);
						
				//Send mtr_enc_scale factors to the controller for pos maintenance code
				sprintf(galil_cmd_str,"scale%c=%lf\n",cntrl->axis_list[0],cntrl->mtr_enc_scale);
				if (galil_send_cmd("G21X3_build_trans",control,galil_cmd_str,buffer)!=DMCNOERROR)
					errlogPrintf("Error telling controller our motor/enc scaling ratio\n");
					
				sprintf(galil_cmd_str,"multi%c=%lf\n",cntrl->axis_list[0],cntrl->mtr_enc_multiplier);
				if (galil_send_cmd("G21X3_build_trans",control,galil_cmd_str,buffer)!=DMCNOERROR)
					errlogPrintf("Error telling controller our motor/enc scale multiplier\n");
				}

			
			//check for changes in Mres, or position maintenance error bars
			//applications may change these error bars dynamically so stup update before move start not guaranteed
			//so ensure master and all slaves get new values here

			// cycle through axis list.  The list can be from masters and slaves
			// slaves specify their master in position 1 in the list.  So we must guard against
			// processing axis_list positions greater than 0 for slaves
			if (cntrl->is_slave == 0)
				{
				loop_end = strlen (cntrl->axis_list);
				//for mres changes, a stup before move start is guaranteed
				if (cntrl->mres != mr->mres)
					loop_end = 1;
				}
			else
				loop_end = 1;

			for (i=0;i<loop_end;i++)
				{
				cntrl2 = (struct G21X3axis *)&control->axis[(cntrl->axis_list[i]-65)];
				
				if ((cntrl2->mres != mr->mres) ||
				(cntrl2->pos_maintenance_min_error!=cntrl2->pos_maintenance_min_error_old) ||
				(cntrl2->pos_maintenance_max_error!=cntrl2->pos_maintenance_max_error_old) ||
				(cntrl2->dev_init < 2))
					{
					/*setup min and max error for pos maintenance*/
					/*convert min and max error from EGU into motor steps before sending to G21x3*/
					temp_calc=(double)cntrl2->pos_maintenance_min_error/mr->mres;
					if (temp_calc<1.0000)
						{
						errlogPrintf("Min error for position maintenance is too low, resetting to 4 msteps\n");
						temp_calc = 4;
						}
					
					sprintf(galil_cmd_str,"mnerr%c=%lf\n",cntrl2->axis_list[i],rint(temp_calc));
					if (galil_send_cmd("G21X3_build_trans",control,galil_cmd_str,buffer)!=DMCNOERROR)
						errlogPrintf("Error telling controller our min error for pos maintenance\n");
							
					temp_calc=(double)cntrl2->pos_maintenance_max_error/mr->mres;
					if (temp_calc<1.0000)
						errlogPrintf("Max error for position maintenance is too low\n");
							
					sprintf(galil_cmd_str,"mxerr%c=%lf\n",cntrl2->axis_list[i],rint(temp_calc));
					if (galil_send_cmd("G21X3_build_trans",control,galil_cmd_str,buffer)!=DMCNOERROR)
						errlogPrintf("Error telling controller our max error for pos maintenance\n");
						
					cntrl2->pos_maintenance_min_error_old = cntrl2->pos_maintenance_min_error;
					cntrl2->pos_maintenance_max_error_old = cntrl2->pos_maintenance_max_error;
					}
				}
				
			if ((cntrl->bdst != mr->bdst) || (cntrl->mres != mr->mres) || (cntrl->dev_init < 2))
				{	
				if (cntrl->encoder_valid!=0)
					{
					temp_calc=(double)mr->bdst/mr->mres;
					sprintf(galil_cmd_str,"bd%c=%lf\n",cntrl->axis_list[0],rint(temp_calc));
					if (galil_send_cmd("G21X3_build_trans",control,galil_cmd_str,buffer)!=DMCNOERROR)
						errlogPrintf("Error telling controller our backlash for pos maintenance\n");
					}
				}
			}
		}
}

/*--------------------------------------------------------------------------------------*/
/*  When stopped force
    val equal to rbv, and
    dval equal to drbv
    This is more generic than mr spmg stop forcing of val=rbv, dval=drbv, as it applies whenever
    the motor is stopped.
*/

void check_readbacks_setpoints(struct motorRecord *mr, 
			       struct G21X3controller *control, 
			       struct G21X3axis *cntrl,
			       motor_cmnd command)
{
	int moving=1;
	struct G21X3axis *cntrl_master;
	
	//work out if we are moving or not
	
	//if motor is a slave, check the slave moving flag and master moving flag for movement
	if ((cntrl->is_slave == 1) && (cntrl->axis_gear[0]!=0.0000))
		{
		cntrl_master = (struct G21X3axis *)&control->axis[(cntrl->axis_list[1]-65)];
		moving = (cntrl->moving==MOVING) || (cntrl_master->moving==MOVING) ? 1:0;
		}
	else
		moving = (cntrl->moving==MOVING) ? 1:0;
	
	if (moving!=MOVING && command==GET_INFO)
		{
		mr->val = mr->rbv;
		mr->dval = mr->drbv;
		}
}

/*--------------------------------------------------------------------------------------*/
/* Synchronize mr and cntrl variables
*/

void sync_mr_cntrl_variables(struct motorRecord *mr, struct G21X3axis *cntrl)
{
    //store motor record settings that are required later
    //these variables are used to minimize comms traffic
    if (cntrl->moving!=MOVING)
	{
	cntrl->eres = mr->eres;
	cntrl->eoff = mr->off;
	cntrl->velo = mr->velo;
	cntrl->hvel = mr->hvel;
	cntrl->mres = mr->mres;
	cntrl->bdst = mr->bdst;
	//we have passed through all required initialization
	//once dev_init = 2
	if (cntrl->dev_init < 2)
		cntrl->dev_init++;
	}
}

/*--------------------------------------------------------------------------------------*/
/* motor interlock check
*/

int motor_interlock_ok(struct G21X3controller *control, struct G21X3axis *cntrl, double digital_port_value)
{
	struct G21X3axis *cntrl2;
	int mask, digital_io;
	int i,j,k,loop_end;
	struct G21X3motor_enables *motor_enables;
	int motors_index;			/* motor list length for given digital port */
	
	digital_io = (int)digital_port_value;
	
	// cycle through axis list.  The list can be from masters and slaves
	// slaves specify their master in position 1 in the list.  So we must guard against
	// processing axis_list positions greater than 0 for slaves
	if (cntrl->is_slave == 0)
		loop_end = strlen (cntrl->axis_list);
	else
		loop_end = 1;

	for (i=0;i<loop_end;i++)
		{
		cntrl2 = (struct G21X3axis *)&control->axis[(cntrl->axis_list[i]-65)];
		// Test master, and slaves with a non zero gear ratio
		if ((cntrl2->axis_gear[0] != 0.0000) && (cntrl2->is_slave == 1) || (i == 0))
			{
			//Cycle through all digital inputs structure looking for current motor
			for (j=0;j<8;j++)
				{
				motor_enables = (G21X3motor_enables *)&control->motor_enables[j];
				motors_index = strlen(motor_enables->motors);
				for (k=0;k<motors_index;k++)
					{
					if (motor_enables->motors[k] == cntrl2->axis_list[0])
						{
						//motor found
						mask = (1 << (j));
						if ((digital_io & mask) == mask)
							{
							/* Motor interlock is "no go", due to digital IO state */
							if (motor_enables->disablestates[k] == 1)
								return(0);
							}
						else
							{
							/* Motor interlock is "no go", due to digital IO state */
							if (motor_enables->disablestates[k] == 0)
								return(0);
							}
							
						}
					}
				}
			}
		}
	// Motor interlock is "go"
	return(1);
}
