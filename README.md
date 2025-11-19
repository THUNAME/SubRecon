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

## Data Open Source Notice

### Usage Restrictions

This data is only allowed to be used for academic research. Any form of commercial use, data resale, or other non-academic use is strictly prohibited. Without permission, the data shall not be used to develop commercial products, conduct profit-making analysis, or disseminate it to commercial institutions.

### Acquisition Method

If you need to obtain the data, please send an email to [zhoujs24@mails.tsinghua.edu.cn] using your academic institution email. The email subject should indicate: Helixir Dataset Application for Academic Use - [Applicant's Name] - [Affiliated Institution]. The content of the email should include the following information:

-   Applicant's name, affiliated academic institution, and title/identity (such as graduate student, researcher, etc.).
-   Specific research project name, research purpose, and brief content for which the data is planned to be used.
-   The required data scale, including the quantity, scope and specific types of data needed.
-   A commitment to using the data solely for academic research and not for commercial use or illegal dissemination.

### Review Process

We will review the email within 7 working days after receiving it. After the review is passed, we will send you the data acquisition link and usage authorization instructions via email. If the application is not approved, the specific reason will also be informed.

### Liability Statement

Since these data are sensitive to some of the probed networks, if it is found that the data user violates the agreement of academic use, we have the right to terminate the data usage authorization immediately and reserve the right to pursue legal liability. The data user shall bear all relevant responsibilities arising from the use of the data, and our side shall not be responsible for any problems that may occur during the data usage process.

We are committed to promoting academic cooperation and knowledge progress. Thank you for your understanding and cooperation! If you have any questions, please feel free to contact us at [zhoujs24@mails.tsinghua.edu.cn].