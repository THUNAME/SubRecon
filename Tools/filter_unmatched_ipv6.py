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
            if not line:
                continue
            try:
                tree[line] = line
            except ValueError as e:
                print(f"Skipped invalid subnet line '{line}'", file=sys.stderr)
    return tree

def select_unmatched_ips(tree, input_file_path, output_file_path):
    total_lines = 0
    written_lines = 0
    skipped_ips = 0
    processed_subnets = set()  # Track /64 subnets that have been processed

    with open(input_file_path, 'r') as input_fh, open(output_file_path, 'w') as output_fh:
        for line in input_fh:
            total_lines += 1
            ip_str = line.strip()
            if not ip_str:
                continue  # Skip empty lines
            
            try:
                ip = ipaddress.IPv6Address(ip_str)
            except ipaddress.AddressValueError:
                print(f"Skipped invalid IPv6 address: {ip_str}", file=sys.stderr)
                skipped_ips += 1
                continue

            # Calculate the /64 subnet of the current IP
            subnet_64 = ipaddress.IPv6Network(f"{ip}/64", strict=False)
            subnet_key = str(subnet_64)

            try:
                # Check if IP matches any subnet in the tree
                tree[ip_str]
            except KeyError:
                # Write only if this /64 subnet hasn't been processed before
                if subnet_key not in processed_subnets:
                    output_fh.write(ip_str + '\n')
                    written_lines += 1
                    processed_subnets.add(subnet_key)

    return total_lines, written_lines, skipped_ips

def main():
    if len(sys.argv) != 4:
        print("Usage: python script.py <subnet_file> <input_ip_file> <output_ip_file>")
        sys.exit(1)

    subnet_file = sys.argv[1]
    input_ip_file = sys.argv[2]
    output_ip_file = sys.argv[3]

    tree = SubnetTree.SubnetTree()
    tree = read_subnets(tree, subnet_file)

    total_lines, written_lines, skipped_ips = select_unmatched_ips(
        tree, 
        input_file_path=input_ip_file, 
        output_file_path=output_ip_file
    )

    print(f"Processed IP addresses and wrote unmatched IPs to {output_ip_file}")
    print(f"Total lines in input file: {total_lines}")
    print(f"Total lines written to output file: {written_lines}")
    print(f"Total lines skipped (invalid IPv6 addresses): {skipped_ips}")
    print(f"Total lines not written (matched or duplicate /64): {total_lines - written_lines - skipped_ips}")


if __name__ == "__main__":
    main()