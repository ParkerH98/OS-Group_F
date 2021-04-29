#include "header.h"

void clientHandleSelection(int selection, void *client);
struct Customer reserveInformationFromUser();

//========================
//   parker_client.c
//========================

void clientSocket_SendReceive(int port)
{
    int clientSocket;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    // sets values: Internet domain, Stream socket, Default protocol (TCP in this case)
    clientSocket = socket(PF_INET, SOCK_STREAM, 0); // Create the socket
    if (clientSocket < 0)
    {
        perror("[-]Error in socket");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;                               //Address family = Internet
    serverAddr.sin_port = htons(port);                             //Set port number, using htons function to use proper byte order
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");           //Set IP address to localhost
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero); //Set all bits of the padding field to 0

    // Connect the socket to the server using the address struct
    addr_size = sizeof serverAddr;
    connect(clientSocket, (struct sockaddr *)&serverAddr, addr_size);
    if (clientSocket == -1)
    {
        perror("[-]Error in socket");
        exit(1);
    }
    int *client_socket_ptr = malloc(sizeof(int));
    *client_socket_ptr = clientSocket;

    printf("CLIENT: Connected at port %d\n", port);

    char menu[256];
    read(clientSocket, menu, sizeof(menu));
    printf("%s\n", menu);

    char selection[2];
    scanf("%s", selection);

    // for automation
    // sprintf(selection, "%d", num);
    // strcpy(selection, "1");

    send(clientSocket, selection, sizeof(selection), 0);

    clientHandleSelection(atoi(selection), client_socket_ptr);

    close(clientSocket);
}


void clientHandleSelection(int selection, void *client)
{
    int client_socket = *((int *)client);
    if (selection == 1)
    {
        struct Customer customer;
        struct Customer *customer_ptr = &customer;

        customer = reserveInformationFromUser();

        printf("Customer information menu:\n");
        printf("Customer ID: %d\n", customer.id);
        printf("Customer Name: %s\n", customer_ptr->name);
        printf("Customer DOB: %d\n", customer.dob);
        printf("Customer Gender: %c\n", customer.gender);
        printf("Customer GOVTID: %d\n", customer.govt_id);
        printf("Customer Travel Date: %d\n", customer.travel_date);
        printf("Customer Num Travelers: %d\n", customer.num_traveler);
        printf("Customer Seats: \n");
        for (int i = 0; i < NUM_SEATS; i++)
        {
            printf("%d ", customer.seats[i]);
        }
        printf("\n");

        send(client_socket, customer_ptr, sizeof(struct Customer), 0);
        printf("RESERVATION SENT\n");
    }

    else if (selection == 2)
    {
        // inquiry();
    }
    else if (selection == 3)
    {
        // modifyReservation();
    }
    else if (selection == 4)
    {
        int *ticket_num = NULL;
        printf("What is your ticket number?\n");
        scanf("%d", ticket_num);

        send(client_socket, ticket_num, sizeof(ticket_num), 0);
    }
    else if (selection == 5)
    {
        exit(0);
    }
}