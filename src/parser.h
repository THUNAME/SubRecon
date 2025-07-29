#ifndef PARSER_H
#define PARSER_H

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

// Function to parse CIDR from a file
void processAndParsePrefix();
void processAndParseAddress();
#endif 