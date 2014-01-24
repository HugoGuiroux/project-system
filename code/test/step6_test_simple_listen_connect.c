#include "libc.c"

#define DATA_SIZE 13 // with the '\0'
#define ACK_SIZE 8
#define NB_LOOP 10

/**
 * Simple test, listen and then accept a connection. Then send a packet and wait for a response ten times
 * Follows the Action 2 of the subject. (Largely inspired by nettest.cc)
 */

/**
 * int Listen(int local_port);
 * int Accept(int listening_socket);
 * int Connect(int remote_machine, int remote_port);
 * int Send(char* buffer, unsigned int size);
 * int Receive(char* buffer, unsigned int size);
 * int Disconnect(int socket);
 **/


int main()
{
	int connected_sid;
	int error_code;
	const char *data = "Hello there!";
    const char *ack = "Got it!";
    char buffer[20]; // buffer used for receive
    
	connected_sid = Connect(0,1); // we connect to the macine 0 on port 1(see step6_test_simple_listen_accept.c)
	if(connected_sid < 0)
	{
		PutString("Message Apocalyptique :  TO BE DEFINED\n");
		return -3;
	}
	
	int i;
	for( i=0; i< NB_LOOP; i++ )
	{
		if ( ( error_code = Receive(buffer,DATA_SIZE) ) == -2 )
		{
			PutString("Could not receive : socket is waiting\n");
			return -2;
		}
		if( error_code == -1 )
		{
			PutString("Error : Socket is not connected \n");
			return -1;
		}
		
		// We received the message
		PutString("Received message : ");
		PutString(buffer);
		if( !strcmp(buffer,(char*)data) )
		{
			PutString(" Erro : The data is corrupted(not the one that was expected\n");
			return -4;
		}
		
		if ( ( error_code = Send((char*)ack,ACK_SIZE) ) == -2 )
		{
			PutString("Could not send : Transission problem\n");
			return -2;
		}
		if( error_code == -1 )
		{
			PutString("Error : Socket is not connected \n");
			return -1;
		}
		
		// Data is sent
		
		
	}
	Disconnect(connected_sid); // we close the socket, need to do only on one side
	
	return 0;
	
	
}