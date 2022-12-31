#include "trackerHeader.h"

int createUserFunctionality(vector<string> clientRequestOperation)
{
    string userID = clientRequestOperation[1];
    string userPassword = clientRequestOperation[2];

    if(userCredentials.find(userID) == userCredentials.end())
    {
        userCredentials.insert({userID, userPassword});
        infoLog("User created with userName: "+userID+" and password: "+userPassword);
    }
    else
    {
        return 0;
    }
    return 1;
}

int loginCheck(vector<string> clientRequestOperation)
{
    string userID = clientRequestOperation[1];
    string userPassword = clientRequestOperation[2];

    if(userCredentials.find(userID) == userCredentials.end() || userCredentials[userID] != userPassword)
    {
        return 0;
    }

    if(isLoggedIn.find(userID) == isLoggedIn.end())
    {
        isLoggedIn.insert({userID, true});
    }
    else
    {
        if(isLoggedIn[userID])
        {
            return 1;
        }
        else{
            isLoggedIn[userID] = true;
        }
    }
    return 2;
}

void logoutFunction(string userID)
{
    isLoggedIn[userID] = false;
}

int createGroupFunction(vector<string> clientRequestOperation, int clientSocket, string userID)
{
    string groupName = clientRequestOperation[1];
    for(auto a: groupArray)
    {
        if(a == groupName)
        {
            return 0;
        }
        
    }
    groupAdminMapping.insert({groupName, userID});
    infoLog(to_string(clientSocket) + ": Added groupName: "+groupName+" userID: "+userID+" in groupAdminMapping");
    groupArray.push_back(groupName);
    groupMembersMapping[groupName].insert(userID);
    return 1;
}

void joinGroupFunctionality(vector<string> clientRequestOperation, string userID, int clientSocket)
{
    string groupID = clientRequestOperation[1];
    if(groupAdminMapping.find(groupID) == groupAdminMapping.end())
    {
        write(clientSocket, "Invalid Group ID", 16);
        infoLog(to_string(clientSocket) + " : Invalid Group ID");
    }
    else if(groupAdminMapping[groupID] == userID)
    {
        write(clientSocket, "You are the admin of this group!",32);
        infoLog(to_string(clientSocket) + " : You are the admin of this group!");
    }
    else if(groupMembersMapping[groupID].find(userID) == groupMembersMapping[groupID].end())
    {
        groupMembersPendngRequests[groupID].push_back(userID);
        write(clientSocket, "Join Group Request Sent!", 24);
        infoLog(to_string(clientSocket) + " : Join Group Request Sent!");
    }
    else
    {
        write(clientSocket, "You are already present in this group", 37);
        infoLog(to_string(clientSocket) + " : You are already present in this group");
    }
}

void listGroupFunctionality(vector<string> clientRequestOperation, int clientSocket)
{
    write(clientSocket, "Group List", 10);

    char dum[5];
    read(clientSocket, dum, 5);

    int noOfGroups = groupArray.size();

    if(noOfGroups == 0)
    {
        write(clientSocket, "No groups found##", 18);
        infoLog("No groups found");
        return;
    }

    string groupList = "";
    for(size_t i=0;i<groupArray.size();i++)
    {
        groupList += groupArray[i] + "##";
    }
    write(clientSocket, &groupList[0], groupList.length());
}

void listPendingRequests(vector<string> clientRequestOperation, int clientSocket, string userID)
{
    write(clientSocket, "Pending Group Requests", 22);
    string groupID = clientRequestOperation[1];
    if(groupAdminMapping.find(groupID) == groupAdminMapping.end())
    {
        write(clientSocket, "YouAreNotAnAdmin", 16);
    }
    else if(groupMembersPendngRequests[groupID].size() == 0)
    {
        write(clientSocket, "NoPendingUsers", 14);
    }
    else
    {
        string pendingUsers = "";
        for(auto a: groupMembersPendngRequests[groupID])
        {
            pendingUsers += a + "##";
        }
        write(clientSocket, &pendingUsers[0], pendingUsers.length());
    }
}

