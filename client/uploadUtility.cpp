#include "clientHeader.h"

void setChunkVector(string filename, long long int l, long long int r, bool isUpload){
    if(isUpload)
    {
        vector<int> tmp(r-l+1, 1);
        fileChunkInfo[filename] = tmp;
    }
    else{
        fileChunkInfo[filename][l] = 1;
        infoLog("chunk vector updated for "+filename+ " at " + to_string(l));
    }
}

int uploadFileFunctionality(vector<string> inputVector, int trackerSocket, string serverReply)
{     
    // bzero(serverResponse, 10240);
    // read(trackerSocket , serverResponse, 10240);
    if(string(serverReply) == "Group Not Exist")
    {
        cout<<"Group "<<inputVector[2]<<" does not exist"<<endl;
        infoLog("Group "+inputVector[2]+" does not exist");
        return 0;
    }
    else if(string(serverReply) == "Not Belong To This Group")
    {
        cout<<"You are not a member of group"<<inputVector[2]<<endl;
        infoLog("You are not a member of group"+inputVector[2]);
        return 0;
    }
    else if(string(serverReply) == "Invalid File Path")
    {
        cout<<"Invalid File Path"<<endl;
        errorLog("Invalid File Path");
        return 0;
    }
    
    string groupID = inputVector[2];
    string uploadFileName = splitString(inputVector[1], "/").back();

    if(fileUploadedMapping[groupID].find(uploadFileName) == fileUploadedMapping[groupID].end())
    {
        fileUploadedMapping[groupID][uploadFileName] = true;
        fileFilePathMapping[uploadFileName] = inputVector[1];
        infoLog("Upload File Path : "+fileFilePathMapping[uploadFileName]);
    }
    else
    {
        if(send(trackerSocket , "error" , 5 , MSG_NOSIGNAL ) == -1){
            cout<<"Error : "<<strerror(errno)<<endl;
            errorLog(strerror(errno));
            return -1;
        }
        return 0;
    }

    string filePath = inputVector[1];
    long long fileSize = getFileSize(filePath);
    string filePiecesHash = getFilePiecesHash(filePath, fileSize);
    string fileSize_str = to_string(fileSize);

    if(filePiecesHash == "#")
    {
        return 0;
    }
    
    string uploadFileDetails = "";
    uploadFileDetails += string(filePath) + "##";
    uploadFileDetails += string(peerIp) + ":" + to_string(peerPort) + "##";
    uploadFileDetails += fileSize_str + "##";
    uploadFileDetails += filePiecesHash;
    if(send(trackerSocket , &uploadFileDetails[0] , strlen(&uploadFileDetails[0]) , MSG_NOSIGNAL ) == -1)
    {
        cout<<"Error: "<<strerror(errno)<<endl;
        errorLog(strerror(errno));
        return -1;
    }
    char serverResponse[10240];
    bzero(serverResponse, 10240);
    read(trackerSocket , serverResponse, 4096);
    cout << "\nServer Response : " <<serverResponse << endl;
    infoLog("Server Response for File Upload "+string(serverResponse));

    setChunkVector(uploadFileName, 0, stoll(fileSize_str)/FILE_SEGMENT_SZ + 1, true);

    return 0;
}
