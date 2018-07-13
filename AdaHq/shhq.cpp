#include "shhq.h"
#include "tool.h"
#include "struct.h"
#include "server.h"



extern HANDLE hMutex;
extern queue<string> msgqueue;

map<string, int> shhqVolMap;

extern char file_dir[50];

void shhq::read_mktdt00()
{
	string contents = "";
	try
	{
		char path[200] = "";
		strcat(path, file_dir);
		strcat(path, "\\shhq\\mktdt00.txt");
		ifstream infile(path);		
		stringstream buffer;		
		buffer << infile.rdbuf();
		infile.close();
		contents = buffer.str();
	}
	catch (std::bad_alloc)
	{
		cout << "shhq exception memory bad_alloc\n" << endl;		
		return;
	}	

	vector<char*> lines = splitbuff((char*)contents.c_str(), "\n");
	
	vector<char*>::iterator it = lines.begin();	

	
	for (; it!=lines.end(); ++it)
	{
		vector<char*> linedata = splitbuff(*it, "|");	
		
		if (strcmp(linedata[0], "TRAILER") == 0)
		{
			break;
		}		
		
		else if (strcmp(linedata[0], "MD001") == 0)
		{
			if (linedata.size() == 13)
			{				
				_md_index* md_index = new _md_index;

				md_index->SecurityID = new char[CODELEN];
				memcpy(md_index->SecurityID, ltrim_lc(linedata[1]), CODELEN);
				md_index->Symbol = new char[CODELEN];
				memcpy(md_index->Symbol, ltrim_lc(linedata[2]), CODELEN);
				md_index->TradeVolume = atoi(ltrim_lc(linedata[3]));
				md_index->TotalValueTraded = atof(ltrim_lc(linedata[4]));
				md_index->PreClosePx = atof(ltrim_lc(linedata[5]));
				md_index->OpenPrice = atof(ltrim_lc(linedata[6]));
				md_index->HighPrice = atof(ltrim_lc(linedata[7]));
				md_index->LowPrice = atof(ltrim_lc(linedata[8]));
				md_index->TradePrice = atof(ltrim_lc(linedata[9]));
				md_index->ClosePx = atof(ltrim_lc(linedata[10]));
				md_index->TradingPhaseCode = new char[CODELEN];
				memcpy(md_index->TradingPhaseCode, ltrim_lc(linedata[11]), CODELEN);
				md_index->TimeStamp = new char[CODELEN];
				memcpy(md_index->TimeStamp, ltrim_lc(linedata[12]), CODELEN);
			
				char code[CODELEN];
				if (strcmp(md_index->SecurityID,"000905")==0)
					memcpy(code, "399905", CODELEN);
				else if (strcmp(md_index->SecurityID, "000016") == 0)
					memcpy(code, "999987", CODELEN);
				else if (strcmp(md_index->SecurityID, "000300") == 0)
					memcpy(code, "399300", CODELEN);
				else if (strcmp(md_index->SecurityID, "000001") == 0)
					memcpy(code, "999999", CODELEN);
				else 
					continue;
							
				
				int nowvol = md_index->TradeVolume;
				int nowincrevol = 0;
				if (shhqVolMap.find(code) != shhqVolMap.end())
				{
					nowincrevol = nowvol - shhqVolMap[code];
					shhqVolMap[code] = nowvol;
				}
				else
				{
					nowincrevol = nowvol;
					shhqVolMap.insert(pair<string, int>(code, nowvol));
				}

				char timestr[7];
				timestr[0] = md_index->TimeStamp[0];
				timestr[1] = md_index->TimeStamp[1];
				timestr[2] = md_index->TimeStamp[3];
				timestr[3] = md_index->TimeStamp[4];
				timestr[4] = md_index->TimeStamp[6];
				timestr[5] = md_index->TimeStamp[7];					
				timestr[6] = '\0';			

				
				SYSTEMTIME sys;
				GetLocalTime(&sys);

				char stockquote[1024];
				memset(stockquote, 0, sizeof(stockquote));
				sprintf(stockquote, "%s,%s,%.3f,%d", code, timestr, md_index->TradePrice, (nowincrevol));
				sprintf(stockquote, "%s,%.3f", stockquote, md_index->PreClosePx);
				sprintf(stockquote, "%s,%s", stockquote, "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0");
				sprintf(stockquote, "%s,%02d:%02d:%02d;", stockquote, sys.wHour, sys.wMinute, sys.wSecond);
								
				WaitForSingleObject(hMutex, INFINITE);
				msgqueue.push(stockquote);
				ReleaseMutex(hMutex);			

				if (md_index->SecurityID)
				{
					delete[]md_index->SecurityID;
					md_index->SecurityID = NULL;
				}
				if (md_index->Symbol)
				{
					delete[]md_index->Symbol;
					md_index->Symbol = NULL;
				}
				if (md_index->TradingPhaseCode)
				{
					delete[]md_index->TradingPhaseCode;
					md_index->TradingPhaseCode = NULL;
				}
				if (md_index->TimeStamp)
				{
					delete[]md_index->TimeStamp;
					md_index->TimeStamp = NULL;
				}

				if (md_index)
				{
					delete md_index;
					md_index = NULL;
				}
			}			
		}
		
		else if (strcmp(linedata[0], "MD002") == 0)
		{
			char code[CODELEN];
			memcpy(code, ltrim_lc(linedata[1]), CODELEN);			
			
			if (linedata.size() == 33 && code[0] == '6')
			{	
				_md_stock* md_stock = new _md_stock;
				md_stock->SecurityID = new char[CODELEN];
				memcpy(md_stock->SecurityID, code, CODELEN);
				md_stock->Symbol = new char[STRLEN];
				memcpy(md_stock->Symbol, ltrim_lc(linedata[2]), STRLEN);
				md_stock->TradeVolume = atoi(ltrim_lc(linedata[3]));
				md_stock->TotalValueTraded = atof(ltrim_lc(linedata[4]));
				md_stock->PreClosePx = atof(ltrim_lc(linedata[5]));
				md_stock->OpenPrice = atof(ltrim_lc(linedata[6]));
				md_stock->HighPrice = atof(ltrim_lc(linedata[7]));
				md_stock->LowPrice = atof(ltrim_lc(linedata[8]));
				md_stock->TradePrice = atof(ltrim_lc(linedata[9]));
				md_stock->ClosePx = atof(ltrim_lc(linedata[10]));
				md_stock->BuyPrice1 = atof(ltrim_lc(linedata[11]));
				md_stock->BuyVolume1 = atoi(ltrim_lc(linedata[12]));
				md_stock->SellPrice1 = atof(ltrim_lc(linedata[13]));
				md_stock->SellVolume1 = atoi(ltrim_lc(linedata[14]));
				md_stock->BuyPrice2 = atof(ltrim_lc(linedata[15]));
				md_stock->BuyVolume2 = atoi(ltrim_lc(linedata[16]));
				md_stock->SellPrice2 = atof(ltrim_lc(linedata[17]));
				md_stock->SellVolume2 = atoi(ltrim_lc(linedata[18]));
				md_stock->BuyPrice3 = atof(ltrim_lc(linedata[19]));
				md_stock->BuyVolume3 = atoi(ltrim_lc(linedata[20]));
				md_stock->SellPrice3 = atof(ltrim_lc(linedata[21]));
				md_stock->SellVolume3 = atoi(ltrim_lc(linedata[22]));
				md_stock->BuyPrice4 = atof(ltrim_lc(linedata[23]));
				md_stock->BuyVolume4 = atoi(ltrim_lc(linedata[24]));
				md_stock->SellPrice4 = atof(ltrim_lc(linedata[25]));
				md_stock->SellVolume4 = atoi(ltrim_lc(linedata[26]));
				md_stock->BuyPrice5 = atof(ltrim_lc(linedata[27]));
				md_stock->BuyVolume5 = atoi(ltrim_lc(linedata[28]));
				md_stock->SellPrice5 = atof(ltrim_lc(linedata[29]));
				md_stock->SellVolume5 = atoi(ltrim_lc(linedata[30]));
				md_stock->TradingPhaseCode = new char[STRLEN];
				memcpy(md_stock->TradingPhaseCode, ltrim_lc(linedata[31]), STRLEN);
				md_stock->TimeStamp = new char[STRLEN];
				memcpy(md_stock->TimeStamp, ltrim_lc(linedata[32]), STRLEN);


				int nowvol = md_stock->TradeVolume;
				int nowincrevol = 0;
				if (shhqVolMap.find(md_stock->SecurityID) != shhqVolMap.end())
				{
					nowincrevol = nowvol - shhqVolMap[md_stock->SecurityID];
					shhqVolMap[md_stock->SecurityID] = nowvol;
				}
				else
				{
					nowincrevol = nowvol;
					shhqVolMap.insert(pair<string, int>(md_stock->SecurityID, nowvol));
				}

				char timestr[7];
				timestr[0] = md_stock->TimeStamp[0];
				timestr[1] = md_stock->TimeStamp[1];
				timestr[2] = md_stock->TimeStamp[3];
				timestr[3] = md_stock->TimeStamp[4];
				timestr[4] = md_stock->TimeStamp[6];
				timestr[5] = md_stock->TimeStamp[7];
				timestr[6] = '\0';

				SYSTEMTIME sys;
				GetLocalTime(&sys);

				char stockquote[1024];
				memset(stockquote, 0, sizeof(stockquote));
				sprintf(stockquote, "%s,%s,%.3f,%d", md_stock->SecurityID, timestr, md_stock->TradePrice, nowincrevol);
				sprintf(stockquote, "%s,%.3f", stockquote, md_stock->PreClosePx);
				sprintf(stockquote, "%s,%.3f,%d", stockquote, md_stock->SellPrice1, md_stock->SellVolume1);
				sprintf(stockquote, "%s,%.3f,%d", stockquote, md_stock->BuyPrice1, md_stock->BuyVolume1);
				sprintf(stockquote, "%s,%.3f,%d", stockquote, md_stock->SellPrice2, md_stock->SellVolume2);
				sprintf(stockquote, "%s,%.3f,%d", stockquote, md_stock->BuyPrice2, md_stock->BuyVolume2);
				sprintf(stockquote, "%s,%.3f,%d", stockquote, md_stock->SellPrice3, md_stock->SellVolume3);
				sprintf(stockquote, "%s,%.3f,%d", stockquote, md_stock->BuyPrice3, md_stock->BuyVolume3);
				sprintf(stockquote, "%s,%.3f,%d", stockquote, md_stock->SellPrice4, md_stock->SellVolume4);
				sprintf(stockquote, "%s,%.3f,%d", stockquote, md_stock->BuyPrice4, md_stock->BuyVolume4);
				sprintf(stockquote, "%s,%.3f,%d", stockquote, md_stock->SellPrice5, md_stock->SellVolume5);
				sprintf(stockquote, "%s,%.3f,%d", stockquote, md_stock->BuyPrice5, md_stock->BuyVolume5);
				sprintf(stockquote, "%s,%02d:%02d:%02d;", stockquote, sys.wHour, sys.wMinute, sys.wSecond);


				if (strcmp(md_stock->SecurityID, "601688") == 0)
					printf("%s\n", stockquote);

				WaitForSingleObject(hMutex, INFINITE);
				msgqueue.push(stockquote);
				ReleaseMutex(hMutex);			
				
				if (md_stock->SecurityID)
				{
					delete[]md_stock->SecurityID;
					md_stock->SecurityID = NULL;
				}
				if (md_stock->Symbol)
				{
					delete[]md_stock->Symbol;
					md_stock->Symbol = NULL;
				}
				if (md_stock->TradingPhaseCode)
				{
					delete[]md_stock->TradingPhaseCode;
					md_stock->TradingPhaseCode = NULL;
				}
				if (md_stock->TimeStamp)
				{
					delete[]md_stock->TimeStamp;
					md_stock->TimeStamp = NULL;
				}

				if (md_stock)
				{
					delete md_stock;
					md_stock = NULL;
				}
			}			
		}
		
	}
};
