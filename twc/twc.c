// TODO: Change units in outputs.
// TODO: Check the standard and method options first and then initialise the corresponding default vars. Have a function pointer for the default inputs
// TODO: Check functions for each input
// TODO: Create help for each option?

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

    /* Select procedures based on inputs */
    sel_proc_outp(&ip);

    /* Calculate the values */
    ip.proc(&ip, &op);

    /* Open file to save outputs */
    if(ip.ofile.oflag) {
        file = fopen(ip.ofile.dest, "w");
        if (!(file)) {
            fprintf(stderr, "File not able to be saved, check input.\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Output the results to the buffer */
    ip.outp(&ip, &op, file);

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
    bool skip_check = false; /* To skip the type and limit check, only used by numerical options or string arguments */

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
            /* printf("%s and %s\n", argv[i], argv[i + 1]); */
            CHECK_RES(sscanf(argv[i + 1], "%lf", &ip->val));
            /* add expected_lf/str/chr  functions instead of using skip check, sscanf will still be called only once per iteration*/
            CHECK_LIMITS(ip->val);
        } else {
            skip_check = false; /* Always make sure it's false for the next iteration */
        }

        if (!(strcmp("-o", argv[i])) || !(strcmp("--output", argv[i]))) {
            printf("%s and %s\n", argv[i], argv[i + 1]);
            ip->ofile.oflag = true;
            set_output_file(&ip->ofile, argv[i + 1]);
            i++;
            skip_check = true;
            continue;
        }
        if(!(strcmp("--standard", argv[i]))) {
            ip->standard.num = ip->val;
            i++;
            continue;
        }
        if(!(strcmp("--method", argv[i]))) {
            char* optstr = argv[i + 1];
            ip->method = optstr[0];
            i++;
            skip_check = true;
            continue;
        }
        if(!(strcmp("-c", argv[i])) || !(strcmp("--current", argv[i]))) {
            ip->current.outval = ip->val;
            ip->current.val = ip->val;
            ip->current.units = "A";
            i++;
            continue;
        }
        if(!strcmp("--current-mA", argv[i])) {
            ip->current.outval = ip->val;
            ip->current.val = 10e-3 * ip->val;
            ip->current.units = "mA";
            i++;
            continue;
        }
        if(!(strcmp("-w", argv[i])) || !(strcmp("--copper-weight", argv[i]))) {
            ip->copper_weight.outval = ip->val;
            ip->copper_weight.val = ip->val;
            ip->copper_weight.units = "oz/ft^2";
            i++;
            continue;
        }
        if(!(strcmp("--copper-weight-mil", argv[i]))) {
            ip->copper_weight.outval = ip->val;
            ip->copper_weight.val = CONV_MIL_TO_OZFT2(ip->val); // convert back to oz/ft^2 for the calculations to work
            ip->copper_weight.units = "mil";
            i++;
            continue;
        }
        if(!(strcmp("--copper-weight-mm", argv[i]))) {
            ip->copper_weight.outval = ip->val;
            ip->copper_weight.val = CONV_MM_TO_OZFT2(ip->val); 
            ip->copper_weight.units = "mm";
            i++;
            continue;
        }
        if(!(strcmp("--copper-weight-um", argv[i]))) {
            ip->copper_weight.outval = ip->val;
            ip->copper_weight.val = CONV_UM_TO_OZFT2(ip->val); 
            ip->copper_weight.units = "um";
            i++;
            continue;
        }
        if(!(strcmp("-r", argv[i])) || !(strcmp("--temperature-rise", argv[i]))) {
            ip->temperature_rise.outval = ip->val;
            ip->temperature_rise.val = ip->val;
            ip->temperature_rise.units = "C";
            i++;
            continue;
        }
        if(!(strcmp("--temperature-rise-F", argv[i]))) {
            ip->temperature_rise.outval = ip->val;
            ip->temperature_rise.val = CONV_FAHR_TO_CELS(ip->val);
            ip->temperature_rise.units = "F";
            i++;
            continue;
        }
        if(!(strcmp("-a", argv[i])) || !(strcmp("--temperature-ambient", argv[i]))) {
            ip->temperature_ambient.outval = ip->val;
            ip->temperature_ambient.val = ip->val;
            ip->temperature_ambient.units = "C";
            i++;
            continue;
        }
        if(!(strcmp("--temperature-ambient-F", argv[i]))) {
            ip->temperature_ambient.outval = ip->val;
            ip->temperature_ambient.val = CONV_FAHR_TO_CELS(ip->val);
            ip->temperature_ambient.units = "F";
            i++;
            continue;
        }
        if(!(strcmp("-l", argv[i])) || !(strcmp("--trace-length", argv[i]))) {
            ip->trace_length.outval = ip->val;
            ip->trace_length.val = ip->val;
            ip->trace_length.units = "cm";
            i++;
            continue;
        }
        if(!(strcmp("--trace-length-mm", argv[i]))) {
            ip->trace_length.outval = ip->val;
            ip->trace_length.val = 10e-1 * ip->val;
            ip->trace_length.units = "mm";
            i++;
            continue;
        }
        if(!(strcmp("--trace-length-mil", argv[i]))) {
            ip->trace_length.outval = ip->val;
            ip->trace_length.val = CONV_MIL_TO_MM(10e-1 * ip->val);
            ip->trace_length.units = "mil";
            i++;
            continue;
        }
        if(!(strcmp("-t", argv[i])) || !(strcmp("--pcb-thickness", argv[i]))) {
            ip->pcb_thickness.outval = ip->val;
            ip->pcb_thickness.val = ip->val;
            ip->pcb_thickness.units = "mil";
            i++;
            continue;
        }
        if(!(strcmp("--pcb-thickness-mm", argv[i]))) {
            ip->pcb_thickness.outval = ip->val;
            ip->pcb_thickness.val = CONV_MM_TO_MIL(ip->val);
            ip->pcb_thickness.units = "mm";
            i++;
            continue;
        }
        if(!(strcmp("-e", argv[i])) || !(strcmp("--pcb-thermal-conductivity", argv[i]))) {
            ip->pcb_thermal_cond.outval = ip->val;
            ip->pcb_thermal_cond.val = CONV_WmK_TO_BTUhftF(ip->val);
            ip->pcb_thermal_cond.units = "W/mK";
            i++;
            continue;
        }
        if(!(strcmp("-p", argv[i])) || !(strcmp("--plane-area", argv[i]))) {
            ip->plane_area.outval = ip->val;
            ip->plane_area.val = ip->val;
            ip->plane_area.units = "in^2";
            i++;
            continue;
        }
        if(!(strcmp("--plane-area-cm2", argv[i]))) {
            ip->plane_area.outval = ip->val;
            ip->plane_area.val = CONV_CM2_TO_INCH2(ip->val);
            ip->plane_area.units = "cm^2";
            i++;
            continue;
        }
        if(!(strcmp("-d", argv[i])) || !(strcmp("--plane-distance", argv[i]))) {
            ip->plane_distance.outval = ip->val;
            ip->plane_distance.val = ip->val;
            ip->plane_distance.units = "mil";
            i++;
            continue;
        }
        if(!(strcmp("--plane-distance-mm", argv[i]))) {
            ip->plane_distance.outval = ip->val;
            ip->plane_distance.val = CONV_MM_TO_MIL(ip->val);
            ip->plane_distance.units = "mm";
            i++;
            continue;
        }
        if(!(strcmp("--resistivity", argv[i]))) {
            ip->resistivity.val = ip->val;
            i++;
            continue;
        }
        if(!(strcmp("--temperature-coefficient", argv[i]))) {
            ip->a.val = ip->val;
            i++;
            continue;
        }

        /* Checks for the numerical options */
        if(sscanf(argv[i], "%lf", &ip->val) && (num_rec == 0)) {
            num_rec++;
            skip_check = true;
            ip->current.outval = ip->val;
            ip->current.val = ip->val;
            ip->current.units = "A";
            continue;
        }
        if((sscanf(argv[i], "%lf", &ip->val)) && (num_rec == 1)) {
            num_rec++;
            skip_check = true;
            ip->copper_weight.outval = ip->val;
            ip->copper_weight.val = ip->val;
            ip->copper_weight.units = "oz/ft^2";
            continue;
        }

        /* Error checking */
        fprintf(stderr, "Unknown option '%s', exiting.", argv[i]);
        exit(EXIT_FAILURE);
    }
    /* Go over the inputs one more time to make sure no crazy output happens */
    if((ip->current.val == 0) || ip->copper_weight.val == 0) {
        fprintf(stderr, "\nPlease input at least the Current and the Copper Weight.\n");
    }
}

