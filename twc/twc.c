#include "twc.h"
#include <stdio.h>

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
        fprintf(stderr, "%s\n", FEW_ARGS_STR);
        exit(EXIT_FAILURE);
    }

    /* Get the inputs and options */
    get_inputs(&argc, argv, &ip);

    switch (ip.standard) {
        case IPC2221:
            ipc2221_calcs(&ip, &op);
            break;
        case IPC2152:
            ipc2152_calcs(&ip, &op);
            break;
        default:
            fprintf(stderr, "Unknown standard inputted. Only acceptable values are '2221' and '2152'.`");
            exit(EXIT_FAILURE);
    }

    /* Open file to save outputs */
    if(ip.ofile.oflag) {
        file = fopen(ip.ofile.dest, "w");
        if (!(file)) {
            fprintf(stderr, "File not able to be saved, check input.\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Output the results to the buffer */
    output_results(&ip, &op, file);

    /* Close file and free memory */
    if (file != stdout) {
        fclose(file);
        printf("\nContents exported to %s\n\n", ip.ofile.dest);
        free(ip.ofile.path);
        free(ip.ofile.fname);
        free(ip.ofile.dest);
    }

    /* Program done. exit succesfully */
    exit(EXIT_SUCCESS);
}

void get_inputs(int* argc, char** argv, ip_t* ip) {
    unsigned char num_rec = 0; /* Used to record the two allowed numerical options */
    bool skip_check = false; /* To skip the the type and limit check, only used by numerical options or string arguments */

    for(int i = 1; i < *argc; i++) {
        if(!(skip_check)){
            CHECK_RES(sscanf(argv[i + 1], "%lf", &ip->val));
            CHECK_LIMITS(ip->val);
        } else {
            skip_check = false; /* Always make sure it's false for the next iteration */
        }

        if (!(strcmp("-o", argv[i]))) {
            ip->ofile.oflag = true;
            set_output_file(&ip->ofile, argv[i + 1]);
            i++;
            skip_check = true;
            continue;
        }
        if(!(strcmp("--standard", argv[i]))) {
            ip->standard = ip->val;
            i++;
            continue;
        }
        if(!(strcmp("-c", argv[i])) || !(strcmp("--current", argv[i]))) {
            ip->current = ip->val;
            i++;
            continue;
        }
        if(!(strcmp("-w", argv[i])) || !(strcmp("--copper-weight", argv[i]))) {
            ip->copper_weight = ip->val;
            i++;
            continue;
        }
        if(!(strcmp("--copper-weight-mil", argv[i]))) {
            ip->copper_weight = CONV_MIL_TO_OZFT2(ip->val); // convert back to oz/ft^2 for the calculations to work
            i++;
            continue;
        }
        if(!(strcmp("--copper-weight-mm", argv[i]))) {
            ip->copper_weight = CONV_MM_TO_OZFT2(ip->val); // convert to mils and then back to oz/ft^2
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
            ip->resistivity = ip->val;
            i++;
            continue;
        }
        if(!(strcmp("--resistivity-temperature-coefficient", argv[i]))) {
            ip->a = ip->val;
            i++;
            continue;
        }

        /* Checks for the numerical options */
        if(sscanf(argv[i], "%lf", &ip->val) && (num_rec == 0)) {
            num_rec++;
            skip_check = true;
            ip->current = ip->val;
            continue;
        }
        if((sscanf(argv[i], "%lf", &ip->val)) && (num_rec == 1)) {
            num_rec++;
            skip_check = true;
            ip->copper_weight = ip->val;
            continue;
        }

        /* Error checking */
        fprintf(stderr, "Unknown option '%s', exiting.", argv[i]);
        exit(EXIT_FAILURE);
    }
}

void set_default_inputs(ip_t* ip) {
    /* Set input value defaults */
    ip->standard = IPC2152;
    ip->temperature_rise = 10; // [Celsius]
    ip->temperature_ambient = 25; // [Celsius]
    ip->trace_length = 0; // [cm]
    ip->resistivity = 1.7e-6; // resistivity of copper
    ip->a = 3.9e-3; // resistivity temperature coefficient for copper
    ip->val = 0;
    ip->res = 0;
    ip->ofile.oflag = false;
    ip->ofile.path = "\0";
    ip->pcb_thickness = 0;
    ip->plane_area = 0;
    ip->plane_distance = 0;

    /* Set modifier defaults */
    ip->cf.copper_weight = 1;
    ip->cf.temperature_rise = 1;
    ip->cf.plane_area = 1;
    ip->cf.pcb_thickness = 1;
    ip->cf.plane_distance = 1;
}

void check_res(int res) {
    if (res != 1) {
        fprintf(stderr, "Argument entered was NaN...\n");
        exit(EXIT_FAILURE);
    }
    return;
}

void autogen_file_name(char* fname) {
    strcpy(fname, "twc-output-"); 
    strcat(fname, get_time()); 
    strcat(fname, ".txt");
}

void set_output_file(ofile_t* ofile, char* optarg) {
    ofile->fname = malloc(sizeof(char) * OUT_FILE_MAX);
    if (*optarg == '.') {
        autogen_file_name(ofile->fname);
    } else if (optarg[strlen(optarg) - 1] == '/') {
        ofile->path = malloc(sizeof(char) * PATH_MAX); // PATH_MAX = 260
        sscanf(optarg, "%s", ofile->path);
        autogen_file_name(ofile->fname);
    } else {
        sscanf(optarg, "%s", ofile->fname);
    }
    ofile->dest = malloc(sizeof(char) * (strlen(ofile->path) + strlen(ofile->fname)));
    sprintf(ofile->dest, "%s%s", ofile->path, ofile->fname);
}

void ipc2221_calcs(ip_t* ip, op_t* op) {
    calc_external_layers(ip, &op->extl);
    calc_internal_layers(ip, &op->intl);
}

void calc_external_layers(ip_t* ip, extl_t* extl) {
    extl->area = calc_2221_area_mils2(ip, k_EXT);
    calc_common(ip, extl);
}

void calc_internal_layers(ip_t* ip, intl_t* intl) {
    intl->area = calc_2221_area_mils2(ip, k_INT);
    calc_common(ip, intl);
}

double calc_2152_area_mils2(ip_t* ip) {
    /* Different one on the website, and different one in the code */
    /* return (117.555 * pow(ip->temperature_rise, -0.913) + 1.15) * pow(ip->current, 0.84 * pow(ip->temperature_rise, -0.018) + 1.159);  */
    return (110.515 * pow(ip->temperature_rise, -0.871) + 0.803) * pow(ip->current, 0.868 * pow(ip->temperature_rise, -0.102) + 1.129);    
}

double calc_2152_corr_area_mils2(ip_t* ip) {
    /* Different one on the website, and different one in the code */
    /* return (117.555 * pow(ip->cf.temperature_rise, -0.913) + 1.15) * pow(ip->current, 0.84 * pow(ip->temperature_rise, -0.018) + 1.159);  */
    return (110.515 * pow(ip->cf.temperature_rise, -0.871) + 0.803) * pow(ip->current, 0.868 * pow(ip->cf.temperature_rise, -0.102) + 1.129);    
}

void ipc2152_calcs(ip_t* ip, op_t* op) {
    op->layer.area = calc_2152_area_mils2(ip);

    if (ip->copper_weight == 2) {
        ip->cf.copper_weight = -0.0185 * log(ip->current) + 0.9861;
    } else if (ip->copper_weight == 3) {
        ip->cf.copper_weight = 1;
    } else {
        ip->cf.copper_weight = -0.0318 * log(ip->current) + 0.9128; 
    }

    if (ip->pcb_thickness != 0) {
        ip->cf.pcb_thickness = 25.959 * pow(ip->pcb_thickness,-0.7666);
    }

    if (ip->plane_area >= 40) {
        ip->cf.plane_area = 0.89;
    } else if (ip->plane_area >= 20) {
        ip->cf.plane_area = 0.94;
    } else {
        ip->cf.plane_area = 1;
    } 

    if (ip->plane_distance > 125) {
        ip->cf.plane_distance = 1;
    } else if (ip->cf.plane_distance != 1){
        ip->cf.plane_distance = 0.0031 * ip->plane_distance + 0.4054;
    }

    ip->cf.temperature_rise = ip->temperature_rise / (ip->cf.copper_weight * ip->cf.pcb_thickness * ip->cf.plane_distance * ip->cf.plane_area);

    /* Calculate the corrected area and trace width  */
    op->layer.corr_area = calc_2152_corr_area_mils2(ip);
    op->layer.corr_trace_width = calc_width_mils(ip, &op->layer.corr_area);

    calc_common(ip, &op->layer);
}

double calc_2221_area_mils2(ip_t* ip, float k) {
    return pow(ip->current/k * pow(ip->temperature_rise, 0.44), 1/0.725); 
}

void calc_common(ip_t* ip, layer_t* layer) {
    layer->trace_width = calc_width_mils(ip, &layer->area);
    if (ip->trace_length > 0) {
        layer->resistance  = calc_resistance(ip, &layer->area); 
    }
    layer->voltage_drop = calc_vdrop(ip, &layer->resistance); 
    layer->power_loss = calc_power_loss(ip, &layer->voltage_drop);
}

double calc_width_mils(ip_t* ip, double* area) {
    return *area/(ip->copper_weight * 1.378);
}

double calc_resistance(ip_t* ip, double* area) {
    return ((ip->trace_length * ip->resistivity)/(CONV_MIL2_TO_CM2(*area))) * (1 + (ip->a * ((ip->temperature_rise + ip->temperature_ambient) - ip->temperature_ambient)));
}

double calc_vdrop(ip_t* ip, double* resistance) {
    return ip->current * (*resistance); 
}

double calc_power_loss(ip_t* ip, double* vdrop) {
    return ip->current * (*vdrop);
}

char* get_time() {
    time_t time_since_epoch = time(NULL);
    struct tm *tm = localtime(&time_since_epoch);
    static char s[13];
    strftime(s, sizeof(s), "%d%m%y%H%M%S", tm);
    return s;
}

void output_help() {
    printf("\nHelp for the Trace Width Calculator (TWC)."
            "\n\t-c <Current [A]>\t\t= Input the trace current in Amps.\n"
            "\n\t-w <Copper Weight [oz/ft^2]>\t= Input the copper weight in oz per ft^2.\n"
            "\n\t-r <Temperature Rise [C]>\t= Input the maximum allowed temperature rise in C.\n"
            "\n\t-a <Ambient Temperature [C]>\t= Input the ambient temperature of the trace in C.\n"
            "\n\t-l <Trace Length [cm]>\t\t= Input the trace length in centimeters.\n"
            "\n\t-o <File Name>\t\t\t= Write the name of the outputted file. Use '.txt' to create a text file. Use a single '.' to auto-generate the name based on date/time. Can also write the full path to the file, e.g. 'C:/Users/user/output.txt' or stop at 'C:/Users/user/' to use the auto-generated file name.\n");
}

void output_results(ip_t* ip, op_t* op, FILE * file) {

    switch (ip->standard) {
        case IPC2221: 
            fprintf(file,   
                    "\n"
                    "### Inputs ###\n"
                    "Current:\t\t%.15lf\t[A]\n"
                    "Copper Weight:\t\t%.15lf\t[oz/ft^2]\n"
                    "Temperature Rise:\t%.15lf\t[C]\n"
                    "Temperature (Ambient):\t%.15lf\t[C]\n"
                    "Trace Length:\t\t%.15lf\t[cm]\n"
                    "### End of Inputs ###\n\n", 
                    ip->current, ip->copper_weight, ip->temperature_rise, ip->temperature_ambient, ip->trace_length);

            fprintf(file,   
                    "### Outputs ###"
                    "\n\n"
                    "//External Layers\\\\\n"
                    "Area: \t\t\t%.15lf\t[mil^2]\n"
                    "Width:\t\t\t%.15lf\t[mil]\n"
                    "Resistance:\t\t%.15lf\t[Ohm]\n"
                    "Voltage Drop:\t\t%.15lf\t[V]\n"
                    "Power Loss:\t\t%.15lf\t[W]\n"
                    "\\\\\\\\\\\\\\\\\\/////////\n\n",
                    op->extl.area, op->extl.trace_width, op->extl.resistance, op->extl.voltage_drop, op->extl.power_loss);

            fprintf(file,   
                    "//Internal Layers\\\\"
                    "\n"
                    "Area: \t\t\t%.15lf\t[mil^2]\n"
                    "Width:\t\t\t%.15lf\t[mil]\n"
                    "Resistance:\t\t%.15lf\t[Ohm]\n"
                    "Voltage Drop:\t\t%.15lf\t[V]\n"
                    "Power Loss:\t\t%.15lf\t[W]\n"
                    "\\\\\\\\\\\\\\\\\\/////////\n\n"
                    "### End of Outputs ###\n\n",
                    op->intl.area, op->intl.trace_width, op->intl.resistance, op->intl.voltage_drop, op->intl.power_loss);

            fprintf(file,   
                    "Notes:\n"
                    "- Constants used for these calculations were,\n\n"
                    "\tk = %.7lf (Internal layers)\n"
                    "\tk = %.7lf (External layers)\n"
                    "\ta = %.7lf (Resistivity Temperature Coefficient)\n"
                    "%6srho = %.7lf (Resistivity)\n\n"
                    "Values for b, c, and k were based on curve fitting to IPC-2221 curves (Revision A, pg. 38)\n"
                    "\n- Maximum trace temperature considered is %.7lf C\n\n",
                    k_INT, k_EXT, ip->a, " " ,ip->resistivity, ip->temperature_rise + ip->temperature_ambient);
            break;
        case IPC2152:

            fprintf(file,
                    "\n"
                    "### Inputs ###"
                    "\n"
                    "Current:\t\t%.15lf\t[A]\n"
                    "Copper Weight:\t\t%.15lf\t[oz/ft^2]\n"
                    "Temperature Rise:\t%.15lf\t[C]\n"
                    "Temperature (Ambient):\t%.15lf\t[C]\n"
                    "Trace Length:\t\t%.15lf\t[cm]\n"
                    "### End of Inputs ###\n\n", 
                    ip->current, ip->copper_weight, ip->temperature_rise, ip->temperature_ambient, ip->trace_length);

            fprintf(file,
                    "### Outputs ###"
                    "\n"
                    "Area: \t\t\t%.15lf\t[mil^2]\n"
                    "Corrected Area: \t%.15lf\t[mil^2]\n"
                    "Width:\t\t\t%.15lf\t[mil]\n"
                    "Corrected Width: \t%.15lf\t[mil]\n"
                    "Resistance:\t\t%.15lf\t[Ohm]\n"
                    "Voltage Drop:\t\t%.15lf\t[V]\n"
                    "Power Loss:\t\t%.15lf\t[W]\n"
                    "### End of Outputs ###\n\n",
                    op->layer.area, op->layer.corr_area, op->layer.trace_width, op->layer.corr_trace_width, op->layer.resistance, op->layer.voltage_drop, op->layer.power_loss);

            fprintf(file, 
                    "### Correction Factors ###"
                    "\n"
                    "Copper Weight CF:\t%.15lf\t[units]\n"
                    "PCB Thickness CF:\t%.15lf\t[units]\n"
                    "Plane Area CF:\t\t%.15lf\t[units]\n"
                    "Plane Distance CF:\t%.15lf\t[units]\n"
                    "Temperature Rise CF:\t%.15lf\t[units]\n"
                    "### End of Correction Factors ###\n\n",
                    ip->cf.copper_weight, ip->cf.pcb_thickness, ip->cf.plane_area, ip->cf.plane_distance,ip->cf.temperature_rise);

            fprintf(file,
                    "Notes:\n"
                    "- Constants used for these calculations were,\n\n"
                    "\ta = %.7lf (Resistivity Temperature Coefficient)\n"
                    "%6srho = %.7lf (Resistivity)\n\n"
                    "write source here"
                    "\n- Maximum trace temperature considered is %.7lf C\n\n",
                    ip->a, " " ,ip->resistivity, ip->temperature_rise + ip->temperature_ambient);
            break;
            fprintf(file, 
                    "Design assistance by the TWC tool is provided with no liability whatsover. For final decisions on electronics designs, please consult your boss");
    }
}
