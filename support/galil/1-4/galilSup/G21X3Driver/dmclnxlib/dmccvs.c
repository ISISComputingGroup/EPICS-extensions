#include "dmclnx.h"

#include <math.h>

#ifdef DMC_BINARYCOMMANDS

short dmccvs_debug = 0;

LegalCommand local_legal_commands[] =
{
  { "KP", 0x81 }, /* Propotional Constant */
  { "KI", 0x82 }, /* Integrator */
  { "KD", 0x83 }, /* Derivative Constant */
  { "DV", 0x84 }, /* Dual Velocity (Dual Loop) */
  { "ER", 0x88 }, /* Error Limit */
  { "IL", 0x89 }, /* Integrator Limit */
  { "TL", 0x8A }, /* Torque Limit */
  { "MT", 0x8B }, /* Motor Type */
  { "CE", 0x8C }, /* Configure Encoder */
  { "OE", 0x8D }, /* Off on Error */
  { "FL", 0x8E }, /* Forward Software Limit */
  { "BL", 0x8F }, /* Reverse Software Limit */

  { "AC", 0x90 }, /* Acceleration */
  { "DC", 0x91 }, /* Deceleration */
  { "SP", 0x92 }, /* Speed */
  { "IT", 0x93 }, /* Independent Time Constant - Smoothing Function */
  { "FA", 0x94 }, /* Acceleration Feedforward */
  { "FV", 0x95 }, /* Velocity Feedforward */
  { "GR", 0x96 }, /* Gear Ratio */
  { "DP", 0x97 }, /* Define Position */
  { "DE", 0x98 }, /* Dual (Auxiliary) Encoder Position */
  { "OF", 0x99 }, /* Offset */

  { "BG", 0xA0 }, /* Begin */
  { "ST", 0xA1 }, /* Stop */
  { "AB", 0xA2 }, /* Abort */
  { "HM", 0xA3 }, /* Home */
  { "FE", 0xA4 }, /* Find Edge */
  { "FI", 0xA5 }, /* Find Index */
  { "PA", 0xA6 }, /* Position Absolute */
  { "PR", 0xA7 }, /* Position Relative */
  { "JG", 0xA8 }, /* Jog */
  { "MO", 0xA9 }, /* Motor Off */
  { "SH", 0xAA }, /* Servo Here */

  { "LM", 0xB0 }, /* Linear Interpolation Mode */
  { "LI", 0xB1 }, /* Linear Interpolation Distance */
  { "VP", 0xB2 }, /* Vector Position */
  { "CR", 0xB3 }, /* Circle */
  { "TN", 0xB4 }, /* Tangent */
  { "LE", 0xB5 }, /* Linear Interpolation End */
  { "VT", 0xB6 }, /* Vector Time Constant - S Curve */
  { "VA", 0xB7 }, /* Vector Acceleration */
  { "VD", 0xB8 }, /* Vector Deceleration */
  { "VS", 0xB9 }, /* Vector Speed */
  { "VR", 0xBA }, /* Vector Speed Ratio */
  { "CM", 0xBD }, /* Contouring Mode */
  { "CD", 0xBE }, /* Contour Data */
  { "DT", 0xBF }, /* Delta Time */

  { "ET", 0xC0 }, /* Electronic cam table */
  { "EM", 0xC1 }, /* Cam cycles */
  { "EP", 0xC2 }, /* Cam table intervals and starting point */
  { "EG", 0xC3 }, /* ECAM go (engage) */
  { "EB", 0xC4 }, /* Enable ECAM */
  { "EQ", 0xC5 }, /* ECAM quit (disengage) */
  { "EC", 0xC6 }, /* ECAM table position */
  { "AM", 0xC8 }, /* After Move */
  { "MC", 0xC9 }, /* Motion Complete - "In Position" */
  { "TW", 0xCA }, /* Timeout for IN-Position (MC) */
  { "MF", 0xCB }, /* Forward Motion to Position */
  { "MR", 0xCC }, /* Reverse Motion to Position */
  { "AD", 0xCD }, /* After Distance */
  { "AP", 0xCE }, /* After Absolute Position */
  { "AR", 0xCF }, /* After Relative Position */

  { "AS", 0xD0 }, /* At Speed */
  { "AI", 0xD1 }, /* After Input */
  { "AT", 0xD2 }, /* At Time */
  { "WT", 0xD3 }, /* Wait */
  { "WC", 0xD4 }, /* Wait for Contour Data */
  { "RP", 0xD8 }, /* Reference Position */
  { "TP", 0xD9 }, /* Tell Position */
  { "TE", 0xDA }, /* Tell Error */
  { "TD", 0xDB }, /* Tell Dual Encoder */
  { "TV", 0xDC }, /* Tell Velocity */
  { "RL", 0xDD }, /* Report Latched Position */
  { "TT", 0xDE }, /* Tell Torque */
  { "TS", 0xDF }, /* Tell Switches */

  { "TI", 0xE0 }, /* Tell Inputs */
  { "SC", 0xE1 }, /* Stop Code */
  { "TM", 0xE5 }, /* Time */
  { "CN", 0xE6 }, /* Configure */
  { "LZ", 0xE7 }, /* Leading Zeros */
  { "OP", 0xE8 }, /* Output Port */
  { "OB", 0xE9 }, /* Output Bit */
  { "SB", 0xEA }, /* Set Bit */
  { "CB", 0xEB }, /* Clear Bit */
  { "II", 0xEC }, /* Input Interrupt */
  { "EI", 0xED }, /* Enable Interrupts */
  { "AL", 0xEE }, /* Arm Latch */

  { "@@", 0xF0 }, /* special command 1 */
  { "@@", 0xF1 }, /* special command 2 */
  { "@@", 0xF2 }, /* special command 3 */
  { "@@", 0xF3 }, /* special command 4 */
  { "@@", 0xF4 }, /* special command 5 */
  { "@@", 0xF5 }, /* special command 6 */
  { "@@", 0xF6 }, /* special command 7 */
  { "@@", 0xF7 }, /* special command 8 */
  { "@@", 0xF8 }, /* special command 9 */
  { "@@", 0xF9 }, /* special command 10 */
  { "@@", 0xFA }, /* special command 11 */
  { "@@", 0xFB }, /* special command 12 */
  { "@@", 0xFC }, /* special command 13 */
  { "@@", 0xFD }, /* special command 14 */
  { "@@", 0xFE }, /* special command 15 */
  { "@@", 0xFF }, /* special command 16 */

  { "\0\0", 0 }
};

/* Clear block of size bytes */
void BlockClear(void *block, int size)
{
	memset(block, 0, size);
}

/* Copy size bytes from source to destination */
void BlockCopy(void *dest, void *source, int size)
{
	memmove(dest, source, size);
}

/* Register special DMC commands. Return 0 on success, else non-0 */
int Register_special_dmc_command(char *ascii_command, unsigned char binary_command)
{
	LegalCommand *cmd_ptr = local_legal_commands;
	int found = 0;

	if (binary_command < 0xF0)
		return(DMCERROR_CONVERSION);

	if (!ascii_command ||
		ascii_command[0] <= 26 ||
		ascii_command[1] <= 26)
		return(DMCERROR_CONVERSION);

	while (cmd_ptr->bcmd && !found)
	{
		if (cmd_ptr->bcmd == binary_command)
			found = 1;
		else
			cmd_ptr++;
	}

	if (!found)
		return(DMCERROR_CONVERSION);

	cmd_ptr->acmd[0] = ascii_command[0];
	cmd_ptr->acmd[1] = ascii_command[1];
	cmd_ptr->acmd[2] = 0;

	return 0;
}

