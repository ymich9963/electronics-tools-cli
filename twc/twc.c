#include "twc.h"

void get_options(int* argc, char** argv, ip_t* ip) {
    unsigned char num_rec = 0; /* Used to record the two allowed numerical options */
    double val; /* Temporary value to store the argument */

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
        if (!(strcmp("-o", argv[i])) || !(strcmp("--output", argv[i]))) {
            ip->ofile.oflag = true;
            set_output_file(&ip->ofile, argv[i + 1]);
            i++;
            continue;
        }
        if(!(strcmp("-c", argv[i])) || !(strcmp("--current", argv[i]))) {
            // TODO: Try to simplify or abstract the code execution at each if-branch
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->current.outval = val;
            ip->current.val = val;
            ip->current.units = "A";
            i++;
            continue;
        }
        if(!strcmp("--current-mA", argv[i])) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->current.outval = val;
            ip->current.val = 10e-3 * val;
            ip->current.units = "mA";
            i++;
            continue;
        }
        if(!(strcmp("-w", argv[i])) || !(strcmp("--copper-weight", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->copper_weight.outval = val;
            ip->copper_weight.val = val;
            ip->copper_weight.units = "oz/ft^2";
            i++;
            continue;
        }
        if(!(strcmp("--copper-weight-mil", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->copper_weight.outval = val;
            ip->copper_weight.val = CONV_MIL_TO_OZFT2(val); /* Convert back to oz/ft^2 for the calculations to work */
            ip->copper_weight.units = "mil";
            i++;
            continue;
        }
        if(!(strcmp("--copper-weight-mm", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->copper_weight.outval = val;
            ip->copper_weight.val = CONV_MM_TO_OZFT2(val); 
            ip->copper_weight.units = "mm";
            i++;
            continue;
        }
        if(!(strcmp("--copper-weight-um", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->copper_weight.outval = val;
            ip->copper_weight.val = CONV_UM_TO_OZFT2(val); 
            ip->copper_weight.units = "um";
            i++;
            continue;
        }
        if(!(strcmp("-r", argv[i])) || !(strcmp("--temperature-rise", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->temperature_rise.outval = val;
            ip->temperature_rise.val = val;
            ip->temperature_rise.units = "C";
            i++;
            continue;
        }
        if(!(strcmp("--temperature-rise-F", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->temperature_rise.outval = val;
            ip->temperature_rise.val = CONV_FAHR_TO_CELS(val);
            ip->temperature_rise.units = "F";
            i++;
            continue;
        }
        if(!(strcmp("-a", argv[i])) || !(strcmp("--temperature-ambient", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->temperature_ambient.outval = val;
            ip->temperature_ambient.val = val;
            ip->temperature_ambient.units = "C";
            i++;
            continue;
        }
        if(!(strcmp("--temperature-ambient-F", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->temperature_ambient.outval = val;
            ip->temperature_ambient.val = CONV_FAHR_TO_CELS(val);
            ip->temperature_ambient.units = "F";
            i++;
            continue;
        }
        if(!(strcmp("-l", argv[i])) || !(strcmp("--trace-length", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->trace_length.outval = val;
            ip->trace_length.val = val;
            ip->trace_length.units = "cm";
            i++;
            continue;
        }
        if(!(strcmp("--trace-length-mm", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->trace_length.outval = val;
            ip->trace_length.val = 10e-1 * val;
            ip->trace_length.units = "mm";
            i++;
            continue;
        }
        if(!(strcmp("--trace-length-mil", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->trace_length.outval = val;
            ip->trace_length.val = CONV_MIL_TO_MM(10e-1 * val);
            ip->trace_length.units = "mil";
            i++;
            continue;
        }
        if(!(strcmp("-t", argv[i])) || !(strcmp("--pcb-thickness", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->pcb_thickness.outval = val;
            ip->pcb_thickness.val = val;
            ip->pcb_thickness.units = "mil";
            i++;
            continue;
        }
        if(!(strcmp("--pcb-thickness-mm", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->pcb_thickness.outval = val;
            ip->pcb_thickness.val = CONV_MM_TO_MIL(val);
            ip->pcb_thickness.units = "mm";
            i++;
            continue;
        }
        if(!(strcmp("-e", argv[i])) || !(strcmp("--pcb-thermal-conductivity", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->pcb_thermal_cond.outval = val;
            ip->pcb_thermal_cond.val = CONV_WmK_TO_BTUhftF(val);
            ip->pcb_thermal_cond.units = "W/mK";
            i++;
            continue;
        }
        if(!(strcmp("-p", argv[i])) || !(strcmp("--plane-cs_area", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->plane_area.outval = val;
            ip->plane_area.val = val;
            ip->plane_area.units = "in^2";
            i++;
            continue;
        }
        if(!(strcmp("--plane-cs_area-cm2", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->plane_area.outval = val;
            ip->plane_area.val = CONV_CM2_TO_INCH2(val);
            ip->plane_area.units = "cm^2";
            i++;
            continue;
        }
        if(!(strcmp("-d", argv[i])) || !(strcmp("--plane-distance", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->plane_distance.outval = val;
            ip->plane_distance.val = val;
            ip->plane_distance.units = "mil";
            i++;
            continue;
        }
        if(!(strcmp("--plane-distance-mm", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->plane_distance.outval = val;
            ip->plane_distance.val = CONV_MM_TO_MIL(val);
            ip->plane_distance.units = "mm";
            i++;
            continue;
        }
        if(!(strcmp("--resistivity", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->resistivity.val = val;
            i++;
            continue;
        }
        if(!(strcmp("--temperature-coefficient", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            ip->a.val = val;
            i++;
            continue;
        }
        if(!(strcmp("-m", argv[i])) || !(strcmp("--metric", argv[i]))) {
            ip->uflag = 'm';
            continue;
        }
        if(!(strcmp("-i", argv[i])) || !(strcmp("--imperial", argv[i]))) {
            ip->uflag = 'i';
            continue;
        }
        /* Checks for the numerical options */
        if(sscanf(argv[i], "%lf", &val) && (num_rec == 0)) {
            num_rec++;
            ip->current.outval = val;
            ip->current.val = val;
            ip->current.units = "A";
            continue;
        }
        if((sscanf(argv[i], "%lf", &val)) && (num_rec == 1)) {
            num_rec++;
            ip->copper_weight.outval = val;
            ip->copper_weight.val = val;
            ip->copper_weight.units = "oz/ft^2";
            continue;
        }

        /* Conversions */
        if(!(strcmp("--convert-mil2-cm2", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            printf("\n%lf\n\n", CONV_MIL2_TO_CM2(val));
            printf("Converted using the TWC.\n");
            exit(EXIT_SUCCESS);
        }
        if(!(strcmp("--convert-mil2-mm2", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            printf("\n%lf\n\n", CONV_MIL2_TO_MM2(val));
            printf("Converted using the TWC.\n");
            exit(EXIT_SUCCESS);
        }
        if(!(strcmp("--convert-cm2-in2", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            printf("\n%lf\n\n", CONV_CM2_TO_INCH2(val));
            printf("Converted using the TWC.\n");
            exit(EXIT_SUCCESS);
        }
        if(!(strcmp("--convert-mil-ozft2", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            printf("\n%lf\n\n", CONV_MIL_TO_OZFT2(val));
            printf("Converted using the TWC.\n");
            exit(EXIT_SUCCESS);
        }
        if(!(strcmp("--convert-mm-ozft2", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            printf("\n%lf\n\n", CONV_MM_TO_OZFT2(val) );
            printf("Converted using the TWC.\n");
            exit(EXIT_SUCCESS);
        }
        if(!(strcmp("--convert-um-ozft2", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            printf("\n%lf\n\n", CONV_UM_TO_OZFT2(val) );
            printf("Converted using the TWC.\n");
            exit(EXIT_SUCCESS);
        }
        if(!(strcmp("--convert-ozft2-mil", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            printf("\n%lf\n\n", CONV_OZFT2_TO_MIL(val));
            printf("Converted using the TWC.\n");
            exit(EXIT_SUCCESS);
        }
        if(!(strcmp("--convert-ozft2-mm", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            printf("\n%lf\n\n", CONV_OZFT2_TO_MM(val));
            printf("Converted using the TWC.\n");
            exit(EXIT_SUCCESS);
        }
        if(!(strcmp("--convert-ozft2-um", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            printf("\n%lf\n\n", CONV_OZFT2_TO_UM(val));
            printf("Converted using the TWC.\n");
            exit(EXIT_SUCCESS);
        }
        if(!(strcmp("--convert-mm-mil", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            printf("\n%lf\n\n", CONV_MM_TO_MIL(val));
            printf("Converted using the TWC.\n");
            exit(EXIT_SUCCESS);
        }
        if(!(strcmp("--convert-mil-mm", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            printf("\n%lf\n\n", CONV_CM2_TO_INCH2(val));
            printf("Converted using the TWC.\n");
            exit(EXIT_SUCCESS);
        }
        if(!(strcmp("--convert-F-C", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            printf("\n%lf\n\n", CONV_FAHR_TO_CELS(val));
            printf("Converted using the TWC.\n");
            exit(EXIT_SUCCESS);
        }
        if(!(strcmp("--convert-C-F", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            printf("\n%lf\n\n", CONV_CELS_TO_FAHR(val));
            printf("Converted using the TWC.\n");
            exit(EXIT_SUCCESS);
        }
        if(!(strcmp("--convert-WmK-BTUhftF", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            printf("\n%lf\n\n", CONV_WmK_TO_BTUhftF(val));
            printf("Converted using the TWC.\n");
            exit(EXIT_SUCCESS);
        }
        if(!(strcmp("--convert-BTUhftF-WmK", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%lf", &val));
            CHECK_LIMITS(val);
            printf("\n%lf\n\n", CONV_BTUhftF_TO_WmK(val));
            printf("Converted using the TWC.\n");
            exit(EXIT_SUCCESS);
        }

        /* Just ignore these two since they are checked before */
        if(!(strcmp("--standard", argv[i]))) {
            i++;
            continue;
        }
        if(!(strcmp("--method", argv[i]))) {
            i++;
            continue;
        }

        /* Error checking */
        fprintf(stderr, "\nUnknown option '%s', exiting.\n", argv[i]);
        exit(EXIT_FAILURE);
    }

    /* At least current and copper weight should be entered */
    if((ip->current.val == 0) || ip->copper_weight.val == 0) {
        fprintf(stderr, "\nPlease input at least the Current and the Copper Weight.\n\n");
        exit(EXIT_FAILURE);
    }
}

void get_standard_method(int* argc, char** argv, ip_t* ip) {
    char* standard_arr[] = {"IPC2221", "IPC2152", "afko"}; /* Standard names array */
    int standard_const[] = {2221, 2152, 666}; /* Number representation of the standards */
    char method_arr[] = {'A', 'B'}; /* Method array */
    char strval[10] = "\0"; /* Temp value for the string argument */

    /* Get the sizes of the arrays */
    unsigned char ssize = sizeof(standard_arr) / sizeof(standard_arr[0]);
    unsigned char msize = sizeof(method_arr) / sizeof(method_arr[0]);

    /* Index where the input was matched to the available standards */
    unsigned char index;

    for(int i = 1; i < *argc; i++) {
        if(!(strcmp("--standard", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%s", strval));
            check_standard(strval, standard_arr, ssize, &index);
            ip->standard.str = strval;
            ip->standard.num = standard_const[index];
            i++;
            continue;
        }
        if(!(strcmp("--method", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%s", strval));
            check_method(strval[0], method_arr, msize);
            ip->method = strval[0];
            i++;
            continue;
        }
    }
}

void check_standard(char* strval, char** standard_arr, unsigned int size, unsigned char* index){
    bool okflag = false;
    for(int i = 0; i < size; i++) {
        if(!(strcmp(strval, standard_arr[i]))) {
            *index = i;
            okflag = true;
            break;
        }
    }
    /* In the case where no match is made */
    if(!okflag) {
        fprintf(stderr, "Unknown standard (%s) used...", strval);
        exit(EXIT_FAILURE);
    }
}

void check_method(char chrval, char* method_arr, unsigned int size){
    bool okflag = false;
    for(int i = 0; i < size; i++) {
        if(chrval == method_arr[i]) {
            okflag = true;
            break;
        }
    }
    if(!okflag) {
        fprintf(stderr, "Unknown method (%c) used...", chrval);
        exit(EXIT_FAILURE);
    }
}

void set_outu_IPC2221(ip_t* ip, op_t* op) {
    switch (ip->uflag) {
        case 'm':
            op->extl.cs_area.units = "mm^2";
            op->extl.cs_area.val = CONV_MIL2_TO_MM2(op->extl.cs_area.val);
            op->extl.trace_width.units = "mm";
            op->extl.trace_width.val = CONV_MIL_TO_MM(op->extl.trace_width.val);

            op->intl.cs_area.units = "mm^2";
            op->intl.cs_area.val = CONV_MIL2_TO_MM2(op->intl.cs_area.val);
            op->intl.trace_width.units = "mm";
            op->intl.trace_width.val = CONV_MIL_TO_MM(op->intl.trace_width.val);
            break;
        case 'i':
            /* This case is the default so just let it go to the default branch*/
        default:
            op->extl.cs_area.units = "mil^2";
            op->extl.trace_width.units = "mil";

            op->intl.cs_area.units = "mil^2";
            op->intl.trace_width.units = "mil";
            break;
    }

}

void set_outu_IPC2152(ip_t* ip, op_t* op) {
    switch (ip->uflag) {
        case 'm':
            op->layer.cs_area.units = "mm^2";
            op->layer.cs_area.val = CONV_MIL2_TO_MM2(op->layer.cs_area.val);
            op->layer.trace_width.units = "mm";
            op->layer.trace_width.val = CONV_MIL_TO_MM(op->layer.trace_width.val);
            op->layer.corr_cs_area.units = "mm^2";
            op->layer.corr_cs_area.val = CONV_MIL2_TO_MM2(op->layer.corr_cs_area.val);
            op->layer.corr_trace_width.units = "mm";
            op->layer.corr_trace_width.val = CONV_MIL_TO_MM(op->layer.corr_trace_width.val);
            break;
        case 'i':
            /* This case is the default so just let it go to the default branch*/
        default:
            op->layer.cs_area.units = "mil^2";
            op->layer.trace_width.units = "mil";
            op->layer.corr_cs_area.units = "mil^2";
            op->layer.corr_trace_width.units = "mil";
    }

}

void set_def_IPC2221(ip_t* ip) {
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
}

void set_deft_IPC2152_A(ip_t* ip) {
    ip->current.outval = 0;      
    ip->current.val = 0;      
    ip->current.units = "A";      

    ip->copper_weight.outval = 0;      
    ip->copper_weight.val = 0;      
    ip->copper_weight.units = "oz/ft^2";      

    ip->temperature_rise.outval = 10;      
    ip->temperature_rise.val = 10;      
    ip->temperature_rise.units = "C";      

    ip->trace_length.outval = 0;           
    ip->trace_length.val = 0;           
    ip->trace_length.units = "cm";           

    ip->plane_area.outval = 0;
    ip->plane_area.val = 0;
    ip->plane_area.units = "in^2"; 

    ip->plane_distance.outval = 0;
    ip->plane_distance.val = 0;
    ip->plane_distance.units = "mil"; 

    ip->pcb_thickness.outval = 62;
    ip->pcb_thickness.val = 62;
    ip->pcb_thickness.units = "mil"; 

    /* Set modifier defaults */
    /* ip->cf.copper_weight is not defaulted to 1 */
    ip->cf.temperature_rise = 1;
    ip->cf.plane_area = 1;
    ip->cf.pcb_thickness = 1;
    ip->cf.plane_distance = 1;
}

void set_deft_IPC2152_B(ip_t* ip) {
    /* Set input value defaults */
    ip->current.outval = 0;      
    ip->current.val = 0;      
    ip->current.units = "A";      

    ip->copper_weight.outval = 0;      
    ip->copper_weight.val = 0;      
    ip->copper_weight.units = "oz/ft^2";      

    ip->temperature_rise.outval = 10;      
    ip->temperature_rise.val = 10;      
    ip->temperature_rise.units = "C";      

    ip->trace_length.outval = 0;           
    ip->trace_length.val = 0;           
    ip->trace_length.units = "cm";           

    ip->plane_distance.outval = 0;
    ip->plane_distance.val = 0;
    ip->plane_distance.units = "mil"; 

    ip->pcb_thermal_cond.outval = 0.20;
    ip->pcb_thermal_cond.val = CONV_WmK_TO_BTUhftF(0.20);
    ip->pcb_thermal_cond.units = "W/mK"; 

    ip->pcb_thickness.outval = 62;
    ip->pcb_thickness.val = 62;
    ip->pcb_thickness.units = "mil"; 

    /* Set modifier defaults */
    /* ip->cf.copper_weight is not defaulted to 1 */
    ip->cf.pcb_thickness = 1;
    ip->cf.plane_distance = 1;
    ip->cf.pcb_thermal_cond = 1;
}

void sel_functions(ip_t* ip) {
    switch (ip->standard.num) {
        case IPC2221:
            ip->method = 'A'; /* Default to A since only one method for IPC2221 */
            switch (ip->method) {
                case 'A':
                    ip->deft = &set_def_IPC2221;
                    ip->proc = &calcs_IPC2221; 
                    ip->outu = &set_outu_IPC2221; 
                    ip->outp = &output_results_IPC2221; 
                    break;
                default:
                    fprintf(stderr, "Method %c for the IPC-%d doesn't exist.", ip->method, ip->standard.num);
                    exit(EXIT_FAILURE);
            }
            return;
        case IPC2152:
            switch (ip->method) {
                case 'A':
                    ip->deft = &set_deft_IPC2152_A;
                    ip->proc = &calcs_IPC2152_A; 
                    ip->outu = &set_outu_IPC2152; 
                    ip->outp = &output_results_IPC2152_A; 
                    break;
                case 'B':
                    ip->deft = &set_deft_IPC2152_B;
                    ip->proc = &calcs_IPC2152_B; 
                    ip->outu = &set_outu_IPC2152; 
                    ip->outp = &output_results_IPC2152_B; 
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

    /* If given a . use the current directory for the output */
    if (*optarg == '.') {
        autogen_file_name(ofile->fname);
        /* If given a path with no name, autogenerate the name at that path */
    } else if (optarg[strlen(optarg) - 1] == '/') {
        ofile->path = malloc(sizeof(char) * PATH_MAX); // PATH_MAX = 260
        sscanf(optarg, "%s", ofile->path);
        autogen_file_name(ofile->fname);
        /* Last case is a file name */ 
    } else {
        sscanf(optarg, "%s", ofile->fname);
    }
    ofile->dest = malloc(sizeof(char) * (strlen(ofile->path) + strlen(ofile->fname)));
    sprintf(ofile->dest, "%s%s", ofile->path, ofile->fname);
}

void calcs_IPC2221(ip_t* ip, op_t* op) {
    op->extl.cs_area.val = pow(ip->current.val/k_EXT * pow(ip->temperature_rise.val, 0.44), 1/0.725); 
    calc_rvp(ip, &op->extl);
    op->intl.cs_area.val = pow(ip->current.val/k_INT * pow(ip->temperature_rise.val, 0.44), 1/0.725); 
    calc_rvp(ip, &op->intl);
}

void calcs_IPC2152_A(ip_t* ip, op_t* op) {
    /* Different one on the website, and different one in the website code */
    op->layer.cs_area.val = (110.515 * pow(ip->temperature_rise.val, -0.871) + 0.803) * pow(ip->current.val, 0.868 * pow(ip->temperature_rise.val, -0.102) + 1.129);    
    /* op->layer.cs_area = (117.555 * pow(ip->temperature_rise.val, -0.913) + 1.15) * pow(ip->current.val, 0.84 * pow(ip->temperature_rise.val, -0.018) + 1.159);  */

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

    /* Plane cs_area correction factor */
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

    /* Calculate the corrected cs_area and trace width  */
    op->layer.corr_cs_area.val = (110.515 * pow(ip->cf.temperature_rise, -0.871) + 0.803) * pow(ip->current.val, 0.868 * pow(ip->cf.temperature_rise, -0.102) + 1.129);    
    op->layer.corr_trace_width.val = calc_width_mils(ip, &op->layer.corr_cs_area.val);

    calc_rvp(ip, &op->layer);
}

void calcs_IPC2152_B(ip_t* ip, op_t* op) {
    op->layer.cs_area.val = pow(ip->current.val/(0.089710902134 * pow(ip->temperature_rise.val, 0.39379253898)), 1/(0.50382053698 * pow(ip->temperature_rise.val, 0.038495772461)));

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

    /* Corrected cs_area */
    op->layer.corr_cs_area.val = op->layer.cs_area.val * ip->cf.copper_weight * ip->cf.pcb_thickness * ip->cf.plane_distance * ip->cf.pcb_thermal_cond;

    /* Corrected Trace Width */
    op->layer.corr_trace_width.val = op->layer.corr_cs_area.val / ip->copper_weight.val;

    calc_rvp(ip, &op->layer);
}

void calc_rvp(ip_t* ip, layer_t* layer) {
    layer->trace_width.val = calc_width_mils(ip, &layer->cs_area.val);
    if (ip->trace_length.val > 0) {
        layer->resistance.val  = calc_resistance(ip, &layer->cs_area.val); 
    }
    layer->voltage_drop.val = calc_vdrop(ip, &layer->resistance.val); 
    layer->power_loss.val = calc_power_loss(ip, &layer->voltage_drop.val);
}

double calc_width_mils(ip_t* ip, double* cs_area) {
    return *cs_area/CONV_OZFT2_TO_MIL(ip->copper_weight.val);
}

double calc_resistance(ip_t* ip, double* cs_area) {
    return ((ip->trace_length.val * ip->resistivity.val)/(CONV_MIL2_TO_CM2(*cs_area))) * (1 + (ip->a.val * ((ip->temperature_rise.val + ip->temperature_ambient.val) - ip->temperature_ambient.val)));
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

void output_results_IPC2221(ip_t* ip, op_t* op, FILE * file) {
    fprintf(file,   
            "\n"
            "Current:\t\t%lf\t[%s]\n"
            "Copper Weight:\t\t%lf\t[%s]\n"
            "Temperature, Rise:\t%lf\t[%s]\n"
            "Temperature, Ambient:\t%lf\t[%s]\n"
            "Trace Length:\t\t%lf\t[%s]\n",
            ip->current.outval, ip->current.units, ip->copper_weight.outval, ip->copper_weight.units, ip->temperature_rise.outval, ip->temperature_rise.units, ip->temperature_ambient.val, ip->temperature_ambient.units, ip->trace_length.outval, ip->trace_length.units);

    fprintf(file,   
            "\n\n"
            "        External Layers\n"
            "Area: \t\t\t%lf\t[mil^2]\n"
            "Width:\t\t\t%lf\t[mil]\n"
            ,op->extl.cs_area.val, op->extl.trace_width.val);

    fprintf(file,
            ip->trace_length.val > 0 ? 
            "Resistance:\t\t%lf\t[Ohm]\n"
            "Voltage Drop:\t\t%lf\t[V]\n"
            "Power Loss:\t\t%lf\t[W]\n" : "\r",
            op->extl.resistance.val, op->extl.voltage_drop.val, op->extl.power_loss.val);

    fprintf(file,"\n\n");

    fprintf(file,   
            "        Internal Layers\n"
            "Area: \t\t\t%lf\t[mil^2]\n"
            "Width:\t\t\t%lf\t[mil]\n"
            ,op->intl.cs_area.val, op->intl.trace_width.val);

    fprintf(file,
            ip->trace_length.val > 0 ? 
            "Resistance:\t\t%lf\t[Ohm]\n"
            "Voltage Drop:\t\t%lf\t[V]\n"
            "Power Loss:\t\t%lf\t[W]\n" : "\r",
            op->intl.resistance.val, op->intl.voltage_drop.val, op->intl.power_loss.val);

    fprintf(file,"\n");

    fprintf(file,
            "\n- Values of k used in calculating the cs_area:\n\n"
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

void output_results_IPC2152_A(ip_t* ip, op_t *op, FILE *file) {
    fprintf(file,
            "\n"
            "Current:\t\t%lf\t[%s]\n"
            "Copper Weight:\t\t%lf\t[%s]\n"
            "Temperature, Rise:\t%lf\t[%s]\n"
            "Trace Length:\t\t%lf\t[%s]\n"
            "PCB Thickness:\t\t%lf\t[%s]\n"
            "Plane Distance:\t\t%lf\t[%s]\n"
            "Plane Area:\t\t%lf\t[%s]\n", 
            ip->current.outval, ip->current.units, ip->copper_weight.outval, ip->copper_weight.units, ip->temperature_rise.outval, ip->temperature_rise.units, ip->trace_length.outval, ip->trace_length.units, ip->pcb_thickness.outval, ip->pcb_thickness.units, ip->plane_distance.outval, ip->plane_distance.units, ip->plane_area.outval, ip->plane_area.units);

    fprintf(file,
            "\n"
            "Area: \t\t\t%lf\t[%s]\n"
            "Corrected Area: \t%lf\t[%s]\n"
            "Width:\t\t\t%lf\t[%s]\n"
            "Corrected Width: \t%lf\t[%s]\n",
            op->layer.cs_area.val, op->layer.cs_area.units, op->layer.corr_cs_area.val, op->layer.corr_cs_area.units, op->layer.trace_width.val, op->layer.trace_width.units, op->layer.corr_trace_width.val, op->layer.corr_trace_width.units);

    fprintf(file,
            ip->trace_length.val > 0 ? 
            "Resistance:\t\t%lf\t[Ohm]\n"
            "Voltage Drop:\t\t%lf\t[V]\n"
            "Power Loss:\t\t%lf\t[W]\n" : "\r",
            op->layer.resistance.val, op->layer.voltage_drop.val, op->layer.power_loss.val);

    fprintf(file, 
            "\n"
            "Copper Weight CF:\t%lf\t[units]\n"
            "PCB Thickness CF:\t%lf\t[units]\n"
            "Plane Area CF:\t\t%lf\t[units]\n" 
            "Plane Distance CF:\t%lf\t[units]\n"
            "Temperature Rise CF:\t%lf\t[units]\n",
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

void output_results_IPC2152_B(ip_t* ip, op_t *op, FILE *file) {
    fprintf(file,
            "\n"
            "Current:\t\t%lf\t[%s]\n"
            "Copper Weight:\t\t%lf\t[%s]\n"
            "Temperature, Rise:\t%lf\t[%s]\n"
            "Trace Length:\t\t%lf\t[%s]\n"
            "PCB Thickness:\t\t%lf\t[%s]\n"
            "Plane Distance:\t\t%lf\t[%s]\n"
            "PCB Thermal Cond.:\t%lf\t[%s]\n",
            ip->current.outval, ip->current.units, ip->copper_weight.outval, ip->copper_weight.units, ip->temperature_rise.outval, ip->temperature_rise.units, ip->trace_length.outval, ip->trace_length.units, ip->pcb_thickness.outval, ip->pcb_thickness.units, ip->plane_distance.outval, ip->plane_distance.units, ip->pcb_thermal_cond.outval, ip->pcb_thermal_cond.units);

    fprintf(file,
            "\n"
            "Area: \t\t\t%lf\t[%s]\n"
            "Corrected Area: \t%lf\t[%s]\n"
            "Width:\t\t\t%lf\t[%s]\n"
            "Corrected Width: \t%lf\t[%s]\n",
            op->layer.cs_area.val, op->layer.cs_area.units, op->layer.corr_cs_area.val, op->layer.corr_cs_area.units, op->layer.trace_width.val, op->layer.trace_width.units, op->layer.corr_trace_width.val, op->layer.corr_trace_width.units);

    fprintf(file,
            ip->trace_length.val > 0 ? 
            "Resistance:\t\t%lf\t[Ohm]\n"
            "Voltage Drop:\t\t%lf\t[V]\n"
            "Power Loss:\t\t%lf\t[W]\n" : "\r",
            op->layer.resistance.val, op->layer.voltage_drop.val, op->layer.power_loss.val);

    fprintf(file, 
            "\n"
            "Copper Weight CF:\t%lf\t[units]\n"
            "PCB Thickness CF:\t%lf\t[units]\n"
            "PCB Thermal Cond. CF:\t%lf\t[units]\n"
            "Plane Distance CF:\t%lf\t[units]\n",
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

// TODO: Create help for each option?
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
            "\n\t-p, \t--plane-cs_area <Plane Area [in^2]>\t\t= Input the plane cs_area in inches squared.\n"
            "\t\t--plane-cs_area-cm2\n"
            "\n\t-d, \t--plane-distance <Plane Distance [mil]>\t\t= Input the plane distance in mil.\n"
            "\t\t--plane-distance-mm\n"
            "\n\t--resistivity <Resistivity [Ohmm]>\t\t\t= Input the resistivity in Ohm meters.\n"
            "\n\t--temperature-coefficient <Temp. Coefficient [1/C]>\t= Input the temperature coefficient.\n"
            "\n\t-o <File Name>\t\t\t= Write the name of the outputted file. Use '.txt' to create a text file. Use a single '.' to auto-generate the name based on date/time. Can also write the full path to the file, e.g. 'C:/Users/user/output.txt' or stop at 'C:/Users/user/' to use the auto-generated file name.\n"
            "\n\t-m, \t--metric\t\t\t\t\t= Make the output units be metric.\n"
            "\n\t-i, \t--imperial\t\t\t\t\t= Make the output units be imperial. Default behaviour, therefore just implemented for completion.\n"
            "\n\n\t\t\tCONVERSIONS\n"
            "\n\t--convert-mil2-cm2\t= From mils sq. to cm sq."
            "\n\t--convert-mil2-mm2\t= From mils sq. to mm sq."
            "\n\t--convert-cm2-in2\t= From cm sq. to inches sq."
            "\n\t--convert-mil-ozft2\t= From mils to ounce per foot sq."
            "\n\t--convert-mm-ozft2\t= From milimeters to ounce per foot sq."
            "\n\t--convert-um-ozft2\t= From micrometers to ounce per foot sq."
            "\n\t--convert-ozft2-mil\t= From ounce per foot sq. to mils."
            "\n\t--convert-ozft2-mm\t= From ounce per foot sq. to milimeters."
            "\n\t--convert-ozft2-um\t= From ounce per foot sq. to micrometers."
            "\n\t--convert-mm-mil\t= From milimeters to mils."
            "\n\t--convert-mil-mm\t= From mils to milimeters."
            "\n\t--convert-F-C\t\t= From Fahrenheit to Celsius."
            "\n\t--convert-C-F\t\t= From Celsius to Fahrenheit."
            "\n\t--convert-WmK-BTUhftF\t= From Watts per mili Kelvin to BTU/h*ft*F."
            "\n\t--convert-BTUhftF-WmK\t= From BTU/h*ft*F to Watts per mili Kelvin."
            "\n\n");
}

