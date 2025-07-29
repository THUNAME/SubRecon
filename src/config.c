#include "config.h"

uint8_t *bloom_error_src = NULL;
uint8_t *bloom_echo_src = NULL;
int fd = 0;
PrefixInfo prefix_table_current[MAX_PREFIX_TABLE_SIZE];
PrefixInfo prefix_table_next[MAX_PREFIX_TABLE_SIZE];
uint64_t prefix_table_size = 0;
int round_num = 0;
uint64_t prefix_indices[MAX_PREFIX_TABLE_SIZE];
uint64_t prefix_count;
uint64_t probe_sent = 0;
char *interface_name;
char *source_mac;
char *source_ip;
char *gateway_mac;
char* input_filename;
char* output_filename_prefix;
char* output_filename_subnet;
char* output_filename_router;
FILE* file_prefix;
FILE* file_subnet;
FILE* file_router;