/* Register special DMC command file */
int Register_special_dmc_file(char *special_filename)
{
	FILE *special_id = 0;
	char ascii_buffer[MAX_CONVERSION_STRING+PATCH];
	char *ascii_ptr;
	char ascii_command[3];
	unsigned char binary_command = 0;
	int special_count = 0;
	char cur_char;
	int done = 0;

	if (!special_filename || !special_filename[0])
		return(DMCERROR_ARGUMENT);

	if ((special_id = fopen(special_filename,"r")) == 0)
		return(DMCERROR_FILE);

	ascii_command[2] = 0;
	while (!done)
	{
		if (fgets(ascii_buffer,MAX_CONVERSION_STRING,special_id) == 0)
		{
			done = 1;
			break;
		}

		if (ascii_buffer[0] != 'F')
			break;

		cur_char = ascii_buffer[1];
		if (cur_char >= '0' && cur_char <= '9')
			binary_command = (unsigned char)(cur_char - '0' + 0xF0);
		else if (cur_char >= 'A' && cur_char <= 'F')
			binary_command = (unsigned char)(cur_char - 'A' + 0xFA);
		else
			continue;

		ascii_ptr = &ascii_buffer[2];
		if (*ascii_ptr == 0)
			continue;

		while ((*ascii_ptr == ' ') || (*ascii_ptr == '\t'))
			ascii_ptr++;

		if (*ascii_ptr <= 26 || *(ascii_ptr+1) <= 26)
			continue;

		ascii_command[0] = *ascii_ptr;
		ascii_command[1] = *(ascii_ptr+1);

		if (Register_special_dmc_command(ascii_command,binary_command) == 0)
			special_count++;
	}

	fclose(special_id);

	return 0;
}

/* Given a binary command, return an ascii command */
char* Get_dmc_ascii_command(unsigned char binary_command)
{
	char *ascii_command = 0;
	short found = 0;
	LegalCommand *cmd_ptr = local_legal_commands;

	if (binary_command == 0)
		 return(0);

	while (cmd_ptr->bcmd && !found)
	{
		 if (cmd_ptr->bcmd == binary_command)
			found = 1;
		else
			cmd_ptr++;
	}

	if (found)
		ascii_command = cmd_ptr->acmd;

	return ascii_command;
}

/* Given an ascii command, return a binary command */
unsigned char Get_dmc_binary_command(char *ascii_command)
{
	unsigned char binary_command = 0;
	short found = 0;
	LegalCommand *cmd_ptr = local_legal_commands;

	if (ascii_command == 0 || ascii_command[0] == '\0')
		return(0);

	while (cmd_ptr->acmd[0] && !found)
	{
		if ((cmd_ptr->acmd[0] == ascii_command[0]) && (cmd_ptr->acmd[1] == ascii_command[1]))
			found = 1;
		else
			cmd_ptr++;
	}

	if (found)
		binary_command = cmd_ptr->bcmd;

	return binary_command;
}

/* Check and set axis_bits and axis_counts, and return axis_count */
short Check_axis_bits(DMCCVS *dmccvs, char *data_ptr)
{
	short axis_counts = 0;
	char str_ptr[MAX_CONVERSION_STRING+PATCH];
	int length = 0;
	char axis_bits = 0;

	if (!data_ptr || !data_ptr[0])
		return(0);

	str_ptr[0] = 0;
	/* Axes are only valid before the = - . $ or , signs */
	while (*data_ptr &&
		((*data_ptr) != '=') &&
		((*data_ptr) != '.') &&
		((*data_ptr) != '-') &&
		((*data_ptr) != '$') &&
		((*data_ptr) != '[') &&
		((*data_ptr) != ']') &&
		((*data_ptr) != ','))
	{
		str_ptr[length] = *data_ptr;
		length++;
		data_ptr++;
		str_ptr[length] = 0;
	}

	if (!str_ptr[0])
		return(0);

	if (strchr(str_ptr,'X') || strchr(str_ptr,'A'))
	{
		axis_counts++;
		axis_bits |= DATA_X_BIT;
		dmccvs->axis_valid[0] = 1;
	}

	if (strchr(str_ptr,'Y') || strchr(str_ptr,'B'))
	{
		axis_counts++;
		axis_bits |= DATA_Y_BIT;
		dmccvs->axis_valid[1] = 1;
	}

	if (strchr(str_ptr,'Z') || strchr(str_ptr,'C'))
	{
		axis_counts++;
		axis_bits |= DATA_Z_BIT;
		dmccvs->axis_valid[2] = 1;
	}

	if (strchr(str_ptr,'W') || strchr(str_ptr,'D'))
	{
		axis_counts++;
		axis_bits |= DATA_W_BIT;
		dmccvs->axis_valid[3] = 1;
	}

	if (strchr(str_ptr,'E'))
	{
		axis_counts++;
		axis_bits |= DATA_E_BIT;
		dmccvs->axis_valid[4] = 1;
	}

	if (strchr(str_ptr,'F'))
	{
		axis_counts++;
		axis_bits |= DATA_F_BIT;
		dmccvs->axis_valid[5] = 1;
	}

	if (strchr(str_ptr,'G'))
	{
		axis_counts++;
		axis_bits |= DATA_G_BIT;
		dmccvs->axis_valid[6] = 1;
	}

	if (strchr(str_ptr,'H'))
	{
		axis_counts++;
		axis_bits |= DATA_H_BIT;
		dmccvs->axis_valid[7] = 1;
	}

	/* Note:	The trick here is that if any axis is found, then the order of data
		should be reassigned accordingly */
	if (axis_counts > 0)
	{
		dmccvs->axis_counts = axis_counts;
		if (!dmccvs->star_counts)
			dmccvs->axis_bits = axis_bits;
	}

	return axis_counts;
}

/* Return 1 if S bit is set, else 0 */
short Check_s_bit(DMCCVS *dmccvs, char *ascii_command)
{
	if (!ascii_command || !ascii_command[0])
		return 0;

	if (strchr(ascii_command,'S'))
	{
		dmccvs->s_bit |= DATA_S_BIT;
		dmccvs->number_format = DATA_BYTE_NULL;
		return 1;
	}
	else
		return 0;
}

/* Convert a hexdecimal number string into a double */
double DmcCvs_hex_to_double(char *number_str)
{
	long double result_number = 0;
	long double max_number = 1;
	double double_number = 0;
	int cur_number = 0;
	int i;
	int length;
	int negative = 0;

	if (!number_str || !number_str[0])
		return(0);

	length = strlen(number_str);

	if (number_str[0] == '8' || number_str[0] == '9' ||
		(number_str[0] >= 'A' && number_str[0] <= 'F') ||
		(number_str[0] >= 'a' && number_str[0] <= 'f'))
		negative = 1;

	for (i = 0; i < length; i++)
	{
		if (number_str[i] >= '0' && number_str[i] <= '9')
			cur_number = number_str[i] - '0';
		else if (number_str[i] >= 'A' && number_str[i] <= 'F')
			cur_number = number_str[i] - 'A' + 10;
		else if (number_str[i] >= 'a' && number_str[i] <= 'f')
			cur_number = number_str[i] - 'a' + 10;
		else
			break;
		max_number *= 16;
		result_number *= 16;
		result_number += cur_number;
	}

	if (negative)
		result_number -= max_number;

	double_number = (double)result_number;

	return double_number;
}