void set_default_inputs(ip_t* ip) {
    /* Set input value defaults */
    ip->standard.num = IPC2152;
    ip->method = 'B';

    ip->current.outval = 0;      
    ip->current.val = 0;      
    ip->current.units = "A";      

    ip->copper_weight.outval = 0;      
    ip->copper_weight.val = 0;      
    ip->copper_weight.units = "oz/ft^2";      

    ip->temperature_rise.outval = 10;      
    ip->temperature_rise.val = 10;      
    ip->temperature_rise.units = "C";      

    ip->temperature_ambient.outval = 25;   
    ip->temperature_ambient.val = 25;   
    ip->temperature_ambient.units = "C";   

    ip->trace_length.outval = 0;           
    ip->trace_length.val = 0;           
    ip->trace_length.units = "cm";           

    ip->plane_area.outval = 0;
    ip->plane_area.val = 0;
    ip->plane_area.units = "in^2"; 

    ip->plane_distance.outval = 0;
    ip->plane_distance.val = 0;
    ip->plane_distance.units = "mil"; 

    ip->pcb_thermal_cond.outval = 0.20;
    ip->pcb_thermal_cond.val = CONV_WmK_TO_BTUhftF(0.20);
    ip->pcb_thermal_cond.units = "W/mK"; 

    ip->pcb_thickness.outval = 62;
    ip->pcb_thickness.val = 62;
    ip->pcb_thickness.units = "mil"; 

    ip->resistivity.val = 1.724e-6;
    ip->a.val = 3.93e-3;

    ip->val = 0;                    
    ip->res = 0;                    
    
    ip->ofile.oflag = false;        
    ip->ofile.path = "\0";

    /* Set modifier defaults */
    ip->cf.temperature_rise = 1;
    ip->cf.plane_area = 1;
    ip->cf.pcb_thickness = 1;
    ip->cf.plane_distance = 1;
    ip->cf.pcb_thermal_cond = 1;
}

