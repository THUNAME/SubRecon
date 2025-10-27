#!/usr/bin/env python3
# extract_ipv6_prefixes_fast.py

import sys
import argparse

try:
    import SubnetTree
except Exception as e:
    print(e, file=sys.stderr)
    print("Use `pip install pysubnettree` to install the required module", file=sys.stderr)
    sys.exit(1)


def process_ipasn_file(input_file, output_file):
    total_count = 0
    removed_long = 0
    removed_subsumed = 0

    prefixes = []

    # Step 1: Read and filter prefixes
    with open(input_file, 'r') as infile:
        for line in infile:
            line = line.strip()
            if not line:
                continue

            parts = line.split('\t')
            if len(parts) < 2:
                continue

            prefix = parts[0]
            if '/' not in prefix:
                continue

            try:
                plen = int(prefix.split('/')[1])
            except ValueError:
                continue

            total_count += 1
            if plen >= 64:
                removed_long += 1
                continue

            prefixes.append(prefix)

    # Step 2: Remove redundant (subsumed) prefixes using SubnetTree
    prefixes.sort(key=lambda x: int(x.split('/')[1]))  # shorter prefixes first

    tree = SubnetTree.SubnetTree()
    kept = []

    for p in prefixes:
        ip_part = p.split('/')[0]
        try:
            _ = tree[ip_part]  # check if covered by an existing prefix
            removed_subsumed += 1
            continue
        except KeyError:
            tree[p] = p
            kept.append(p)

    # Step 3: Write results
    with open(output_file, 'w') as outfile:
        for p in kept:
            outfile.write(p + '\n')

    # Step 4: Print statistics
    print("Processing complete.")
    print(f"Total prefixes: {total_count}")
    print(f"Removed (prefix length >= 64): {removed_long}")
    print(f"Removed (subsumed by larger prefix): {removed_subsumed}")
    print(f"Final kept: {len(kept)}")
    print(f"Result saved to {output_file}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Extract IPv6 prefixes (<64 and remove redundant ones) efficiently using SubnetTree."
    )
    parser.add_argument("input_file", help="Path to the input IP-ASN file")
    parser.add_argument("output_file", help="Path to the output file for extracted prefixes")
    args = parser.parse_args()

    process_ipasn_file(args.input_file, args.output_file)
