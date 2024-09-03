#include "twc.h"

int main(int argc, char** argv) {

    /* Inputs */
    ip_t ip;

    /* Outputs */
    op_t op;

    /* Other Vars */
    FILE * file = stdout;

    set_default_inputs(&ip);

    if(argc == 1) {
        /* Output welcome string */
        printf(WELCOME_STR);
        exit(EXIT_SUCCESS);
    }

    if (argc == 2) {
        /* Output help */
        if (!(strcmp("-h", argv[1])) || !(strcmp("--help", argv[1]))) {
            output_help();
            exit(EXIT_SUCCESS);
        } 
        /* Output version */
        if (!(strcmp("--version", argv[1]))) {
            printf(VERSION_STR);
            exit(EXIT_SUCCESS);
        } 

        /* Output too few args error */ 
        fprintf(stderr, "%s\n", TOO_FEW_ARGS_STR);
        exit(EXIT_FAILURE);
    }

    get_inputs(&argc, argv, &ip);

    calc_external_layers(&ip, &op.extl);
    calc_internal_layers(&ip, &op.intl);

    if(ip.oflag) {
        file = fopen(ip.ofile.dest, "w");
        if (!(file)) {
            fprintf(stderr, "File not able to be saved, check input.\n");
            exit(EXIT_FAILURE);
        }
    }
    output_results(&ip, &op, file);

    if (file != stdout) {
        fclose(file);
        printf("\nContents exported to %s\n\n", ip.ofile.dest);
        free(ip.ofile.path);
        free(ip.ofile.fname);
        free(ip.ofile.dest);
    }
    exit(EXIT_SUCCESS);
}


void get_inputs(int* argc, char** argv, ip_t* ip) {
    /* Check if only Current and Thickness were provided */
    if (*argc == 3) {
        ip->res = sscanf(argv[1], "%lf", &ip->val); 
        check_res(&ip->res);
        check_limits(&ip->val);
        ip->current = ip->val;

        ip->res = sscanf(argv[2], "%lf", &ip->val); 
        check_res(&ip->res);
        check_limits(&ip->val);
        ip->trace_thickness = ip->val;
        return;
    }

    for(int i = 1; i < *argc; i++) {
        if (!(strcmp("-o", argv[i]))) {
            ip->oflag = true;
            set_output_file(&ip->ofile.fname, &ip->ofile.path, &ip->ofile.dest, argv[i + 1]);
            i++;
            continue;
        } else {
            ip->res = sscanf(argv[i + 1], "%lf", &ip->val); 
            check_res(&ip->res);
            check_limits(&ip->val);
        }
        if(!(strcmp("-c", argv[i])) || !(strcmp("--current", argv[i]))) {
            ip->current = ip->val;
            i++;
            continue;
        }
        if(!(strcmp("-t", argv[i])) || !(strcmp("--trace-thickness", argv[i]))) {
            ip->trace_thickness = ip->val;
            i++;
            continue;
        }
        if(!(strcmp("--trace-thickness-mil", argv[i]))) {
            ip->trace_thickness = CONV_MIL_OZFT2(ip->val); // convert back to oz/ft^2 for the calculations to work
            i++;
            continue;
        }
        if(!(strcmp("--trace-thickness-mm", argv[i]))) {
            ip->trace_thickness = CONV_MM_OZFT2(ip->val); // convert to mils and then back to oz/ft^2
            i++;
            continue;
        }
        if(!(strcmp("-r", argv[i])) || !(strcmp("--temperature-rise", argv[i]))) {
            sscanf(argv[i + 1], "%lf", &ip->temperature_rise);
            ip->temperature_rise = ip->val;
            i++;
            continue;
        }
        if(!(strcmp("-a", argv[i])) || !(strcmp("--temperature-ambient", argv[i]))) {
            ip->temperature_ambient = ip->val;
            i++;
            continue;
        }
        if(!(strcmp("-l", argv[i])) || !(strcmp("--trace-length", argv[i]))) {
            ip->trace_length = ip->val;
            i++;
            continue;
        }
        if(!(strcmp("--resistivity", argv[i]))) {
            sscanf(argv[i + 1], "%lf", &ip->resistivity);
            ip->resistivity = ip->val;
            i++;
            continue;
        }
        if(!(strcmp("--resistivity-temperature-coefficient", argv[i]))) {
            ip->a = ip->val;
            i++;
            continue;
        }
        fprintf(stderr, "Unknown option '%s', exiting.", argv[i]);
        exit(EXIT_FAILURE);
    }
}

