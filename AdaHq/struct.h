#ifndef STRUCT_H_H
#define STRUCT_H_H
#include <string>

#define CODELEN 7
#define STRLEN 256

struct _md_index
{	
	char* SecurityID;
	char* Symbol;
	int TradeVolume;
	double TotalValueTraded;
	double PreClosePx;
	double OpenPrice;
	double HighPrice;
	double LowPrice;
	double TradePrice;
	double ClosePx;	
	char* TradingPhaseCode;
	char* TimeStamp;

	_md_index()
	{
		SecurityID = NULL;
		Symbol = NULL;
		TradeVolume = 0;
		TotalValueTraded = 0;
		PreClosePx = 0;
		OpenPrice = 0;
		HighPrice = 0;
		LowPrice = 0;
		TradePrice = 0;
		ClosePx = 0;		
		TradingPhaseCode = NULL;
		TimeStamp = NULL;
	};

	~_md_index()
	{
		if (SecurityID)
		{
			delete[]SecurityID;
			SecurityID = NULL;
		}
		if (Symbol)
		{
			delete[]Symbol;
			Symbol = NULL;
		}
		if (TradingPhaseCode)
		{
			delete[]TradingPhaseCode;
			TradingPhaseCode = NULL;
		}
		if (TimeStamp)
		{
			delete[]TimeStamp;
			TimeStamp = NULL;
		}
	};
};

struct _md_stock
{
	char* SecurityID;
	char* Symbol;
	int TradeVolume;
	double TotalValueTraded;
	double PreClosePx;
	double OpenPrice;
	double HighPrice;
	double LowPrice;
	double TradePrice;
	double ClosePx;
	double BuyPrice1;
	int BuyVolume1;
	double SellPrice1;
	int SellVolume1;
	double BuyPrice2;
	int BuyVolume2;
	double SellPrice2;
	int SellVolume2;
	double BuyPrice3;
	int BuyVolume3;
	double SellPrice3;
	int SellVolume3;
	double BuyPrice4;
	int BuyVolume4;
	double SellPrice4;
	int SellVolume4;
	double BuyPrice5;
	int BuyVolume5;
	double SellPrice5;
	int SellVolume5;
	char* TradingPhaseCode;
	char* TimeStamp;

	_md_stock()
	{
		SecurityID = NULL;
		Symbol = NULL;
		TradeVolume = 0;
		TotalValueTraded = 0;
		PreClosePx = 0;
		OpenPrice = 0;
		HighPrice = 0;
		LowPrice = 0;
		TradePrice = 0;
		ClosePx = 0;
		BuyPrice1 = 0;
		BuyVolume1 = 0;
		SellPrice1 = 0;
		SellVolume1 = 0;
		BuyPrice2 = 0;
		BuyVolume2 = 0;
		SellPrice2 = 0;
		SellVolume2 = 0;
		BuyPrice3 = 0;
		BuyVolume3 = 0;
		SellPrice3 = 0;
		SellVolume3 = 0;
		BuyPrice4 = 0;
		BuyVolume4 = 0;
		SellPrice4 = 0;
		SellVolume4 = 0;
		BuyPrice5 = 0;
		BuyVolume5 = 0;
		SellPrice5 = 0;
		SellVolume5 = 0;
		TradingPhaseCode = NULL;
		TimeStamp = NULL;
	};

	~_md_stock()
	{
		if (SecurityID)
		{
			delete[]SecurityID;
			SecurityID = NULL;
		}
		if (Symbol)
		{
			delete[]Symbol;
			Symbol = NULL;
		}
		if (TradingPhaseCode)
		{
			delete[]TradingPhaseCode;
			TradingPhaseCode = NULL;
		}
		if (TimeStamp)
		{
			delete[]TimeStamp;
			TimeStamp = NULL;
		}
	};
};

#endif