#!/bin/bash

# Define parameters
INTERFACE_NAME=""
SOURCE_MAC=""
SOURCE_IP=""
GATEWAY_MAC=""
OUTPUT_DIR="output"

# Define input and output files
INPUT_FILE_DELIM="input/unmatched_ips_delimitation.txt"
OUTPUT_PREFIX_DELIM="$OUTPUT_DIR/expansion_prefix_delimitation"
OUTPUT_ROUTER_DELIM="$OUTPUT_DIR/expansion_router_delimitation"

INPUT_FILE_RESP="input/unmatched_ips_responsive.txt"
OUTPUT_PREFIX_RESP="$OUTPUT_DIR/expansion_prefix_responsive"
OUTPUT_ROUTER_RESP="$OUTPUT_DIR/expansion_router_responsive"


# Ensure the output directory exists
if [ ! -d "$OUTPUT_DIR" ]; then
    mkdir -p "$OUTPUT_DIR"
    echo "Created directory: $OUTPUT_DIR"
fi

# Check if the executable exists
if [ ! -f "bin/expansion_main" ]; then
    echo "Executable not found. Please compile the project first using 'make'."
    exit 1
fi


# Run expansion for delimitation
# sudo bin/expansion_main "$INTERFACE_NAME" "$SOURCE_MAC" "$SOURCE_IP" "$GATEWAY_MAC" "$INPUT_FILE_DELIM" "$OUTPUT_PREFIX_DELIM" "$OUTPUT_ROUTER_DELIM"
# echo "Expansion completed for unmatched_ips_delimitation.txt. Output saved to $OUTPUT_PREFIX_DELIM and $OUTPUT_ROUTER_DELIM."

# Run expansion for responsive
sudo bin/expansion_main "$INTERFACE_NAME" "$SOURCE_MAC" "$SOURCE_IP" "$GATEWAY_MAC" "$INPUT_FILE_RESP" "$OUTPUT_PREFIX_RESP" "$OUTPUT_ROUTER_RESP"
echo "Expansion completed for unmatched_ips_responsive.txt. Output saved to $OUTPUT_PREFIX_RESP and $OUTPUT_ROUTER_RESP."