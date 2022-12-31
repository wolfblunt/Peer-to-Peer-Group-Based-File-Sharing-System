#include "clientHeader.h"

string tracker1_ip, tracker2_ip, peerIp, logFileName;
uint16_t peerPort, tracker1_port, tracker2_port;
unordered_map<string, unordered_map<string, bool>> fileUploadedMapping; // <groupName, <fileName, bool>>
unordered_map<string, string> fileFilePathMapping; // <fileName, filePath>
unordered_map<string, vector<int>> fileChunkInfo;  // <fileName, <1,0,0,1,...>>
vector<string> downloadFilePiecewiseHashMap;
vector<vector<string>> currentDownloadFileChunks;
unordered_map<string, string> downloadedFiles;
bool isFileCorrupted;
bool loggedIn;