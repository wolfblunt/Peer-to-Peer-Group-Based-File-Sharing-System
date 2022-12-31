#include "trackerHeader.h"

void fetchArguments(int argc, char *argv[]){

    char* path = argv[1];

    infoLog("PATH: "+string(path));

    fstream trackerInfoFile;
    trackerInfoFile.open(path, ios::in);

    vector<string> trackeraddress;
    if(trackerInfoFile.is_open()){
        string t;
        while(getline(trackerInfoFile, t)){
            string trackerIp = t.substr(0, t.find(":"));
            string trackerPort = t.substr(t.find(":")+1, -1);
            trackeraddress.push_back(trackerIp);
            trackeraddress.push_back(trackerPort);
        }
        trackerInfoFile.close();
    }
    else{
        cout << "Tracker Info file not found.\n";
        exit(-1);
    }

    if(string(argv[2]) == "1"){
        tracker1_ip = trackeraddress[0];
        tracker1_port = stoi(trackeraddress[1]);
        curTrackerIP = tracker1_ip;
        curTrackerPort = tracker1_port;
        cout<<"\t [-] TrackerIP : "<<curTrackerIP<<endl;
        cout<<"\t [-] TrackerPort: "<<curTrackerPort<<endl;
    }
    else{
        tracker2_ip = trackeraddress[2];
        tracker2_port = stoi(trackeraddress[3]);
        curTrackerIP = tracker2_ip;
        curTrackerPort = tracker2_port;
    }
}


void* programExitCheck(void* arg)
{
    while(true){
        string input;
        getline(cin, input);
        if(input == "quit")
        {
            // Clearing the screen before exiting the program
            cout<<"\x1b[H";
            cout<<"\x1b[2J";  // clear screen and send cursor to home position.
            cout<<"\x1b[H";  // send cursor to cursor home position.
            exit(0);
        }
    }
}


int main(int argc, char *argv[])
{
    cout<<"\x1b[2J";
    cout<<"\x1b[H";
    logFileName = "trackerLogs.log";
    clearLog();
    cout<<"-------------------- Peer-to-Peer Group Based File Sharing System --------------------"<<endl;
    cout<<"\n-------------------- TRACKER INFORMATION --------------------"<<endl;
    if(argc != 3){
        cout << "Please provide the required arguments:  <tracker_info fileName> & <tracker_number>"<<endl;
        debugLog("Please provide the required arguments:  <tracker_info fileName> & <tracker_number>");
        return -1;
    }
    fetchArguments(argc, argv);

    int trackerSocket; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    pthread_t exitDetectionThreadId;

    if ((trackerSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
        perror("socket failed");
        debugLog("Socket Failed");
        exit(EXIT_FAILURE); 
    }

    if (setsockopt(trackerSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
        perror("setsockopt");
        debugLog("setsockopt error");
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_port = htons(curTrackerPort); 

    if(inet_pton(AF_INET, &curTrackerIP[0], &address.sin_addr)<=0)  { 
        printf("\nInvalid address/ Address not supported \n");
        debugLog("Invalid address/ Address not supported");
        return -1; 
    } 
       
    if (bind(trackerSocket, (SA *)&address,  sizeof(address))<0) { 
        perror("bind failed"); 
        debugLog("Bind Failed");
        exit(EXIT_FAILURE); 
    }

    if (listen(trackerSocket, 3) < 0) { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    }
    else{
        cout<<"-------------------- Tracker Listening ---------------------------\n";
        infoLog("TRACKER LISTENING");
    }

    vector<thread> threadVector;

    if(pthread_create(&exitDetectionThreadId, NULL, programExitCheck, NULL) == -1)
    {
        perror("pthread");
        debugLog("pthread error");
        exit(EXIT_FAILURE); 
    }
    while(true){
        int clientSocket;

        if((clientSocket = accept(trackerSocket, (SA *)&address, (socklen_t *)&addrlen)) < 0)
        {
            // Syntax : int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
            // On success, these system calls return a non-negative integer that is a file descriptor for the accepted socket. On error
            // -1 is returned, and errno is set appropriately.
            perror("Acceptance error");
            cout<<"Acceptance Error"; 
        }
        // cout<<"Connection Accepted: "<<clientSocket<<"\n";
        infoLog("Connection Accepted: " + to_string(clientSocket));
        threadVector.push_back(thread(trackerClientOperations, clientSocket));
    }

    for(auto i=threadVector.begin(); i!=threadVector.end(); i++){
        if(i->joinable()) i->join();
    }

    // Clearing the screen before exiting the program
    cout<<"\x1b[H";
    cout<<"\x1b[2J";  // clear screen and send cursor to home position.
    cout<<"\x1b[H";  // send cursor to cursor home position.


    return 0;
}