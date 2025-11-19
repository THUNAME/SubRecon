#include "config.h"
#include "construct.h"
#include "budget.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/ether.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <linux/filter.h>
#include <linux/if_packet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#include <inttypes.h>

uint64_t total_probes_sent = 0;
void Scan(uint64_t index) {
    struct ethhdr eth;
    struct ip6_hdr ip6;
    struct icmp6_hdr icmp6;
    constructICMPv6Packet(probe_sent, &eth, &ip6, &icmp6, index);

    // Create a buffer large enough to store the entire packet
    unsigned char buffer[sizeof(struct ethhdr) + sizeof(struct ip6_hdr) + sizeof(struct icmp6_hdr)];

    // Copy Ethernet, IPv6, and ICMPv6 headers into the buffer sequentially
    memcpy(buffer, &eth, sizeof(struct ethhdr));
    memcpy(buffer + sizeof(struct ethhdr), &ip6, sizeof(struct ip6_hdr));
    memcpy(buffer + sizeof(struct ethhdr) + sizeof(struct ip6_hdr), &icmp6, sizeof(struct icmp6_hdr));
    send(fd, buffer, sizeof(buffer), 0);
    __sync_add_and_fetch(&total_probes_sent, 1);
}

void* Recv(void* arg) {
    uint8_t buf[1000];
    ssize_t n;
    struct sockaddr_storage src_addr;
    socklen_t addr_len = sizeof(src_addr);

    while (1) {
        n = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr*)&src_addr, &addr_len);
        if (n < 0) {
            perror("recvfrom");
            continue;
        }

        struct ether_header* eth_hdr = (struct ether_header*)buf;
        if (ntohs(eth_hdr->ether_type) != ETH_P_IPV6) {
            continue;  // Ignore non-IPv6 packets
        }

        struct ip6_hdr* ipv6_hdr = (struct ip6_hdr*)(buf + sizeof(struct ether_header));
        if (ipv6_hdr->ip6_nxt != IPPROTO_ICMPV6) {
            continue;  // Ignore non-ICMPv6 packets
        }

        uint8_t icmp_type = buf[54];
        uint8_t icmp_code = buf[55];
        uint32_t status_code;

        if (icmp_type == 129) {
            status_code = ntohl(*(uint32_t*)&buf[58]);

            struct in6_addr ip_addr_dst;
            memcpy(&ip_addr_dst, buf + 22, sizeof(struct in6_addr));

            char ip_str_dst[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &ip_addr_dst, ip_str_dst, sizeof(ip_str_dst));

            uint32_t last_4_bytes_dst;
            memcpy(&last_4_bytes_dst, ip_addr_dst.s6_addr + 12, sizeof(uint32_t));
            last_4_bytes_dst = ntohl(last_4_bytes_dst);

            uint32_t checksum_dst = murmur3(ip_addr_dst.s6_addr, 12, 0x11112222);
            if (last_4_bytes_dst != checksum_dst) {
                continue;
            }

            uint32_t bloom_echo_index_1 = murmur3(buf + 22, 16, 0x12345678);
            uint32_t bloom_echo_index_2 = murmur3(buf + 22, 16, 0x87654321);

            if ((bloom_echo_src[bloom_echo_index_1 / 8] & (1 << (bloom_echo_index_1 % 8))) &&
                (bloom_echo_src[bloom_echo_index_2 / 8] & (1 << (bloom_echo_index_2 % 8)))) {
                continue;
            }

            bloom_echo_src[bloom_echo_index_1 / 8] |= (1 << (bloom_echo_index_1 % 8));
            bloom_echo_src[bloom_echo_index_2 / 8] |= (1 << (bloom_echo_index_2 % 8));

            uint64_t pkt_index_dst = status_code >> 8;
            if (pkt_index_dst >= prefix_table_size) {
                continue;
            }

            PrefixInfo* tPrefixInfo;
            if (round_num % 2) {
                tPrefixInfo = &prefix_table_current[pkt_index_dst];
            } else {
                tPrefixInfo = &prefix_table_next[pkt_index_dst];
            }

            __sync_add_and_fetch(&(tPrefixInfo->total_replies), 1);
        }

        else if (icmp_type == 1 && icmp_code == 3) {
            status_code = ntohl(*(uint32_t*)&buf[106]);

            struct in6_addr ip_addr_payload_dst;
            memcpy(&ip_addr_payload_dst, buf + 86, sizeof(struct in6_addr));

            char ip_str_dst[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &ip_addr_payload_dst, ip_str_dst, sizeof(ip_str_dst));

            struct in6_addr ip_addr_router;
            memcpy(&ip_addr_router, buf + 22, sizeof(struct in6_addr));

            char ip_str_router[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &ip_addr_router, ip_str_router, sizeof(ip_str_router));

            uint32_t last_4_bytes;
            memcpy(&last_4_bytes, ip_addr_payload_dst.s6_addr + 12, sizeof(uint32_t));
            last_4_bytes = ntohl(last_4_bytes);

            uint32_t checksum = murmur3(ip_addr_payload_dst.s6_addr, 12, 0x11112222);
            if (last_4_bytes != checksum) {
                continue;
            }

            uint64_t pkt_index = status_code >> 8;
            if (pkt_index >= prefix_table_size) {
                continue;
            }

            PrefixInfo* tPrefixInfo;
            if (round_num % 2) {
                tPrefixInfo = &prefix_table_current[pkt_index];
            } else {
                tPrefixInfo = &prefix_table_next[pkt_index];
            }

            uint32_t checksum_payload_dst = murmur3(ip_addr_payload_dst.s6_addr, 12, 0x11112222);
            uint32_t checksum_router = murmur3(ip_addr_router.s6_addr, 12, 0x11112222);
            if (checksum_payload_dst == checksum_router) {
                continue;  // Ignore if source and router hash match (possible loopback or reflection)
            }

            if (tPrefixInfo->unique_ifaces != 0) {
                if (strcmp(ip_str_router, tPrefixInfo->first_iface) != 0) {
                    __sync_add_and_fetch(&(tPrefixInfo->unique_ifaces), 1);
                }
            } else {
                memcpy(&(tPrefixInfo->first_iface), ip_str_router, sizeof(ip_str_router));
                __sync_add_and_fetch(&(tPrefixInfo->unique_ifaces), 1);
            }

            __sync_add_and_fetch(&(tPrefixInfo->total_replies), 1);

            uint32_t bloom_error_index_1 = murmur3(buf + 22, 16, 0x12345678);
            uint32_t bloom_error_index_2 = murmur3(buf + 22, 16, 0x87654321);

            if ((bloom_error_src[bloom_error_index_1 / 8] & (1 << (bloom_error_index_1 % 8))) &&
                (bloom_error_src[bloom_error_index_2 / 8] & (1 << (bloom_error_index_2 % 8)))) {
                continue;
            }

            bloom_error_src[bloom_error_index_1 / 8] |= (1 << (bloom_error_index_1 % 8));
            bloom_error_src[bloom_error_index_2 / 8] |= (1 << (bloom_error_index_2 % 8));

            fprintf(file_router, "%s\n", ip_str_router);
            fflush(file_router);
        }
    }

    return NULL;
}

