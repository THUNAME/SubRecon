#!/bin/bash

# # Define the input and output directory
INPUT_DIR="input"
OUTPUT_DIR="output"

# # Ensure the input directory exists
# mkdir -p "$INPUT_DIR"

# # Function to check if the previous command succeeded
# check_error() {
#     if [ $? -ne 0 ]; then
#         echo "Error: $1"
#         echo "Exiting script."
#         exit 1
#     fi
# }

# # Download the latest IPv6 Hitlist data
# echo "Downloading latest IPv6 Hitlist data..."
# wget -P "$INPUT_DIR" https://alcatraz.net.in.tum.de/ipv6-hitlist-service/open/responsive-addresses.txt.xz
# check_error "Failed to download responsive-addresses.txt.xz."

# # Decompress the downloaded file
# echo "Decompressing the downloaded file..."
# xz -d "$INPUT_DIR/responsive-addresses.txt.xz"
# check_error "Failed to decompress responsive-addresses.txt.xz."

# Extract unmatched IPv6 addresses from responsive list
echo "Extracting unmatched IPv6 addresses... This may take a few minutes."
python3 Tools/filter_unmatched_ipv6.py "$OUTPUT_DIR/delimitation_subnet" "$INPUT_DIR/responsive-addresses.txt" "$INPUT_DIR/unmatched_ips.txt"
check_error "Failed to extract unmatched IPv6 addresses from responsive-addresses.txt."


# # Clean up intermediate files
# echo "Cleaning up intermediate files..."
# rm "$INPUT_DIR/responsive-addresses.txt"
# check_error "Failed to remove intermediate file responsive-addresses.txt."

# echo "unmatched_ips.txt has been successfully generated in $INPUT_DIR/unmatched_ips.txt"