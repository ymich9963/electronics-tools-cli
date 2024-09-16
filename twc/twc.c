#include "twc.h"

int main(int argc, char** argv) {

    /* Inputs */
    ip_t ip;

    /* Outputs */
    op_t op;

    /* Output stream */
    FILE * file = stdout;

    /* Set default values to avoid any memory issues */
    set_default_inputs(&ip);

    /* Get the inputs and options */
    get_options(&argc, argv, &ip);

    /* Have a function that sets all the functions to be used as struct members. Definitely one function for calculations and one for output. Have a variable for method too?  */

    /* Continue based on the standard */
    switch (ip.standard) {
        case IPC2221:
            ipc2221_calcs(&ip, &op);
            break;
        case IPC2152:
            /* Have an enum to choose between A or B, a function pointer array will point to the correct funcitons to use. Also have an enum to relate the function names to the index */
            /* ipc2152_calcs(&ip, &op); */
            /* ipc2152_calcsA(&ip, &op); */
            ipc2152_calcsB(&ip, &op);
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

void get_options(int* argc, char** argv, ip_t* ip) {
    unsigned char num_rec = 0; /* Used to record the two allowed numerical options */
    bool skip_check = false; /* To skip the the type and limit check, only used by numerical options or string arguments */

    if(*argc == 1) {
        printf(WELCOME_STR);
        exit(EXIT_SUCCESS);
    }

    for(int i = 1; i < *argc; i++) {
        /* Check these options before anything else */
        if (!(strcmp("-h", argv[i])) || !(strcmp("--help", argv[i]))) {
            output_help();
            exit(EXIT_SUCCESS);
        } 
        if (!(strcmp("--version", argv[i]))) {
            printf(VERSION_STR);
            exit(EXIT_SUCCESS);
        } 
        /* Important to check when using the default numerical behaviour */
        if (*argc == 2) {
            fprintf(stderr, "%s\n", FEW_ARGS_STR);
            exit(EXIT_FAILURE);
        }

        /* To skip the numerical checks for the argument */
        if(!(skip_check)){
            CHECK_RES(sscanf(argv[i + 1], "%lf", &ip->val));
            CHECK_LIMITS(ip->val);
        } else {
            skip_check = false; /* Always make sure it's false for the next iteration */
        }

        if (!(strcmp("-o", argv[i])) || !(strcmp("--output", argv[i]))) {
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
        if(!strcmp("--current-mA", argv[i])) {
            ip->current = 10e-3 * ip->val;
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
            ip->copper_weight = CONV_MM_TO_OZFT2(ip->val); 
            i++;
            continue;
        }
        if(!(strcmp("--copper-weight-um", argv[i]))) {
            ip->copper_weight = CONV_UM_TO_OZFT2(ip->val); 
            i++;
            continue;
        }
        if(!(strcmp("-r", argv[i])) || !(strcmp("--temperature-rise", argv[i]))) {
            ip->temperature_rise = ip->val;
            i++;
            continue;
        }
        if(!(strcmp("--temperature-rise-F", argv[i]))) {
            ip->temperature_rise = CONV_FAHR_TO_CELS(ip->val);
            i++;
            continue;
        }
        if(!(strcmp("-a", argv[i])) || !(strcmp("--temperature-ambient", argv[i]))) {
            ip->temperature_ambient = ip->val;
            i++;
            continue;
        }
        if(!(strcmp("--temperature-ambient-F", argv[i]))) {
            ip->temperature_ambient = CONV_FAHR_TO_CELS(ip->val);
            i++;
            continue;
        }
        if(!(strcmp("-l", argv[i])) || !(strcmp("--trace-length", argv[i]))) {
            ip->trace_length = ip->val;
            i++;
            continue;
        }
        if(!(strcmp("--trace-length-mm", argv[i]))) {
            ip->trace_length = 10e-1 * ip->val;
            i++;
            continue;
        }
        if(!(strcmp("--trace-length-mil", argv[i]))) {
            ip->trace_length = CONV_MIL_TO_MM(10e-1 * ip->val);
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
        if(!(strcmp("-t", argv[i])) || !(strcmp("--pcb-thickness", argv[i]))) {
            ip->pcb_thickness = ip->val;
            i++;
            continue;
        }
        if(!(strcmp("--pcb-thickness-mm", argv[i]))) {
            ip->trace_length = CONV_MM_TO_MIL(ip->val);
            i++;
            continue;
        }
        if(!(strcmp("-e", argv[i])) || !(strcmp("--pcb-thermal-conductivity", argv[i]))) {
            ip->pcb_thermal_cond = ip->val;
            i++;
            continue;
        }
        if(!(strcmp("-p", argv[i])) || !(strcmp("--plane-area", argv[i]))) {
            ip->plane_area = ip->val;
            i++;
            continue;
        }
        if(!(strcmp("--plane-area-cm2", argv[i]))) {
            ip->trace_length = CONV_CM2_TO_INCH2(ip->val);
            i++;
            continue;
        }
        if(!(strcmp("-d", argv[i])) || !(strcmp("--plane-distance", argv[i]))) {
            ip->plane_distance = ip->val;
            i++;
            continue;
        }
        if(!(strcmp("--plane-distance-mm", argv[i]))) {
            ip->trace_length = CONV_MM_TO_MIL(ip->val);
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
    /* Go over the inputs one more time to make sure no crazy output happens */
    recheck_options(ip);
}

void set_default_inputs(ip_t* ip) {
    /* Set input value defaults */
    ip->standard = IPC2152;
    ip->temperature_rise = 10;      
    ip->temperature_ambient = 25;   
    ip->trace_length = 0;           
    ip->resistivity = 1.7e-6;       // resistivity of copper
    ip->a = 3.9e-3;                 // resistivity temperature coefficient for copper
    ip->val = 0;                    
    ip->res = 0;                    
    ip->ofile.oflag = false;        
    ip->ofile.path = "\0";
    ip->pcb_thickness = 0;
    ip->plane_area = 0;
    ip->plane_distance = 0;
    ip->pcb_thermal_cond = 0.20;
    ip->pcb_thickness = 1.6;

    /* Set modifier defaults */
    ip->cf.copper_weight = 1;
    ip->cf.temperature_rise = 1;
    ip->cf.plane_area = 1;
    ip->cf.pcb_thickness = 1;
    ip->cf.plane_distance = 1;
    ip->cf.pcb_thermal_cond = 1;
}

void recheck_options(ip_t* ip) {
    /* Mainly to check if all of the required inputs are used so that no crazy values are calculated */
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

double calc_2221_area_mils2(ip_t* ip, float k) {
    return pow(ip->current/k * pow(ip->temperature_rise, 0.44), 1/0.725); 
}

void ipc2221_calcs(ip_t* ip, op_t* op) {
    op->extl.area = calc_2221_area_mils2(ip, k_EXT); 
    calc_common(ip, &op->extl);
    op->intl.area = calc_2221_area_mils2(ip, k_INT);
    calc_common(ip, &op->intl);
}

void ipc2152_calcs(ip_t *ip, op_t *op) {
    ipc2152_calcsA(ip, op);
    ipc2152_calcsB(ip, op);
}

double calc_2152_areaA_mils2(ip_t* ip, double temperature_rise) {
    /* Different one on the website, and different one in the website code */
    return (110.515 * pow(temperature_rise, -0.871) + 0.803) * pow(ip->current, 0.868 * pow(temperature_rise, -0.102) + 1.129);    
    /* return (117.555 * pow(ip->temperature_rise, -0.913) + 1.15) * pow(ip->current, 0.84 * pow(ip->temperature_rise, -0.018) + 1.159);  */
}

void ipc2152_calcsA(ip_t* ip, op_t* op) {
    op->layer.area = calc_2152_areaA_mils2(ip, ip->temperature_rise);

    /* Copper weight correction factor */
    if (ip->copper_weight == 2) {
        ip->cf.copper_weight = -0.0185 * log(ip->current) + 0.9861;
    } else if (ip->copper_weight == 3) {
        ip->cf.copper_weight = 1;
    } else {
        ip->cf.copper_weight = -0.0318 * log(ip->current) + 0.9128; 
    }

    /* PCB thickness correction factor */
    if (ip->pcb_thickness != 0) {
        ip->cf.pcb_thickness = 25.959 * pow(ip->pcb_thickness,-0.7666);
    }

    /* Plane area correction factor */
    if (ip->plane_area >= 40) {
        ip->cf.plane_area = 0.89;
    } else if (ip->plane_area >= 20) {
        ip->cf.plane_area = 0.94;
    } else {
        ip->cf.plane_area = 1;
    } // Plane Area in inch^2 but must be >=20 for it to be modified? sus...

    /* Plane distance correction factor */
    if (ip->plane_distance > 125) {
        ip->cf.plane_distance = 1;
    } else {
        ip->cf.plane_distance = 0.0031 * ip->plane_distance + 0.4054;
    }

    /* Calculate the corrected temperature rise */
    ip->cf.temperature_rise = ip->temperature_rise / (ip->cf.copper_weight * ip->cf.pcb_thickness * ip->cf.plane_distance * ip->cf.plane_area);

    /* Calculate the corrected area and trace width  */
    op->layer.corr_area = calc_2152_areaA_mils2(ip, ip->cf.temperature_rise);
    op->layer.corr_trace_width = calc_width_mils(ip, &op->layer.corr_area);

    calc_common(ip, &op->layer);
}

double calc_2152_areaB_mils2(ip_t* ip) {
    return pow(ip->current/(0.089710902134 * pow(ip->temperature_rise, 0.39379253898)), 1/(0.50382053698 * pow(ip->temperature_rise, 0.038495772461))); // * 0.0254 * 0.0254;
}

void ipc2152_calcsB(ip_t* ip, op_t* op) {
    op->layer.area = calc_2152_areaB_mils2(ip); 

    /* Coefficients array */
    double coeff_arr[6][4] = {
        {0.98453567795,     -0.22281787548,     0.20061423196,      -0.041541116264},
        {-0.01657194921,     0.00017520059279, -0.0050615234096,     0.002281483634},
        {0.00088711317661,   0.0013631745743,  -0.0002237330971,    -0.00010974218613},
        {-66729255031e-16,  -0.00014976736827,  58082340133e-15,    -24728159584e-16},
        {-7.9576264561e-7,   55788354958e-16,  -24912026388e-16,     2.4000295954e-7},
        {1.6619678738e-8,   -7.1122635445e-8,   3.3800191741e-8,    -3.9797591878e-9}
    };

    for(int i = 0; i < 6; i++) {
        for(int c = 0; c < 4; c++) {
            /* Copper weight here must be in oz/ft2 */
            ip->cf.copper_weight += coeff_arr[i][c] * pow(ip->copper_weight, c) * pow(ip->current, i); 
        }
    }

    /* PCB Thickness must be in mil */
    ip->cf.pcb_thickness = 24.929779905 * pow(ip->pcb_thickness, -0.75501997929);

    /* Plane Distance must be in mil */
    ip->cf.plane_distance = 0.0031298662911 * ip->plane_distance + 0.40450883823;

    /* PCB Thermal Conductivity must be in BTU/h*ft*F */
    ip->cf.pcb_thermal_cond = -1.4210148167 * ip->pcb_thermal_cond + 1.1958174134;

    /* Corrected area */
    op->layer.corr_area = op->layer.area * ip->cf.copper_weight * ip->cf.pcb_thickness * ip->cf.plane_distance * ip->cf.pcb_thermal_cond;

    /* Corrected Trace Width */
    op->layer.corr_trace_width = op->layer.corr_area / ip->copper_weight;

    calc_common(ip, &op->layer);
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
    return *area/(ip->copper_weight * 1.37);
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
                    "PCB Thickness:\t\t%.15lf\t[mil]\n"
                    "PCB Thermal Cond. CF:\t%.15lf\t[W/mK]\n" // only in method B
                    "Plane Distance:\t\t%.15lf\t[mil]\n"
                    "Plane Area:\t\t%.15lf\t[in^2]\n" // only in method A
                    "### End of Inputs ###\n\n", 
                    ip->current, ip->copper_weight, ip->temperature_rise, ip->temperature_ambient, ip->trace_length, ip->pcb_thickness, ip->pcb_thermal_cond, ip->plane_distance, ip->plane_area);

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
                    "PCB Thermal Cond. CF:\t%.15lf\t[units]\n" // only in method B
                    "Plane Area CF:\t\t%.15lf\t[units]\n" // only in method A
                    "Plane Distance CF:\t%.15lf\t[units]\n"
                    "Temperature Rise CF:\t%.15lf\t[units]\n" // only in method A
                    "### End of Correction Factors ###\n\n",
                    ip->cf.copper_weight, ip->cf.pcb_thickness, ip->cf.pcb_thermal_cond, ip->cf.plane_area, ip->cf.plane_distance,ip->cf.temperature_rise);

            fprintf(file,
                    "Notes:\n"
                    "- Constants used for these calculations were,\n\n"
                    "\ta = %.7lf (Resistivity Temperature Coefficient)\n"
                    "%6srho = %.7lf (Resistivity)\n\n"
                    "write source here"
                    "\n- Maximum trace temperature considered is %.7lf C\n\n",
                    ip->a, " " ,ip->resistivity, ip->temperature_rise + ip->temperature_ambient);
            break;
    }
    fprintf(file, 
            "\nDesign assistance by the TWC tool is provided with no liability whatsover. For final decisions on electronics designs, please consult your boss\n");
}
