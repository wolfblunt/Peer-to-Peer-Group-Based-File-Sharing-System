#include "clientHeader.h"

void* clientAsServer(void* arg)
{
    // The peer acts as a server, continuously listening for connection requests
    int serverSocket; 
    struct sockaddr_in address; 
    int addrlen = sizeof(address); 
    int opt = 1; 

    // cout<<"\n"<<"Client at "<<to_string(peerPort)<< " will start running as server"<<endl;
    debugLog("Client at "+to_string(peerPort)+" will start running as server");
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    debugLog("Server socket created.");

    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_port = htons(peerPort); 

    if(inet_pton(AF_INET, &peerIp[0], &address.sin_addr)<=0)  { 
        printf("\nInvalid address/ Address not supported \n"); 
        return NULL; 
    } 
       
    if (bind(serverSocket, (SA *)&address,  sizeof(address))<0) { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    infoLog("Client (as a Server) binding completed.");

    if (listen(serverSocket, 3) < 0) { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    debugLog("Listening other peers");

    vector<thread> vThread;
    while(true){

        int client_socket;

        if((client_socket = accept(serverSocket, (SA *)&address, (socklen_t *)&addrlen)) < 0){
            perror("Acceptance error");
            cout<<"Error in accept"; 
        }

        vThread.push_back(thread(handleClientRequest, client_socket));
    }
    for(auto it=vThread.begin(); it!=vThread.end();it++){
        if(it->joinable()) it->join();
    }
    close(serverSocket);
}