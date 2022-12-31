#include "trackerHeader.h"

string tracker1_ip, tracker2_ip, curTrackerIP, logFileName;
uint16_t tracker1_port, tracker2_port, curTrackerPort;
unordered_map<string, string> userToPortMapping;
unordered_map<string, string> userCredentials;
unordered_map<string, bool> isLoggedIn;
vector<string> groupArray; // all the groups
unordered_map<string, string> groupAdminMapping;  // <groupName, groupAdminID>
unordered_map<string, set<string>> groupMembersMapping;
unordered_map<string, vector<string>> groupMembersPendngRequests;  // <groupName, <pendingIDs>>
unordered_map<string, string> filePieceHashMap;  // <fileName, filehashPieces>
unordered_map<string, unordered_map<string, set<string>>> seedersList; // <groupName, {mapOfFiles, <peerAddresses>}>
unordered_map<string, string> fileSize; // <fileName, fileSize>