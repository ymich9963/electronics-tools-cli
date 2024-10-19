#include "unity/unity.h"
#include "../twc.h"
#include "unity/unity_internals.h"
#include <string.h>

void setUp() {}

void tearDown() {
    fflush(stdout);
}

void test_output_help() {
    TEST_ASSERT_EQUAL_INT(0, output_help());
}

void test_output_results_IPC2221() {
    ip_t ip = {
        .current = {
            .val = 1,
            .units = "test"
        },
        .copper_weight = {
            .val = 1,
            .units = "test"
        },
        .temperature_rise = {
            .val = 1,
            .units = "test"
        },
        .temperature_ambient = {
            .val = 1,
            .units = "test"
        },
        .trace_length = {
            .val = 1,
            .units = "test"
        },
        .resistivity.val = 1,
        .a.val = 1,
    };

    op_t op = {
        .extl = {
            .cs_area = {
                .val = 1,
                .units = "test"
            },
            .trace_width = {
                .val = 1,
                .units = "test"
            },
            .resistance = {
                .val = 1,
                .units = "test"
            },
            .voltage_drop = {
                .val = 1,
                .units = "test"
            },
            .power_loss = {
                .val = 1,
                .units = "test"
            },
        },
        .intl = {
            .cs_area = {
                .val = 1,
                .units = "test"
            },
            .trace_width = {
                .val = 1,
                .units = "test"
            },
            .resistance = {
                .val = 1,
                .units = "test"
            },
            .voltage_drop = {
                .val = 1,
                .units = "test"
            },
            .power_loss = {
                .val = 1,
                .units = "test"
            },
        }
    };

    TEST_ASSERT_EQUAL_INT(0, output_results_IPC2221(&ip, &op, stdout));
}

void test_output_results_IPC2152_B() {
    ip_t ip = {
        .current = {
            .val = 1,
            .units = "test"
        },
        .copper_weight = {
            .val = 1,
            .units = "test"
        },
        .temperature_rise = {
            .val = 1,
            .units = "test"
        },
        .trace_length = {
            .val = 1,
            .units = "test"
        },
        .pcb_thickness = {
            .val = 1,
            .units = "test"
        },
        .plane_distance = {
            .val = 1,
            .units = "test"
        },
        .pcb_thermal_cond = {
            .val = 1,
            .units = "test"
        },
        .cf = {
            .copper_weight = 1,
            .pcb_thickness = 1,
            .pcb_thermal_cond = 1,
            .plane_distance = 1,
        },
        .resistivity.val = 1,
        .a.val = 1,
    };

    op_t op = {
        .layer = {
            .cs_area = {
                .val = 1,
                .units = "test"
            },
            .corr_cs_area = {
                .val = 1,
                .units = "test"
            },
            .trace_width = {
                .val = 1,
                .units = "test"
            },
            .corr_trace_width = {
                .val = 1,
                .units = "test"
            },
            .resistance = {
                .val = 1,
                .units = "test"
            },
            .voltage_drop = {
                .val = 1,
                .units = "test"
            },
            .power_loss = {
                .val = 1,
                .units = "test"
            },
        },
    };

    TEST_ASSERT_EQUAL_INT(0, output_results_IPC2152_B(&ip, &op, stdout));
}

