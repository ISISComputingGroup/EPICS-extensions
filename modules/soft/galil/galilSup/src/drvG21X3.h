/* File: drvG21X3.h          */

/* Device Driver Support definitions for motor
 *
 *      Original Author: Mark Clift
 *
 */

#ifndef INCdrvG21X3h
#define INCdrvG21X3h 1

#define G21X3_NUM_CHANNELS	8
#define G21X3_NUM_CONTROLLERS   MAX_CONTROLLERS                 /*Support as many as dmclnx (manufacturer library) supports*/
#define G21X3_COMM_ERR		-1
#define G21X3_NUM_CMDS		4
#define G21X3_LINE_LENGTH	80
#define LIMIT_CODE_LEN		80000
#define INP_CODE_LEN  		80000
#define THREAD_CODE_LEN     	80000
#define AXIS_HOMENOT_MASK	2
#define INVERT_MOTOR		1
#define INVERT_ENCODER		2
#define INVERT_POSMAIN		4
#define INVERT_SWITCHES		8
#define AXIS_REVLIMIT_INACTIVE	4
#define AXIS_FORLIMIT_INACTIVE  8
#define AXIS_INMOTION		128
#define MAXFORWARD		2147483646
#define MAXREVERSE		-2147483647
#define KPMAX			1023.875
#define KIMAX			2047.875
#define KDMAX			4095.875
#define MODEL_1400_DO_MASK	7
#define MODEL_4AXIS_DO_MASK	255
#define MODEL_8AXIS_DO_MASK     65535

#define MIP_MOVE_BL	0x0100	/* Done moving; now take out backlash. */


/*------ This defines the G21X3 specific property structures */
/*first the axis structure*/
struct G21X3axis
{
    double motor_posi;	   			/*Where the Galil thinks the motor is, based on output pulses to stepper*/
    double encoder_posi;   			/*Where the Galil thinks the motor is, based on encoder value*/
    double uencoder_posi;			/*User defined encoder raw counts*/
    double uencoder_dial;			/*User defined encoder in dial coordinates*/
    double uencoder_user;			/*User defined encoder in user coordinates*/
    unsigned switches;	   			/*used to store status of inputs associated with this axis on the controller eg. limit switch*/
    int moving;		   			/* 1 = moving! */
    int motortype;	   			/*stores the type of motor attached to the axis*/
    int stepper_pos_maintenance;   		/*stores the user set parameter that tells us if the galil is running the pos maintenance software for stepper motors*/
    char uenc_str[G21X3_LINE_LENGTH];	        /*user string which defines user encoder*/
    char uenc_valid_flag[G21X3_LINE_LENGTH];	/*a variable name used to determine if user encoder reading is valid ? if variable = 1 then reading valid*/
    int uenc_state;				/*state that user encoder readback is in*/
    					        /*1 = On.  Mr targetting, pos maintenance, stall detection, valid reading statuses, and pos measurement*/
						/*when on, relative move commands from motor record are "hijacked" by this driver.*/
    						/*0 = Off. Stall detection, valid reading statuses, and pos measurement only.*/
    int uenc_requested_state;			/*state that this driver would like user encoder in*/
						