void acceptRequestFunctionality(vector<string> clientRequestOperation, int clientSocket, string userID)
{
    write(clientSocket, "Accepting Request", 17);

    string groupID = clientRequestOperation[1];
    string pendingUserID = clientRequestOperation[2];
    // cout<<"\ngroupID : "<<groupID;
    infoLog("groupID : "+ groupID);
    // cout<<"\npendingUserID : "<<pendingUserID;
    infoLog("pendingUserID : "+ pendingUserID);
    // cout<<"\nuserID :"<<userID;
    infoLog("userID : "+ userID);

    if(groupAdminMapping.find(groupID) == groupAdminMapping.end())
    {
        write(clientSocket, "Invalid Group ID", 16);
    }
    else if(groupAdminMapping[groupID] != userID)
    {
        write(clientSocket, "You are not admin of this group", 31);
    }
    // else if(groupMembersMapping[groupID].find(pendingUserID) == groupMembersMapping[groupID].end())
    // {
    //     cout<<"\nUser does not belong to this group";
    //     write(clientSocket, "User does not belong to this group", 34);
    // }
    else
    {   
        int index=0;
        for(size_t i=0;i<groupMembersPendngRequests[groupID].size();i++)
        {
            if(groupMembersPendngRequests[groupID][i] == pendingUserID)
            {
                index = i;
            }
        }
        groupMembersPendngRequests[groupID].erase(groupMembersPendngRequests[groupID].begin()+index);
        groupMembersMapping[groupID].insert(pendingUserID);

        write(clientSocket, "Request Accepted", 17);
    }
}

void leaveGroupFunctionality(vector<string> clientRequestOperation, int clientSocket, string userID)
{
    string groupID = clientRequestOperation[1];
    // string removeUserID = clientRequestOperation[2];
    if(find(groupArray.begin(), groupArray.end(), groupID) == groupArray.end())
    {
        write(clientSocket, "Invalid group ID", 18);
        return;
    }
    else if(groupMembersMapping[groupID].find(userID) != groupMembersMapping[groupID].end())
    {
        if(groupAdminMapping[groupID] == userID)
        {
            write(clientSocket, "Admin can't leave the group", 27);
            infoLog(userID + " Admin can't leave the group");
        }
        else
        {
            groupMembersMapping[groupID].erase(userID);
            write(clientSocket, "Successfully Left the Group", 27);
            infoLog(clientSocket + ": Successfully Left the Group");
        }
    }
    else
    {
        write(clientSocket, "User is not present in this group", 33);
        infoLog(clientSocket + " : User is not present in this group");
    }
}

void uploadFileFunctionality(vector<string> clientRequestOperation, int clientSocket, string userID)
{
    string groupID = clientRequestOperation[2];
    string uploadFilePath = clientRequestOperation[1];
    string trackerReply="";
    if(groupMembersMapping.find(groupID) == groupMembersMapping.end())
    {
        trackerReply = "Group Not Exist";
        write(clientSocket, trackerReply.c_str(), 15);
    }
    else if(groupMembersMapping[groupID].find(userID) == groupMembersMapping[groupID].end())
    {
        trackerReply = "Not Belong To This Group";
        write(clientSocket, trackerReply.c_str(), 24);
    }
    else
    {
        struct stat buffer;
        bool pathVerification = (stat (uploadFilePath.c_str(), &buffer) == 0);
        if(!pathVerification)
        {
            trackerReply = "Invalid File Path";
            write(clientSocket, trackerReply.c_str(), 17);
        }
        else
        {
            char uploadFile[512000];
            write(clientSocket, "Uploading...", 12);
            if(read(clientSocket, uploadFile, 512000))
            {
                if(string(uploadFile) == "error")
                {
                    return;
                }
                vector<string> uploadFileDetailsVector = splitStringUtility(string(uploadFile), "##");
                string uploadFileName = splitStringUtility(uploadFilePath, "/").back();
                string filePiecesHash = "";
                for(size_t i=3; i<uploadFileDetailsVector.size(); i++)
                {
                    filePiecesHash += uploadFileDetailsVector[i];
                    if(i != uploadFileDetailsVector.size()-1)
                    {
                        filePiecesHash += "##";
                    }
                }

                if(filePieceHashMap.find(uploadFileName) == filePieceHashMap.end())
                {
                    // filePieceHashMap[uploadFileName] = filePiecesHash;
                    filePieceHashMap.insert({uploadFileName, filePiecesHash});
                }

                if(seedersList[groupID].find(uploadFileName) == seedersList[groupID].end())
                {
                    seedersList[groupID].insert({uploadFileName, {userID}});
                }
                else
                {
                    seedersList[groupID][uploadFileName].insert(userID);
                }
                fileSize[uploadFileName] = uploadFileDetailsVector[2];
                write(clientSocket, "FILE SUCCESSFULLY UPLOADED", 26);
            }
        }
    }
}