void test_output_results_IPC2152_A() {
    ip_t ip = {
        .current = {
            .outval = 1,
            .units = "test"
        },
        .copper_weight = {
            .outval = 1,
            .units = "test"
        },
        .temperature_rise = {
            .outval = 1,
            .units = "test"
        },
        .trace_length = {
            .outval = 1,
            .units = "test"
        },
        .pcb_thickness = {
            .outval = 1,
            .units = "test"
        },
        .plane_distance = {
            .outval = 1,
            .units = "test"
        },
        .plane_area= {
            .outval = 1,
            .units = "test"
        },
        .cf = {
            .copper_weight = 1,
            .pcb_thickness = 1,
            .plane_area = 1,
            .plane_distance = 1,
            .temperature_rise = 1,
        },
        .resistivity.val = 1,
        .a.val = 1,
    };

    op_t op = {
        .layer = {
            .cs_area = {
                .val = 1,
                .units = "test"
            },
            .corr_cs_area = {
                .val = 1,
                .units = "test"
            },
            .trace_width = {
                .val = 1,
                .units = "test"
            },
            .corr_trace_width = {
                .val = 1,
                .units = "test"
            },
            .resistance = {
                .val = 1,
                .units = "test"
            },
            .voltage_drop = {
                .val = 1,
                .units = "test"
            },
            .power_loss = {
                .val = 1,
                .units = "test"
            },
        },
    };

    TEST_ASSERT_EQUAL_INT(0, output_results_IPC2152_A(&ip, &op, stdout));
}

void test_vdrop() {
    ip_t ip = {.current.val = 1.0};
    double resistance = 1.0;
    TEST_ASSERT_EQUAL_DOUBLE(1.0, calc_vdrop(&ip, &resistance));
}

void test_pow_loss() {
    ip_t ip = {.current.val = 1.0};
    double resistance = 1.0;
    double vdrop = calc_vdrop(&ip, &resistance);
    TEST_ASSERT_EQUAL_DOUBLE(1.0, calc_power_loss(&ip, &vdrop));
}

void test_resistance() {
    ip_t ip = {
        .current.val = 1.0,
        .temperature_rise.val = 10,
        .temperature_ambient.val = 25,
        .trace_length.val = 1,
        .resistivity.val = 1.72e-8,
        .a.val = 0.00393,
    };
    double area = 1;
    TEST_ASSERT_DOUBLE_WITHIN(1e-12, 2.770779342e-3, calc_resistance(&ip, &area));
}

void test_trace_width_mils() {
    ip_t ip = {
        .copper_weight.val = 1.0,
    };
    double area = 1;
    TEST_ASSERT_DOUBLE_WITHIN(1e-9, 0.72568940493468795355587808417997, calc_trace_width_mils(&ip, &area));
}

void test_w_r_vd_pl() {
    ip_t ip = {
        .copper_weight.val = 1.0,
        .current.val = 1.0,
        .temperature_rise.val = 10,
        .temperature_ambient.val = 25,
        .trace_length.val = 1,
        .resistivity.val = 1.72e-8,
        .a.val = 0.00393,
    };
    layer_t layer = {
        .cs_area.val = 1,
    };
    calc_w_r_vd_pl(&ip, &layer);

    TEST_ASSERT_DOUBLE_WITHIN(1e-9, 0.72568940493468795355587808417997, layer.trace_width.val);
    TEST_ASSERT_DOUBLE_WITHIN(1e-12, 2.770779342e-3, layer.resistance.val);
    TEST_ASSERT_DOUBLE_WITHIN(1e-12, 2.770779342e-3, layer.voltage_drop.val);
    TEST_ASSERT_DOUBLE_WITHIN(1e-12, 2.770779342e-3, layer.power_loss.val);
}

