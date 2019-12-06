#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

//works for upper case letters but not for spaces
char encrypt(char p_char, char k_char) {
	char c_char = ((p_char - 65) + (k_char - 65)) % 26;
	c_char += 65;

	return c_char;
}

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead, p_size;
	socklen_t sizeOfClientInfo;
	char buffer[256]; // message
	struct sockaddr_in serverAddress, clientAddress;
	char plaintext_buffer[99999];
	char key_buffer[99999];
	char cipher_buffer[p_size]; //same size as plaintext

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	// Accept a connection, blocking if one is not available until one connects
	sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
	establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
	if (establishedConnectionFD < 0) error("ERROR on accept");

	// Get the message from the client and display it
	// memset(buffer, '\0', 256);
	// charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
	// if (charsRead < 0) error("ERROR reading from socket");
	// printf("SERVER: I received this from the client: \"%s\"\n", buffer);

	while(1) {

		// gets the int from client and display it
		charsRead = recv(establishedConnectionFD, &p_size, sizeof(int), 0);
		if (charsRead < 0) error("ERROR reading from socket");
		printf("SERVER: I received the buffer length from the client: \"%d\"\n", p_size);
		fflush(stdout);

		// get the plaintext buffer
		memset(plaintext_buffer, '\0', p_size);
		charsRead = 0;
		while(charsRead < p_size) {
			charsRead += recv(establishedConnectionFD, plaintext_buffer + charsRead, p_size - charsRead, 0);
		}
		if (charsRead < 0) error("ERROR reading from socket");
		printf("SERVER: I received this plaintext from the client: \"%s\"\n", plaintext_buffer);
		fflush(stdout);

		// get the key buffer
		memset(key_buffer, '\0', p_size);
		charsRead = 0;
		while(charsRead < p_size) {
			charsRead += recv(establishedConnectionFD, key_buffer + charsRead, p_size - charsRead, 0);
		}
		if (charsRead < 0) error("ERROR reading from socket");
		printf("SERVER: I received this key from the client: \"%s\"\n", key_buffer);
		fflush(stdout);

		//fork

		//encrypt
		int i;
		for(i = 0; i < p_size; i++) {
			cipher_buffer[i] = encrypt(plaintext_buffer[i], key_buffer[i]);
		}

		printf("SERVER: Cipher: %s\n", cipher_buffer);
		fflush(stdout);

		// Send a Success message and cipher back to the client 
		//charsRead = 0;
		// charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
		// if (charsRead < 0) error("ERROR writing to socket");
		break;
	}

	close(establishedConnectionFD); // Close the existing socket which is connected to the client
	close(listenSocketFD); // Close the listening socket
	return 0; 
}
