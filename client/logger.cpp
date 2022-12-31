#include "clientHeader.h"

void clearLog(){
    ofstream out;
    out.open(logFileName);
    out.clear();
    out.close();
}

void debugLog(const string &text ){
    ofstream log_file(logFileName, ios_base::out | ios_base::app );
    log_file << "DEBUG : " << text << endl;
}

void infoLog(const string &text ){
    ofstream log_file(logFileName, ios_base::out | ios_base::app );
    log_file << "INFO : " << text << endl;
}

void errorLog(const string &text ){
    ofstream log_file(logFileName, ios_base::out | ios_base::app );
    log_file << "ERROR : " << text << endl;
}