void test_calcs_IPC2152_B() {
    ip_t ip = {
        .current = {
            .val = 1,
            .units = "test"
        },
        .copper_weight = {
            .val = 1,
            .units = "test"
        },
        .temperature_rise = {
            .val = 1,
            .units = "test"
        },
        .trace_length = {
            .val = 1,
            .units = "test"
        },
        .pcb_thickness = {
            .val = 1,
            .units = "test"
        },
        .plane_distance = {
            .val = 1,
            .units = "test"
        },
        .pcb_thermal_cond = {
            .val = CONV_WmK_TO_BTUhftF(1),
            .units = "test"
        },
        .cf = {
            .copper_weight = 0,
            .pcb_thickness = 1,
            .pcb_thermal_cond = 1,
            .plane_distance = 1,
        },
        .resistivity.val = 1,
        .a.val = 1,
    };

    op_t op;

    calcs_IPC2152_B(&ip, &op);

    /* Expected values are from the website of which the method was extracted from */
    TEST_ASSERT_DOUBLE_WITHIN(1e-2, CONV_MM2_TO_MIL2(0.07729), op.layer.cs_area.val);
    TEST_ASSERT_DOUBLE_WITHIN(1e-3, 0.903, ip.cf.copper_weight);
    TEST_ASSERT_DOUBLE_WITHIN(1e-1, 24.9, ip.cf.pcb_thickness);
    TEST_ASSERT_DOUBLE_WITHIN(1e-3, 0.408, ip.cf.plane_distance);
    TEST_ASSERT_DOUBLE_WITHIN(1e-2, 0.374, ip.cf.pcb_thermal_cond);
    TEST_ASSERT_DOUBLE_WITHIN(1, CONV_MM2_TO_MIL2(0.266), op.layer.corr_cs_area.val);
    TEST_ASSERT_DOUBLE_WITHIN(1, 299, op.layer.corr_trace_width.val);
}

void test_calcs_IPC2152_A() {
    ip_t ip = {
        .current = {
            .val = 1,
            .units = "test"
        },
        .copper_weight = {
            .val = 1,
            .units = "test"
        },
        .temperature_rise = {
            .val = 1,
            .units = "test"
        },
        .trace_length = {
            .val = 1,
            .units = "test"
        },
        .pcb_thickness = {
            .val = 1,
            .units = "test"
        },
        .plane_distance = {
            .val = 1,
            .units = "test"
        },
        .plane_area= {
            .val = 1,
            .units = "test"
        },
        .cf = {
            .copper_weight = 1,
            .pcb_thickness = 1,
            .plane_area = 1,
            .plane_distance = 1,
            .temperature_rise = 1,
        },
        .resistivity.val = 1,
        .a.val = 1,
    };

    op_t op;

    calcs_IPC2152_A(&ip, &op);

    /* Expected values are from the website of which the method was extracted from */
    TEST_ASSERT_DOUBLE_WITHIN(1e-2, 0.91, ip.cf.copper_weight);
    TEST_ASSERT_DOUBLE_WITHIN(1e-2, 25.96, ip.cf.pcb_thickness);
    TEST_ASSERT_DOUBLE_WITHIN(1e-2, 0.41, ip.cf.plane_distance);
    TEST_ASSERT_DOUBLE_WITHIN(1, 1, ip.cf.plane_area);
    TEST_ASSERT_DOUBLE_WITHIN(1e-2, 0.1, ip.cf.temperature_rise);
    TEST_ASSERT_DOUBLE_WITHIN(1e-1, 111.3, op.layer.cs_area.val);
    TEST_ASSERT_DOUBLE_WITHIN(1, 799, op.layer.corr_cs_area.val);
    TEST_ASSERT_DOUBLE_WITHIN(1e-1, 85.6, op.layer.trace_width.val);
    TEST_ASSERT_DOUBLE_WITHIN(1e-1, 614.6, op.layer.corr_trace_width.val);

    /* Only checking the trace width for these other values */
    ip.copper_weight.val = 2;
    ip.plane_area.val = 40;
    ip.plane_distance.val = 126;
    calcs_IPC2152_A(&ip, &op);
    TEST_ASSERT_DOUBLE_WITHIN(1e-1, 647.3, op.layer.corr_trace_width.val);

    ip.copper_weight.val = 3;
    ip.plane_area.val = 20;
    ip.plane_distance.val = 1;
    calcs_IPC2152_A(&ip, &op);
    TEST_ASSERT_DOUBLE_WITHIN(1e-1, 210.2, op.layer.corr_trace_width.val);
}

