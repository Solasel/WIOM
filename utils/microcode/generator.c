/*
Rough outline:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Read from a file listing

instruction code | microcode

combinations.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Expand all "x" variations of the instruction codes, and compile
an array with structs containing:

long instr_code;
char *microcode;

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sort the array based on instr_code.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Write to the output file, with gaps of (INVALID) between valid instructions.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

/* Headers and constants. */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct string_pair {
	char *instr_signal;
	char *microcode;
};

struct signal_pair {
	long instr_code;
	char *microcode;
};

/* Error codes! */
enum errs {
	/* Debug codes. */
	NOT_YET_IMP = 1,
	/* Runtime error codes. */
	BAD_ARGS,
	FOPEN_FAILURE,
	MALLOC_FAILURE
};

/* Useful error messages. */
static char *err_msg(enum errs code)
{
	switch (code) {
		/* Debug codes. */
		case NOT_YET_IMP:
			return "Not yet implemented.";
		/* Runtime error codes. */
		case BAD_ARGS:
			return "Bad command-line arguments.";
		case FOPEN_FAILURE:
			return "Failed to open a file.";
		case MALLOC_FAILURE:
			return "Malloc failed.";
	}
	/* This is only here to appease the compiler.
	 * Note that since we're switching on an enum,
	 * we are enforced to handle all of the cases,
	 * so this is guaranteed to never be readhed.  */
	return NULL;
}

/* Function prototypes. */

/* Useful error messages. */
static char *err_msg(enum errs code);

/* Interprets the contents of src as microcode, and
 * stores the logisim-compatible microcode in dst. */
static int generate_microcode(char *src, char *dst);

/* Extracts information from src. */
static int read_src(char *src, char **fstring,
		int *is_len, int *mc_len, char **inv,
		int *num_x, struct string_pair **x_mc);

/* Generates the sorted final list of microcode. */
static int gen_fin_mc(struct string_pair *x_mc,
		int *num_f, struct signal_pair **f_mc);

/* Writes the final microcode to the destination file. */
static int write_mc(char *dst, char *inv, int num_f, struct signal_pair *f_mc);

/* Main. */
int main(int argc, char *argv[])
{
	int failure;
	printf("\n");

	/* Makes sure there are an appropriate
	 * number of command-line arguments.   */
	if (argc != 3) {
		printf("ERROR: The generator should take two command-line arguments.\n\n"
			"Sample usage:\n"
			"./{exec} {src file} {target file}\n");
		failure = BAD_ARGS;
		goto end;
	}

	/* Attempts to parse argv[1] -> argv[2] */
	printf("Attempting to generate microcode"
		" based on the contents of '%s'"
		" and store it in '%s'...\n\n",
			argv[1], argv[2]);
	
	failure = generate_microcode(argv[1], argv[2]);

end:
	/* HAndles errors and gives the user useful information. */
	printf("\n##############################################################\n\n");

	if (failure)
		printf("Microcode generation failed! Neither file has been changed.\n"
			"ERROR MSG: %s\n\n", err_msg(failure));
	else
		printf("Microcode generation successful!\n"
			"%s now contains logisim 2.0 compatible microcode!\n\n",
				argv[2]);
	return failure - 1;
}

/* #################
 * ### MAIN CODE ###
 * ################# */

