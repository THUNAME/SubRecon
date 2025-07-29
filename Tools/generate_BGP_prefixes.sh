#!/bin/bash

# Define the input directory
INPUT_DIR="input"

# Ensure the input directory exists
mkdir -p "$INPUT_DIR"

# Function to check if the previous command succeeded
check_error() {
    if [ $? -ne 0 ]; then
        echo "Error: $1"
        echo "Exiting script."
        exit 1
    fi
}

# Download the latest IPv6 RIB data
echo "Downloading latest IPv6 RIB data..."
python3 Tools/pyasn_util_download.py --latestv6 --filename "$INPUT_DIR/rib.bz2"
check_error "Failed to download RIB data."

# Convert the RIB data to an IP-ASN database file
echo "Converting RIB data to IP-ASN database..."
python3 Tools/pyasn_util_convert.py --single "$INPUT_DIR/rib.bz2" "$INPUT_DIR/ipasn.dat"
check_error "Failed to convert RIB data to IP-ASN database."

# Extract BGP prefixes from the IP-ASN database
echo "Extracting BGP prefixes..."
python3 Tools/extract_BGP_prefixes.py "$INPUT_DIR/ipasn.dat" "$INPUT_DIR/BGP_prefixes.txt"
check_error "Failed to extract BGP prefixes."

# Clean up intermediate files
echo "Cleaning up intermediate files..."
rm "$INPUT_DIR/rib.bz2" || check_error "Failed to remove intermediate file rib.bz2."
rm "$INPUT_DIR/ipasn.dat" || check_error "Failed to remove intermediate file ipasn.dat."

echo "BGP_prefixes.txt has been successfully generated in $INPUT_DIR/BGP_prefixes.txt"