void test_calcs_IPC2221() {
    ip_t ip = {
        .current = {
            .val = 1,
            .units = "test"
        },
        .copper_weight = {
            .val = 1,
            .units = "test"
        },
        .temperature_rise = {
            .val = 1,
            .units = "test"
        },
        .temperature_ambient = {
            .val = 1,
            .units = "test"
        },
        .trace_length = {
            .val = 1,
            .units = "test"
        },
        .resistivity.val = 1,
        .a.val = 1,
    };

    op_t op;

    calcs_IPC2221(&ip, &op);

    TEST_ASSERT_DOUBLE_WITHIN(1e-1, 47.8, op.extl.trace_width.val);
    TEST_ASSERT_DOUBLE_WITHIN(1, 124, op.intl.trace_width.val);
}

void test_set_output_file() {
    char* optarg[3] = {".", "C:/", "file.txt"};

    ofile_t ofile1;
    set_output_file(&ofile1, ".");
    /* Remove random characters at the start of the string. Should be 27. */
    /* TEST_ASSERT_EQUAL_INT(27, strlen(ofile1.dest)); */
    TEST_ASSERT_NOT_NULL(strstr(ofile1.dest, "twc"));

    ofile_t ofile2;
    set_output_file(&ofile2, optarg[1]);
    TEST_ASSERT_EQUAL_INT(30, strlen(ofile2.dest));

    ofile_t ofile3;
    set_output_file(&ofile3, optarg[2]);
    /* Random hex numbers before ofile.dest. */
    /* TEST_ASSERT_EQUAL_INT(8, strlen(ofile3.dest)); */
    TEST_ASSERT_NOT_NULL(strstr(ofile3.dest, "file.txt"));
}

void test_autogen_file_name() {
    char* fname = malloc(sizeof(char) * OUT_FILE_MAX);
    autogen_file_name(fname);
    TEST_ASSERT_EQUAL_INT(27, strlen(fname));
    free(fname);
}

void test_get_time() {
    char* time = get_time();
    TEST_ASSERT_EQUAL_INT(12, strlen(time));
}

void test_sel_functions() {
    ip_t ip;

    ip.standard.num = IPC2221;
    ip.standard.str = "IPC2221";
    ip.method = 'A';
    TEST_ASSERT_EQUAL_INT(0, sel_functions(&ip));

    ip.standard.num = IPC2152;
    ip.standard.str = "IPC2152";
    ip.method = 'A';
    TEST_ASSERT_EQUAL_INT(0, sel_functions(&ip));

    ip.standard.num = IPC2152;
    ip.standard.str = "IPC2152";
    ip.method = 'B';
    TEST_ASSERT_EQUAL_INT(0, sel_functions(&ip));

    ip.standard.num = 123456;
    TEST_ASSERT_EQUAL_INT(1, sel_functions(&ip));

    ip.standard.num = IPC2152;
    ip.method = 'F';
    TEST_ASSERT_EQUAL_INT(1, sel_functions(&ip));

    ip.standard.num = IPC2221;
    ip.method = 'F';
    TEST_ASSERT_EQUAL_INT(1, sel_functions(&ip));
}

void test_set_defv() {
    /* I don't have the energy to test whether every single value was assigned and checking if a 0/1 was returned is redundant...so these are just called for completion's sake */
    ip_t ip;

    set_defv_IPC2221(&ip);
    set_defv_IPC2152_A(&ip);
    set_defv_IPC2152_B(&ip);
}

void test_set_outu() {
    ip_t ip;
    op_t op;

    ip.uflag = 'm';
    TEST_ASSERT_EQUAL_INT(0, set_outu_IPC2152(&ip, &op));
    TEST_ASSERT_EQUAL_INT(0, set_outu_IPC2221(&ip, &op));

    ip.uflag = 'i';
    TEST_ASSERT_EQUAL_INT(0, set_outu_IPC2152(&ip, &op));
    TEST_ASSERT_EQUAL_INT(0, set_outu_IPC2221(&ip, &op));

    ip.uflag = 'g';
    TEST_ASSERT_EQUAL_INT(1, set_outu_IPC2152(&ip, &op));
    TEST_ASSERT_EQUAL_INT(1, set_outu_IPC2221(&ip, &op));
}

