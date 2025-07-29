#!/usr/bin/python
# extract_ipv6_prefixes.py

import argparse

def process_ipasn_file(input_file, output_file):
    try:
        with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
            for line in infile:
                # Remove leading and trailing whitespace from the line
                line = line.strip()
                if not line:
                    continue  # Skip empty lines

                # Split each line using the tab character as the delimiter
                parts = line.split('\t')
                if len(parts) < 2:
                    print(f"Warning: Skipping invalid line: {line}")
                    continue

                # Extract the first prefix (IPv6 address part)
                ipv6_prefix = parts[0]

                # Write the extracted prefix to the new file
                outfile.write(ipv6_prefix + '\n')

        print(f"Processing complete. Extracted prefixes saved to {output_file}")
    except FileNotFoundError:
        print(f"Error: File {input_file} not found.")
    except Exception as e:
        print(f"An error occurred: {e}")


if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Extract IPv6 prefixes from an IP-ASN file.")
    parser.add_argument("input_file", help="Path to the input IP-ASN file")
    parser.add_argument("output_file", help="Path to the output file for extracted prefixes")
    args = parser.parse_args()

    # Call the function to process the file
    process_ipasn_file(args.input_file, args.output_file)