    int uenc_settings_valid;			/*is the user encoder specified valid*/
    int uenc_reading_valid;			/*is the user encoder reading valid.  Derived from uenc_valid_flag*/
    int pm_state;				/*position maintenance current state for this axis*/
    double pos_maintenance_min_error;  		/*Min error in before position maintenance will engage, in EGU*/
    double pos_maintenance_max_error;  		/*Min error before position maintenance will engage, in EGU*/
    double pos_maintenance_min_error_old;  	/*Old value Min error in before position maintenance will engage, in EGU*/
    double pos_maintenance_max_error_old;  	/*Old value Min error before position maintenance will engage, in EGU*/
    int ssitotalbits;				/*Stores the number of bits required for ssi encoder*/
    int ssierrbits;				/*stores the number of status bits in the encoder word of information*/
    int ssidataform;				/*stores if ssi encoder output is binary or gray code*/
    int status;		   			/* Used to detect comms errors during controller polling in readstatus*/
    int invert_coordinates;
    int limit_as_home;				/*0=is use home switch; 1=use limit switch.  */
    double home_value;				/*home location in EGU*/
    int home_prog_reg;				/*Program home register once home 0=No, 1=Yes */
    int homming_now;				/*Are we homing the motor now ? */
    double egu_after_limit;		        /*egu travelled after limit/home trip*/
    int oldpositionstep;			/*last motor step count read*/
    int oldpositionenc;				/*last position read from encoder*/
    int olddirection;				/*direction set to this when not moving.*/
    int oldpositionuenc;			/*last position read from user encoder*/
    epicsInt32 velocity;			/*raw velocity readback, mr doesnt implement this feature yet*/
    char axis_list[G21X3_NUM_CHANNELS+1];	/*store master axis, and all geared slave axis names*/
    double axis_gear[G21X3_NUM_CHANNELS+1];	/*store master axis gearing (for alignment to axis_list only), and the slave gearing ratio*/
    int encoder_valid;				/*0 for invalid, 1 for quadrature encoder register, 2 for SSI*/
    int encoder_setting;			/*stores the command setting required to put encoder settings as specified by user*/
    int is_slave;				/*is this motor a slave*/
    int slaveallowedtohome;			/*is the slave allowed to home separate from the master*/
    int gantry_mode;				/*is the master/slave relationship a gantry one?*/
    int no_motion_count;			/*keeps track of how many times we have polled, and found the axis not moving*/
    int enc_no_move_count;			/*Used to count poll cycles where encoder appears frozen, while step count is moving*/
    int encoder_stall;				/*Encoder stall flag*/
    int ssi_connect;				/*SSI encoder connect flag*/
    double velo;				/*motor record velo.  Used to minimize comms traffic*/
    double hvel;				/*motor record hvel.  Used to minimize comms traffic*/
    double mres;				/*motor record mres.  Used to minimize comms traffic*/
    double bdst;				/*motor record bdst.  Used to minimize comms traffic*/
    double eres;				/*motor record eres.  Used to minimize comms traffic*/
    double eoff;				/*motor record offset.  Used to minimize comms traffic*/
    double neres;				/*native encoder eres*/
    double neoff;				/*native encoder offset*/
    double ueres;				/*User encoder eres*/
    double ueoff;				/*User encoder offset for user coordinate system*/
    double mtr_enc_scale;			/*mres/eres or mres/ueres*/
    double mtr_enc_multiplier;			/*multiplier to scale mtr/enc factor back to proper value*/
    int status_update;				/*are we allowed to update the limits info, without being stopped for the required time period.*/
    int status_read;				/*flag indicating readG21X3Status for the axis has been called, but set_status hasnt*/
    int init_update;				/*are we allowed to update the limits info, without being stopped for the required time period.*/
    int dev_init;				/*used to minimize comms in devG21X3*/
    int stop_state;				/*is the driver in a stop state, or go state, for this motor*/
    int on_state;				/*Is the motor in servo mode, or off */
    double step_smooth;				/*Stepper motor smoothing value*/
    unsigned homesw_act;			/*value of home switch input when switch not activated*/
    unsigned homesw_inact;			/*value of home switch input when switch is activated*/
    int motor_connected;			/*Set to 1 if motor connected, 0 otherwise.  Both limits active is motor disconnected state*/
};

/* For each digital input, we maintain a list of motors, and the state the input should be in*/
/* To disable the motor */
struct G21X3motor_enables {
	char motors[G21X3_NUM_CHANNELS];
	char disablestates[G21X3_NUM_CHANNELS];
};

/*Now the controller structure*/
struct G21X3controller{
    epicsMutexId G21X3Lock;
    struct G21X3axis axis[G21X3_NUM_CHANNELS];
    struct G21X3motor_enables motor_enables[G21X3_NUM_CHANNELS];	/* stores the motor enable disable interlock digital IO setup, only first 8 digital in ports supported */
    int limit_type;				/*switch type: -1 is normally closed, 1 is normally open*/
    int home_type;				/*switch type: -1 is normally closed, 1 is normally open*/
    HANDLEDMC      hdmc;   			/* Handle to Galil controller */
    int model_num;				/*used to store the controller model number*/
    int card;					/*used to store the card number*/
    char ip[15];		   		/*ip addy of Galil Controller*/
    int connectok;	   			/*flag indicating if we have connected to controller ok*/
    int num_axis;				/*holds the total number of axis on the controller*/
    int digital_in;				/*holds the last read value of the digital input ports*/
    						/*does not included extended I/O*/
    float analog_in[16];			/*last read analog in value */
    };

#endif  /* INCdrvG21X3h */
