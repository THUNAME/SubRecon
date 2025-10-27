# SubRecon

## I. Introduction

This repository hosts **SubRecon**, an Internet-wide IPv6 subnet discovery system. SubRecon consists of two key phases:

* **Subnet Delimitation**
  Starting from BGP-announced prefixes, this module progressively prunes and refines prefix boundaries to determine the actual size of each subnet.

* **Target Expansion**
  This module uses an active IPv6 address dataset to discover additional subnets that were not revealed by the Subnet Delimitation module.

## II. Requirements

Before running SubRecon, ensure the following dependencies are installed:

* GCC (for compiling source code)
* Bash (for executing scripts)
* libpcap (for packet crafting and capture)
* Linux system with raw socket support (required for probing)

## III. Quick Start

### Step 1: BGP Prefix Data Preparation

Before starting the probing process, generate the initial prefix dataset. This dataset is extracted from global BGP routing tables and contains IPv6 prefixes announced by Autonomous Systems (ASes). Prefixes with lengths greater than or equal to /64 have been excluded, as our analysis focuses only on prefixes with lengths less than /64.

Run the following script:

```bash
bash Tools/generate_BGP_prefixes.sh
```

### Step 2: Compile SubRecon

Compile the SubRecon source code to generate the executables `bin/delimitation_main` and `bin/expansion_main`:

```bash
make
```

These binaries correspond to the two core modules:

* `bin/delimitation_main`: used in the Subnet Delimitation.
* `bin/expansion_main`: used in the Target Expansion.

### Step 3: Run the Subnet Delimitation Module

Edit `run_delimitation.sh` to configure the probing parameters. The following variables must be specified:

```bash
# Define parameters
INTERFACE_NAME=""
SOURCE_MAC=""
SOURCE_IP=""
GATEWAY_MAC=""
INPUT_FILENAME="input/BGP_prefixes.txt"
OUTPUT_DIR="output"
OUTPUT_FILENAME_SUBNET="$OUTPUT_DIR/delimitation_subnet"
OUTPUT_FILENAME_ROUTER="$OUTPUT_DIR/delimitation_router"
```

#### Parameter Descriptions

| Parameter                  | Description                                                  |
| -------------------------- | ------------------------------------------------------------ |
| `INTERFACE_NAME`           | Network interface used for sending and receiving packets.    |
| `SOURCE_MAC`               | MAC address of the probing machine.                          |
| `SOURCE_IP`                | IPv6 address of the probing machine.                         |
| `GATEWAY_MAC`              | MAC address of the default gateway router.                   |
| `INPUT_FILENAME`           | File containing the list of target prefixes for probing.     |
| `OUTPUT_DIR`               | Directory to store probing results.                          |
| `OUTPUT_FILENAME_SUBNET` | Output file storing the discovered subnets from this module. |
| `OUTPUT_FILENAME_ROUTER`   | Output file storing the discovered last-hop routers from this module. |

Start the probing process by executing:

```bash
bash run_delimitation.sh
```

This command initiates subnet probing and saves the results:

* Discovered subnets are saved to `delimitation_subnet`.
* Discovered last-hop routers are saved to `delimitation_router`.

### Step 4: Prepare Unmatched IPs

This step downloads an IPv6 hitlist and filters out addresses that were not matched in the Subnet Delimitation phase. These unmatched addresses are used as input to the Target Expansion module.

Run the following script:

```bash
bash Tools/generate_unmatched_ips.sh
```

### Step 5: Run the Target Expansion Module

Edit `run_expansion.sh` to configure the probing parameters. The format is similar to the previous module and all parameter meanings are the same as described in Step 3.

```bash
# Define parameters
INTERFACE_NAME=""
SOURCE_MAC=""
SOURCE_IP=""
GATEWAY_MAC=""
INPUT_FILENAME="input/unmatched_ips.txt"
OUTPUT_DIR="output"
OUTPUT_FILENAME_PREFIX="$OUTPUT_DIR/expansion_prefix"
OUTPUT_FILENAME_ROUTER="$OUTPUT_DIR/expansion_router"
```

To start the Target Expansion process, run:

```bash
bash run_expansion.sh
```

This command initiates probing using unmatched active addresses:

* Prefixes requiring further delimitation are saved to `expansion_prefix`.

`expansion_prefix` is then used as the input to re-run `run_delimitation.sh`, which further identifies subnets and their corresponding last-hop routers.

By combining the results from both phases, `delimitation_subnet` and `expansion_subnet` for discovered subnets, and `delimitation_router` and `expansion_router` for last-hop routers, SubRecon constructs a more complete and accurate view of the active IPv6 subnet landscape.


