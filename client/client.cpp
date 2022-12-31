#include "clientHeader.h"

void fetchArguments(int argc, char *argv[]){
    string peerInfo = argv[1];
    char* path = argv[2];
    // cout<<"PATH : "<<path<<endl;
    peerIp = peerInfo.substr(0, peerInfo.find(":"));
    peerPort = stoi(peerInfo.substr(peerInfo.find(":")+1, -1));
    cout<<"############################################################# "<<endl;
    cout<<"------------ CLIENT <"<<peerIp<<":"<<peerPort<<"> ------------- "<<endl;
    cout<<"############################################################# "<<endl;
    logFileName = peerInfo+"clientLogs.log";
    clearLog();
    cout<<"\t [-] IP : "<<peerIp<<endl;
    cout<<"\t [-] PORT : "<<peerPort<<endl;
    // ###########################

    fstream trackerInfoFile;
    trackerInfoFile.open(path, ios::in);

    vector<string> trackeraddress;
    if(trackerInfoFile.is_open()){
        string t;
        while(getline(trackerInfoFile, t)){
            //#############
            string trackerIp = t.substr(0, t.find(":"));
            string trackerPort = t.substr(t.find(":")+1, -1);
            //###########
            trackeraddress.push_back(trackerIp);
            trackeraddress.push_back(trackerPort);
        }
        trackerInfoFile.close();
    }
    else{
        cout << "Tracker Info File not found.\n";
        exit(-1);
    }

    tracker1_ip = trackeraddress[0];
    tracker1_port = stoi(trackeraddress[1]);
    tracker2_ip = trackeraddress[2];
    tracker2_port = stoi(trackeraddress[3]);
    
    // cout<<"Peer Address : "<< string(peerIp) << ":" << to_string(peerPort)<<endl;
    cout<<"Tracker1 Address : "<< string(tracker1_ip)<< ":"<<to_string(tracker1_port)<<endl;
    cout<<"Tracker2 Address : "<< string(tracker2_ip)<< ":"<<to_string(tracker2_port)<<endl;
}

int connectToTracker(int trackerNum, struct sockaddr_in &serv_addr, int socketfd){
    char* curTrackIP;
    uint16_t curTrackPort;
    if(trackerNum == 1){
        curTrackIP = &tracker1_ip[0]; 
        curTrackPort = tracker1_port;
    }
    else{
        curTrackIP = &tracker2_ip[0]; 
        curTrackPort = tracker2_port;
    }

    bool err = 0;

    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(curTrackPort); 
       
    if(inet_pton(AF_INET, curTrackIP, &serv_addr.sin_addr)<=0)  { 
        err = 1;
    } 
    if (connect(socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { 
        err = 1;
    } 
    if(err){
        if(trackerNum == 1)
            return connectToTracker(2, serv_addr, socketfd);
        else
            return -1;
    }
    cout<<"\n*************CONNECTED TO SERVER "<< to_string(curTrackPort) <<" ****************"<<endl;
    return 0;
}


int main(int argc, char* argv[])
{
    cout<<"\x1b[2J";
    cout<<"\x1b[H";
    if(argc != 3){
        cout << "Please provide the required arguments:  <tracker_info file name> & <tracker_number>\n";
        return -1;
    }
    fetchArguments(argc, argv);

    int socketfd = 0; 
    struct sockaddr_in serv_addr; 
    pthread_t serverThread;
    
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {  
        printf("\n Socket creation error \n"); 
        return -1; 
    } 

    if(pthread_create(&serverThread, NULL, clientAsServer, NULL) == -1)
    {
        perror("pthread"); 
        exit(EXIT_FAILURE); 
    }

    if(connectToTracker(1, serv_addr, socketfd) < 0)
    {
        exit(-1); 
    }

    while(true)
    { 
        cout << "Enter Command >> ";
        string inptline, s;
        getline(cin, inptline);

        if(inptline.length() < 1)
        {
            continue;
        }
        
        stringstream ss(inptline);
        vector<string> inpt;
        while(ss >> s){
            inpt.push_back(s);
        } 

        if(inpt[0] == "login" && loggedIn)
        {
            cout << "You already logged in and have one active session" << endl;
            continue;
        }
        // cout<<"loggedIn : "<<loggedIn<<endl;
        if(inpt[0] != "login" && inpt[0] != "create_user" && !loggedIn)
        {
            cout << "Please login or create new account if you are a new user" << endl;
            continue;
        }

        if(send(socketfd , &inptline[0] , strlen(&inptline[0]) , MSG_NOSIGNAL ) == -1)
        {
            cout<<"Error: "<<strerror(errno)<<endl;
            return -1;
        }

        clientOperations(inpt, socketfd);
    }
    close(socketfd);
    return 0;
}