/* Check comma counts and return axis count */
short Check_data_numbers(DMCCVS *dmccvs, char *data_ptr)
{
	int i;
	int length;
	int axis_index = 0;
	char cur_char = 0;
	char *end_ptr = 0;
	char number_str[MAX_DATA_SIZE+PATCH];
	int number_index = 0;
	short data_counts = 0;
	int decimal_point_found = 0;
	int hexdecimal_found = 0;
	char data_bits = 0x1;
	double largest_number = 0;
	double least_number = 0;
	double cur_number = 0;

	if (!data_ptr || !data_ptr[0])
		return(0);

	if (dmccvs->equal_counts)
	{
		while (*data_ptr && (*data_ptr) != '=')
			data_ptr++;
		if (*data_ptr == 0)
			return(DMCERROR_CONVERSION);
	}

	/* Note: The trick here is to use the terminating null as the last comma */
	length = strlen(data_ptr) + 1;

	number_str[0] = 0;
	for (i = 0; i < length; i++)
	{
		cur_char = data_ptr[i];
		if (cur_char == ',' || cur_char == 0)
		{
			if (number_index)
			{
				if (!hexdecimal_found)
					cur_number = dmccvs->data_number[axis_index] =
						strtod(number_str,&end_ptr);
				else
					cur_number = dmccvs->data_number[axis_index] =
				DmcCvs_hex_to_double(number_str);
				if (!decimal_point_found)
				{
					if (cur_number > largest_number)
						largest_number = cur_number;
					if (cur_number < least_number)
						least_number = cur_number;
				}
				dmccvs->data_valid[axis_index] = 1;
				dmccvs->axis_bits |= data_bits;
				data_counts++;
			}
			if (!cur_char)
				break;
			number_str[0] = 0;
			number_index = 0;
			axis_index++;
			data_bits *= 2;
			hexdecimal_found = 0;
			if (axis_index >= AXIS_MAXIMUM)
				break;
		}
		else if ((cur_char >= '0' && cur_char <= '9') ||
			(hexdecimal_found && cur_char >= 'A' && cur_char <= 'F') ||
			(hexdecimal_found && cur_char >= 'a' && cur_char <= 'f') ||
			cur_char == '.' ||
			cur_char == '-')
		{
			if (cur_char == '.')
			{
				if (hexdecimal_found)
					return(DMCERROR_CONVERSION);
				decimal_point_found = 1;
			}
			else if (cur_char == '-')
			{
				/* - has to be first character */
				if (number_index)
					return(DMCERROR_CONVERSION);
			}
			number_str[number_index] = cur_char;
			number_str[number_index+1] = 0;
			number_index++;
		}
		else if (cur_char == '$')
		{
			/* $ has to be first character */
			if (number_index)
				return(DMCERROR_CONVERSION);
			hexdecimal_found = 1;
		}
	}

	if (data_counts > 0)
	{
		dmccvs->data_counts = data_counts;
		if (decimal_point_found)
		{
			dmccvs->number_format = DATA_BYTE_REAL_6;
			dmccvs->data_bytes = DATA_BYTE_REAL_6;
		}
		else
		{
			if (largest_number < -least_number)
				largest_number = -least_number;
			if (largest_number < 128)
			{
				dmccvs->number_format = DATA_BYTE_INTEGER_1;
				dmccvs->data_bytes = DATA_BYTE_INTEGER_1;
			}
			else if (largest_number < 32768L)
			{
				dmccvs->number_format = DATA_BYTE_INTEGER_2;
				dmccvs->data_bytes = DATA_BYTE_INTEGER_2;
			}
			else
			{
				dmccvs->number_format = DATA_BYTE_INTEGER_4;
				dmccvs->data_bytes = DATA_BYTE_INTEGER_4;
			}
		}
	}
	else
	{
		dmccvs->number_format = DATA_BYTE_NULL;
		dmccvs->data_bytes = DATA_BYTE_NULL;
	}

  	return data_counts;
}

/* Return 1 if ascii_command is a comment or empty line, else 0 */
int Is_comment_command(char *ascii_command)
{
	if (!ascii_command || !ascii_command[0])
		return(1);
	else if (ascii_command[0] == 'R' &&
		ascii_command[1] == 'E' &&
		ascii_command[2] == 'M')
		return(1);
	else if (ascii_command[0] == 10 ||
		ascii_command[0] == 13)
		return(1);
	else
		return(0);
}

/* Return 1 is ascii_command has ?, else 0 */
int Is_interrogation_command(DMCCVS *dmccvs, char *ascii_command)
{
	if (strchr(ascii_command,'?'))
	{
		dmccvs->interrogation = 1;
		return(1);
	}
	else
		return(0);
}

/* Return 1 if ascii_command is a message command, else 0 */
int Is_message_command(DMCCVS *dmccvs, char *ascii_command)
{
	if (ascii_command[0] == 'M' && ascii_command[1] == 'G')
		return(1);
	else if (strchr(ascii_command,'\"'))
	{
		dmccvs->quote = 1;
		return(1);
	}
	else
		return(0);
}

/* Return total number of equal signs */
int Total_equal_signs(DMCCVS *dmccvs, char *ascii_command)
{
	int length;
	int i;

	dmccvs->equal_counts = 0;
	if (!ascii_command || !ascii_command[0])
		return(0);

	length = strlen(ascii_command);

	for (i = 0; i < length; i++)
	{
		if (ascii_command[i] == '=')
			dmccvs->equal_counts++;
	}

	return(dmccvs->equal_counts);
}

/* Check square brackets */
int Check_square_brackets(DMCCVS *dmccvs, char *ascii_command)
{
	int left_brackets = 0;
	int right_brackets = 0;
	int length;
	int hexdecimal_found = 0;
	int number_found = 0;
	char cur_char = 0;
	int cur_number = 0;
	int result_number = 0;
	int i;

	dmccvs->bracket_counts = 0;
	if (!ascii_command || !ascii_command[0])
		return(0);

	length = strlen(ascii_command);

	for (i = 0; i < length; i++)
	{
		cur_char = ascii_command[i];
		if (cur_char == '[')
		{
			left_brackets++;
			if (left_brackets > 1)
				return(DMCERROR_CONVERSION);
		}
		else if (cur_char == ']')
		{
			right_brackets++;
			if (right_brackets > 1 || !left_brackets || !number_found)
				return(DMCERROR_CONVERSION);
		}
		else if (left_brackets && !right_brackets)
		{
			if (cur_char == '=')
				return(DMCERROR_CONVERSION);
			else if (cur_char == '$')
			{
				/* $ has to be the first character */
				if (hexdecimal_found || number_found)
					return(DMCERROR_CONVERSION);
				hexdecimal_found = 1;
			}
			/* no - is allowed */
			else if (cur_char == '-')
				return(DMCERROR_CONVERSION);
			else if (cur_char >= 'A' && cur_char <= 'F')
			{
				if (!hexdecimal_found)
					return(DMCERROR_CONVERSION);
				number_found = 1;
				cur_number = cur_char - 'A' + 10;
				result_number *= 16;
				result_number += cur_number;
			}
			else if (cur_char >= 'a' && cur_char <= 'f')
			{
				if (!hexdecimal_found)
					return(DMCERROR_CONVERSION);
				number_found = 1;
				cur_number = cur_char - 'a' + 10;
				result_number *= 16;
				result_number += cur_number;
			}
			else if (cur_char >= '0' && cur_char <= '9')
			{
				number_found = 1;
				cur_number = cur_char - '0';
				if (!hexdecimal_found)
					result_number *= 10;
				else
					result_number *= 16;
				result_number += cur_number;
			}
			if (result_number > 256)
				return(DMCERROR_CONVERSION);
		}
	}

	if (left_brackets != right_brackets)
		return(DMCERROR_CONVERSION);
	if (result_number > 256)
		return(DMCERROR_CONVERSION);

	dmccvs->bracket_counts = (short)left_brackets;
	dmccvs->bracket_number = (short)result_number;

	if (left_brackets == 1)
	{
		dmccvs->prefix_required = 1;
		dmccvs->prefix_command[0] = DMC_EC_COMMAND;
		dmccvs->prefix_command[1] = DATA_BYTE_INTEGER_2;
		dmccvs->prefix_command[2] = 0;
		dmccvs->prefix_command[3] = DATA_X_BIT;
		if (result_number == 256)
		{
			dmccvs->prefix_command[4] = 1;
			dmccvs->prefix_command[5] = 0;
		}
		else
		{
			dmccvs->prefix_command[4] = 0;
			dmccvs->prefix_command[5] = (char)result_number;
		}
	}

	return(0);
}

