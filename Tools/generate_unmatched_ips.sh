#!/bin/bash

# Define the input and output directory
INPUT_DIR="input"
OUTPUT_DIR="output"

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

# Download the latest IPv6 Hitlist data
echo "Downloading latest IPv6 Hitlist data..."
wget -P "$INPUT_DIR" https://alcatraz.net.in.tum.de/ipv6-hitlist-service/open/responsive-addresses.txt.xz
check_error "Failed to download responsive-addresses.txt.xz."

Decompress the downloaded file
echo "Decompressing the downloaded file..."
xz -d "$INPUT_DIR/responsive-addresses.txt.xz"
check_error "Failed to decompress responsive-addresses.txt.xz."

# Extract unmatched IPv6 addresses from multiple sources
echo "Extracting unmatched IPv6 addresses... This may take a few minutes."

# Define input files
RESPONSIVE_FILE="$INPUT_DIR/responsive-addresses.txt"
DELIMITATION_FILE="$OUTPUT_DIR/delimitation_router"

# Define output files with distinct suffixes
UNMATCHED_RESPONSIVE="$INPUT_DIR/unmatched_ips_responsive.txt"
UNMATCHED_DELIMITATION="$INPUT_DIR/unmatched_ips_delimitation.txt"

# Run extraction for responsive-addresses
python3 Tools/filter_unmatched_ipv6.py "$OUTPUT_DIR/delimitation_subnet" "$RESPONSIVE_FILE" "$UNMATCHED_RESPONSIVE"
check_error "Failed to extract unmatched IPv6 addresses from responsive-addresses.txt."
echo "Successfully generated $UNMATCHED_RESPONSIVE."

# Run extraction for delimitation_subnet
python3 Tools/filter_unmatched_ipv6.py "$OUTPUT_DIR/delimitation_subnet" "$DELIMITATION_FILE" "$UNMATCHED_DELIMITATION"
check_error "Failed to extract unmatched IPv6 addresses from delimitation_router."
echo "Successfully generated $UNMATCHED_DELIMITATION."



# Clean up intermediate files
echo "Cleaning up intermediate files..."
rm "$INPUT_DIR/responsive-addresses.txt"
check_error "Failed to remove intermediate file responsive-addresses.txt."

echo "unmatched_ips_responsive.txt has been successfully generated in $INPUT_DIR."
echo "unmatched_ips_delimitation.txt has been successfully generated in $INPUT_DIR."