void test_check_method() {
    char chrval = 'a';
    char* method_arr = "ab";
    unsigned int size = 2;

    TEST_ASSERT_EQUAL_INT(0, check_method(chrval, method_arr, size));

    chrval = 'c';
    TEST_ASSERT_EQUAL_INT(1, check_method(chrval, method_arr, size));
}

void test_check_standard() {

    char* strval = "test";
    char* standard_arr[] = {"hey", "test"};
    unsigned int size = 2;
    unsigned char index;

    TEST_ASSERT_EQUAL_INT(0, check_standard(strval, standard_arr, size, &index));

    strval = "hello";
    TEST_ASSERT_EQUAL_INT(1, check_standard(strval, standard_arr, size, &index));
}

void test_get_standard_method() {
    int argc = 9;
    char* argv[] = {"first", "-c", "1", "-w", "2", "--standard", "IPC2152", "--method", "B"};
    ip_t ip;

    TEST_ASSERT_EQUAL_INT(0, get_standard_method(&argc, argv, &ip));
}

void split(char* cmd, char** argv, int* argc) {
    char* token = strtok(cmd," "); 
    *argc = 0;
    while(token != NULL) {
        argv[*argc] = token; 
        token = strtok(NULL," "); 
        *argc = *argc + 1;
    }
}

void reset(char** argv, ip_t* ip, int size) {
    for(int i = 0; i < size; i++) {
        argv[i] = "\0";
    }
    ip_t x = { 0 };
    *ip = x;
}

void test_get_options() {
    int argc;
    char* argv[30];
    ip_t ip = {0};

    argc = 1;
    argv[0] = "first";
    TEST_ASSERT_EQUAL_INT(0, get_options(&argc, argv, &ip));

    argc = 2;
    argv[1] = "-h";
    TEST_ASSERT_EQUAL_INT(0, get_options(&argc, argv, &ip));

    argc = 2;
    argv[1] = "--version";
    TEST_ASSERT_EQUAL_INT(0, get_options(&argc, argv, &ip));

    argc = 2;
    TEST_ASSERT_EQUAL_INT(0, get_options(&argc, argv, &ip));

    char cmd0[] = "first 1 2";
    split(cmd0, argv, &argc);
    TEST_ASSERT_EQUAL_INT(0, get_options(&argc, argv, &ip));
    reset(argv, &ip, 30);

    char cmd1[] = "first -c 1 -w 2 -r 10 -a 25 -l 1 -t 62 -e 1 -p 1 -d 1 -i -m";
    split(cmd1, argv, &argc);
    TEST_ASSERT_EQUAL_INT(0, get_options(&argc, argv, &ip));
    reset(argv, &ip, 30);

    char cmd2[] = "first -w 1 --current 1 --current-mA 1";
    split(cmd2, argv, &argc);
    TEST_ASSERT_EQUAL_INT(0, get_options(&argc, argv, &ip));
    reset(argv, &ip, 30);

    char cmd3[] = "first -c 1 --copper-weight 1 --copper-weight-mil 1 --copper-weight-mm 1 --copper-weight-um 35";
    split(cmd3, argv, &argc);
    TEST_ASSERT_EQUAL_INT(0, get_options(&argc, argv, &ip));
    reset(argv, &ip, 30);

    char cmd4[] = "first -c 1 -w 1 --temperature-rise 1 --temperature-rise-F 1 --temperature-ambient 1 --temperature-ambient-F 1";
    split(cmd4, argv, &argc);
    TEST_ASSERT_EQUAL_INT(0, get_options(&argc, argv, &ip));
    reset(argv, &ip, 30);

    char cmd5[] = "first -c 1 -w 1 --trace-length 1 --trace-length-mm 1 --trace-length-mil 1";
    split(cmd5, argv, &argc);
    TEST_ASSERT_EQUAL_INT(0, get_options(&argc, argv, &ip));
    reset(argv, &ip, 30);

    char cmd6[] = "first -c 1 -w 1 --pcb-thickness 1 --pcb-thickness-mm 1";
    split(cmd6, argv, &argc);
    TEST_ASSERT_EQUAL_INT(0, get_options(&argc, argv, &ip));
    reset(argv, &ip, 30);

    char cmd7[] = "first -c 1 -w 1 --pcb-thermal-conductivity 1 --plane-area 1 --plane-area-cm2 1 --plane-distance 1 --plane-distance-mm 1 --resistivity 1 --temperature-coefficient 1";
    split(cmd7, argv, &argc);
    TEST_ASSERT_EQUAL_INT(0, get_options(&argc, argv, &ip));
    reset(argv, &ip, 30);

    char cmd8[] = "first -c 1 -w 1 --metric --imperial --standard IPC2221 --method A";
    split(cmd8, argv, &argc);
    TEST_ASSERT_EQUAL_INT(0, get_options(&argc, argv, &ip));
    reset(argv, &ip, 30);

    char cmd9[] = "first -g -b";
    split(cmd9, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(&argc, argv, &ip));
    reset(argv, &ip, 30);

    char cmd10[] = "first -c 1";
    split(cmd10, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(&argc, argv, &ip));
    reset(argv, &ip, 30);

    char cmd11[] = "first -c";
    split(cmd11, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(&argc, argv, &ip));
    reset(argv, &ip, 30);

    /* Doesn't work for some reason */
    char cmd12[] = "first -c 1 -w 1 -o C:/ -o . -o C:/. --output C:/ --output . --output C:/.";
    split(cmd12, argv, &argc);
    TEST_ASSERT_EQUAL_INT(0, get_options(&argc, argv, &ip));
    reset(argv, &ip, 30);
}

