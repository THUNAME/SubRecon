#!/bin/bash

# Define parameters
INTERFACE_NAME=""
SOURCE_MAC=""
SOURCE_IP=""
GATEWAY_MAC=""
INPUT_FILENAME="input/unmatched_ips.txt"
OUTPUT_DIR="output"
OUTPUT_FILENAME_PREFIX="$OUTPUT_DIR/expansion_prefix"
OUTPUT_FILENAME_ROUTER="$OUTPUT_DIR/expansion_router"


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

# Run the program
echo "Running the program..."
sudo bin/expansion_main "$INTERFACE_NAME" "$SOURCE_MAC" "$SOURCE_IP" "$GATEWAY_MAC" "$INPUT_FILENAME" "$OUTPUT_FILENAME_PREFIX" "$OUTPUT_FILENAME_ROUTER"