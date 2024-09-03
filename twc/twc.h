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
#define k_INT   0.024
#define k_EXT   0.048
#define b       0.44
#define c       0.725
#define VAL_MAX 999999999.999999999 
#define VAL_MIN DBL_MIN
#define OUT_FILE_MAX 30
#define WELCOME_STR "\nTrace Width Calculator, Made by Yiannis Michael (2024). \n\nPlease 'type twc.exe <Current [A]> <Thickness [oz/ft^2]>' to get output results. Use '--help' for explanation of the flags and more advanced usage, for different units, optional inputs, etc.\n"
#define TOO_FEW_ARGS_STR "\nAn input of at least Current [A] and Thickness [oz/ft^2] is required. Use no arguments to get the welcome message and either '-h' or '--help' to get the list of commands.\n"
#define VERSION_STR "Trace Width Calculator (TWC)\n Version 1.0.0\n"

#define CONV_MIL2_CM2(x) ((x) * 0.00254 * 0.00254)
#define CONV_MIL_OZFT2(x) ((x) / 1.378)
#define CONV_MM_MIL(x) ((x) * 39.37007874)
#define CONV_MM_OZFT2(x) (CONV_MIL_OZFT2(CONV_MM_MIL(x)))

/* Outputs Structures */
typedef struct {
    double trace_width;         // [mils]
    double resistance;          // [Ohms]
    double voltage_drop;        // [V]
    double power_loss;          // [W]
    double trace_temperature;   // [Celsius]
    double area;                // [mils^2]
}intl_t;

typedef intl_t extl_t;

typedef struct {
    bool path_no_name;
    bool no_name_no_path;
    bool path_name;
}save_t;

typedef struct {
    char* fname;
    char* path; 
    char* dest;
}ofile_t;

typedef struct {
    intl_t intl;
    extl_t extl;
    save_t save_flag;
}op_t;

/* Input Structure */
typedef struct {
    /* Mandatory Inputs */
    double current;             // [A]
    double trace_thickness;     // [oz/ft^2]

    /* Optional Inputs */
    double temperature_rise;    // [Celsius]
    double temperature_ambient; // [Celsius]
    double trace_length;        // [cm]
    double resistivity;         // [Ohm*cm]
    double a;                   // [1/C] :resistivity temperature coefficient
    double val;                 // Input value 
    int res;                    // Result
    bool oflag;                 // Output file flag
    ofile_t ofile;              // Output file properties
}ip_t;

void set_default_inputs(ip_t* ip);
void check_res(int* res);
void check_limits(double* val);
void set_output_file(char** fname, char** path, char** dest, char* opt);
void autogen_file_name(char** fname);
char* get_time();
double calc_area_mils2(ip_t* ip, float k);
double calc_width_mils(ip_t* ip, double* area);
double calc_resistance(ip_t* ip, double* area);
double calc_vdrop(ip_t* ip, double* resistance);
double calc_power_loss(ip_t* ip, double* vdrop);
void calc_external_layers(ip_t* ip, extl_t* extl);
void calc_internal_layers(ip_t* ip, intl_t* intl);
void get_inputs(int* argc, char** argv, ip_t* ip);
void output_results(ip_t* ip, op_t* op, FILE * file);
void output_help();
