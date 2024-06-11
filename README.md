# CS 118 Course Project 2 - Reliable Data Transfer over lossy links

**Note**: This README is provided as supplementary documentation to assist with the setup and execution of the course project. It does not replace the official project specification. Please refer to the official project specification for detailed requirements and instructions. In case of any discrepancies or ambiguities, the official project specification takes precedence.

## Overview

This project aims to familiarize students with the concepts of reliable data transfer and provide students with some hands-on experience in building client-server applications. Students will implement a client and server in C/C++ that can reliably transfer files from the client to the server over UDP under potentially lossy links.

## Getting Started

### Prerequisites

- C/C++ compiler (gcc recommended)
- Python 3.7.3 for running the `rdcc_proxy.py` link simulator

### Compiling the Code

You may use the provided `Makefile` for compiling the server and client applications:

```sh
make build
```

This will generate two executable files: `server` and `client`.

To clean the build files, simply run:

```sh
make clean
```

### Running the Link Simulator

To simulate the link with packet loss and queuing, run the `rdcc_proxy.py` python script:

```sh
python rdcc_proxy.py [options]
```

Options:

- `--in_port_from_client <port>`: Port to listen to incoming packets from client.
- `--in_port_from_server <port>`: Port to listen to incoming packets from server.
- `--client_port <port>`: Port where the client is reachable.
- `--server_port <port>`: Port where the server is reachable.
- `--test_type <rd|cc>`: Choose 'rd' for random packet drop simulation or 'cc' for congestion control simulation. For this project, you will only ever use the 'rd' mode.
- `--loss_rate <rate>`: The probability of packet loss in 'rd' mode. e.g. 0.0 is no loss and 1.0 is all loss.
- `--random_seed <seed>`: The random seed for packet loss simulation.
- `--prop_delay <delay>`: The propagation delay in seconds.

Example usage:

```sh
python rdcc_proxy.py --test_type rd --loss_rate 0.1
```

For testing purposes, you may want to adjust the parameters such as loss rate. The default values are only for reference. We may use different values for grading. This being said, the loss rate is most probably the only parameter you will need to adjust for testing.

### Running the Client and Server

First, run the server:

```sh
./server
```

Then, in a separate terminal, run the client providing the filename to upload (say that the file to upload is "input.txt"):

```sh
./client input.txt
```

## Project Tasks

### Client (`client.c`)

Implement the logic for:

- Segmenting the file into packets.
- Sending packets to the server implementing the reliable data transfer protocol.
- Handling acknowledgments from the server.

### Server (`server.c`)

Implement the logic for:

- Receiving packets from the client.
- Sending acknowledgments back to the client.
- Reassembling the packets into the original file.
- Writing the reassembled file to the filesystem as `output.txt`.

## Modifying the Code

Students are allowed and encouraged to modify the provided skeleton code and `utils.h` to fit the needs of their implementation as long as the core objectives are met.

## Submission

Please compress your modified `utils.h` `server.c`, `client.c`, `report.txt`, and `Makefile` into a zip file using the following command:

```sh
make zip
```

Submit it according to the course submission guidelines.
