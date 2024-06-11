#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include "utils.h"

#define TIMEOUT_SECS 0.1 // Timeout value of 0.1 seconds

int main(int argc, char *argv[]) {
    int listen_sockfd, send_sockfd;
    struct sockaddr_in client_addr, server_addr_to, server_addr_from;
    socklen_t addr_size = sizeof(server_addr_to);
    struct timeval start_time, current_time, timeout;         
    struct packet pkt;
    struct packet ack_pkt;
    char buffer[PAYLOAD_SIZE];
    unsigned short seq_num = 0;
    bool last_packet = false;
    int retries = 0;

    if (argc != 2) {
        printf("Usage: ./client <filename>\n");
        return 1;
    }
    char *filename = argv[1];

    listen_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    send_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (listen_sockfd < 0 || send_sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    timeout.tv_sec = TIMEOUT_SECS;
    timeout.tv_usec = 0;
    if (setsockopt(listen_sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("Error setting timeout");
        close(listen_sockfd);
        close(send_sockfd);
        return 1;
    }

    memset(&server_addr_to, 0, sizeof(server_addr_to));
    server_addr_to.sin_family = AF_INET;
    server_addr_to.sin_port = htons(SERVER_PORT_TO);
    server_addr_to.sin_addr.s_addr = inet_addr(SERVER_IP);

    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(CLIENT_PORT);
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listen_sockfd, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
        perror("Bind failed");
        close(listen_sockfd);
        close(send_sockfd);
        return 1;
    }

    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Error opening file");
        close(listen_sockfd);
        close(send_sockfd);
        return 1;
    }

   while (!last_packet) {
        // Read data from file into packet payload
        int bytes_read = fread(pkt.payload, sizeof(char), PAYLOAD_SIZE, fp);
        if (bytes_read < PAYLOAD_SIZE) {
            pkt.last = 1; // Set FIN flag for last packet
            last_packet = true;
        }
        else {
            pkt.last = 0;
        }

        pkt.seqnum = seq_num;
        //seq_num = (seq_num + 1) % 2; // Alternate sequence number

        // Send data packet to server
        //(send_sockfd, &pkt, bytes_read + HEADER_SIZE, 0, (struct sockaddr *)&server_addr_to, sizeof(server_addr_to));

        // Get current time
        //gettimeofday(&start_time, NULL);

        while(retries < MAX_RETRIES) {
           /*while (true) {
                // Check for ACK or timeout
                gettimeofday(&current_time, NULL);
                double elapsed_time = (current_time.tv_sec - start_time.tv_sec) + (current_time.tv_usec - start_time.tv_usec) / 1000000.0;

                if (elapsed_time >= TIMEOUT_SECS) {
                    // Timeout, retransmit packet*/
            sendto(send_sockfd, &pkt, bytes_read + HEADER_SIZE, 0, (struct sockaddr *)&server_addr_to, sizeof(server_addr_to));
                    /*gettimeofday(&start_time, NULL); // Reset start time
                    continue;
                }*/
                
            int recv_len = recvfrom(listen_sockfd, &ack_pkt, sizeof(ack_pkt), 0, (struct sockaddr *)&server_addr_from, &addr_size);
            if (recv_len < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Timeout occurred, retransmit
                printf("Timeout occurred. Retransmitting packet: %u\n", pkt.seqnum);
                retries++;
                continue;
                } 
                
            }
            // Correct ACK received, exit the loop
            if (ack_pkt.seqnum == seq_num) {
                printf("Received ACK for packet: %u\n", pkt.seqnum);
                seq_num = (seq_num + 1) % 2; // Alternate sequence number
                retries = 0;
                break;
            }
        }
        if (retries == MAX_RETRIES) {
            printf("Max retries reached. Exiting...\n");
            break;
        }
    }

    fclose(fp);
    close(listen_sockfd);
    close(send_sockfd);
    return 0;
}