#include <bits/stdc++.h>
#include <openssl/sha.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <signal.h> 
#include <string.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
#include <stdarg.h> 
#include <errno.h> 
#include <fcntl.h>
#include <sys/time.h> 
#include <sys/ioctl.h> 
#include <netdb.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
using namespace std; 


#define SA struct sockaddr 

extern string logFileName;
extern uint16_t tracker1_port, tracker2_port, curTrackerPort;
extern string tracker1_ip, tracker2_ip, curTrackerIP;
extern unordered_map<string, bool> isLoggedIn;
extern unordered_map<string, string> userCredentials;
extern unordered_map<string, string> userToPortMapping;
extern vector<string> groupArray;
extern unordered_map<string, string> groupAdminMapping;
extern unordered_map<string, set<string>> groupMembersMapping;
extern unordered_map<string, vector<string>> groupMembersPendngRequests;
extern unordered_map<string, string> filePieceHashMap;
extern unordered_map<string, unordered_map<string, set<string>>> seedersList;
extern unordered_map<string, string> fileSize;

void fetchArguments(int, char **);
void* programExitCheck(void*);
vector<string> splitStringUtility(string, string);
void trackerClientOperations(int);
void joinGroupFunctionality(vector<string>, string, int);
void acceptRequestFunctionality(vector<string>, int, string);
void leaveGroupFunctionality(vector<string>, int, string);
void uploadFileFunctionality(vector<string>, int, string);
void listAllSharableFilesInGroup(vector<string>, int, string);
void stopShareFunctionality(vector<string>, int, string);
int createUserFunctionality(vector<string>);
int loginCheck(vector<string>);
void logoutFunction(string);
int createGroupFunction(vector<string>, int, string);
void joinGroupFunctionality(vector<string>, string, int);
void listGroupFunctionality(vector<string>, int);
void listPendingRequests(vector<string>, int, string);
void downloadFileFunctionality(vector<string>, int, string);
void clearLog();
void debugLog(const string &);
void infoLog(const string &);