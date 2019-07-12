#ifndef GETOPT_H
#define GETOPT_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

extern char	*optarg;
extern int	optind;

int getopt(int argc, char *const argv[], const char *optstring);

#endif