void set_default_inputs(ip_t* ip) {
    ip->temperature_rise = 10; // [Celsius]
    ip->temperature_ambient = 25; // [Celsius]
    ip->trace_length = -1; // [cm]
    ip->resistivity = 1.7 * pow(10, -6); // resistivity of copper
    ip->a = 3.9 * pow(10, -3); // resistivity temperature coefficient for copper
    ip->val = 0;
    ip->res = 0;
    ip->oflag = false;
    ip->ofile.path = "\0";
}

void check_res(int* res) {
    if (*res != 1) {
        fprintf(stderr, "Argument entered was NaN...\n");
        exit(EXIT_FAILURE);
    }
    return;
}

void check_limits(double* val) {
    if (*val > VAL_MAX || *val < VAL_MIN) {
        fprintf(stderr, "Detected numbers out of range. Please check inputs and enter numbers between, \n%.15lf and %.15lf", VAL_MIN, VAL_MAX);
        exit(EXIT_FAILURE);
    }
    return;
}

void autogen_file_name(char** fname) {
    strcpy(*fname, "twc-output-"); 
    strcat(*fname, get_time()); 
    strcat(*fname, ".txt");
}

void set_output_file(char** fname, char** path, char** dest, char* optarg) {
    *fname = malloc(sizeof(char) * OUT_FILE_MAX);
    if (*optarg == '.') {
        autogen_file_name(fname);
    } else if (optarg[strlen(optarg) - 1] == '/') {
        *path = malloc(sizeof(char) * PATH_MAX); // PATH_MAX = 260
        sscanf(optarg, "%s", *path);
        autogen_file_name(fname);
    } else {
        sscanf(optarg, "%s", *fname);
    }
    *dest = malloc(sizeof(char) * (strlen(*path) + strlen(*fname)));
    sprintf(*dest, "%s%s", *path, *fname);
}

double calc_area_mils2(ip_t* ip, float k) {
    return ip->current/(pow(k * pow(ip->temperature_rise,b),1/c)); 
}

double calc_width_mils(ip_t* ip, double* area) {
    return *area/(ip->trace_thickness * 1.378);
}

double calc_resistance(ip_t* ip, double* area) {
    return ((ip->trace_length * ip->resistivity)/(CONV_MIL2_CM2(*area))) * (1 + (ip->a * ((ip->temperature_rise + ip->temperature_ambient) - ip->temperature_ambient)));
}

double calc_vdrop(ip_t* ip, double* resistance) {
    return ip->current * (*resistance); 
}

double calc_power_loss(ip_t* ip, double* vdrop) {
    return ip->current * (*vdrop);
}

void calc_external_layers(ip_t* ip, extl_t* extl) {
    extl->area = calc_area_mils2(ip, k_EXT);
    extl->trace_width = calc_width_mils(ip, &extl->area);
    if (ip->trace_length > 0) {
        extl->resistance  = calc_resistance(ip, &extl->area); 
    }
    extl->voltage_drop = calc_vdrop(ip, &extl->resistance); 
    extl->power_loss = calc_power_loss(ip, &extl->voltage_drop);
}

void calc_internal_layers(ip_t* ip, intl_t* intl) {
    intl->area = calc_area_mils2(ip, k_INT);
    intl->trace_width = calc_width_mils(ip, &intl->area);
    if (ip->trace_length > 0) {
        intl->resistance  = calc_resistance(ip, &intl->area); 
    }
    intl->voltage_drop = calc_vdrop(ip, &intl->resistance); 
    intl->power_loss = calc_power_loss(ip, &intl->voltage_drop);
}

