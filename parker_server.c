#include "header.h"

//========================
//   parker_semaphore.c
//========================

void *handleConnection(void *client_socket);
int errorCheck(int returned, const char *errMsg);
void *waitForWork(void *);
int *removeData();
void insert(int *data);
void cancellation(int *ticket);
struct Customer reserveInformationFromUser();
void reserveSeats(struct Customer *customer);

pthread_mutex_t mutex;
pthread_cond_t condition_thread = PTHREAD_COND_INITIALIZER;


void serverSocket_SendReceive(int port)
{
    pthread_t thread_pool[THREAD_NUMBER];

    for (int i = 0; i < THREAD_NUMBER; i++)
    {
        pthread_create(&thread_pool[i], NULL, waitForWork, NULL);
    }

    int entrySocket, connectionSocket; // socket file descriptors
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

    // The three arguments are: Internet domain, Stream socket, Default protocol (TCP in this case)
    errorCheck(entrySocket = socket(PF_INET, SOCK_STREAM, 0), "Error creating socket"); // Create the socket

    // Configure settings of the server address struct
    serverAddr.sin_family = AF_INET; //Address family = Internet
    serverAddr.sin_port = htons(port); //Set port number, using htons function to use proper byte order
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //Sets IP to accept from any IP address
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero); //Set all bits of the padding field to 0

    errorCheck(bind(entrySocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)), "Error in bind"); //Bind the address struct to the socket

    errorCheck(listen(entrySocket, BACKLOG), "Error listening");

    printf("SERVER: Listening on port %d\n", port);

    while (1)
    {
        //Accept call creates a new socket for the incoming connection
        addr_size = sizeof(serverStorage);
        connectionSocket = accept(entrySocket, (struct sockaddr *)&serverStorage, &addr_size);
        printf("SERVER: Connected to client\n");

        int *client_socket = malloc(sizeof(int));
        *client_socket = connectionSocket;

        //make sure only one thread messes with the queue at a time
        pthread_mutex_lock(&mutex);
        insert(client_socket);
        pthread_cond_signal(&condition_thread);
        pthread_mutex_unlock(&mutex);
    }
}


void *waitForWork(void *arg)
{
    while (1)
    {
        int *client_socket;

        pthread_mutex_lock(&mutex);

        pthread_cond_wait(&condition_thread, &mutex);
        client_socket = removeData();

        pthread_mutex_unlock(&mutex);

        if (client_socket != NULL)
        {
            handleConnection(client_socket);
        }
    }
}


void *handleConnection(void *client)
{
    int client_socket = *((int *)client);

    printf("SERVER: Connected to client.\n");

    char menu[256] = "\n========================\n    Reservation Menu\n========================\n1: Make a reservation\n2: Inquiry about the ticket\n3: Modify the reservation\n4: Cancel the reservation\n5: Exit the program\n";
    send(client_socket, menu, sizeof(menu), 0);

    char selection[2];
    recv(client_socket, selection, sizeof(selection), 0);
    printf("SERVER: Selection [%s] was chosen by the customer.\n", selection);
    fflush(stdout);

    if (atoi(selection) == 1)
    {
        struct Customer customer;
        struct Customer *customer_ptr = &customer;

        recv(client_socket, customer_ptr, sizeof(struct Customer), 0);

        printf("Customer information received:\n");
        printf("Customer ID: %d\n", customer.id);
        printf("Customer Name: %s\n", customer_ptr->name);
        printf("Customer DOB: %c\n", customer.dob);
        printf("Customer Gender: %d\n", customer.gender);
        printf("Customer GOVTID: %d\n", customer.govt_id);
        printf("Customer Travel Date: %d\n", customer.travel_date);
        printf("Customer Num Travelers: %d\n", customer.num_traveler);
        printf("Customer Seats: \n");
        for (int i = 0; i < NUM_SEATS; i++){
            printf("%d ", customer.seats[i]);
        }
        printf("\n");

        reserveSeats(customer_ptr);
    }
    else if (atoi(selection) == 2)
    {
        // inquiry();
    }
    else if (atoi(selection) == 3)
    {
        // modifyReservation();
    }
    else if (atoi(selection) == 4)
    {
        int *ticket_num = NULL;

        recv(client_socket, ticket_num, sizeof(ticket_num), 0);

        cancellation(ticket_num);
    }
    else if (atoi(selection) == 5)
    {
        exit(0);
    }

    close(client_socket);
    return NULL;
}


int errorCheck(int returned, const char *errMsg)
{
    if (returned == -1)
    {
        perror(errMsg);
        exit(1);
    }
    else
        return 1;
}


int printRandoms(int lower, int upper)
{
    // int i;
    // for (i = 0; i < count; i++)
    // {
    srand(time(NULL));
    int num = (rand() % (upper - lower + 1)) + lower;
    printf("%d ", num);
    // }

    return num;
}



//========================
//   parker_semaphore.c
//========================

// sem_t wrt;
// pthread_mutex_t mutex;
// int read_count = 0;

void writer()
{
    sem_wait(wrt);

    // CRITICAL SECTION
    // write operation here

    sem_post(wrt);
}

void reader()
{
    // Reader acquire the lock before modifying read_count
    pthread_mutex_lock(&mutex);
    read_count++;

    // If this is the first reader, then it will block the writer
    if (read_count == 1)
    {
        sem_wait(wrt);
    }
    pthread_mutex_unlock(&mutex);

    // read operation here

    // Reader acquire the lock before modifying read_count
    pthread_mutex_lock(&mutex);
    read_count--;

    // if this is the last reader, it will signal the writer
    if (read_count == 0)
    {
        sem_post(wrt);
    }
    pthread_mutex_unlock(&mutex);
}

// int main()
// {
//     pthread_mutex_init(&mutex, NULL);
//     sem_init(wrt, 0, 1);

//     pthread_mutex_destroy(&mutex);
//     sem_destroy(wrt);

//     return 0;
// }



#define QUEUE_SIZE 6

int intArray[QUEUE_SIZE];
int head = 0;
int tail = -1;
int count = 0;




void insert(int *data)
{
    if (count != QUEUE_SIZE)
    {
        if (tail == QUEUE_SIZE - 1)
        {
            tail = -1;
        }

        intArray[++tail] = *data;
        count++;
    }
    printf("QUEUE: Count --> %d\n", count);
}

int *removeData()
{
    if (count == 0)
    {
        return NULL;
    }

    int *data = malloc(sizeof(int));
    data = &intArray[head++];

    if (head == QUEUE_SIZE)
    {
        head = 0;
    }

    count--;
    return data;
}

void printQueue()
{
    for (int i = 0; i < QUEUE_SIZE; i++)
    {
        printf(" %d -->", intArray[i]);
    }
}