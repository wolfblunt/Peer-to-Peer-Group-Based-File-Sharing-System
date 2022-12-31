#include "clientHeader.h"

class seederDetails
{
    public:
        string seederAddress;
        string fileName;
        long long int fileSize;
};

class downloadChunkDetail
{
    public:
    string seederIP;
    string fileName;
    string destination;
    long long int chunkNumber;
};


void getFileChunkInformation(seederDetails* sd)
{
    infoLog("Getting chunk info of : "+ sd->fileName + " from "+ sd->seederAddress);

    vector<string> seederAddressArray = splitString(string(sd->seederAddress), ":");
    string seederIP = seederAddressArray[0];
    string seederPort = seederAddressArray[1];
    string executeIns = "getChunkVectorInformation##" + string(sd->fileName);
    string seederResponse = connectToSeeder(seederIP, seederPort, executeIns);

    for(size_t i=0;i<currentDownloadFileChunks.size();i++)
    {
        if(seederResponse[i] == '1')
        {
            currentDownloadFileChunks[i].push_back(string(sd->seederAddress));
        }
    }

    delete sd;
    return;
}

void sendChunkData(string filePath, int chunkNumber, int clientSocket)
{
    std::ifstream fp1(filePath, std::ios::in|std::ios::binary);
    fp1.seekg(chunkNumber*FILE_SEGMENT_SZ, fp1.beg);

    infoLog("sending data starting at " + to_string(fp1.tellg()));
    char buffer[FILE_SEGMENT_SZ] = {0}; 
    int rc = 0;
    string sent = "";

    fp1.read(buffer, sizeof(buffer));
    int count = fp1.gcount();

    if ((rc = send(clientSocket, buffer, count, 0)) == -1) {
        perror("Error in sending file.");
        exit(1);
    }
    
    infoLog("sent till "+to_string(fp1.tellg()));

    // The tellg() function is used with input streams, and returns the current “get” position of the pointer in the stream. 
    // It has no parameters and returns a value of the member type pos_type, which is an integer data type
    // representing the current position of the get stream pointer.

    fp1.close();
}

void fetchChunkData(downloadChunkDetail* requestedChunkDetails)
{
    string fileName = requestedChunkDetails->fileName;
    vector<string> seederAddress = splitString(requestedChunkDetails->seederIP, ":");
    string seederIP = seederAddress[0];
    string seederPort = seederAddress[1];
    long long int chunkNumber = requestedChunkDetails->chunkNumber;
    string destinationPath = requestedChunkDetails->destination;
    string executeIns = "fetchChunkData##" + fileName + "##" + to_string(chunkNumber) + "##" + destinationPath;
    connectToSeeder(seederIP, seederPort, executeIns);

    delete requestedChunkDetails;
    return;
}

void storeChunkData(int seederSocket, long long int chunkNumber, string destinationPath)
{
    int n, tot = 0;
    char buffer[FILE_SEGMENT_SZ];

    string content = "";
    while (tot < FILE_SEGMENT_SZ) {
        n = read(seederSocket, buffer, FILE_SEGMENT_SZ-1);
        if (n <= 0){
            break;
        }
        buffer[n] = 0;
        fstream outfile(destinationPath, std::fstream::in | std::fstream::out | std::fstream::binary);
        outfile.seekp(chunkNumber*FILE_SEGMENT_SZ+tot, ios::beg);
        outfile.write(buffer, n);
        outfile.close();

        infoLog("written at: "+ to_string(chunkNumber*FILE_SEGMENT_SZ + tot));
        infoLog("written till: " + to_string(chunkNumber*FILE_SEGMENT_SZ + tot + n-1) +"\n");

        content += buffer;
        tot += n;
        bzero(buffer, FILE_SEGMENT_SZ);
    }
    
    string fileHashString = "";
    // ###################################
    unsigned char md[20];
    if(SHA1(reinterpret_cast<const unsigned char *>(&content[0]), content.length(), md))
    {
        for(int i=0; i<20; i++){
            char buf[3];
            sprintf(buf, "%02x", md[i]&0xff);
            fileHashString += string(buf);
        }
        
    }
    else{
        cout<<"Error in hashing"<<endl;
    }
    // fileHashString += "##";
    // ###################################
    // fileHashString.pop_back();
    // fileHashString.pop_back();
    if(fileHashString != downloadFilePiecewiseHashMap[chunkNumber]){
        isFileCorrupted = true;
    } 
    
    string filename = splitString(string(destinationPath), "/").back();
    setChunkVector(filename, chunkNumber, chunkNumber, false);
}

