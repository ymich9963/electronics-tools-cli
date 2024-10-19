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
#define WELCOME_STR     "\nTrace Width Calculator, Made by Yiannis Michael (2024). \n\nPlease 'type twc.exe <Current [A]> <Copper Weight [oz/ft^2]>' to get output results. Use '--help' for explanation of the flags and more advanced usage, for different units, optional inputs, etc.\n\nThis tool should only be used to assist design decisions and not be used to replace professional advice. Developer(s) have no liability whatsoever.\n"
#define FEW_ARGS_STR    "\nAn input of at least Current [A] and Copper Weight [oz/ft^2] is required. Use no arguments to get the welcome message and either '-h' or '--help' to get the list of commands.\n"
#define VERSION_STR     "\nTrace Width Calculator (TWC)\n Version 1.0.0\n"
#define DISCLAIMER_STR  "\nDesign assistance by the TWC tool is provided with no liability whatsover. For final decisions on electronics designs, please consult an actual qualified person.\n"

/* Conversion macros */
#define CONV_MIL2_TO_CM2(x)    ((x) * 0.00254 * 0.00254)
#define CONV_MIL2_TO_MM2(x)    ((x) * 0.0254 * 0.0254)
#define CONV_MM2_TO_MIL2(x)    ((x) / 0.0254 / 0.0254)
#define CONV_CM2_TO_INCH2(x)   ((x) / (2.54 * 2.54))
#define CONV_MIL_TO_OZFT2(x)   ((x) / 1.378) // most sources say 1.37, few others say 1.378.
#define CONV_MM_TO_OZFT2(x)    ((x) * 39.37007874 / 1.378)
#define CONV_UM_TO_OZFT2(x)    ((x) * 39.37007874 / 1.378 * 1e-3)
#define CONV_OZFT2_TO_MIL(x)   ((x) * 1.378)
#define CONV_OZFT2_TO_MM(x)    ((x) * 1.378 * 0.0254) 
#define CONV_OZFT2_TO_UM(x)    ((x) * 1.378 * 0.0254 * 1e3) 
#define CONV_MM_TO_MIL(x)      ((x) * 39.37007874)
#define CONV_MIL_TO_MM(x)      ((x) * 0.0254)
#define CONV_FAHR_TO_CELS(x)   (((x) - 32) / 1.8)
#define CONV_CELS_TO_FAHR(x)   (((x) * 1.8) + 32)
#define CONV_WmK_TO_BTUhftF(x) ((x) / 1.730735)
#define CONV_BTUhftF_TO_WmK(x) ((x) * 1.73735)

/* Check macros */

/* Check response from sscanf */
#define CHECK_RES(x)        ({ if (!(x)) { \
        fprintf(stderr, "Argument entered was wrong...\n"); \
        return 1; \
        } }) 

/* Check if value is between numerical limits */
#define CHECK_LIMITS(x)     ({ if ((x) > VAL_MAX || (x) < VAL_MIN) { \
        fprintf(stderr, "Detected numbers out of range. Please check inputs and enter numbers between, \n%.15lf and %.15lf", VAL_MIN, VAL_MAX); \
        return 1; \
        } }) 

/* Check if an error occured to exit program */
#define CHECK_ERR(x)        ({ if ((x)) { \
        exit(EXIT_FAILURE); \
        } }) 

/* Check if a function returns failure */
#define CHECK_RET(x)        ({ if ((x)) { \
        return 1; \
        } }) 

typedef struct Dbl {
    double val;
    double outval;
    char* units;
}dbl_t; /* Struct for inputs of type double */

typedef struct Std {
    char* str;
    unsigned int num;
}std_t; /* Struct for the standards */

typedef struct Layer{
    dbl_t trace_width;         // [mils]
    dbl_t resistance;          // [Ohms]
    dbl_t voltage_drop;        // [V]
    dbl_t power_loss;          // [W]
    dbl_t trace_temperature;   // [Celsius]
    dbl_t cs_area;                // [mils^2]
    dbl_t corr_cs_area;           // [mils^2]
    dbl_t corr_trace_width;    // [mils]
}layer_t; /* Outputs Structures */

typedef layer_t extl_t;
typedef layer_t intl_t;

typedef struct OFile{
    char* fname;
    char* path; 
    char* dest;
    bool oflag;                // Output file flag
}ofile_t; /* Output file strcture */

typedef struct OP{
    intl_t intl;
    extl_t extl;
    layer_t layer;
}op_t; /* Output layer struct */

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
    void (*defv)(ip_t*);        // Set default values 
    void (*proc)(ip_t*, op_t*); // Calculation procedure
    int (*outp)(ip_t*, op_t*, FILE *file); // Output function
    int (*outu)(ip_t*, op_t*);  // Set output units 
}ip_t;

enum {
    IPC2152 = 2152,
    IPC2221 = 2221,
}; /* Standards Enumeration */

/**
 * @brief Get the options and arguments provided. 
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @param ip Input struct to store the inputs.
 *
 * @return Success or failure. 
 */
int get_options(int* argc, char** argv, ip_t* ip);

/**
 * @brief Calculate using the IPC2221 standard, sourced from http://circuitcalculator.com/wordpress/2006/03/12/pcb-via-calculator/.
 *
 * @param ip Input struct to store the inputs.
 * @param op Output struct to store the outputs.
 */
void calcs_IPC2221(ip_t* ip, op_t* op);

/**
 * @brief Calculate using the IPC2152 standard, Method A which is sourced from https://www.smps.us/pcb-calculator.html.
 *
 * @param ip Input struct to store the inputs.
 * @param op Output struct to store the outputs.
 */