void sel_proc_outp(ip_t* ip) {
    switch (ip->standard.num) {
        case IPC2221:
            ip->method = 'A'; /* Default to A since only one method for IPC2221 */
            switch (ip->method) {
                case 'A':
                    ip->proc = &ipc2221_calcs; 
                    ip->outp = &output_results_2221; 
                    break;
                default:
                    fprintf(stderr, "Method %c for the IPC-%d doesn't exist.", ip->method, ip->standard.num);
                    exit(EXIT_FAILURE);
            }
            return;
        case IPC2152:
            switch (ip->method) {
                case 'A':
                    ip->proc = &ipc2152_calcsA; 
                    ip->outp = &output_results_2152_A; 
                    break;
                case 'B':
                    ip->proc = &ipc2152_calcsB; 
                    ip->outp = &output_results_2152_B; 
                    break;
                default:
                    fprintf(stderr, "Method %c for the IPC-%d doesn't exist.", ip->method, ip->standard.num);
                    exit(EXIT_FAILURE);
            }
            break;
        default:
            fprintf(stderr, "Standard IPC-%d doesn't exist.",ip->standard.num);

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
    op->extl.area = pow(ip->current.val/k_EXT * pow(ip->temperature_rise.val, 0.44), 1/0.725); 
    calc_rvp(ip, &op->extl);
    op->intl.area = pow(ip->current.val/k_INT * pow(ip->temperature_rise.val, 0.44), 1/0.725); 
    calc_rvp(ip, &op->intl);
}

void ipc2152_calcsA(ip_t* ip, op_t* op) {
    /* Different one on the website, and different one in the website code */
    op->layer.area = (110.515 * pow(ip->temperature_rise.val, -0.871) + 0.803) * pow(ip->current.val, 0.868 * pow(ip->temperature_rise.val, -0.102) + 1.129);    
    /* op->layer.area = (117.555 * pow(ip->temperature_rise.val, -0.913) + 1.15) * pow(ip->current.val, 0.84 * pow(ip->temperature_rise.val, -0.018) + 1.159);  */

    /* Copper weight correction factor */
    if (ip->copper_weight.val == 2) {
        ip->cf.copper_weight = -0.0185 * log(ip->current.val) + 0.9861;
    } else if (ip->copper_weight.val == 3) {
        ip->cf.copper_weight = 1;
    } else {
        ip->cf.copper_weight = -0.0318 * log(ip->current.val) + 0.9128; 
    }

    /* PCB thickness correction factor */
    if (ip->pcb_thickness.val != 0) {
        ip->cf.pcb_thickness = 25.959 * pow(ip->pcb_thickness.val, -0.7666);
    }

    /* Plane area correction factor */
    if (ip->plane_area.val >= 40) {
        ip->cf.plane_area = 0.89;
    } else if (ip->plane_area.val >= 20) {
        ip->cf.plane_area = 0.94;
    } else {
        ip->cf.plane_area = 1;
    } // Plane Area in inch^2 but must be >=20 for it to be modified? sus...

    /* Plane distance correction factor */
    if (ip->plane_distance.val > 125) {
        ip->cf.plane_distance = 1;
    } else {
        ip->cf.plane_distance = 0.0031 * ip->plane_distance.val + 0.4054;
    }

    /* Calculate the corrected temperature rise */
    ip->cf.temperature_rise = ip->temperature_rise.val / (ip->cf.copper_weight * ip->cf.pcb_thickness * ip->cf.plane_distance * ip->cf.plane_area);

    /* Calculate the corrected area and trace width  */
    op->layer.corr_area = (110.515 * pow(ip->cf.temperature_rise, -0.871) + 0.803) * pow(ip->current.val, 0.868 * pow(ip->cf.temperature_rise, -0.102) + 1.129);    
    op->layer.corr_trace_width = calc_width_mils(ip, &op->layer.corr_area);

    calc_rvp(ip, &op->layer);
}

void ipc2152_calcsB(ip_t* ip, op_t* op) {
    op->layer.area = pow(ip->current.val/(0.089710902134 * pow(ip->temperature_rise.val, 0.39379253898)), 1/(0.50382053698 * pow(ip->temperature_rise.val, 0.038495772461)));


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
            ip->cf.copper_weight += coeff_arr[i][c] * pow(ip->copper_weight.val, c) * pow(ip->current.val, i); 
        }
    }

    /* PCB Thickness must be in mil */
    ip->cf.pcb_thickness = 24.929779905 * pow(ip->pcb_thickness.val, -0.75501997929);

    /* Plane Distance must be in mil */
    if(ip->plane_distance.val != 0) {
        ip->cf.plane_distance = 0.0031298662911 * ip->plane_distance.val + 0.40450883823;
    }

    /* PCB Thermal Conductivity must be in BTU/h*ft*F */
    ip->cf.pcb_thermal_cond = -1.4210148167 * ip->pcb_thermal_cond.val + 1.1958174134;

    /* Corrected area */
    op->layer.corr_area = op->layer.area * ip->cf.copper_weight * ip->cf.pcb_thickness * ip->cf.plane_distance * ip->cf.pcb_thermal_cond;

    /* Corrected Trace Width */
    op->layer.corr_trace_width = op->layer.corr_area / ip->copper_weight.val;

    calc_rvp(ip, &op->layer);
}

