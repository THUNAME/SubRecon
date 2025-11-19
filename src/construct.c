#include "construct.h"


uint32_t generate_32bit_random() {
    uint32_t random_number = ((uint32_t)rand() << 16) | (uint32_t)rand();
    return random_number;
}

uint64_t generate_64bit_random() {
    uint64_t random_number = (((uint64_t)rand() << 48) |
                              ((uint64_t)rand() << 32) |
                              ((uint64_t)rand() << 16) |
                              (uint64_t)rand());
    return random_number;
}

void constructICMPv6Packet(uint64_t loop, struct ethhdr *eth_hdr, struct ip6_hdr *ip6_hdr, struct icmp6_hdr *icmp6_hdr, uint64_t index) {
    PrefixInfo* tPrefixInfo;
    if(round_num % 2)  tPrefixInfo = &prefix_table_current[index];
    else tPrefixInfo = &prefix_table_next[index];

    // Ethernet header
    if (sscanf(gateway_mac, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
        &eth_hdr->h_dest[0], &eth_hdr->h_dest[1], 
        &eth_hdr->h_dest[2], &eth_hdr->h_dest[3], 
        &eth_hdr->h_dest[4], &eth_hdr->h_dest[5]) != 6) {
        perror("Invalid gateway MAC address format");
        exit(1);
    }

    if (sscanf(source_mac, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
        &eth_hdr->h_source[0], &eth_hdr->h_source[1], 
        &eth_hdr->h_source[2], &eth_hdr->h_source[3], 
        &eth_hdr->h_source[4], &eth_hdr->h_source[5]) != 6) {
        perror("Invalid source MAC address format");
        exit(1);
    }

    eth_hdr->h_proto = htons(ETH_P_IPV6);

    // IPv6 header
    ip6_hdr->ip6_flow = htonl(0x60000000); // Version, traffic class, flow label
    ip6_hdr->ip6_plen = htons(sizeof(struct icmp6_hdr)); // Payload length
    ip6_hdr->ip6_nxt = IPPROTO_ICMPV6; // Next header (ICMPv6)
    ip6_hdr->ip6_hlim = 255;
    
    if (inet_pton(AF_INET6, source_ip, &ip6_hdr->ip6_src) != 1) {
        perror("inet_pton failed to convert source IP");
        exit(1);
    }
    unsigned char dst_addr[16] = {0};

    uint64_t random_val, prefix_stub, mask_suffix, prefix_length_next;
    
    if (tPrefixInfo -> prefix_length > 56) 
    {
        random_val = tPrefixInfo->brute_count;
        tPrefixInfo->brute_count = tPrefixInfo->brute_count + 1; 
        prefix_stub = tPrefixInfo -> prefix_stub;
        mask_suffix = tPrefixInfo -> mask_suffix;
    }
    else 
    {
        random_val = generate_64bit_random();
        prefix_length_next = tPrefixInfo -> prefix_length + 1;
        mask_suffix = (~0ULL >> prefix_length_next) & 0xFFFFFFFFFFFFFFFF;

        if (tPrefixInfo -> sent_next0 > 0) {
            prefix_stub = tPrefixInfo -> prefix_stub;
            tPrefixInfo -> sent_next0 = (tPrefixInfo -> sent_next0) - 1;
        }
        else {
            prefix_stub = (tPrefixInfo -> prefix_stub) | (1ULL << (64 - prefix_length_next));
            tPrefixInfo -> sent_next1 = (tPrefixInfo -> sent_next1) - 1;
        }
    }

    uint64_t dst_prefix = prefix_stub + ((mask_suffix) & random_val);


    dst_prefix = htonll(dst_prefix);
    memcpy(dst_addr, &dst_prefix, sizeof(dst_prefix));

    uint32_t random_suffix = generate_32bit_random();
    memcpy(dst_addr + 8, &random_suffix, sizeof(random_suffix));
    uint32_t checksum = murmur3(dst_addr, 12, 0x11112222); 
    checksum = htonl(checksum);
    memcpy(dst_addr + 12, &checksum, sizeof(checksum));

    memcpy(&ip6_hdr->ip6_dst, dst_addr, sizeof(dst_addr));

    // ICMPv6 header
    icmp6_hdr->icmp6_type = ICMP6_ECHO_REQUEST; // Echo request
    icmp6_hdr->icmp6_code = 0;
    icmp6_hdr->icmp6_cksum = 0; // Checksum will be calculated later
    
    
    icmp6_hdr->icmp6_id = htons((uint16_t)(index >> 8)); // Identifier
    icmp6_hdr->icmp6_seq = htons((uint16_t)(index << 8) + (uint16_t)(ip6_hdr->ip6_hlim)); // Sequence number
    
    // Calculate ICMPv6 checksum
    struct {
        struct in6_addr src;
        struct in6_addr dst;
        uint32_t len;
        uint8_t zero[3];
        uint8_t next_header;
    } pseudo_hdr;

    memset(&pseudo_hdr, 0, sizeof(pseudo_hdr));
    pseudo_hdr.src = ip6_hdr->ip6_src;
    pseudo_hdr.dst = ip6_hdr->ip6_dst;
    pseudo_hdr.len = htonl(sizeof(struct icmp6_hdr));
    pseudo_hdr.next_header = IPPROTO_ICMPV6;

    uint8_t checksum_buffer[sizeof(pseudo_hdr) + sizeof(struct icmp6_hdr)];
    memcpy(checksum_buffer, &pseudo_hdr, sizeof(pseudo_hdr));
    memcpy(checksum_buffer + sizeof(pseudo_hdr), icmp6_hdr, sizeof(struct icmp6_hdr));

    uint32_t sum = 0;
    for (size_t i = 0; i < sizeof(checksum_buffer); i += 2) {
        sum += (checksum_buffer[i] << 8) | checksum_buffer[i + 1];
        if (sum > 0xFFFF) {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
    }
    icmp6_hdr->icmp6_cksum = htons(~sum);
}