void calcs_IPC2152_A(ip_t* ip, op_t* op);

/**
 * @brief Calculate using the IPC2152 standard, Method B which is sourced from https://ninjacalc.mbedded.ninja/calculators/electronics/pcb-design/track-current-ipc2152.
 *
 * @param ip Input struct to store the inputs.
 * @param op Output struct to store the outputs.
 */
void calcs_IPC2152_B(ip_t* ip, op_t* op);

/**
 * @brief Set default values needed for the IPC2221 calculations.
 *
 * @param ip Input struct to store the inputs.
 */
void set_defv_IPC2221(ip_t* ip);


/**
 * @brief Set default values needed for the IPC2152, Method A calculations.
 *
 * @param ip Input struct to store the inputs.
 */
void set_defv_IPC2152_A(ip_t* ip);

/**
 * @brief Set default values needed for the IPC2152, Method B calculations.
 *
 * @param ip Input struct to store the inputs.
 */
void set_defv_IPC2152_B(ip_t* ip);

/**
 * @brief Set the units for the IPC2221 outputs.
 *
 * @param ip Input struct to store the inputs.
 * @param op Output struct to store the outputs.
 *
 * @return Success or failure. 
 */
int set_outu_IPC2221(ip_t* ip, op_t* op);

/**
 * @brief Set the units for the IPC2152 outputs.
 *
 * @param ip Input struct to store the inputs.
 * @param op Output struct to store the outputs.
 *
 * @return Success or failure. 
 */
int set_outu_IPC2152(ip_t* ip, op_t* op);

/**
 * @brief Get the chosen standard and method from the input options. Called before get_options() so that the correct default variables are set.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @param ip Input struct to store the inputs.
 *
 * @return Success or failure. 
 */
int get_standard_method(int* argc, char** argv, ip_t* ip);

/**
 * @brief Used to check the inputted standard option string, with the array containing the names of the standards. Index is used to set the standard numerical representation.
 *
 * @param strval String value to compare to the array containing the standard names.
 * @param standard_arr Array containing the standard names.
 * @param size Size of the array.
 * @param index Index when the string value matches an entry in the array.
 *
 * @return Success or failure. 
 */
int check_standard(char* strval, char** standard_arr, unsigned int size, unsigned char* index);

/**
 * @brief Used to check the inputted method agains an array containing the existing method codes/characters. No need to use the index since it's just characters.
 *
 * @param chrval Inputted character argumen.
 * @param method_arr Array containing the method characters.
 * @param size Size of the array.
 *
 * @return Success or failure. 
 */
int check_method(char chrval, char* method_arr, unsigned int size);

/**
 * @brief Set the default, process, output unit, and output print set functions based on the standard and the method chosen.
 *
 * @param ip Input struct to check the method and the standard.
 *
 * @return Success or failure. 
 */
int sel_functions(ip_t* ip);

/**
 * @brief Set the path and file for the outputs to be saved in.
 *
 * @param ofile Output file struct containing everything necessary to save the file.
 * @param optarg Input used by the option.
 */
void set_output_file(ofile_t* ofile, char* optarg);

/**
 * @brief Auto-generate the file name using the current date and time. Concatinate the strings together to form the full name.
 *
 * @param fname File name to use in the concatination.
 */
void autogen_file_name(char* fname);

/**
 * @brief Get the date in time as a string.
 *
 * @return String of the date and time joined together.
 */
char* get_time();

/**
 * @brief Calculate Resistance, Voltage drop, and Power loss.
 *
 * @param ip Input struct to get the inputs used in the calculation.
 * @param layer Output layer struct to store the results.
 */
void calc_w_r_vd_pl(ip_t* ip, layer_t* layer);

/**
 * @brief Used to calculate the width of the trace.
 *
 * @param ip Input struct to get the required values.
 * @param cs_area Cross sectional area of the trace.
 * @return Trace width in type double.
 */
double calc_trace_width_mils(ip_t* ip, double* cs_area);

/**
 * @brief Calculate the resistance of the trace. Resistivity gets converted to Ohm cm.
 *
 * @param ip Input struct to get the required values.
 * @param cs_area Cross sectional area of the trace.
 * @return Resistance in type double.
 */
double calc_resistance(ip_t* ip, double* cs_area);

/**
 * @brief Calculate the Voltage drop.
 *
 * @param ip Input struct to get the values.
 * @param resistance Resistance in Ohms.
 * @return Voltage drop in type double.
 */
double calc_vdrop(ip_t* ip, double* resistance);

/**
 * @brief Calculate the power loss.
 *
 * @param ip Input struct to get the values.
 * @param vdrop Voltage drop in V.
 * @return Power loss in type double.
 */
double calc_power_loss(ip_t* ip, double* vdrop);

/**
 * @brief Outputted results when using the IPC2221.
 *
 * @param ip Input struct to store the inputs.
 * @param op Output struct to store the outputs.
 * @param file Buffer to output the strings.
 */
int output_results_IPC2221(ip_t* ip, op_t* op, FILE * file);

/**
 * @brief Outputted results when using the IPC2512, Method A.
 *
 * @param ip Input struct to store the inputs.
 * @param op Output struct to store the outputs.
 * @param file Buffer to output the strings.
 */
int output_results_IPC2152_A(ip_t* ip, op_t* op, FILE * file);

/**
 * @brief Outputted results when using the IPC2512, Method B.
 *
 * @param ip Input struct to store the inputs.
 * @param op Output struct to store the outputs.
 * @param file Buffer to output the strings.
 */
int output_results_IPC2152_B(ip_t* ip, op_t* op, FILE * file);

/**
 * @brief Output the help string to the terminal.
 */
int output_help();