void calc_rvp(ip_t* ip, layer_t* layer) {
    layer->trace_width = calc_width_mils(ip, &layer->area);
    if (ip->trace_length.val > 0) {
        layer->resistance  = calc_resistance(ip, &layer->area); 
    }
    layer->voltage_drop = calc_vdrop(ip, &layer->resistance); 
    layer->power_loss = calc_power_loss(ip, &layer->voltage_drop);
}

double calc_width_mils(ip_t* ip, double* area) {
    return *area/(ip->copper_weight.val * 1.37);
}

double calc_resistance(ip_t* ip, double* area) {
    return ((ip->trace_length.val * ip->resistivity.val)/(CONV_MIL2_TO_CM2(*area))) * (1 + (ip->a.val * ((ip->temperature_rise.val + ip->temperature_ambient.val) - ip->temperature_ambient.val)));
}

double calc_vdrop(ip_t* ip, double* resistance) {
    return ip->current.val * (*resistance); 
}

double calc_power_loss(ip_t* ip, double* vdrop) {
    return ip->current.val * (*vdrop);
}

char* get_time() {
    time_t time_since_epoch = time(NULL);
    struct tm *tm = localtime(&time_since_epoch);
    static char s[13];
    strftime(s, sizeof(s), "%d%m%y%H%M%S", tm);
    return s;
}