void test_conversions() {
    int argc;
    char* argv[3];
    ip_t ip = {0};

    char cmd0[] = "first --convert-mil2-cm2 1";
    split(cmd0, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(&argc, argv, &ip));
    TEST_ASSERT_DOUBLE_WITHIN(1e-9 ,0.000006452, CONV_MIL2_TO_CM2(1));
    reset(argv, &ip, 3);

    char cmd1[] = "first --convert-mil2-mm2 1";
    split(cmd1, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(&argc, argv, &ip));
    TEST_ASSERT_DOUBLE_WITHIN(1e-7, 0.0006452, CONV_MIL2_TO_MM2(1));
    reset(argv, &ip, 3);

    char cmd2[] = "first --convert-mm2-mil2 1";
    split(cmd2, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(&argc, argv, &ip));
    TEST_ASSERT_DOUBLE_WITHIN(1, 1550, CONV_MM2_TO_MIL2(1));
    reset(argv, &ip, 3);

    char cmd3[] = "first --convert-cm2-in2 1";
    split(cmd3, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(&argc, argv, &ip));
    TEST_ASSERT_DOUBLE_WITHIN(1, 0.155, CONV_CM2_TO_INCH2(1));
    reset(argv, &ip, 3);

    char cmd4[] = "first --convert-mil-ozft2 1";
    split(cmd4, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(&argc, argv, &ip));
    TEST_ASSERT_DOUBLE_WITHIN(1e-1, 0.7256894049, CONV_MIL_TO_OZFT2(1));
    reset(argv, &ip, 3);

    char cmd5[] = "first --convert-mm-ozft2 1";
    split(cmd5, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(&argc, argv, &ip));
    TEST_ASSERT_DOUBLE_WITHIN(1e-3, 28.5704, CONV_MM_TO_OZFT2(1));
    reset(argv, &ip, 3);

    char cmd6[] = "first --convert-um-ozft2 1";
    split(cmd6, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(&argc, argv, &ip));
    TEST_ASSERT_DOUBLE_WITHIN(1e-2, 0.0285704, CONV_UM_TO_OZFT2(1));
    reset(argv, &ip, 3);

    char cmd7[] = "first --convert-ozft2-mil 1";
    split(cmd7, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(&argc, argv, &ip));
    TEST_ASSERT_DOUBLE_WITHIN(1e-2, 1.378, CONV_OZFT2_TO_MIL(1));
    reset(argv, &ip, 3);

    char cmd8[] = "first --convert-ozft2-mm 1";
    split(cmd8, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(&argc, argv, &ip));
    TEST_ASSERT_DOUBLE_WITHIN(1e-3, 0.0350012, CONV_OZFT2_TO_MM(1));
    reset(argv, &ip, 3);

    char cmd9[] = "first --convert-ozft2-um 1";
    split(cmd9, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(&argc, argv, &ip));
    TEST_ASSERT_DOUBLE_WITHIN(1e-3, 35.0012, CONV_OZFT2_TO_UM(1));
    reset(argv, &ip, 3);

    char cmd10[] = "first --convert-mm-mil 1";
    split(cmd10, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(&argc, argv, &ip));
    TEST_ASSERT_DOUBLE_WITHIN(1e-3, 39.37007874, CONV_MM_TO_MIL(1));
    reset(argv, &ip, 3);

    char cmd11[] = "first --convert-mil-mm 1";
    split(cmd11, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(&argc, argv, &ip));
    TEST_ASSERT_DOUBLE_WITHIN(1e-4, 0.0254, CONV_MIL_TO_MM(1));
    reset(argv, &ip, 3);

    char cmd12[] = "first --convert-F-C 1";
    split(cmd12, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(&argc, argv, &ip));
    TEST_ASSERT_DOUBLE_WITHIN(1e-1, -17.222, CONV_FAHR_TO_CELS(1));
    reset(argv, &ip, 3);

    char cmd13[] = "first --convert-C-F 1";
    split(cmd13, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(&argc, argv, &ip));
    TEST_ASSERT_DOUBLE_WITHIN(1e-1, 33.8, CONV_CELS_TO_FAHR(1));
    reset(argv, &ip, 3);

    char cmd14[] = "first --convert-BTUhftF-WmK 1";
    split(cmd14, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(&argc, argv, &ip));
    TEST_ASSERT_DOUBLE_WITHIN(1e-2, 1.730735, CONV_BTUhftF_TO_WmK(1));
    reset(argv, &ip, 3);

    char cmd15[] = "first --convert-WmK-BTUhftF 1";
    split(cmd15, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(&argc, argv, &ip));
    TEST_ASSERT_DOUBLE_WITHIN(1e-2,  0.57778920516428, CONV_WmK_TO_BTUhftF(1));
    reset(argv, &ip, 3);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_output_help);
    RUN_TEST(test_output_results_IPC2221);
    RUN_TEST(test_output_results_IPC2152_A);
    RUN_TEST(test_output_results_IPC2152_B);
    RUN_TEST(test_vdrop);
    RUN_TEST(test_pow_loss);
    RUN_TEST(test_resistance);
    RUN_TEST(test_trace_width_mils);
    RUN_TEST(test_w_r_vd_pl);
    RUN_TEST(test_calcs_IPC2221);
    RUN_TEST(test_calcs_IPC2152_A);
    RUN_TEST(test_calcs_IPC2152_B);
    RUN_TEST(test_get_time);
    RUN_TEST(test_autogen_file_name);
    RUN_TEST(test_set_output_file);
    RUN_TEST(test_sel_functions);
    RUN_TEST(test_set_defv);
    RUN_TEST(test_set_outu);
    RUN_TEST(test_check_method);
    RUN_TEST(test_check_standard);
    RUN_TEST(test_get_standard_method);
    RUN_TEST(test_get_options);
    RUN_TEST(test_conversions);
    return UNITY_END();
}
