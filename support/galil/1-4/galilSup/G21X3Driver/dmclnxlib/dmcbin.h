#ifndef INCLUDE_BINARYCOMMANDS
#define INCLUDE_BINARYCOMMANDS

/* Binary Command Format -
	A binary command is 4-byte header + (0 to 6)-byte data * (0 to 8) axis
	which could be 4 to 52 bytes long. */
#define BYTE_COMMAND                0x0
#define BYTE_FORMATS                0x1
#define BYTE_S_BIT                  0x2
#define BYTE_AXES                   0x3
#define BYTE_DATA_START             0x4

#define PATCH                       4
#define HEADER_SIZE                 4
#define MAX_PREFIX_COMMAND_SIZE     6
#define MAX_DATA_FIELD              6
#define MAX_DATA_SIZE               48
#define MAX_COMMAND_SIZE            52
#define MIN_COMMAND_SIZE            4
#define MAX_AXIS_NUMBER             8
#define MAX_CONVERSION_STRING       255

/* Command byte bit 7 has to be 1 to ensure a valid binary command */
#define BINARY_COMMAND_BIT          0x80

/* S-bit Byte signifies the existence of the S-Axis */
#define DATA_S_BIT                  0x1

/* Each bit in the axis byte corresponds to one axis -
	1 means X only, 2 means Y only, 3 means X and Y, etc. */
#define DATA_NULL_BIT               0x0
#define DATA_X_BIT                  0x1
#define DATA_Y_BIT                  0x2
#define DATA_Z_BIT                  0x4
#define DATA_W_BIT                  0x8
#define DATA_E_BIT                  0x10
#define DATA_F_BIT                  0x20
#define DATA_G_BIT                  0x40
#define DATA_H_BIT                  0x80
#define DATA_ALL_BITS               0xFF

/* Data format could be 0 to 6 bytes per axis */
#define DATA_BYTE_NULL              0
#define DATA_BYTE_INTEGER_1         1
#define DATA_BYTE_INTEGER_2         2
#define DATA_BYTE_INTEGER_4         4
#define DATA_BYTE_REAL_6            6

#define AXIS_MAXIMUM    8
#define AXIS_0		0
#define AXIS_1		1
#define AXIS_2		2
#define AXIS_3		3
#define AXIS_4		4
#define AXIS_5		5
#define AXIS_6		6
#define AXIS_7		7
#define AXIS_8		8

#define AXIS_A		'A'
#define AXIS_B		'B'
#define AXIS_C		'C'
#define AXIS_D		'D'
#define AXIS_E		'E'
#define AXIS_F		'F'
#define AXIS_G		'G'
#define AXIS_H		'H'
#define AXIS_S		'S'
#define AXIS_W		'W'
#define AXIS_X		'X'
#define AXIS_Y		'Y'
#define AXIS_Z		'Z'

#define DMC_ET_COMMAND    0xC0
#define DMC_EC_COMMAND    0xC6
#define DMC_EC_LENGTH     6

typedef struct _DMCCVS
{
	unsigned char binary_command; /* 1st byte */
	unsigned char number_format;  /* 2nd byte */
	unsigned char s_bit;          /* 3rd byte */
	unsigned char axis_bits;      /* 4th byte */
	unsigned char binary_data[MAX_DATA_SIZE+PATCH];
											/* 5th to 52nd bytes, 48 bytes maximum */
	short prefix_required;        /* prefix command is required or not */
	unsigned char prefix_command[MAX_PREFIX_COMMAND_SIZE+PATCH];
										  /* a prefix 6-byte EC command */
	short axis_counts;           /* how many axes */
	short data_counts;           /* how many data */
	short data_bytes;            /* how many bytes per data, 0 to 6 bytes */
	short data_size;             /* how many bytes in total, 0 to 48 bytes */
	char *ascii_command;         /* ascii command for parsing */
	short interrogation;         /* no ? is allowed */
	short quote;                 /* no " is allowed */
	short equal_counts;          /* only one = is allowed */
	short bracket_counts;        /* only one pair of [] is allowed */
	short bracket_number;        /* number inside the brackets */
	short star_counts;           /* only one * is allowed */
	short axis_valid[AXIS_MAXIMUM];
	short data_valid[AXIS_MAXIMUM];
	double data_number[AXIS_MAXIMUM];
	short print_message;
} DMCCVS;

typedef struct _LegalCommand
{
	char acmd[3];
	unsigned char bcmd;
}  LegalCommand;

long CommandBinaryToAscii(unsigned char *binary_command,
	long binary_command_length, char *ascii_result_buffer,
	long ascii_buffer_size, long *ascii_result_length);
long CommandAsciiToBinary(char *ascii_command,
	long ascii_command_length, unsigned char *binary_result_buffer,
	long binary_buffer_size, long *binary_result_length);
long FileAsciiToBinary(char *input_filename, char *output_filename);
long FileBinaryToAscii(char *input_filename, char *output_filename);
long ReadSpecialConversionFile(char *special_filename);
int Total_binary_axis_counts(unsigned char axis_bits);
void BlockClear(void *block, int size);
void BlockCopy(void *dest, void *source, int size);
int Register_special_dmc_command(char *ascii_command, unsigned char binary_command);
int Register_special_dmc_file(char *special_filename);
char* Get_dmc_ascii_command(unsigned char binary_command);
unsigned char Get_dmc_binary_command(char *ascii_command);
short Check_axis_bits(DMCCVS *dmccvs, char *data_ptr);
short Check_s_bit(DMCCVS *dmccvs, char *ascii_command);
double DmcCvs_hex_to_double(char *number_str);
short Check_data_numbers(DMCCVS *dmccvs, char *data_ptr);
int Is_comment_command(char *ascii_command);
int Is_interrogation_command(DMCCVS *dmccvs, char *ascii_command);
int Is_message_command(DMCCVS *dmccvs, char *ascii_command);
int Total_equal_signs(DMCCVS *dmccvs, char *ascii_command);
int Check_square_brackets(DMCCVS *dmccvs, char *ascii_command);
int Total_star_signs(DMCCVS *dmccvs, char *ascii_command);
void DmcCvs_error(DMCCVS *dmccvs, char *error_message);
void Show_dmccvs_data(DMCCVS *dmccvs, int atob_conversion);
int Valid_input(char input_char);
void Reassign_single_data_number(DMCCVS *dmccvs);
void Do_compliment(unsigned char *data_ptr, int data_offset);
void Assign_data_bytes(DMCCVS *dmccvs);
int Parse_ascii_command(DMCCVS *dmccvs, char *ascii_command,
	unsigned char *binary_command, int *binary_length);
short TotalBinaryAxisCounts(DMCCVS *dmccvs, char *axis_string);
double DmcCvs_binary_to_double(unsigned char *data_string, int data_bytes);
int Check_binary_data(DMCCVS *dmccvs, unsigned char *binary_data, char *data_string);
long Parse_binary_command(DMCCVS *dmccvs, unsigned char *binary_command,
	long *binary_parsing_length, char *ascii_command, long *ascii_length);

#endif /* INCLUDE_BINARYCOMMANDS */

