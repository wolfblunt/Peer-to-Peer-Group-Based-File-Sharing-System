#include "clientHeader.h"

void acceptRequestFunctionality(int sockett)
{
    char trackerResponse[96];
    read(sockett, trackerResponse, 96);
    cout << "\nSERVER RESPONSE: " <<trackerResponse << "\n";
    infoLog("SERVER RESPONSE: "+string(trackerResponse));
}

void leaveGroupFunctionality(int sockett)
{
    char trackerResponse[96];
    read(sockett, trackerResponse, 96);
    cout<< "\nSERVER RESPONSE: "<< trackerResponse << "\n";
    // infoLog("SERVER RESPONSE: "+string(trackerResponse));
    return;
}

int listGroupsFunctionality(int sockett)
{
    char dum[5];
    strcpy(dum, "test");
    write(sockett, dum, 5);
    char reply[3*SIZE];
    memset(reply, 0, sizeof(reply));
    read(sockett, reply, 3*SIZE);

    vector<string> groupList = splitString(string(reply), "##");
    groupList.pop_back();
    infoLog("Printing All Groups");
    cout<<"********* GROUP LIST ***********"<<endl;
    for(size_t i=0; i<groupList.size();i++)
    {
        cout<<(i+1)<<". "<<groupList[i]<<"\n";
    }
    return 0;
}

void listPendingRequests(int sockett)
{
    char trackerResponse[5*SIZE];
    memset(trackerResponse, 0, 5*SIZE);
    read(sockett, trackerResponse, 5*SIZE);
    if(string(trackerResponse) == "YouAreNotAnAdmin")
    {
        cout<<"You are not the admin of this group"<<endl;
        infoLog("You are not the admin of this group");
        return;
    }

    if(string(trackerResponse) == "NoPendingUsers")
    {
        cout<<"No pending User requests"<<endl;
        infoLog("No pending User requests");
        return;
    }

    vector<string> pendingUsersList;
    pendingUsersList = splitString(string(trackerResponse), "##");
    pendingUsersList.pop_back();
    infoLog("Printing All Pending Requests");
    cout<<"******** REQUEST PENDING USERS LIST ***********"<<endl;
    int i=1;
    for(auto a: pendingUsersList)
    {
        cout<<i<<". "<<a<<"\n";
        i++;
    }
    return;
}

void listAllSharableFilesInGroup(int trackerSocket, vector<string> inputVector)
{
    string groupID = inputVector[1];
    char trackerResponse[10240]={0};
    read(trackerSocket, trackerResponse, 10240);
    vector<string> filesList = splitString(string(trackerResponse), "##");
    if(filesList[0] == "Invalid Group Name")
    {
        cout<<"ERROR: GROUP DOES NOT EXIST"<<endl;
        infoLog("ERROR: GROUP DOES NOT EXIST");
        return;
    }
    if(filesList[0] == "NO FILES FOUND")
    {
        cout<<"ERROR: NO FILES FOUND"<<endl;
        infoLog("ERROR: NO FILES FOUND");
        return;
    }
    int i = 1;
    infoLog("Printing All Sharable files in Group "+groupID);
    cout<<"*********** ALL SHARABLE FILES IN GROUP "<<groupID<<" ***********"<<endl;
    for(auto a: filesList)
    {
        cout<<"["<<i<<"] "<<a<<endl;
        i++;
    }
}

