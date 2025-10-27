#!/bin/bash

# Define parameters
INTERFACE_NAME="enp33s0f0"
SOURCE_MAC="6c:b3:11:ac:b9:a4"
SOURCE_IP="2402:f000:6:1e00::227"
GATEWAY_MAC="74:ea:c8:b4:24:d4"
# INPUT_FILENAME="input/BGP_prefixes.txt"
# OUTPUT_DIR="output"
# OUTPUT_FILENAME_SUBNET="$OUTPUT_DIR/delimitation_subnet"
# OUTPUT_FILENAME_ROUTER="$OUTPUT_DIR/delimitation_router"


INPUT_FILENAME="output/expansion_prefix"
OUTPUT_DIR="output"
OUTPUT_FILENAME_SUBNET="$OUTPUT_DIR/delimitation_subnet_2"
OUTPUT_FILENAME_ROUTER="$OUTPUT_DIR/delimitation_router_2"

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