/* Return total number of star signs */
int Total_star_signs(DMCCVS *dmccvs, char *ascii_command)
{
	int length;
	int i;

	dmccvs->star_counts = 0;
	if (!ascii_command || !ascii_command[0])
		return(0);

	length = strlen(ascii_command);

	for (i = 0; i < length; i++)
	{
		if (ascii_command[i] == '*')
			dmccvs->star_counts++;
	}

	if (dmccvs->star_counts)
		dmccvs->axis_bits |= DATA_ALL_BITS;

	return(dmccvs->star_counts);
}

/* Print the error */
void DmcCvs_error(DMCCVS *dmccvs, char *error_message)
{
#ifdef DMC_DEBUG
	if (error_message)
		DMCTrace("   REM Error - %s\n",error_message);

	if (dmccvs && dmccvs->ascii_command)
		DMCTrace("   %s\n",dmccvs->ascii_command);
#endif
}

/* Show dmccvs data on the screen */
void Show_dmccvs_data(DMCCVS *dmccvs, int atob_conversion)
{
#ifdef DMC_DEBUG
	int i;

	if (!dmccvs)
		return;

	if (atob_conversion)
		DMCTrace("   REM atob [%s]",dmccvs->ascii_command);
	else
		DMCTrace("   REM btoa [%s]",dmccvs->ascii_command);

	if (dmccvs->data_counts)
	{
		if (atob_conversion)
			DMCTrace(" data ");
		for (i = 0; i < AXIS_MAXIMUM; i++)
		{
			if (dmccvs->data_valid[i])
			{
				if (dmccvs->number_format > DATA_BYTE_INTEGER_4)
					DMCTrace("[%.4f]",dmccvs->data_number[i]);
				else
					DMCTrace("[%.0f]",dmccvs->data_number[i]);
			}
			else if (dmccvs->star_counts == 1 && dmccvs->equal_counts == 1)
			{
				if (dmccvs->number_format > DATA_BYTE_INTEGER_4)
					DMCTrace("[%.4f]",dmccvs->data_number[0]);
				else
					DMCTrace("[%.0f]",dmccvs->data_number[0]);
			}
			else
				DMCTrace("[]");
		}
	}

	DMCTrace("\n");

	if (dmccvs->prefix_required)
	{
		DMCTrace("   ");
		for (i = 0; i < 4; i++)
			DMCTrace("%.2x",dmccvs->prefix_command[i]);
		DMCTrace(" ");
		DMCTrace("%.2x",dmccvs->prefix_command[4]);
		DMCTrace("%.2x",dmccvs->prefix_command[5]);
		DMCTrace(" ");
	}

	DMCTrace("%.2x",dmccvs->binary_command);
	DMCTrace("%.2x",dmccvs->number_format);
	DMCTrace("%.2x",dmccvs->s_bit);
	DMCTrace("%.2x",dmccvs->axis_bits);

	if (dmccvs->data_counts)
	{
		DMCTrace(" ");
		for (i = 0; i < dmccvs->data_size; i++)
			DMCTrace("%.2x",dmccvs->binary_data[i]);
	}
	DMCTrace("\n");
#endif
}

/* Return 1 if input_char is a valid input, else 0 */
int Valid_input(char input_char)
{
	if ((input_char >= 'A' && input_char <= 'Z') ||
		(input_char >= '0' && input_char <= '9') ||
		input_char == '.' ||
		input_char == ',' ||
		input_char == '*' ||
		input_char == '=' ||
		input_char == '[' ||
		input_char == ']' ||
		input_char == '$' ||
		input_char == '-')
		return(1);
	else
		return(0);
}

/* Reassign single data number */
void Reassign_single_data_number(DMCCVS *dmccvs)
{
	int i;
	int axis_index = -1;
	double axis_number = 0;
	int data_found = 0;

	if (dmccvs->axis_counts != 1 || dmccvs->data_counts != 1)
		return;

	for (i = 0; i < AXIS_MAXIMUM; i++)
	{
		if (dmccvs->data_valid[i])
		{
			axis_number = dmccvs->data_number[i];
			dmccvs->data_valid[i] = 0;
			data_found = 1;
		}
			if (dmccvs->axis_valid[i])
				axis_index = i;
	}

	if (data_found && axis_index >= 0)
	{
		dmccvs->data_valid[axis_index] = 1;
		dmccvs->data_number[axis_index] = axis_number;
	}
}

/* This function is really ugly, but it works this way for Borland C++
	v4.53 compiler due to possible compiler overflow bugs */
void Do_compliment(unsigned char *data_ptr, int data_offset)
{
	unsigned int value = 0;
	int borrow = 0;

#ifdef DMC_DEBUG
	int i;

	DMCTrace("Before compliment ");
	for (i = 0; i < data_offset; i++)
		DMCTrace("[%.2x]",data_ptr[i]);
	DMCTrace("\n");
#endif

	if (!data_ptr || data_offset <= 0)
		return;

	/* data_offset-1 could be 0, 1, 3, 5 */
	if (data_ptr[data_offset-1])
	{
		borrow = 1;
		value = 256 - data_ptr[data_offset-1];
		data_ptr[data_offset-1] = (unsigned char)value;
	}

	if (data_offset >= 6)
	{
		if (borrow)
			value = 255 - data_ptr[4];
		else if (data_ptr[4])
		{
			borrow = 1;
			value = 256 - data_ptr[4];
		}
		data_ptr[4] = (unsigned char)value;

		if (borrow)
			value = 255 - data_ptr[3];
		else if (data_ptr[3])
		{
			borrow = 1;
			value = 256 - data_ptr[3];
		}
		data_ptr[3] = (unsigned char)value;
	}

	if (data_offset >= 4)
	{
		if (borrow)
			value = 255 - data_ptr[2];
		else if (data_ptr[2])
		{
			borrow = 1;
			value = 256 - data_ptr[2];
		}
		data_ptr[2] = (unsigned char)value;

		if (borrow)
			value = 255 - data_ptr[1];
		else if (data_ptr[1])
		{
			borrow = 1;
			value = 256 - data_ptr[1];
		}
		data_ptr[1] = (unsigned char)value;
	}

	if (data_offset >= 2)
	{
		if (borrow)
			value = 255 - data_ptr[0];
		else if (data_ptr[0])
		{
			borrow = 1;
			value = 256 - data_ptr[0];
		}
		data_ptr[0] = (unsigned char)value;
	}

#ifdef DMC_DEBUG
	DMCTrace("After compliment  ");
	for (i = 0; i < data_offset; i++)
		DMCTrace("[%.2x]",data_ptr[i]);
	DMCTrace("\n");
#endif
}

