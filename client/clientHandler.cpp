#include "clientHeader.h"

long long getFileSize(std::string filePath) // path to file
{
    FILE *p_file = NULL;
    p_file = fopen(filePath.c_str(),"rb");
    long long size=-1;
    if(p_file)
    {
        fseek(p_file,0,SEEK_END);
        size = ftell(p_file);
        fclose(p_file);
    }
    else
    {
        cout<<"Invalid File Path"<<endl;
        debugLog("Invalid File Path");
        return -1;
    }
    return size;
}

vector<string> splitString(string address, string delim = ":")
{
    vector<string> res;

    size_t pos = 0;
    while ((pos = address.find(delim)) != string::npos) {
        string t = address.substr(0, pos);
        res.push_back(t);
        address.erase(0, pos + delim.length());
    }
    res.push_back(address);

    return res;
}

int clientOperations(vector<string> userInput, int trackerSocket)
{
    char serverReply[10240]; 
    bzero(serverReply, 10240);
    read(trackerSocket , serverReply, 10240); 
    // cout << serverReply << endl;
    cout<<"Server Response: "<<string(serverReply)<<endl;
    infoLog("Server Response: "+string(serverReply));
 
    if(string(serverReply) == "Invalid argument count")
    { 
        return 0;
    }

    if(string(serverReply) == "Invalid Command")
    { 
        return 0;
    }

    if(userInput[0] == "login")
    {
        if(string(serverReply) == "Login Successful!")
        {
            loggedIn = true;
            string peerAddress = peerIp + ":" + to_string(peerPort);
            write(trackerSocket, &peerAddress[0], peerAddress.length());
        }
    }
    else if(userInput[0] == "logout")
    {
        loggedIn = false;
    }

    else if(userInput[0] == "list_groups")
    {
        return listGroupsFunctionality(trackerSocket);
    }

    else if(userInput[0] == "list_requests")
    {
        listPendingRequests(trackerSocket);
    }

    else if(userInput[0] == "accept_request")
    {
        acceptRequestFunctionality(trackerSocket);
    }

    else if(userInput[0] == "leave_group")
    {
        leaveGroupFunctionality(trackerSocket);
        cout<<"Exit leave Group";
    }

    else if(userInput[0] == "upload_file")
    {
        uploadFileFunctionality(userInput, trackerSocket, string(serverReply));
    }

    else if(userInput[0] == "list_files")
    {
        listAllSharableFilesInGroup(trackerSocket, userInput);
    }

    else if(userInput[0] == "download_file")
    {
        return downLoadFileHandler(userInput ,trackerSocket, string(serverReply));
    }

    else if(userInput[0] == "show_downloads")
    {
        showDownloads();
    }

    else if(userInput[0] == "stop_share")
    {
        stopshareHandler(userInput);
    }

    return 0;
}