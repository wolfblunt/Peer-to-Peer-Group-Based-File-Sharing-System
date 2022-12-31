#include "clientHeader.h"

// Handles different requests from seeder (i.e peer Client)
void handleClientRequest(int clientSocket)
{
    infoLog("Requested Client Socket Num: " + to_string(clientSocket) + "\n");

    char clientResponseString[1024] = {0}; 

    if(read(clientSocket , clientResponseString, 1024) <=0){
        close(clientSocket);
        return;
    }
    
    infoLog("Client request to Server:  "+string(clientResponseString));
    vector<string> clientResponse = splitString(string(clientResponseString), "##");

    //clientResponse< instruction , fileName>
    if(clientResponse[0] == "getChunkVectorInformation")
    {
        infoLog("Getting Chunk Vector Information");
        string downloadFileName = clientResponse[1];
        vector<int> chunkVector = fileChunkInfo[downloadFileName];
        string seederReply = "";
        for(int i: chunkVector)
        {
            seederReply += to_string(i);
        }

        // seeder respond with check Vector i.e which chunk it has <1,0,1,...>
        write(clientSocket, seederReply.c_str(), seederReply.length());

        infoLog("seederReply: " + string(seederReply));
    }
    else if(clientResponse[0] == "fetchChunkData"){
        //clientResponse = [fetchChunkData, fileName, chunkNumber, destinationPath]

        infoLog("Fetching/Sending Chunk Vector");
        string downloadFileName = clientResponse[1];
        string filePath = fileFilePathMapping[downloadFileName];
        long long int chunkNumber = stoll(clientResponse[2]);
        infoLog("filepath: "+ filePath);
        
        infoLog("Sending " + to_string(chunkNumber) + " from " + to_string(clientSocket));

        sendChunkData(filePath, chunkNumber, clientSocket);
    }
    else if(clientResponse[0] == "getFilePath")
    {
        string filepath = fileFilePathMapping[clientResponse[1]];
        infoLog("Command from Client: " +  string(clientResponseString));
        write(clientSocket, &filepath[0], strlen(filepath.c_str()));
    }
    close(clientSocket);
    return;
}

// Connects to <seederIP:seederPort> and sends it executeInstructions
string connectToSeeder(string seederIP, string seederPort, string executeIns)
{
    int seederSocket = 0;
    struct sockaddr_in peer_serv_addr; 
    infoLog("\nTrying Connecting to seeder whose seederIP => "+ seederIP +" and seederPort => "+ seederPort);

    if ((seederSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {  
        cout<<"\n Socket creation error \n"; 
        errorLog("Socket creation error");
        return "error"; 
    } 
    infoLog("Socket Created");

    peer_serv_addr.sin_family = AF_INET; 
    uint16_t peerPort = stoi(seederPort);
    peer_serv_addr.sin_port = htons(peerPort); 
    // infoLog("\n needs to connect to " + seederIP + ":" + seederPort);

    if(inet_pton(AF_INET, seederIP.c_str(), &peer_serv_addr.sin_addr) < 0)
    { 
        perror("Peer Connection Error(INET)");
    } 
    if (connect(seederSocket, (struct sockaddr *)&peer_serv_addr, sizeof(peer_serv_addr)) < 0)
    { 
        perror("Peer Connection Error");
    } 
    infoLog("Successfully connected to Seeder " + seederIP + ":" + to_string(peerPort));
 
    string currInstruction = splitString(executeIns, "##").front();
    infoLog("Current Instruction for Seeder: " + currInstruction);

    if(currInstruction == "getChunkVectorInformation")
    {
        if(send(seederSocket , executeIns.c_str() , executeIns.length() , MSG_NOSIGNAL ) == -1)
        {
            cout<<"Error: "<<strerror(errno)<<endl;
            errorLog(strerror(errno));
            return "error"; 
        }

        infoLog("Sent Command to Seeder: " + executeIns);

        char seederResponse[10240] = {0};
        if(read(seederSocket, seederResponse, 10240) < 0)
        {
            perror("err: ");
            return "error";
        }
        infoLog("Reply from seeder_"+seederPort +" => " + string(seederResponse));
        close(seederSocket);
        debugLog("Closed Socket Connection with Seeder_"+seederPort);
        return string(seederResponse);
    }

    else if(currInstruction == "fetchChunkData")
    {
        // executeIns = "fetchChunkData##fileName##chunkNumber##destinationPath"
        if(send(seederSocket , executeIns.c_str() , executeIns.length() , MSG_NOSIGNAL ) == -1)
        {
            cout<<"Error: "<<strerror(errno)<<endl;
            errorLog(strerror(errno));
            return "error"; 
        }

        infoLog("sent command to peer: " + executeIns);
        // executeInsArray<fetchChunkData, fileName, chunkNumber, destinationPath>
        vector<string> executeInsArray = splitString(executeIns, "##");
        long long int chunkNumber = stoll(executeInsArray[2]);
        string destinationPath = executeInsArray[3];
        infoLog("Getting chunk " + to_string(chunkNumber) + " from "+ string(seederPort));

        storeChunkData(seederSocket, chunkNumber, destinationPath);
        return "Write CHunk Data";
    }

    else if(currInstruction == "getFilePath"){
        if(send(seederSocket , executeIns.c_str() , executeIns.length() , MSG_NOSIGNAL ) == -1)
        {
            cout<<"Error: "<<strerror(errno)<<endl;
            errorLog(strerror(errno));
            return "error"; 
        }
        infoLog("sent command to peer: " + executeIns);
        char server_reply[10240] = {0};
        if(read(seederSocket, server_reply, 10240) < 0){
            perror("err: ");
            return "error";
        }
        infoLog("server reply for get file path:" + string(server_reply));
        fileFilePathMapping[splitString(executeIns, "##").back()] = string(server_reply);
    }

    close(seederSocket);
    // infoLog("terminating connection with " + string(serverPeerIP) + ":" + to_string(peerPort));
    return "done";
}