static int generate_microcode(char *src, char *dst)
{
	int failure, instr_sig_len, mc_len, num_x, num_f;
	char *fstring, *invalid_mc;
	struct string_pair *x_microcode;
	struct signal_pair *f_microcode;

	/* Extracts the following information from src:
	 * 	instr_sig_len	- length of instr_signals.
	 * 	mc_len		- length of microcode signals.
	 * 	invalid_mc	- invalid microcode (for filler).
	 * 	num_x		- number of 'x' containing instr_signals.
	 * 	x_microcode	- list of 'x' containing instr_signals.
	 * Also outputs fstring, used to free the whole affair at the end. */
	failure = read_src(src, &fstring,
			&instr_sig_len, &mc_len, &invalid_mc,
			&num_x, &x_microcode);
	if (failure) {
		printf("%d: Failed to extract information from '%s'.\n",
				__LINE__, src);
		return failure;
	}

	/* Using x_microcode, generates the following:
	 * 	num_f		- number of final microcode entries.
	 * 	f_microcode	- an array containing the final microcode,
	 * 				sorted by instr_code.		   */
	failure = gen_fin_mc(x_microcode,
			&num_f, &f_microcode);
	if (failure) {
		printf("%d: Failed to generate a final list of microcode.\n",
				__LINE__);
		goto free_fstring_xmc;
	}
	free(x_microcode);

	/* Using invalid_mc, num_f, and f_microcode, writes microcode to dst. */
	failure = write_mc(dst, invalid_mc, num_f, f_microcode);
	if (failure)
		printf("%d: Failed to write microcode to '%s'.\n",
				__LINE__, dst);

	free(f_microcode);
free_fstring_xmc:
	free(fstring);
	free(x_microcode);
	
	return failure;
}

/* Extracts information from src. */
static int read_src(char *src, char **fstring,
		int *is_len, int *mc_len, char **inv,
		int *num_x, struct string_pair **x_mc)
{
	int failure = 0;
	int i;
	char *NEWLINES = "\r\n";

	FILE *source;
	long len;
	char *fstringdup, *line;
	char *line_sptr = NULL;

	/* Compile a filestring for src. */
	source = fopen(src, "rb");
	if (!source) {
		printf("%d: Failed to read from file '%s'.\n",
				__LINE__, src);
		return FOPEN_FAILURE;
	}

	fseek(source, 0, SEEK_END);
	len = ftell(source);
	fseek(source, 0, SEEK_SET);

	*fstring = malloc((len + 1) * sizeof(char));
	if (!*fstring) {
		printf("%d: Failed to malloc a filestring for '%s'.\n",
				__LINE__, src);
		fclose(source);
		return MALLOC_FAILURE;
	}

	fread(*fstring, 1, len, source);
	fclose(source);
	(*fstring)[len] = '\0';

	/* Finds x_mc. */
	fstringdup = strdup(*fstring);
	if (!fstringdup) {
		printf("%d: Failed to malloc a duplicate filestring for '%s'.\n",
				__LINE__, src);
		free(*fstring);
		return MALLOC_FAILURE;
	}

	line = strtok(fstringdup, "\n");
	*num_x = -2;
	while ((line = strtok(NULL, "\n")))
		(*num_x)++;
	free(fstringdup);

	/* Parses through the file and finds the remaining data. */
	/* First line = instr_sig_len. */
	line = strtok_r(*fstring, NEWLINES, &line_sptr);
	*is_len = strtol(line, NULL, 10);
	/* Second line = mc_len. */
	line = strtok_r(NULL, NEWLINES, &line_sptr);
	*mc_len = strtol(line, NULL, 10);
	/* Third line = invalid_mc. */
	line = strtok_r(NULL, NEWLINES, &line_sptr);
	*inv = line;

	/* Rest of the file = microcode combinations. */
	*x_mc = malloc((*num_x) * sizeof(struct string_pair));
	if (!*x_mc) {
		printf("%d: Failed to malloc a list of string_pairs for microcode in '%s'.\n",
				__LINE__, src);
		free(*fstring);
		return MALLOC_FAILURE;
	}

	for (i = 0; i < *num_x; i++) {
		line = strtok_r(NULL, NEWLINES, &line_sptr);
		strtok(line, " |");
		(*x_mc)[i].instr_signal = strtok(NULL, " |");
		(*x_mc)[i].microcode = strtok(NULL, " |");
	}

	return failure;
}

/* Generates the sorted final list of microcode. */
static int gen_fin_mc(struct string_pair *x_mc,
		int *num_f, struct signal_pair **f_mc)
{
	return NOT_YET_IMP;
}

/* Writes the final microcode to the destination file. */
static int write_mc(char *dst, char *inv, int num_f, struct signal_pair *f_mc)
{
	return NOT_YET_IMP;
}

