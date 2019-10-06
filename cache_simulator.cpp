#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

using namespace std;

int convertTypeToNumber(string line) {
	char dOrIString[2];
	int dOrI;
	dOrIString[0] = line[0];
	dOrI = (int)strtol(dOrIString, NULL, 16);
	return dOrI;
}

int convertDataToNumber(string line) {
	int i = 2, data;
	char temp[100];
	while (i != 8) {
		temp[i - 2] = line[i];
		i++;
	}
	data = (int)strtol(temp, NULL, 16);
	return data;
}

int main()
{
	int combinedCache[2][32768], instructionCache[2][16384], dataCache[2][16384];		// [0][index] -> tag, [1][index] -> LRU priority
	char fileName[15] = "trace.din";
	string line;
	int data, dOrI, sOrC, blockSize, blockSizeSelect, associativitySelect, associativity;
	int tagLength, indexLength, offsetLength, cacheSize, offsetTemp, indexTemp, tagTemp;
	int dataTag, dataIndex, dataOffset;
	int combinedFetches = 0, combinedHit = 0, instructionFetches = 0, dataFetches = 0, instructionHit = 0, dataHit = 0;
//***************************************************************************************
// Menu to input data																	*
//***************************************************************************************
	{
		cout << "\nSelect\n1. Split cache\n2. Combined Cache\n\t";
		cin >> sOrC;
		switch (sOrC) {
		case 1:
			cacheSize = 16384;
			break;
		case 2:
			cacheSize = 32768;
			break;
		}
		cout << "\nBlock size\n1. 8B\n2. 32B\n3. 128B\n\t";
		cin >> blockSizeSelect;
		switch (blockSizeSelect) {
		case 1:
			blockSize = 8;
			break;
		case 2:
			blockSize = 32;
			break;
		case 3:
			blockSize = 128;
			break;
		}
		cout << "\nSelect associativity\n1. Direct Mapped\n2. 4-Way associativity\n\t";
		cin >> associativitySelect;
		switch (associativitySelect) {
		case 1:
			associativity = 1;
			break;
		case 2:
			associativity = 4;
			break;
		}
		ifstream in(fileName);
		if (!in) {
			cout << "\nFile does not exist";
		}
	}

	offsetLength = (int)(log2(blockSize));
	indexLength = (int)(log2(cacheSize / blockSize));
	tagLength = (int)(32 - indexLength - offsetLength);

	cout << "\nTag = " << tagLength << "\tIndex = " << indexLength << "\tOffset = " << offsetLength << endl;

	offsetTemp = blockSize - 1;
	indexTemp = ((cacheSize / blockSize) - 1) << offsetLength;
	tagTemp = ((1 << tagLength) - 1) << (indexLength + offsetLength);

	if (sOrC == 2) {
		while (getline(in, line)) {
			dOrI = convertTypeToNumber(line);
			data = convertDataToNumber(line);
			//cout << data << endl;
			dataTag = ((data & tagTemp) >> (indexLength + offsetLength));
			dataIndex = ((data & indexTemp) >> offsetLength);
			dataOffset = (data & offsetTemp);
			//cout<<"\ndataTag = "<<dataTag<<"\tdataIndex = "<<dataIndex<<"\tdataOffset = "<<dataOffset<<endl;
			combinedFetches++;
			if (associativity == 1) {
				if (combinedCache[0][dataIndex] == dataTag) {
					combinedHit++;
				}
				else {
					combinedCache[0][dataIndex] = dataTag;
				}
			}
			if (associativity == 4) {
				
			}
		}
		cout << "\nfetches = " << combinedFetches << "\thit = " << combinedHit << "\tmisses = " << (combinedFetches - combinedHit) << endl;
	}



	if (sOrC == 1) {
		while (getline(in, line)) {
			dOrI = convertTypeToNumber(line);
			data = convertDataToNumber(line);
			//cout << data << endl;
			dataTag = ((data & tagTemp) >> (indexLength + offsetLength));
			dataIndex = ((data & indexTemp) >> offsetLength);
			dataOffset = (data & offsetTemp);
			//cout<<"\ndataTag = "<<dataTag<<"\tdataIndex = "<<dataIndex<<"\tdataOffset = "<<dataOffset<<endl;
			if (dOrI == 2) {
				instructionFetches++;
				if (instructionCache[0][dataIndex] == dataTag) {
					instructionHit++;
				}
				else {
					instructionCache[0][dataIndex] = dataTag;
				}
			}
			if (dOrI == 1) {// This is wrong?
				dataFetches++;
				if (dataCache[0][dataIndex] == dataTag) {
					dataHit++;
				}
				else {
					dataCache[0][dataIndex] = dataTag;
				}
			}
			if (dOrI == 0) {
				dataFetches++;
				if (dataCache[0][dataIndex] == dataTag) {
					dataHit++;
				}
				else {
					dataCache[0][dataIndex] = dataTag;
				}
			}
		}
		cout << "\nI fetches = " << instructionFetches << "\tI hit = " << instructionHit << "\tmisses = " << (instructionFetches - instructionHit) << "\tD Fetches = " << dataFetches << "\tD Hit = " << dataHit << "\tD Misses = " << (dataFetches - dataHit) << endl;
	}


	return 0;
}