void output_results_2221(ip_t* ip, op_t* op, FILE * file) {

    fprintf(file,   
            "\n"
            "Current:\t\t%.15lf\t[%s]\n"
            "Copper Weight:\t\t%.15lf\t[%s]\n"
            "Temperature, Rise:\t%.15lf\t[%s]\n"
            "Temperature, Ambient:\t%.15lf\t[%s]\n"
            "Trace Length:\t\t%.15lf\t[%s]\n",
            ip->current.outval, ip->current.units, ip->copper_weight.outval, ip->copper_weight.units, ip->temperature_rise.outval, ip->temperature_rise.units, ip->temperature_ambient.val, ip->temperature_ambient.units, ip->trace_length.outval, ip->trace_length.units);

    fprintf(file,   
            "\n\n"
            "===External Layers===\n"
            "Area: \t\t\t%.15lf\t[mil^2]\n"
            "Width:\t\t\t%.15lf\t[mil]\n"
            ,op->extl.area, op->extl.trace_width);

    fprintf(file,
            ip->trace_length.val > 0 ? 
            "Resistance:\t\t%.15lf\t[Ohm]\n"
            "Voltage Drop:\t\t%.15lf\t[V]\n"
            "Power Loss:\t\t%.15lf\t[W]\n" : "\r",
            op->extl.resistance, op->extl.voltage_drop, op->extl.power_loss);

    fprintf(file, "=====================\n\n");

    fprintf(file,   
            "===Internal Layers==="
            "\n"
            "Area: \t\t\t%.15lf\t[mil^2]\n"
            "Width:\t\t\t%.15lf\t[mil]\n"
            ,op->intl.area, op->intl.trace_width);

    fprintf(file,
            ip->trace_length.val > 0 ? 
            "Resistance:\t\t%.15lf\t[Ohm]\n"
            "Voltage Drop:\t\t%.15lf\t[V]\n"
            "Power Loss:\t\t%.15lf\t[W]\n" : "\r",
            op->intl.resistance, op->intl.voltage_drop, op->intl.power_loss);

    fprintf(file, 
            "=====================\n\n");

    fprintf(file,
            "\nValues of k used in calculating the area:"
            "\tk = %.7lf (Internal layers)\n"
            "\tk = %.7lf (External layers)\n",
            k_INT, k_EXT);

    fprintf(file, ip->trace_length.val == 0 ? 
            "\n- Use trace length with '-l' to get voltage, resistance and power calculations.\n" : 
            "\n- Constants used for the P/I/V calculations were,\n\n"
            "\ta = %.7lf (Temperature Coefficient)\n"
            "%6srho = %.7lf (Resistivity)\n",
            ip->a.val, " " ,ip->resistivity.val); 

    fprintf(file,   
            "\n- Constants and method used were derived from http://circuitcalculator.com/wordpress/2006/03/12/pcb-via-calculator/.\n");

    fprintf(file, DISCLAIMER_STR);
}

