#include "twc.h"

int main(int argc, char** argv) {
    /* Inputs */
    ip_t ip;

    /* Outputs */
    op_t op;

    /* Output stream */
    FILE * file = stdout;

    /* Universal Defaults */
    ip.standard.num = IPC2152;
    ip.method = 'B';
    ip.uflag = 'i';
    ip.resistivity.val = 1.724e-6;
    ip.a.val = 3.93e-3;
    ip.ofile.oflag = false;        
    ip.ofile.path = "\0";

    /* Get the standard and the method for the calculations */
    get_standard_method(&argc, argv, &ip);

    /* Set functions based on the inputs */
    sel_functions(&ip);

    /* Set defaults */
    ip.deft(&ip);

    /* Get the inputs and options */
    get_options(&argc, argv, &ip);

    /* Calculate the values */
    ip.proc(&ip, &op);

    /* Set output units */
    ip.outu(&ip, &op);

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
