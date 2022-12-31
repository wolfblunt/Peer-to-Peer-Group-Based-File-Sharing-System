#include <bits/stdc++.h>
#include <openssl/sha.h>
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
using namespace std;

#define FILE_SEGMENT_SZ 512000
#define SIZE 32768
#define SA struct sockaddr 

extern string tracker1_ip, tracker2_ip, peerIp;
extern uint16_t peerPort, tracker1_port, tracker2_port;
extern bool loggedIn;
extern unordered_map<string, unordered_map<string, bool>> fileUploadedMapping; // <group, <fileName, bool>>
extern unordered_map<string, string> fileFilePathMapping; // <fileName, filePath>
extern unordered_map<string, vector<int>> fileChunkInfo;
extern vector<string> downloadFilePiecewiseHashMap;
extern vector<vector<string>> currentDownloadFileChunks;
extern unordered_map<string, string> downloadedFiles;
extern bool isFileCorrupted;
extern string logFileName;


void fetchArguments(int, char **);
int list_requests(int);
void acceptRequestFunctionality(int);
void leaveGroupFunctionality(int);
int clientOperations(vector<string>, int);
vector<string> splitString(string, string);
string getFilePiecesHash(string, long long);
void setChunkVector(string, long long int, long long int, bool);
int uploadFileFunctionality(vector<string>, int, string);
void listAllSharableFilesInGroup(int, vector<string>);
int listGroupsFunctionality(int);
void listPendingRequests(int);
void* clientAsServer(void*);
long long getFileSize(string);
int downLoadFunctionality(vector<string>, int, string, string);
void pieceWiseAlgorithm(vector<string>, vector<string>, long long int, long long int);
void sendChunkData(string, int, int);
void storeChunkData(int, long long int cnkNum, string);
int downLoadFileHandler(vector<string>, int, string);
void handleClientRequest(int);
string connectToSeeder(string, string, string);
void stopshareHandler(vector<string>);
void showDownloads();
void clearLog();
void debugLog(const string &);
void infoLog(const string &);
void errorLog(const string &);