/* Assign values to dmccvs->data accordingly */
void Assign_data_bytes(DMCCVS *dmccvs)
{
	int i;
	long double axis_number = 0;
	long double floor_number = 0;
	long double fraction_number = 0;
	int data_offset;
	unsigned char *data_ptr;
	unsigned long high_byte_integer = 0;
	unsigned long low_byte_integer = 0;
	unsigned long fraction_integer = 0;
	int negative = 0;

	if (!dmccvs || !dmccvs->data_counts)
		return;

	data_ptr = dmccvs->binary_data;
	data_offset = dmccvs->data_bytes;

	for (i = 0; i < AXIS_MAXIMUM; i++)
	{
		if (dmccvs->star_counts || dmccvs->data_valid[i])
		{
			if (dmccvs->star_counts)
				axis_number = dmccvs->data_number[0];
			else
				axis_number = dmccvs->data_number[i];
			if (axis_number < 0)
			{
				negative = 1;
				axis_number = -axis_number;
			}
			else
				negative = 0;
			floor_number = (long double)floor((double)axis_number);
			fraction_number = (axis_number - floor_number) * 65536L;
			if (data_offset >= 4)
			{
				high_byte_integer = (unsigned long)(floor_number / 65536L);
				low_byte_integer = (unsigned long)(floor_number - high_byte_integer * 65536L);
				*data_ptr = (unsigned char)(high_byte_integer / 256);
				*(data_ptr+1) = (unsigned char)(high_byte_integer & 0xFF);
				*(data_ptr+2) = (unsigned char)(low_byte_integer / 256);
				*(data_ptr+3) = (unsigned char)(low_byte_integer & 0xFF);

				if (data_offset == 6)
				{
					fraction_integer = (long)fraction_number;
					*(data_ptr+4) = (unsigned char)(fraction_integer / 256);
					*(data_ptr+5) = (unsigned char)(fraction_integer & 0xFF);
				}
			}
			else
			{
				low_byte_integer = (long)floor_number;
				if (data_offset == 1)
					*data_ptr = (unsigned char)low_byte_integer;
				else if (data_offset == 2)
				{
					*data_ptr = (unsigned char)(low_byte_integer / 256);
					*(data_ptr+1) = (unsigned char)(low_byte_integer & 0xFF);
				}
			}

			/* Calculate its complement */
			if (negative)
				Do_compliment(data_ptr,data_offset);

			/* data should be assigned consecutively */
			data_ptr += data_offset;
		}
	}
}

/* Examples that should pass the conversion -
		SPX = 1000
		SPY = 1000
		SP* = 1000
		SP 1000
			SP,1000,,,,,,1000

	Examples that are tough to convert -
		SP X=$ABCD
		SP $FFFF,1000,$ABCD

		Examples that should fail the conversion -
		SP -$FFFF.FF
		SP $-FFFF
			SP ?
			SP ,,,,?,,,?
		MG "HELLO"
			MG _SPX
		OB 1,POS 1
		ZZ

	Notice that all the parsing are order dependent, do not rearrange
	them randomly */
int Parse_ascii_command(DMCCVS *dmccvs, char *ascii_command,
	unsigned char *binary_command, int *binary_length)
{
	char *data_str = 0;

	if (!binary_command || !binary_length)
		return(DMCERROR_CONVERSION);

	binary_command[0] = 0;
	*binary_length = 0;

	if (!dmccvs)
		return(DMCERROR_CONVERSION);

	dmccvs->ascii_command = ascii_command;

	if (!ascii_command || !ascii_command[0] || !ascii_command[1])
		return(DMCERROR_CONVERSION);

	if (Is_comment_command(ascii_command))
		return(DMCERROR_CONVERSION);

	if (Is_interrogation_command(dmccvs,ascii_command))
	{
		DmcCvs_error(dmccvs,"Can not convert this interrogation command.");
		return(DMCERROR_CONVERSION);
	}

	if (Is_message_command(dmccvs,ascii_command))
	{
		DmcCvs_error(dmccvs,"Can not convert this message command.");
		return(DMCERROR_CONVERSION);
	}

	dmccvs->binary_command = Get_dmc_binary_command(ascii_command);
	if (!dmccvs->binary_command)
	{
		DmcCvs_error(dmccvs,"Can not find its binary command.");
		return(DMCERROR_CONVERSION);
	}

	data_str = &ascii_command[2];
	if (!data_str[0])
	{
		*binary_length = 4;
		BlockCopy(binary_command,dmccvs,4);
		return(0);
	}

	if (Total_equal_signs(dmccvs,data_str) > 1)
	{
		DmcCvs_error(dmccvs,"Can not convert multiple equal signs.");
		return(DMCERROR_CONVERSION);
	}

	if (Check_square_brackets(dmccvs,data_str) != 0)
	{
		DmcCvs_error(dmccvs,"Invalid square bracket usage.");
		return(DMCERROR_CONVERSION);
	}
	else if (dmccvs->bracket_counts == 1)
	{
		if (dmccvs->binary_command != DMC_ET_COMMAND)
		{
			DmcCvs_error(dmccvs,"Brackets are only allowed for ET command.");
			return(DMCERROR_CONVERSION);
		}
		else if (dmccvs->equal_counts != 1)
		{
			DmcCvs_error(dmccvs,"Equal sign mismatched brackets.");
			return(DMCERROR_CONVERSION);
		}
	}

	if (Total_star_signs(dmccvs,data_str) > 1)
	{
		DmcCvs_error(dmccvs,"Can not convert multiple star signs.");
		return(DMCERROR_CONVERSION);
	}
	else if (dmccvs->star_counts == 1 && dmccvs->equal_counts != 1)
	{
		DmcCvs_error(dmccvs,"Equal sign is missing.");
		return(DMCERROR_CONVERSION);
	}

	Check_s_bit(dmccvs,data_str);
	if (dmccvs->s_bit && dmccvs->equal_counts)
	{
		DmcCvs_error(dmccvs,"No assignment is allowed for S.");
		return(DMCERROR_CONVERSION);
	}

	if (Check_data_numbers(dmccvs,data_str) < 0)
	{
		DmcCvs_error(dmccvs,"Can not convert number.");
		return(DMCERROR_CONVERSION);
	}

	if (dmccvs->s_bit && dmccvs->data_counts)
	{
		DmcCvs_error(dmccvs,"No data is allowed for S.");
		return(DMCERROR_CONVERSION);
	}

	Check_axis_bits(dmccvs,data_str);
	if (dmccvs->axis_counts > 1)
	{
		if (dmccvs->data_counts ||
			dmccvs->star_counts ||
			dmccvs->equal_counts)
			{
				DmcCvs_error(dmccvs,"Can not assign values to multiple axes.");
				return(DMCERROR_CONVERSION);
			}
	}
	else if (dmccvs->axis_counts == 1)
	{
		if (dmccvs->data_counts > 1)
		{
			DmcCvs_error(dmccvs,"Single axis can not have multiple data.");
			return(DMCERROR_CONVERSION);
		}
		else if (dmccvs->data_counts && dmccvs->equal_counts != 1)
		{
			DmcCvs_error(dmccvs,"Mismatched equal signs and data counts.");
			return(DMCERROR_CONVERSION);
		}
		if (dmccvs->data_counts == 1)
			Reassign_single_data_number(dmccvs);
	}
	else if (dmccvs->axis_counts == 0)
	{
		if (dmccvs->equal_counts &&
			!dmccvs->star_counts &&
			!dmccvs->bracket_counts)
		{
			DmcCvs_error(dmccvs,"Axis is missing.");
			return(DMCERROR_CONVERSION);
		}
	}

	if (dmccvs->star_counts)
		dmccvs->data_size = (short)(dmccvs->data_bytes * AXIS_MAXIMUM);
	else if (dmccvs->axis_counts)
		dmccvs->data_size = (short)dmccvs->data_bytes * (short)dmccvs->axis_counts;
	else if (dmccvs->data_counts)
		dmccvs->data_size = (short)dmccvs->data_bytes * (short)dmccvs->data_counts;

	Assign_data_bytes(dmccvs);

	if (dmccvs->prefix_required)
	{
		BlockCopy(binary_command,dmccvs->prefix_command,MAX_PREFIX_COMMAND_SIZE);
		binary_command += MAX_PREFIX_COMMAND_SIZE;
	}

	*binary_length = 4 + dmccvs->data_size;
	BlockCopy(binary_command,dmccvs,*binary_length);

	if (dmccvs->prefix_required)
		*binary_length += MAX_PREFIX_COMMAND_SIZE;

	return(0);
}