void listAllSharableFilesInGroup(vector<string> clientRequestOperation, int clientSocket, string userID)
{
    string groupID = clientRequestOperation[1];
    write(clientSocket, "FETCHING SHARABLE FILES", 23);
    if(groupAdminMapping.find(groupID) == groupAdminMapping.end())
    {
        write(clientSocket, "Invalid Group Name", 20);
        return;
    }
    else if(seedersList[groupID].size() == 0)
    {
        write(clientSocket, "NO FILES FOUND", 15);
        return;
    }
    else
    {
        string fileList = "";
        for(auto a: seedersList[groupID])
        {
            fileList += a.first + "##";
        }
        fileList = fileList.substr(0, fileList.length()-2);
        write(clientSocket, fileList.c_str(), fileList.length());
        return;
    }
}

void downloadFileFunctionality(vector<string> clientRequestOperation, int clientSocket, string userID)
{
    string groupID = clientRequestOperation[1];
    string destinationPath = clientRequestOperation[3];
    struct stat buffer;
    bool pathVerification = (stat (destinationPath.c_str(), &buffer) == 0);
    if(groupMembersMapping.find(groupID) == groupMembersMapping.end())
    {
        write(clientSocket, "InvalidGroup", 12);
        return;
    }
    else if(groupMembersMapping[groupID].find(userID) == groupMembersMapping[groupID].end())
    {
        write(clientSocket, "InvalidUser", 11);
        return;
    }
    else if(!pathVerification)
    {
        write(clientSocket, "InvalidPath", 11);
        return;
    }
    else
    {
        write(clientSocket, "DOWNLOADING", 11);
        char downloadFileDetails[512000]; // downloadFileDetails = [fileName, destination, groupName]

        if(read(clientSocket, downloadFileDetails, 512000))
        {
            vector<string> fileDetailsArray = splitStringUtility(string(downloadFileDetails), "##");

            string response = "";
            string fileName = fileDetailsArray[0];
            cout<<"FileName : "<<fileName<<endl;
            cout<<"groupID : "<<groupID<<endl;
            for(auto b :seedersList)
            {
                cout << b.first<<" : ";
                for(auto c: b.second)
                {  
                    cout<<c.first<<endl;
                }
            }
            if(seedersList[groupID].find(fileName) != seedersList[groupID].end())
            {
                for(auto a: seedersList[groupID][fileName])
                {
                    if(isLoggedIn[a])
                    {
                        response += userToPortMapping[a] + "##";
                    }
                }

                response += fileSize[fileName];
                infoLog("List of all the Seeders : "+ response);
                write(clientSocket, response.c_str(), response.length());

                write(clientSocket, filePieceHashMap[fileName].c_str(), filePieceHashMap[fileName].length());
                seedersList[groupID][fileName].insert(userID);
            }
            else
            {
                write(clientSocket, "FileNotFound", 12);
            }
        }

        return;
    }
}

void stopShareFunctionality(vector<string> clientRequestOperation, int clientSocket, string userID)
{
    string groupID = clientRequestOperation[1];
    string fileName = clientRequestOperation[2];
    if(groupAdminMapping.find(groupID) == groupAdminMapping.end())
    {
        write(clientSocket, "InvalidGroup", 12);
    }
    else if(seedersList[groupID].find(fileName) == seedersList[groupID].end())
    {
        write(clientSocket, "File not yet shared in the group", 32);
        infoLog("File not yet shared in the group");
    }
    else
    {
        seedersList[groupID][fileName].erase(userID);

        if(seedersList[groupID][fileName].size() == 0)
        {
            seedersList[groupID].erase(fileName);
        }
        write(clientSocket, "Sharing Stopped", 15);
        infoLog("Sharing Stopped for file "+fileName+" from groupName : "+groupID);
    }
}
