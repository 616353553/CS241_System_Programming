/**
* Know Your Tools Lab
* CS 241 - Fall 2018
*/

#pragma once

#include "read_wrap.h"
#include "shred.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*
* Prints how to use this utility
*/
void print_usage(char *filename);