/* Return axis count from binary axis_bits */
short TotalBinaryAxisCounts(DMCCVS *dmccvs, char *axis_string)
{
	unsigned char axis_bits;
	short axis_counts = 0;

	if (!dmccvs)
		return(0);

	axis_bits = dmccvs->axis_bits;

	if (axis_bits & DATA_X_BIT)
	{
		dmccvs->axis_valid[0] = 1;
		axis_string[axis_counts] = AXIS_X;
		axis_counts++;
	}
	if (axis_bits & DATA_Y_BIT)
	{
		dmccvs->axis_valid[1] = 1;
		axis_string[axis_counts] = AXIS_Y;
		axis_counts++;
	}
	if (axis_bits & DATA_Z_BIT)
	{
		dmccvs->axis_valid[2] = 1;
		axis_string[axis_counts] = AXIS_Z;
		axis_counts++;
	}
	if (axis_bits & DATA_W_BIT)
	{
		dmccvs->axis_valid[3] = 1;
		axis_string[axis_counts] = AXIS_W;
		axis_counts++;
	}
	if (axis_bits & DATA_E_BIT)
	{
		dmccvs->axis_valid[4] = 1;
		axis_string[axis_counts] = AXIS_E;
		axis_counts++;
	}
	if (axis_bits & DATA_F_BIT)
	{
		dmccvs->axis_valid[5] = 1;
		axis_string[axis_counts] = AXIS_F;
		axis_counts++;
	}
	if (axis_bits & DATA_G_BIT)
	{
		dmccvs->axis_valid[6] = 1;
		axis_string[axis_counts] = AXIS_G;
		axis_counts++;
	}
	if (axis_bits & DATA_H_BIT)
	{
		dmccvs->axis_valid[7] = 1;
		axis_string[axis_counts] = AXIS_H;
		axis_counts++;
	}

	axis_string[axis_counts] = 0;

	return(axis_counts);
}

/* Return double value from binary data */
double DmcCvs_binary_to_double(unsigned char *data_string, int data_bytes)
{
	long double result_value = 0;
	long double fraction_value = 0;
	double double_value;
	int int_index;
	int negative = 0;
	int i;

	if (data_bytes <= 0)
		return(0);

	if (data_string[0] > 127)
	{
		negative = 1;
		Do_compliment(data_string,data_bytes);
	}

	int_index = data_bytes;
	if (int_index > DATA_BYTE_INTEGER_4)
		int_index = DATA_BYTE_INTEGER_4;

	for (i = 0; i < int_index; i++)
	{
		result_value *= 256;
		result_value += data_string[i];
	}

	if (data_bytes > DATA_BYTE_INTEGER_4)
	{
		fraction_value = data_string[4];
		result_value += fraction_value / 256;
		fraction_value = data_string[5];
		result_value += fraction_value / 65536L;
	}

	double_value = (double)result_value;

	if (negative)
		double_value = -double_value;

	return(double_value);
}

/* Check binary data */
int Check_binary_data(DMCCVS *dmccvs, unsigned char *binary_data, char *data_string)
{
	int data_bytes = 0;
	char number_string[MAX_CONVERSION_STRING+PATCH];
	unsigned char byte_string[MAX_DATA_FIELD+PATCH];
	unsigned char *data_ptr;
	int first_data_found = 0;
	int i;

	if (!dmccvs)
		return(DMCERROR_CONVERSION);

	data_bytes = dmccvs->data_bytes;
	if (data_bytes <= DATA_BYTE_NULL)
		return(DMCERROR_CONVERSION);

	data_ptr = binary_data;

	strcat(data_string," ");
	for (i = 0; i < AXIS_MAXIMUM; i++)
	{
		if (dmccvs->axis_valid[i])
		{
			BlockCopy(byte_string,data_ptr,data_bytes);
			byte_string[data_bytes] = 0;
			dmccvs->data_number[i] = DmcCvs_binary_to_double(byte_string,data_bytes);
			dmccvs->data_valid[i] = 1;
			data_ptr += data_bytes;
			if (!first_data_found)
			{
				first_data_found = 1;
				sprintf(number_string,"%.4f",dmccvs->data_number[i]);
			}
			else
				sprintf(number_string,",%.4f",dmccvs->data_number[i]);
			strcat(data_string,number_string);
			}
		else
			strcat(data_string,",");
	}

	return(0);
}