// Function to get the current prefix table based on the round number
PrefixInfo *get_prefix_table(int round_num) {
    return (round_num % 2 == 1) ? prefix_table_current : prefix_table_next;
}

// Function to get the next prefix table based on the round number
PrefixInfo *get_next_prefix_table(int round_num) {
    return (round_num % 2 == 1) ? prefix_table_next : prefix_table_current;
}

// Function to add a refined prefix to the table
void add_refined_prefix(PrefixInfo *table, uint64_t stub, uint64_t mask, uint64_t length, uint64_t *size_ptr) {
    table[*size_ptr].prefix_stub = stub;
    table[*size_ptr].mask_suffix = mask;
    table[*size_ptr].prefix_length = length;
    table[*size_ptr].unique_ifaces = 0;
    table[*size_ptr].total_replies = 0;
    table[*size_ptr].brute_count = 0;
    (*size_ptr)++;
}

int main(int argc, char *argv[]) {
    struct timespec start_time, end_time;
    double elapsed_sec;
    int hours, minutes;
    double seconds;

    clock_gettime(CLOCK_MONOTONIC, &start_time);


    // Check if the correct number of arguments is provided
    if (argc != 8) {
        printf("Usage: %s <interface_name> <source_mac> <source_ip> <gateway_mac> <input_filename> <output_filename_prefix> <output_filename_router>\n", argv[0]);
        return 1;
    }

    // Assign values from the command line arguments
    interface_name = argv[1];
    source_mac = argv[2];
    source_ip = argv[3];
    gateway_mac = argv[4];
    input_filename = argv[5];
    output_filename_prefix = argv[6];
    output_filename_router = argv[7];

    double time_diff;
    struct ifreq ifr;
    struct sockaddr_ll sll;

    // Create a raw socket
    fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IPV6));
    if (fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set the network interface name
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, interface_name, IFNAMSIZ - 1);

    // Use ioctl to get the interface index
    if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl(SIOCGIFINDEX)");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Bind directly using the interface index
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_protocol = htons(ETH_P_IPV6);
    sll.sll_ifindex = ifr.ifr_ifindex;

    if (bind(fd, (struct sockaddr *)&sll, sizeof(sll)) < 0) {
        perror("bind");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Set receive buffer size
    int rcvbuf_size = 400 * 1024 * 1024;  // 400MB
    if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcvbuf_size, sizeof(rcvbuf_size)) < 0) {
        perror("setsockopt SO_RCVBUF failed");
        close(fd);
        return -1;
    }

    // Initialize Bloom filters for echo and error sources
    bloom_error_src = calloc(1, BLOOM_FILTER_SIZE);
    bloom_echo_src = calloc(1, BLOOM_FILTER_SIZE);
    if (!bloom_error_src) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    if (!bloom_echo_src) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    // Process and parse input CIDR list
    processAndParseAddress(input_filename);
    printf("prefix_table_size: %ld\n", prefix_table_size);
    fflush(stdout);

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, Recv, NULL);
     
    uint64_t last_prefix_table_size = prefix_table_size;
    uint64_t prefix_index;

    file_prefix = fopen(output_filename_prefix, "w");
    if (file_prefix == NULL) {
        perror("Failed to create file");
        exit(1);
    }
    file_router = fopen(output_filename_router, "w");
    if (file_router == NULL) {
        perror("Failed to create file");
        exit(1);
    }

    round_num = 0;
    while (prefix_table_size != 0) {
        printf("prefix_table_size: %ld\n", prefix_table_size);
        fflush(stdout);
        round_num = round_num + 1;
        prefix_count = 0;
        PrefixInfo *cur_table = get_prefix_table(round_num);
        PrefixInfo *next_table = get_next_prefix_table(round_num);

        // Copy all prefix indices into the array
        for (uint64_t i = 0; i < prefix_table_size; i++) {
            prefix_indices[prefix_count++] = i;
        }

        probe_sent = 0;
        while (prefix_count != 0) {
            probe_sent++;

            // Send probes for each prefix
            for (uint64_t i = 0; i < prefix_count; i++) { 
                prefix_index = prefix_indices[i];
                Scan(prefix_index); 
            }

            uint64_t i = 0;
            while (i < prefix_count) {
                prefix_index = prefix_indices[i];
                if ((cur_table[prefix_index].unique_ifaces > 1) ||
                    (probe_sent >= getValueByLength(cur_table[prefix_index].prefix_length))) {
                    prefix_indices[i] = prefix_indices[--prefix_count]; // Remove and decrease count
                } else {
                    i++; // Only increment i if not deleted
                }
            }
        }   

        // Sleep between rounds
        sleep(20);

        last_prefix_table_size = prefix_table_size;
        prefix_table_size = 0;
        for (uint64_t i = 0; i < last_prefix_table_size; i++) {
            if ((cur_table[i].unique_ifaces > 1 || 
                cur_table[i].total_replies <= 
                0.9 * getValueByLength(cur_table[i].prefix_length))) {
                fprintf(file_prefix, "%04lx:%04lx:%04lx:%04lx::/%lu\n",
                    (cur_table[i].prefix_stub >> 48) & 0xFFFF,
                    (cur_table[i].prefix_stub >> 32) & 0xFFFF,
                    (cur_table[i].prefix_stub >> 16) & 0xFFFF,
                    cur_table[i].prefix_stub & 0xFFFF,
                    cur_table[i].prefix_length);
                fflush(file_prefix);
            } else {
                uint64_t prefix_length_next = cur_table[i].prefix_length - 1;
                uint64_t stub = cur_table[i].prefix_stub;
                uint64_t mask = (~0ULL >> prefix_length_next) & 0xFFFFFFFFFFFFFFFF; 
                add_refined_prefix(next_table, stub, mask, prefix_length_next, &prefix_table_size);
            }
        }
    }
    sleep(10);
    close(fd);
    free(bloom_error_src);
    free(bloom_echo_src);


    clock_gettime(CLOCK_MONOTONIC, &end_time);


    elapsed_sec = (end_time.tv_sec - start_time.tv_sec) +
                (end_time.tv_nsec - start_time.tv_nsec) / 1.0e9;
                
    hours = (int)(elapsed_sec / 3600);
    minutes = (int)((elapsed_sec - hours * 3600) / 60);
    seconds = elapsed_sec - hours * 3600 - minutes * 60;

    printf("Loop execution time: %dh %dm %.2fs\n", hours, minutes, seconds);
    printf("Total probe packets sent: %" PRIu64 "\n", total_probes_sent);

    return 0;
}