char* get_time() {
    time_t time_since_epoch = time(NULL);
    struct tm *tm = localtime(&time_since_epoch);
    static char s[13];
    strftime(s, sizeof(s), "%d%m%y%H%M%S", tm);
    return s;
}

void output_help() {
    printf("\nHelp for the Trace Width Calculator (TWC).t-c <Current [A]>\t\t= Input the trace current in Amps.\n"
            "\n\t-t <Thickness [oz/ft^2]>\t= Input the thickness in oz per ft^2.\n"
            "\n\t-r <Temperature Rise [C]>\t= Input the maximum allowed temperature rise in C.\n"
            "\n\t-a <Ambient Temperature [C]>\t= Input the ambient temperature of the trace in C.\n"
            "\n\t-l <Trace Length [cm]>\t\t= Input the trace length in centimeters.\n"
            "\n\t-o <File Name>\t\t\t= Write the name of the outputted file. Use '.txt' to create a text file. Use a single '.' to auto-generate the name based on date/time. Can also write the full path to the file, e.g. 'C:/Users/user/output.txt' or stop at 'C:/Users/user/' to use the auto-generated file name.\n");
}

void output_results(ip_t* ip, op_t* op, FILE * file) {

    fprintf(file,   "###Inputs###\n"
                    "Current:\t\t%.15lf\t[A]\n"
                    "Trace Thickness:\t%.15lf\t[oz/ft^2]\n"
                    "Temperature Rise:\t%.15lf\t[C]\n"
                    "Temperature (Ambient):\t%.15lf\t[C]\n"
                    "Trace Length:\t\t%.15lf\t[cm]\n"
                    "###End of Inputs###\n\n", 
                    ip->current, ip->trace_thickness, ip->temperature_rise, ip->temperature_ambient, ip->trace_length);

    fprintf(file,   "###Outputs###\n\n"
                    "//External Layers\\\\\n"
                    "Width:\t\t\t%.15lf\t[mil]\n"
                    "Resistance:\t\t%.15lf\t[Ohm]\n"
                    "Voltage Drop:\t\t%.15lf\t[V]\n"
                    "Power Loss:\t\t%.15lf\t[W]\n"
                    "\\\\\\\\\\\\\\\\\\/////////\n\n",
                    op->extl.trace_width, op->extl.resistance, op->extl.voltage_drop, op->extl.power_loss);

    fprintf(file,   "//Internal Layers\\\\\n"
                    "Width:\t\t\t%.15lf\t[mil]\n"
                    "Resistance:\t\t%.15lf\t[Ohm]\n"
                    "Voltage Drop:\t\t%.15lf\t[V]\n"
                    "Power Loss:\t\t%.15lf\t[W]\n"
                    "\\\\\\\\\\\\\\\\\\/////////\n\n"
                    "###End of Outputs###\n\n",
                    op->intl.trace_width, op->intl.resistance, op->intl.voltage_drop, op->intl.power_loss);

    fprintf(file,   "Notes:\n"
                    "- Constants used for these calculations were,\n\n"
                    "\tb = %.7lf\n"  
                    "\tc = %.7lf\n"
                    "\tk = %.7lf (Internal layers)\n"
                    "\tk = %.7lf (External layers)\n"
                    "\ta = %.7lf (Resistivity Temperature Coefficient)\n"
                    "%6srho = %.7lf (Resistivity)\n\n"
                    "Values for b, c, and k were based on curve fitting to IPC-2221 curves (Revision A, pg. 38)\n"
                    "\n- Maximum trace temperature considered is %.7lf C\n\n",
                    b, c, k_INT, k_EXT, ip->a, " " ,ip->resistivity, ip->temperature_rise + ip->temperature_ambient);
}