void pieceWiseAlgorithm(vector<string> inputVector, vector<string> seedersWithFileInfo, long long int downloadFileSize, long long int noOfSegments)
{
    // infoLog("PieceWise Algorithm Started");
    string fileName = inputVector[2];
    vector<thread> thread1;
    vector<thread> thread2;

    currentDownloadFileChunks.clear();
    currentDownloadFileChunks.resize(noOfSegments);
    for(size_t i=0; i< seedersWithFileInfo.size(); i++)
    {
        seederDetails *sd = new seederDetails();
        sd->seederAddress = seedersWithFileInfo[i];
        sd->fileName = fileName;
        sd->fileSize = noOfSegments;
        thread1.push_back(thread(getFileChunkInformation, sd));
    }

    for(auto it=thread1.begin(); it!=thread1.end();it++)
    {
        if(it->joinable())
        {
            it->join();
        }
    }

    for(size_t i=0;i<currentDownloadFileChunks.size(); i++)
    {
        if(currentDownloadFileChunks.size() == 0)
        {
            cout<<"INFO: All chunks of the file are not available" << endl;
            return;
        }
    }

    thread1.clear();
    srand((unsigned) time(0));
    long long int noOfSegmentsReceived = 0;

    string downloadPath = inputVector[3] + "/" + fileName;

    FILE* fp = fopen(&downloadPath[0], "r+");
	if(fp != 0){
		cout<<"Requested Download File already exists\n";
        fclose(fp);
        return;
	}

    // Creating Empty File
    string ss(downloadFileSize, '\0');  
    fstream in(downloadPath.c_str(),ios::out|ios::binary);
    in.write(ss.c_str(),strlen(ss.c_str()));  
    in.close();

    fileChunkInfo[fileName].resize(noOfSegments, 0);
    isFileCorrupted = false;

    string peerToGetFilepath;
    while(noOfSegmentsReceived < noOfSegments)
    {
        infoLog("getting segment no: " + to_string(noOfSegmentsReceived));
        long long int randomChunk;
        for(;;)
        {
            randomChunk = rand()%noOfSegments;
            if(fileChunkInfo[fileName][randomChunk] == 0)
            {
                break;
            }
        }
        infoLog("randomChunk = " + to_string(randomChunk));

        long long int peersWithThisChunk = currentDownloadFileChunks[randomChunk].size();
        long long int randUser = rand()%peersWithThisChunk;
        string randomSeeder = currentDownloadFileChunks[randomChunk][randUser];

        downloadChunkDetail* requestedChunkDetails = new downloadChunkDetail();
        requestedChunkDetails->fileName = fileName;
        requestedChunkDetails->chunkNumber = randomChunk;
        requestedChunkDetails->seederIP = randomSeeder;
        requestedChunkDetails->destination = downloadPath;

        infoLog("starting thread for chunk number "+ to_string(requestedChunkDetails->chunkNumber));

        fileChunkInfo[fileName][randomChunk] = 1;

        thread2.push_back(thread(fetchChunkData, requestedChunkDetails));
        noOfSegmentsReceived++;
        peerToGetFilepath = randomSeeder;
    }

    for(auto it=thread2.begin(); it!=thread2.end();it++)
    {
        if(it->joinable())
        {
            it->join();
        }
    }

    if(isFileCorrupted)
    {
        cout << "Downloaded completed" << endl;
    }
    else
    {
         cout << "Download completed" << endl;
    }

    downloadedFiles.insert({fileName, inputVector[1]});
    // fileFilePathMapping[fileName] = inputVector[3];
    vector<string> seederAddress = splitString(peerToGetFilepath, ":");
    connectToSeeder(&seederAddress[0][0], &seederAddress[1][0], "getFilePath##"+fileName);
    return;
}

int downLoadFunctionality(vector<string> inputVector, int traSocket, string fileName, string groupID)
{
    // inputVector = [groupID, fileName, destinationPath]
    string fileDetails = "";
    string destPath = inputVector[2];
    fileDetails += destPath + "##" + fileName + "##" + groupID;
    
    // infoLog("sending file details for download : " + fileDetails);

    if(send(traSocket, fileDetails.c_str(), fileDetails.length(), MSG_NOSIGNAL) == -1)
    {
        cout<<"ERROR : "<<strerror(errno);
        return -1;
    }

    char serverResponse[512000];
    read(traSocket, serverResponse, 512000);

    if(string(serverResponse) == "FileNotFound")
    {
        cout << serverResponse << endl;
        return 0;
    }

    vector<string> seedersWithFileInfo = splitString(string(serverResponse), "##");

    bzero(serverResponse, 512000);
    read(traSocket , serverResponse, 512000);

    downloadFilePiecewiseHashMap = splitString(string(serverResponse), "##");

    long long int downloadFileSize = stoll(seedersWithFileInfo.back());
    seedersWithFileInfo.pop_back();
    
    long long int noOfSegments = downloadFileSize / FILE_SEGMENT_SZ+1;
    infoLog("Total Number of segments required : "+to_string(noOfSegments));
    pieceWiseAlgorithm(inputVector, seedersWithFileInfo, downloadFileSize, noOfSegments);

    return 0;
}

int downLoadFileHandler(vector<string> inputVector, int traSocket, string serverReply)
{
    string groupID = inputVector[1];
    string fileName = inputVector[2];
    if(downloadedFiles.find(fileName) != downloadedFiles.end())
    {
        cout<<"File Already Downloaded"<<endl;
        infoLog("File Already Downloaded");
        return 0;
    }
    else if(string(serverReply) == "InvalidGroup")
    {
        cout<<"Group "<<groupID<<" does not exist"<<endl;
        infoLog("Group "+groupID+" does not exist");
        return 0;
    }
    else if(string(serverReply) == "InvalidUser")
    {
        cout<<"You are not a member of group "<<groupID<<endl;
        infoLog("You are not a member of group "+groupID);
        return 0;
    }
    else if(string(serverReply) == "InvalidPath")
    {
        cout<<"Invalid Destination Path"<<endl;
        infoLog("Invalid Destination Path");
        return 0;
    }

    return downLoadFunctionality(inputVector, traSocket, fileName, groupID);
}

void showDownloads()
{
    infoLog("Printing all the downloaded files");
    if(downloadedFiles.size() == 0)
    {
        cout<<"[-] NO DOWNLOADS TILL NOW"<<endl;
        infoLog("No Download till now");
    }
    for(auto i: downloadedFiles)
    {
        cout << "[-] " << i.second << " " << i.first << endl;
    }
}

void stopshareHandler(vector<string> inputVector)
{
    string groupName = inputVector[1];
    string fileName = inputVector[2]; 
    fileUploadedMapping[groupName].erase(fileName);
    infoLog("Stopped Sharing the file");
}