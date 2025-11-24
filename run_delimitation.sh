#!/bin/bash

# Define parameters
INTERFACE_NAME=""
SOURCE_MAC=""
SOURCE_IP=""
GATEWAY_MAC=""
INPUT_FILENAME="input/BGP_prefixes.txt"
OUTPUT_DIR="output"
OUTPUT_FILENAME_SUBNET="$OUTPUT_DIR/delimitation_subnet"
OUTPUT_FILENAME_ROUTER="$OUTPUT_DIR/delimitation_router"

INPUT_FILENAME="output/expansion_prefix_delimitation"
OUTPUT_DIR="output"
OUTPUT_FILENAME_SUBNET="$OUTPUT_DIR/delimitation2_expansion_subnet_delimitation"
OUTPUT_FILENAME_ROUTER="$OUTPUT_DIR/delimitation2_expansion_router_delimitation"

INPUT_FILENAME="output/expansion_prefix_responsive"
OUTPUT_DIR="output"
OUTPUT_FILENAME_SUBNET="$OUTPUT_DIR/delimitation2_expansion_subnet_responsive"
OUTPUT_FILENAME_ROUTER="$OUTPUT_DIR/delimitation2_expansion_router_responsive"

# INPUT_FILENAME="input/test.txt"
# OUTPUT_DIR="output"
# OUTPUT_FILENAME_SUBNET="$OUTPUT_DIR/subnet_test"
# OUTPUT_FILENAME_ROUTER="$OUTPUT_DIR/router_test"


# Ensure the output directory exists
if [ ! -d "$OUTPUT_DIR" ]; then
    mkdir -p "$OUTPUT_DIR"
    echo "Created directory: $OUTPUT_DIR"
fi

# Check if the executable exists
if [ ! -f "bin/delimitation_main" ]; then
    echo "Executable not found. Please compile the project first using 'make'."
    exit 1
fi

# Run the program
echo "Running the program..."
sudo bin/delimitation_main "$INTERFACE_NAME" "$SOURCE_MAC" "$SOURCE_IP" "$GATEWAY_MAC" "$INPUT_FILENAME" "$OUTPUT_FILENAME_SUBNET" "$OUTPUT_FILENAME_ROUTER"