void output_results_2152_A(ip_t* ip, op_t *op, FILE *file) {
    fprintf(file,
            "\n"
            "Current:\t\t%.15lf\t[%s]\n"
            "Copper Weight:\t\t%.15lf\t[%s]\n"
            "Temperature, Rise:\t%.15lf\t[%s]\n"
            "Trace Length:\t\t%.15lf\t[%s]\n"
            "PCB Thickness:\t\t%.15lf\t[%s]\n"
            "Plane Distance:\t\t%.15lf\t[%s]\n"
            "Plane Area:\t\t%.15lf\t[%s]\n", 
            ip->current.outval, ip->current.units, ip->copper_weight.outval, ip->copper_weight.units, ip->temperature_rise.outval, ip->temperature_rise.units, ip->trace_length.outval, ip->trace_length.units, ip->pcb_thickness.outval, ip->pcb_thickness.units, ip->plane_distance.outval, ip->plane_distance.units, ip->plane_area.outval, ip->plane_area.units);

    fprintf(file,
            "\n"
            "Area: \t\t\t%.15lf\t[mil^2]\n"
            "Corrected Area: \t%.15lf\t[mil^2]\n"
            "Width:\t\t\t%.15lf\t[mil]\n"
            "Corrected Width: \t%.15lf\t[mil]\n",
            op->layer.area, op->layer.corr_area, op->layer.trace_width, op->layer.corr_trace_width);

    fprintf(file,
            ip->trace_length.val > 0 ? 
            "Resistance:\t\t%.15lf\t[Ohm]\n"
            "Voltage Drop:\t\t%.15lf\t[V]\n"
            "Power Loss:\t\t%.15lf\t[W]\n" : "\r",
            op->extl.resistance, op->extl.voltage_drop, op->extl.power_loss);

    fprintf(file, 
            "\n"
            "Copper Weight CF:\t%.15lf\t[units]\n"
            "PCB Thickness CF:\t%.15lf\t[units]\n"
            "Plane Area CF:\t\t%.15lf\t[units]\n" 
            "Plane Distance CF:\t%.15lf\t[units]\n"
            "Temperature Rise CF:\t%.15lf\t[units]\n",
            ip->cf.copper_weight, ip->cf.pcb_thickness, ip->cf.plane_area, ip->cf.plane_distance,ip->cf.temperature_rise);

    fprintf(file, ip->trace_length.val == 0 ? 
            "\n- Use trace length with '-l' to get voltage, resistance and power calculations.\n" : 
            "\n- Constants used for the P/I/V calculations were,\n\n"
            "\ta = %.7lf (Temperature Coefficient)\n"
            "%6srho = %.7lf (Resistivity)\n",
            ip->a.val, " " ,ip->resistivity.val); 

    fprintf(file,   
            "\n- Constants and method used were derived from https://www.smps.us/pcb-calculator.html.\n");

    fprintf(file, DISCLAIMER_STR);
}


