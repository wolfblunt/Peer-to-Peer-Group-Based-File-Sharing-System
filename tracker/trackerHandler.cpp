#include "trackerHeader.h"

vector<string> splitStringUtility(string inputString, string delimiter){
    size_t pos = 0;
    vector<string> splitStringVector;
    while ((pos = inputString.find(delimiter)) != string::npos)
    {
        string t = inputString.substr(0, pos);
        splitStringVector.push_back(t);
        inputString.erase(0, pos + delimiter.length());
    }
    splitStringVector.push_back(inputString);

    return splitStringVector;
}



void trackerClientOperations(int clientSocket)
{
    string clientID = "";
    string clientGroupID = "";
    infoLog("********** Thread Started For Client Socket Number " + to_string(clientSocket) + "*************");
    // cout<<"********** Thread Started For Client Socket Number " <<to_string(clientSocket)<<"************"<<endl;

    for(;;)
    {
        char inputArray[1024] = {0};

        if(read(clientSocket, inputArray, 1024) <=0 ) // Syntax: read(fd, buf, sizeof(buf)-1)
        {
            // when connection breaks between client and tracker
            isLoggedIn[clientID] = false;
            close(clientSocket);
            break;
        }

        string temp;
        string inp = string(inputArray);
        // cout<<"Client Request : "<<inp;

        stringstream ss(inp);
        vector<string> clientRequestOperation;
        while(ss >> temp)
        {
            clientRequestOperation.push_back(temp);
        } 

        if(clientRequestOperation[0] == "create_user")
        {
            if(clientRequestOperation.size() != 3)
            {
                write(clientSocket, "Invalid argument count", 22);
                infoLog(to_string(clientSocket) + " : Invalid argument count");
                // Syntax : write(int fs, const void *buf, size_t N);
                // Writes N bytes from buf to the file or socket associated with fs
            }
            else
            {
                if(createUserFunctionality(clientRequestOperation) == 0)
                {
                    write(clientSocket, "UserID Already Exists", 21);
                    infoLog(to_string(clientSocket) + " : UserID Already Exists");
                }
                else
                {
                    write(clientSocket, "Account Created", 15);
                    infoLog("Account Created for socketNumber: "+ to_string(clientSocket));
                }
            }
        }

        else if(clientRequestOperation[0] == "login")
        {
            if(clientRequestOperation.size() != 3)
            {

                write(clientSocket, "Invalid argument count", 22);
                infoLog(to_string(clientSocket) + " : Invalid argument count");
                // Syntax :  write(int fs, const void *buf, size_t N);
                // Writes N bytes from buf to the file or socket associated with fs
            }
            else
            {
                int result = loginCheck(clientRequestOperation);
                if(result == 0)
                {
                    write(clientSocket, "Incorrect UserName and Password", 31);
                    infoLog(to_string(clientSocket) + "Incorrect UserName and Password");
                }
                if(result == 1)
                {
                    write(clientSocket, "You are already logged in!!", 27);
                    infoLog(to_string(clientSocket) + "You are already logged in!!");
                }
                if(result == 2)
                {
                    write(clientSocket, "Login Successful!", 17);
                    clientID = clientRequestOperation[1];
                    char buffer[96];
                    read(clientSocket, buffer, 96);
                    string peerAddress = string(buffer);
                    userToPortMapping[clientID] = peerAddress;
                    infoLog(to_string(clientSocket) + " Login Successful!");
                }
            }
        }

        else if(clientRequestOperation[0] == "logout")
        {

            logoutFunction(clientID);
            write(clientSocket, "Logout Successful", 17);
            infoLog(to_string(clientSocket) + " Logout Successful");
            cout<<"LogOut successful\n";
        }

        else if(clientRequestOperation[0] == "create_group")
        {
            if(clientRequestOperation.size() != 2)
            {
                write(clientSocket, "Invalid argument count", 22);
                infoLog(to_string(clientSocket) + " Invalid argument count");
                // Syntax : write(int fs, const void *buf, size_t N);
                // Writes N bytes from buf to the file or socket associated with fs
            }
            int result = createGroupFunction(clientRequestOperation, clientSocket, clientID);
            if(result==1)
            {
                clientGroupID = clientRequestOperation[1];
                write(clientSocket, "Group Created Successfully", 26);
                infoLog(to_string(clientSocket) + "Group Created Successfully");
                cout<<"Group Created Successfully"<<"\n";
            }
            else
            {
                write(clientSocket, "Group Already Exists", 20);
                infoLog(to_string(clientSocket) + "Group Already Exists");
                cout<<"Group Already Exists"<<"\n";
            }
        }

        else if(clientRequestOperation[0] == "join_group")
        {
            if(clientRequestOperation.size() != 2)
            {
                write(clientSocket, "Invalid argument count", 22);
                infoLog(to_string(clientSocket) + "Invalid argument count");
            }
            else
            {
                joinGroupFunctionality(clientRequestOperation, clientID, clientSocket);
            }
        }

        else if(clientRequestOperation[0] == "list_groups")
        {
            if(clientRequestOperation.size() != 1)
            {
                write(clientSocket, "Invalid argument count", 22);
                infoLog(to_string(clientSocket) + "Invalid argument count");
                // Syntax : write(int fs, const void *buf, size_t N);
                // Writes N bytes from buf to the file or socket associated with fs
            }
            else
            {
                listGroupFunctionality(clientRequestOperation, clientSocket);
            }
        }

        else if(clientRequestOperation[0] == "list_requests")
        {
            if(clientRequestOperation.size() != 2)
            {
                write(clientSocket, "Invalid argument count", 22);
                infoLog(to_string(clientSocket) + "Invalid argument count");
            }
            else
            {
                listPendingRequests(clientRequestOperation, clientSocket, clientID);
            }
        }

        else if(clientRequestOperation[0] == "accept_request")
        {
            if(clientRequestOperation.size() != 3)
            {
                write(clientSocket, "Invalid argument count", 22);
                infoLog(to_string(clientSocket) + "Invalid argument count");
                // Syntax : write(int fs, const void *buf, size_t N);
                // Writes N bytes from buf to the file or socket associated with fs
            }
            else
            {
                acceptRequestFunctionality(clientRequestOperation, clientSocket, clientID);
            }
        }

        else if(clientRequestOperation[0] == "leave_group")
        {
            if(clientRequestOperation.size() != 2)
            {
                write(clientSocket, "Invalid argument count", 22);
                infoLog(to_string(clientSocket) + "Invalid argument count");
            }
            else
            {
                leaveGroupFunctionality(clientRequestOperation, clientSocket, clientID);
            }
        }

        else if(clientRequestOperation[0] == "upload_file")
        {
            if(clientRequestOperation.size() != 3)
            {
                write(clientSocket, "Invalid argument count", 22);
                infoLog(to_string(clientSocket) + "Invalid argument count");
            }
            else
            {
                uploadFileFunctionality(clientRequestOperation, clientSocket, clientID);
            }
        }

        else if(clientRequestOperation[0] == "list_files")
        {
            if(clientRequestOperation.size() != 2)
            {
                write(clientSocket, "Invalid argument count", 22);
                infoLog(to_string(clientSocket) + "Invalid argument count");
            }
            else
            {
                listAllSharableFilesInGroup(clientRequestOperation, clientSocket, clientID);
            }
        }

        else if(clientRequestOperation[0] == "download_file")
        {
            if(clientRequestOperation.size() != 4)
            {
                write(clientSocket, "Invalid argument count", 22);
                infoLog(to_string(clientSocket) + "Invalid argument count");
            }
            else
            {
                downloadFileFunctionality(clientRequestOperation, clientSocket, clientID);
            }
        }

        else if(clientRequestOperation[0] == "show_downloads")
        {
            if(clientRequestOperation.size() != 1)
            {
                write(clientSocket, "Invalid argument count", 22);
                infoLog(to_string(clientSocket) + "Invalid argument count");
            }
            else
            {
                write(clientSocket, "********** DOWNLOADS ***********", 32);
            }
        }

        else if(clientRequestOperation[0] == "stop_share")
        {
            if(clientRequestOperation.size() != 3)
            {
                write(clientSocket, "Invalid argument count", 22);
                infoLog(to_string(clientSocket) + "Invalid argument count");
            }
            else
            {
                stopShareFunctionality(clientRequestOperation, clientSocket, clientID);
            }
        }

        else
        {
            write(clientSocket, "Invalid Command", 15);
            infoLog(to_string(clientSocket) + " : Invalid Command");
        }
    }

    close(clientSocket);
}