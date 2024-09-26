#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include <time.h>

/* Constants */
#define k_INT           0.024
#define k_EXT           0.048
#define VAL_MAX         999999999.999999999 
#define VAL_MIN         DBL_MIN
#define OUT_FILE_MAX    30
#define WELCOME_STR     "\nTrace Width Calculator, Made by Yiannis Michael (2024). \n\nPlease 'type twc.exe <Current [A]> <Copper Weight [oz/ft^2]>' to get output results. Use '--help' for explanation of the flags and more advanced usage, for different units, optional inputs, etc.\n\nThis tool should only be used to assist design decisions and not be used to replace professional advice. Developer(s) have no liability whatsoever."
#define FEW_ARGS_STR    "\nAn input of at least Current [A] and Copper Weight [oz/ft^2] is required. Use no arguments to get the welcome message and either '-h' or '--help' to get the list of commands.\n"
#define VERSION_STR     "Trace Width Calculator (TWC)\n Version 1.0.0\n"
#define DISCLAIMER_STR  "\nDesign assistance by the TWC tool is provided with no liability whatsover. For final decisions on electronics designs, please consult an actual qualified person.\n\n"

/* Conversion macros */
#define CONV_MIL2_TO_CM2(x)    ((x) * 0.00254 * 0.00254)
#define CONV_MIL2_TO_MM2(x)    ((x) * 0.0254 * 0.0254)
#define CONV_CM2_TO_INCH2(x)   ((x) * 2.54 * 2.54)

#define CONV_MIL_TO_OZFT2(x)   ((x) / 1.37) // most sources say 1.37, few others say 1.378.
#define CONV_MM_TO_OZFT2(x)    ((x) * 39.37007874 / 1.37)
#define CONV_UM_TO_OZFT2(x)    ((x) * 39.37007874 / 1.37 * 1e-3)
#define CONV_OZFT2_TO_MIL(x)   ((x) * 1.37)
#define CONV_OZFT2_TO_MM(x)    ((x) * 1.37 * 0.0254) 
#define CONV_OZFT2_TO_UM(x)    ((x) * 1.37 * 0.0254 * 1e3) 

#define CONV_MM_TO_MIL(x)      ((x) * 39.37007874)
#define CONV_MIL_TO_MM(x)      ((x) * 0.0254)

#define CONV_FAHR_TO_CELS(x)   (((x) - 32) / 1.8)
#define CONV_CELS_TO_FAHR(x)   (((x) * 1.8) + 32)

#define CONV_WmK_TO_BTUhftF(x) ((x) / 1.73)
#define CONV_BTUhftF_TO_WmK(x) ((x) * 1.73)

/* Check macros */
#define CHECK_RES(x)        ({ if (!(x)) { \
        fprintf(stderr, "Argument entered was wrong...\n"); \
        exit(EXIT_FAILURE); \
        } }) 
#define CHECK_LIMITS(x)     ({ if ((x) > VAL_MAX || (x) < VAL_MIN) { \
        fprintf(stderr, "Detected numbers out of range. Please check inputs and enter numbers between, \n%.15lf and %.15lf", VAL_MIN, VAL_MAX); \
        exit(EXIT_FAILURE); \
        } }) 

typedef struct Dbl {
    double val;
    double outval;
    char* units;
}dbl_t;

typedef struct Std {
    char* str;    // TODO: use the str instead of the num
    unsigned int num;
}std_t;

/* Outputs Structures */
typedef struct Layer{
    dbl_t trace_width;         // [mils]
    dbl_t resistance;          // [Ohms]
    dbl_t voltage_drop;        // [V]
    dbl_t power_loss;          // [W]
    dbl_t trace_temperature;   // [Celsius]
    dbl_t area;                // [mils^2]
    dbl_t corr_area;           // [mils^2]
    dbl_t corr_trace_width;    // [mils]
}layer_t;

typedef layer_t extl_t;
typedef layer_t intl_t;

typedef struct Save{
    bool path_no_name;
    bool no_name_no_path;
    bool path_name;
}save_t;

typedef struct OFile{
    char* fname;
    char* path; 
    char* dest;
    bool oflag;                 // Output file flag
}ofile_t;

typedef struct OP{
    intl_t intl;
    extl_t extl;
    layer_t layer;
    save_t save_flag;
}op_t;

typedef struct CF {
    double copper_weight;
    double pcb_thickness;
    double plane_area;
    double plane_distance;
    double temperature_rise;
    double pcb_thermal_cond;     
}cf_t; /* Correction Factors Struct */

typedef struct IP ip_t;

/* Input Structure */
typedef struct IP{
    /* Mandatory Inputs */
    dbl_t current;             // [A]
    dbl_t copper_weight;       // [oz/ft^2]

    /* Optional Inputs */
    std_t standard;             // IPC standard
    char method;                // Method to use for calculations
    dbl_t temperature_rise;     // [Celsius]
    dbl_t temperature_ambient;  // [Celsius]
    dbl_t trace_length;         // [cm]
    dbl_t resistivity;          // [Ohm*cm]
    dbl_t pcb_thickness;        // [mm]
    dbl_t pcb_thermal_cond;     // [W/mK]
    dbl_t plane_area;           // [in^2] 
    dbl_t plane_distance;       // [mils] 
    dbl_t a;                    // [1/C] :resistivity temperature coefficient
    cf_t cf;                    // Correction Factors   
    ofile_t ofile;              // Output file properties
    char uflag;                 // Units flag
    void (*deft)(ip_t*);        // Set default values 
    void (*proc)(ip_t*, op_t*); // Calculation procedure
    void (*outp)(ip_t*, op_t*, FILE *file); // Output function
    void (*outu)(ip_t*, op_t*); // Set output units 
}ip_t;

enum {
    IPC2152 = 2152,
    IPC2221 = 2221,
}; /* Standards Enumeration */

void get_options(int* argc, char** argv, ip_t* ip);
void calcs_IPC2221(ip_t* ip, op_t* op);
void calcs_IPC2152_A(ip_t* ip, op_t* op);
void calcs_IPC2152_B(ip_t* ip, op_t* op);
void set_deft_IPC2221(ip_t* ip);
void set_deft_IPC2152_A(ip_t* ip);
void set_deft_IPC2152_B(ip_t* ip);
void set_outu_IPC2221(ip_t* ip, op_t* op);
void set_outu_IPC2152(ip_t* ip, op_t* op);
void output_results_IPC2221(ip_t* ip, op_t* op, FILE * file);
void output_results_IPC2152_A(ip_t* ip, op_t* op, FILE * file);
void output_results_IPC2152_B(ip_t* ip, op_t* op, FILE * file);
void get_standard_method(int* argc, char** argv, ip_t* ip);
void check_standard(char* strval, char** standard_arr, unsigned int size, unsigned int* index);
void check_method(char chrval, char* method_arr, unsigned int size);
void set_default_inputs(ip_t* ip);
void sel_functions(ip_t* ip);
void set_output_file(ofile_t* ofile, char* optarg);
void autogen_file_name(char* fname);
char* get_time();
void calc_rvp(ip_t* ip, layer_t* layer);
double calc_width_mils(ip_t* ip, double* area);
double calc_resistance(ip_t* ip, double* area);
double calc_vdrop(ip_t* ip, double* resistance);
double calc_power_loss(ip_t* ip, double* vdrop);
void output_help();
