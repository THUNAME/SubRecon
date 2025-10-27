#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BLOOM_FILTER_SIZE  (1 << 30)
#define MAX_PREFIX_TABLE_SIZE (1 << 28)

#ifndef htonll
#define htonll(x) (((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))
#endif

typedef struct {
    uint64_t prefix_stub;
    uint64_t mask_suffix;
    uint64_t prefix_length;
    uint64_t unique_ifaces;
    uint64_t total_replies;
    uint64_t brute_count;
    char first_iface[INET6_ADDRSTRLEN];
} PrefixInfo;

extern uint8_t *bloom_error_src;
extern uint8_t *bloom_echo_src;
extern int fd;
extern PrefixInfo prefix_table_current[MAX_PREFIX_TABLE_SIZE];
extern PrefixInfo prefix_table_next[MAX_PREFIX_TABLE_SIZE];
extern uint64_t prefix_table_size;
extern int round_num;
extern uint64_t prefix_indices[MAX_PREFIX_TABLE_SIZE];
extern uint64_t prefix_count;
extern uint64_t probe_sent;

extern char *interface_name;
extern char *source_mac;
extern char *source_ip;
extern char *gateway_mac;
extern char* input_filename;
extern char* output_filename_prefix;
extern char* output_filename_subnet;
extern char* output_filename_router;
extern FILE* file_prefix;
extern FILE* file_subnet;
extern FILE* file_router;
#endif