/* Parse binary command to ascii command, return 0 on success, else non-0 */
long Parse_binary_command(DMCCVS *dmccvs, unsigned char *binary_command,
	long *binary_parsing_length, char *ascii_command, long *ascii_length)
{
	char axis_string[MAX_CONVERSION_STRING+PATCH];
	char data_string[MAX_CONVERSION_STRING+PATCH];

	if (!ascii_command || !ascii_length)
		return(DMCERROR_ARGUMENT);

	if (!dmccvs || !binary_command || !binary_parsing_length)
		return(DMCERROR_ARGUMENT);

	*binary_parsing_length = 0;
	dmccvs->ascii_command = Get_dmc_ascii_command(binary_command[0]);
	if (dmccvs->ascii_command == 0)
	{
		DmcCvs_error(dmccvs,"Can not find the ascii command.");
		ascii_command[0] = 0;
		*ascii_length = 0;
		return(DMCERROR_CONVERSION);
	}

	ascii_command[0] = dmccvs->ascii_command[0];
	ascii_command[1] = dmccvs->ascii_command[1];
	ascii_command[2] = 0;
	*ascii_length = 2;
	dmccvs->binary_command = binary_command[0];
	*binary_parsing_length = 4;

	if (binary_command[1] > DATA_BYTE_REAL_6)
	{
		DmcCvs_error(dmccvs,"Invalid number format.");
		return(DMCERROR_CONVERSION);
	}

	dmccvs->number_format = binary_command[1];
	dmccvs->data_bytes = dmccvs->number_format;

	dmccvs->s_bit = binary_command[2];
	if (dmccvs->s_bit)
	{
		ascii_command[2] = ' ';
		ascii_command[3] = 'S';
		ascii_command[4] = 0;
		*ascii_length = 4;
	}

	axis_string[0] = 0;
	dmccvs->axis_bits = binary_command[3];
	dmccvs->axis_counts = TotalBinaryAxisCounts(dmccvs,axis_string);
	if (dmccvs->axis_counts == 0)
	{
		if (dmccvs->data_bytes != DATA_BYTE_NULL)
		{
			DmcCvs_error(dmccvs,"Wrong number format.");
			return(DMCERROR_CONVERSION);
		}
		else
		{
			Show_dmccvs_data(dmccvs,0);
			return(0);
		}
	}

	dmccvs->data_size = dmccvs->data_bytes * dmccvs->axis_counts;
	*binary_parsing_length += dmccvs->data_size;
	if (dmccvs->data_size == DATA_BYTE_NULL)
	{
		if (!dmccvs->s_bit)
			strcat(ascii_command," ");
		strcat(ascii_command,axis_string);
		*ascii_length = strlen(ascii_command);
		Show_dmccvs_data(dmccvs,0);
		return(0);
	}
	else if (dmccvs->data_bytes > DATA_BYTE_NULL)
	{
		dmccvs->data_counts = dmccvs->axis_counts;
		if (dmccvs->s_bit)
		{
			DmcCvs_error(dmccvs,"Can not have data for S bit.");
			return(DMCERROR_CONVERSION);
		}
		data_string[0] = 0;
		if (Check_binary_data(dmccvs,&binary_command[4],data_string) == 0)
		{
			BlockCopy(dmccvs->binary_data,&binary_command[4],dmccvs->data_size);
			Show_dmccvs_data(dmccvs,0);
		}
		strcat(ascii_command,data_string);
		*ascii_length = strlen(ascii_command);
	}

  return(0);
}

/* Convert a binary_command to the ascii_result_buffer, fill up the
	result_length, and return 0 on success, else non-0.
	In case that ascii_buffer_size is too small, conversion also fails */
long CommandBinaryToAscii(unsigned char *binary_command, long binary_command_length,
	char *ascii_result_buffer, long ascii_buffer_size, long *ascii_result_length)
{
	char cur_result_buffer[MAX_CONVERSION_STRING+PATCH];
	long cur_result_length = 0;
	long binary_parsing_length = 0;
	long total_parsing_length = 0;
	DMCCVS localDmcCvs;
	DMCCVS *dmccvs;

	dmccvs = &localDmcCvs;
	BlockClear(dmccvs,sizeof(localDmcCvs));

	if (!binary_command || binary_command[0] == 0)
	{
		DmcCvs_error(dmccvs,"Null binary command.");
		return(DMCERROR_ARGUMENT);
	}

	if (binary_command_length < 4)
	{
		DmcCvs_error(dmccvs,"Invalid binary command.");
		return(DMCERROR_CONVERSION);
	}

	if (ascii_buffer_size < 2)
	{
		DmcCvs_error(dmccvs,"Ascii buffer is too small.");
		return(DMCERROR_CONVERSION);
	}

	ascii_result_buffer[0] = 0;
	*ascii_result_length = 0;
	while (total_parsing_length < binary_command_length)
	{
		if (Parse_binary_command(dmccvs,binary_command,&binary_parsing_length,
			cur_result_buffer,&cur_result_length) != 0)
			return(DMCERROR_CONVERSION);
		else if (binary_parsing_length <= 0)
			return(DMCERROR_CONVERSION);
		else
		{
			total_parsing_length += binary_parsing_length;
			binary_command += (int)binary_parsing_length;
		}

		if (cur_result_length <= 0 || !cur_result_buffer[0])
			return(DMCERROR_CONVERSION);
		else if (cur_result_length > ascii_buffer_size)
			return(DMCERROR_CONVERSION);

		if (*ascii_result_length)
		{
			*ascii_result_buffer = 10;
			*(ascii_result_buffer+1) = 13;
			ascii_result_buffer += 2;
			*ascii_result_length += 2;
			ascii_buffer_size -= 2;
		}

		BlockCopy(ascii_result_buffer,cur_result_buffer,(int)cur_result_length);
		ascii_result_buffer += (int)cur_result_length;
		*ascii_result_length += cur_result_length;
		ascii_buffer_size -= cur_result_length;
	}

	return(0);
}

/* Copy conversion results of ascii_command to the binary_result_buffer,
	fill up the result_length, and return 0 on success, else non-0.
	In case that binary_buffer_size is too small, conversion also fails. */
long CommandAsciiToBinary(char *ascii_command, long ascii_command_length,
	unsigned char *binary_result_buffer, long binary_buffer_size, long *binary_result_length)
{
	char ascii_buffer[MAX_CONVERSION_STRING+PATCH];
	unsigned char cur_result_buffer[MAX_CONVERSION_STRING+PATCH];
	unsigned char *result_ptr;
	int cur_result_length = 0;
	int total_result_length = 0;
	int ascii_index;
	int i;
	DMCCVS localDmcCvs;
	DMCCVS *dmccvs;

	if (!ascii_command || !ascii_command[0] || !ascii_command[1])
		return(DMCERROR_ARGUMENT);

	if (!binary_result_buffer || binary_buffer_size < 4)
		return(DMCERROR_ARGUMENT);

	dmccvs = &localDmcCvs;
	ascii_index = 0;
	binary_result_buffer[0] = 0;
	*binary_result_length = 0;
	total_result_length = 0;
	cur_result_length = 0;

	for (i = 0; i < ascii_command_length; i++)
	{
		if (Valid_input(ascii_command[i]))
		{
			ascii_buffer[ascii_index] = ascii_command[i];
			ascii_index++;
			ascii_buffer[ascii_index] = 0;
		}

		if (ascii_command[i] == ';' ||
			(i == ascii_command_length - 1) ||
			ascii_command[i] == 0)
		{
			/* if valid data is found before the last ;, then no error imposed */
			if (ascii_buffer[0] == 0)
			{
				if (total_result_length)
				{
					*binary_result_length = total_result_length;
					return(0);
				}
			}

			BlockClear(dmccvs,sizeof(localDmcCvs));
			if (Parse_ascii_command(dmccvs,ascii_buffer,cur_result_buffer,
				&cur_result_length) != 0)
			return(DMCERROR_CONVERSION);

			if ((total_result_length + cur_result_length) > binary_buffer_size)
				return(DMCERROR_CONVERSION);

			Show_dmccvs_data(dmccvs,1);
			result_ptr = &binary_result_buffer[total_result_length];
			BlockCopy(result_ptr,cur_result_buffer,cur_result_length);
			total_result_length += cur_result_length;
			ascii_buffer[0] = 0;
			ascii_index = 0;
		}
	}

	*binary_result_length = total_result_length;

  	return(0);
}

/* Convert a DMC ascii command file to a DMC binary command file. All
	non-translatable commands will be kept as their old formats. */
