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


# INPUT_FILENAME="output/expansion_prefix"
# OUTPUT_DIR="output"
# OUTPUT_FILENAME_SUBNET="$OUTPUT_DIR/delimitation_subnet_expansion"
# OUTPUT_FILENAME_ROUTER="$OUTPUT_DIR/delimitation_subnet_expansion"

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