void output_results_2152_B(ip_t* ip, op_t *op, FILE *file) {
    fprintf(file,
            "\n"
            "Current:\t\t%.15lf\t[%s]\n"
            "Copper Weight:\t\t%.15lf\t[%s]\n"
            "Temperature, Rise:\t%.15lf\t[%s]\n"
            "Trace Length:\t\t%.15lf\t[%s]\n"
            "PCB Thickness:\t\t%.15lf\t[%s]\n"
            "Plane Distance:\t\t%.15lf\t[%s]\n"
            "PCB Thermal Cond.:\t%.15lf\t[%s]\n",
            ip->current.outval, ip->current.units, ip->copper_weight.outval, ip->copper_weight.units, ip->temperature_rise.outval, ip->temperature_rise.units, ip->trace_length.outval, ip->trace_length.units, ip->pcb_thickness.outval, ip->pcb_thickness.units, ip->plane_distance.outval, ip->plane_distance.units, ip->pcb_thermal_cond.outval, ip->pcb_thermal_cond.units);

    fprintf(file,
            "\n"
            "Area: \t\t\t%.15lf\t[mil^2]\n"
            "Corrected Area: \t%.15lf\t[mil^2]\n"
            "Width:\t\t\t%.15lf\t[mil]\n"
            "Corrected Width: \t%.15lf\t[mil]\n",
            op->layer.area, op->layer.corr_area, op->layer.trace_width, op->layer.corr_trace_width);

    fprintf(file,
            ip->trace_length.val > 0 ? 
            "Resistance:\t\t%.15lf\t[Ohm]\n"
            "Voltage Drop:\t\t%.15lf\t[V]\n"
            "Power Loss:\t\t%.15lf\t[W]\n" : "\r",
            op->layer.resistance, op->layer.voltage_drop, op->layer.power_loss);

    fprintf(file, 
            "\n"
            "Copper Weight CF:\t%.15lf\t[units]\n"
            "PCB Thickness CF:\t%.15lf\t[units]\n"
            "PCB Thermal Cond. CF:\t%.15lf\t[units]\n"
            "Plane Distance CF:\t%.15lf\t[units]\n",
            ip->cf.copper_weight, ip->cf.pcb_thickness, ip->cf.pcb_thermal_cond, ip->cf.plane_distance);

    fprintf(file, ip->trace_length.val == 0 ? 
            "\n- Use trace length with '-l' to get voltage, resistance and power calculations.\n" : 
            "\n- Constants used for the P/I/V calculations were,\n\n"
            "\ta = %.7lf (Temperature Coefficient)\n"
            "%6srho = %.7lf (Resistivity)\n",
            ip->a.val, " " ,ip->resistivity.val); 

    fprintf(file,   
            "\n- Constants and method used were derived from https://ninjacalc.mbedded.ninja/calculators/electronics/pcb-design/track-current-ipc2152.\n");

    fprintf(file, DISCLAIMER_STR);
}

void output_help() {
    printf("\nHelp for the Trace Width Calculator (TWC). Specify units with the long options, listed below the short options."
            "\n\t-c, \t--current <Current [A]>\t\t\t\t= Input the trace current in Amps.\n"
            "\t\t--current-mA\n"
            "\n\t-w, \t--copper-weight <Copper Weight [oz/ft^2]>\t= Input the copper weight in oz per ft^2.\n"
            "\t\t--copper-weight-mil\n"
            "\t\t--copper-weight-mm\n"
            "\t\t--copper-weight-um\n"
            "\n\t-r \t--temperature-rise <Temperature Rise [C]>\t= Input the maximum allowed temperature rise in C.\n"
            "\t\t--temperature-rise-F\n"
            "\n\t-a, \t--temperature-ambient <Ambient Temperature [C]>\t= Input the ambient temperature of the trace in C.\n"
            "\t\t--temperature-ambient-F\n"
            "\n\t-l, \t--trace--length<Trace Length [cm]>\t\t= Input the trace length in centimeters.\n"
            "\t\t--trace--length-mm\n"
            "\t\t--trace--length-mil\n"
            "\n\t-t, \t--pcb-thickness <Thickness [mm]>\t\t= Input the PCB thickness in milimeters.\n"
            "\t\t--pcb-thickness-mil\n"
            "\n\t-e, \t--pcb-thermal-conductivity <Therm. Con. [W/mK]>\t= Input the PCB thermal conductivity in Watts per meter Kelvin.\n"
            "\n\t-p, \t--plane-area <Plane Area [in^2]>\t\t= Input the plane area in inches squared.\n"
            "\t\t--plane-area-cm2\n"
            "\n\t-d, \t--plane-distance <Plane Distance [mil]>\t\t= Input the plane distance in mil.\n"
            "\t\t--plane-distance-mm\n"
            "\n\t--resistivity <Resistivity [Ohmm]>\t\t\t= Input the resistivity in Ohm meters.\n"
            "\n\t--temperature-coefficient <Temp. Coefficient [1/C]>\t= Input the temperature coefficient.\n"
            "\n\t-o <File Name>\t\t\t= Write the name of the outputted file. Use '.txt' to create a text file. Use a single '.' to auto-generate the name based on date/time. Can also write the full path to the file, e.g. 'C:/Users/user/output.txt' or stop at 'C:/Users/user/' to use the auto-generated file name.\n");
}