long FileAsciiToBinary(char *input_filename, char *output_filename)
{
	char ascii_buffer[MAX_CONVERSION_STRING+PATCH];
	unsigned char binary_buffer[MAX_CONVERSION_STRING+PATCH];
	long ascii_length = 0;
	long binary_length = 0;
	int i;
	long ret_code;
	FILE *input_file_id = 0;
	FILE *output_file_id = 0;

	if (!input_filename || !input_filename[0])
		return(DMCERROR_ARGUMENT);

	input_file_id = fopen(input_filename,"r");
	if (!input_file_id)
		return(DMCERROR_FILE);

	if (output_filename && output_filename[0])
	{
		output_file_id = fopen(output_filename,"w");
		if (!output_file_id)
			return(DMCERROR_FILE);
	}
	else
		dmccvs_debug = 1;

	do
	{
		BlockClear(ascii_buffer,MAX_CONVERSION_STRING);
		BlockClear(binary_buffer,MAX_CONVERSION_STRING);
		ascii_length = 0;
		binary_length = 0;

		if (fgets(ascii_buffer,MAX_CONVERSION_STRING,input_file_id) == 0)
			break;

		ascii_length = strlen(ascii_buffer);
		while (ascii_length > 0)
		{
			if (ascii_buffer[(int)ascii_length-1] <= 26 ||
				ascii_buffer[(int)ascii_length-1] == ' ')
			{
				ascii_buffer[(int)ascii_length-1] = 0;
				ascii_length--;
			}
			else
				break;
		}

		ret_code = CommandAsciiToBinary(ascii_buffer,ascii_length,binary_buffer,
			MAX_CONVERSION_STRING,&binary_length);

		if (output_file_id)
		{
			if (ret_code == 0)
			{
				for (i = 0; i < binary_length; i++)
					fputc(binary_buffer[i],output_file_id);
			}
			else if (!Is_comment_command(ascii_buffer))
			{
				for (i = 0; i < ascii_length; i++)
					fputc(ascii_buffer[i],output_file_id);
				fputs("\n",output_file_id);
			}
		}
	} while (input_file_id);

	fclose(input_file_id);
	if (output_file_id)
		fclose(output_file_id);

  	return(0);
}

/* Convert a DMC binary command file to a DMC ascii command file. All
	non-translatable commands will be kept as their old formats. */
long FileBinaryToAscii(char *input_filename, char *output_filename)
{
	char ascii_buffer[MAX_CONVERSION_STRING+PATCH];
	unsigned char binary_buffer[MAX_CONVERSION_STRING+PATCH];
	long ascii_length = 0;
	long binary_length = 0;
	int i;
	long ret_code = 0;
	FILE *input_file_id = 0;
	FILE *output_file_id = 0;
	int input_char = 0;
	int quit = 0;
	int no_conversion = 0;
	DMCCVS localDmcCvs;
	DMCCVS *dmccvs;

	if (!input_filename || !input_filename[0])
		return(DMCERROR_ARGUMENT);

	input_file_id = fopen(input_filename,"rb");
	if (!input_file_id)
		return(DMCERROR_FILE);

	if (output_filename && output_filename[0])
	{
		output_file_id = fopen(output_filename,"w");
		if (!output_file_id)
			return(DMCERROR_FILE);
	}
	else
		dmccvs_debug = 1;

	dmccvs = &localDmcCvs;
	do
	{
		BlockClear(ascii_buffer,MAX_CONVERSION_STRING);
		BlockClear(binary_buffer,MAX_CONVERSION_STRING);
		ascii_length = 0;
		binary_length = 0;
		no_conversion = 0;
		ret_code = 0;

		if (feof(input_file_id))
			break;
		else if (!input_char)
		{
			input_char = fgetc(input_file_id);
			if(feof(input_file_id))
				break;
		}

		if (input_char & BINARY_COMMAND_BIT)
		{
			binary_buffer[0] = (unsigned char)input_char;
			input_char = 0;
			input_char = fgetc(input_file_id);
			if(feof(input_file_id))
		  		return(DMCERROR_CONVERSION);
			binary_buffer[1] = (unsigned char)input_char;
			input_char = fgetc(input_file_id);
			if(feof(input_file_id))
				return(DMCERROR_CONVERSION);
			binary_buffer[2] = (unsigned char)input_char;
			input_char = fgetc(input_file_id);
			if(feof(input_file_id))
				return(DMCERROR_CONVERSION);
			binary_buffer[3] = (unsigned char)input_char;

			binary_length = 4;
			BlockClear(dmccvs,sizeof(localDmcCvs));
			dmccvs->number_format = binary_buffer[1];
			dmccvs->axis_bits = binary_buffer[3];
			dmccvs->axis_counts = TotalBinaryAxisCounts(dmccvs,ascii_buffer);
			dmccvs->data_size = dmccvs->number_format * dmccvs->axis_counts;
			if (dmccvs->number_format > 0)
			{
				for (i = 0; i < dmccvs->data_size; i++)
				{
					input_char = fgetc(input_file_id);
					if (feof(input_file_id))
						return(DMCERROR_CONVERSION);
					binary_buffer[(int)binary_length] = (unsigned char)input_char;
					binary_length++;
				}
			}
			input_char = 0;

			ret_code = CommandBinaryToAscii(binary_buffer,binary_length,
			ascii_buffer,MAX_CONVERSION_STRING,&ascii_length);
		}
		else
		{
			no_conversion = 1;
			quit = 0;
			ascii_buffer[(int)ascii_length] = (char)input_char;
			ascii_length++;
			do
			{
				input_char = fgetc(input_file_id);
				if (feof(input_file_id))
					quit = 1;
				else if (input_char & BINARY_COMMAND_BIT)
					quit = 1;
				else if (input_char <= 26)
				{
					input_char = 0;
					quit = 1;
				}
				else
				{
					ascii_buffer[(int)ascii_length] = (char)input_char;
					ascii_length++;
				}
			} while (!quit);
			ascii_buffer[(int)ascii_length] = 0;
		}

		if (output_file_id)
		{
			if (no_conversion)
			{
				fputs("REM Ascii Command\n",output_file_id);
				fprintf(output_file_id,"%s\n",ascii_buffer);
			}
			else if (ret_code == 0)
			{
				for (i = 0; i < ascii_length; i++)
					fputc(ascii_buffer[i],output_file_id);
				fputs("\n",output_file_id);
			}
			else
			{
				fputs("REM Conversion Error ",output_file_id);
				for (i = 0; i < binary_length; i++)
					fprintf(output_file_id,"%.2x",binary_buffer[i]);
				fputs("\n",output_file_id);
			}
		}
		else
		{
			ascii_buffer[(int)ascii_length] = 0;
#ifdef DMC_DEBUG
			if (ret_code)
				DMCTrace("   REM Conversion Error\n");
			if (ascii_buffer[0])
			{
				if (no_conversion)
					DMCTrace("   REM Ascii Command\n");
				DMCTrace("   %s\n",ascii_buffer);
			}
#endif
		}
	} while (input_file_id);

	fclose(input_file_id);
	if (output_file_id)
		fclose(output_file_id);

	return(0);
}

/* Read special conversion file */
long ReadSpecialConversionFile(char *special_filename)
{
	return(Register_special_dmc_file(special_filename));
}

#endif /* DMC_BINARYCOMMANDS */
