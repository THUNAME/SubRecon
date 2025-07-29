#!/usr/bin/env python

from __future__ import print_function

import sys
import ipaddress
try:
    import SubnetTree
except Exception as e:
    print(e, file=sys.stderr)
    print("Use `pip install pysubnettree` to install the required module", file=sys.stderr)
    sys.exit(1)

def read_subnets(tree, file_path):
    print(f"Reading subnets from {file_path}")
    with open(file_path, 'r') as fh:
        for line in fh:
            line = line.strip()
            try:
                # Validate and add the subnet to the tree
                tree[line] = line
            except ValueError as e:
                print(f"Skipped invalid subnet line '{line}'", file=sys.stderr)
    return tree

def select_unmatched_ips(tree, input_file_path, output_file_path):
    total_lines = 0  # Total number of lines in the input file
    written_lines = 0  # Number of lines written to the output file
    skipped_ips = 0  # Number of skipped invalid IPv6 addresses

    with open(input_file_path, 'r') as input_fh, open(output_file_path, 'w') as output_fh:
        for line in input_fh:
            total_lines += 1  # Increment total lines counter
            ip = line.strip()
            try:
                # Validate the IP address
                ipaddress.IPv6Address(ip)
            except ipaddress.AddressValueError:
                # If the IP address is invalid, skip it and print a message
                print(f"Skipped invalid IPv6 address: {ip}", file=sys.stderr)
                skipped_ips += 1
                continue

            try:
                # If the IP matches any subnet, skip it
                tree[ip]
            except KeyError:
                # If the IP does not match any subnet, write it to the output file
                output_fh.write(ip + '\n')
                written_lines += 1  # Increment written lines counter

    return total_lines, written_lines, skipped_ips  # Return the counters

def main():
    if len(sys.argv) != 4:
        print("Usage: python script.py <subnet_file> <input_ip_file> <output_ip_file>")
        sys.exit(1)

    subnet_file = sys.argv[1]
    input_ip_file = sys.argv[2]
    output_ip_file = sys.argv[3]

    # Create a subnet tree and read subnets from the file
    tree = SubnetTree.SubnetTree()
    tree = read_subnets(tree, subnet_file)

    # Process IP address file and write unmatched IPs to the output file
    total_lines, written_lines, skipped_ips = select_unmatched_ips(tree, input_file_path=input_ip_file, output_file_path=output_ip_file)

    print(f"Processed IP addresses and wrote unmatched IPs to {output_ip_file}")
    print(f"Total lines in input file: {total_lines}")
    print(f"Total lines written to output file: {written_lines}")
    print(f"Total lines skipped (invalid IPv6 addresses): {skipped_ips}")
    print(f"Total lines not written (matched): {total_lines - written_lines - skipped_ips}")


if __name__ == "__main__":
    main()