#include "clientHeader.h"

// Returns combined piecewiseHash of the file
string getFilePiecesHash(string filePath, long long fileSize)
{
    int  i, accum;
    if(fileSize == -1){
        return "#";
    }
    int segments = fileSize/FILE_SEGMENT_SZ + 1;
    char line[SIZE + 1];
    string fileHashString = "";

    FILE *fp1 = fopen(filePath.c_str(), "r");

    if(fp1)
    { 
        for(i=0;i<segments;i++){
            accum = 0;
            string segmentString;

            int rc;
            while(accum < FILE_SEGMENT_SZ && (rc = fread(line, 1, min(SIZE-1, FILE_SEGMENT_SZ-accum), fp1)))
            {
                line[rc] = '\0';
                accum += strlen(line);
                segmentString += line;
                memset(line, 0, sizeof(line));
            }

            //############
            unsigned char md[20];
            if(SHA1(reinterpret_cast<const unsigned char *>(&segmentString[0]), segmentString.length(), md))
            {
                for(int i=0; i<20; i++){
                    char buf[3];
                    sprintf(buf, "%02x", md[i]&0xff);
                    fileHashString += string(buf);
                }
                
            }
            else{
                cout<<"Error in hashing"<<endl;
                errorLog("Error in hashing");
            }
            fileHashString += "##";
            //############

        }
        
        fclose(fp1);
    }
    else
    {
        cout<<"File Not Found"<<endl;
    }
    fileHashString.pop_back();
    fileHashString.pop_back();
    return fileHashString;
}
