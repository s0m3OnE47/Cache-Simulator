#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int cacheInstuctionLocation = 0, cacheDataLocation = 0, cacheCombinedLocation = 0;

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

void dataToInstuctionCacheLRU(int data) {

	cacheInstuctionLocation++;
}

void dataToDataWriteCacheLRU(int data) {

	cacheDataLocation++;
}

void dataToDataReadCacheLRU(int data) {

	cacheDataLocation++;
}

void dataToCombinedCacheLRU(int data) {
	cacheCombinedLocation++;
}

int instructionCache[16385], dataCache[16385], combiledCache[32769];
int instructionCacheHitRate = 0, instructionCacheMissRate = 0, dataCacheHitRate = 0, dataCacheMissRate = 0, combinedCacheHitRate = 0, combiledCacheMissRate = 0;

int main() {

	int data, dOrI, sOrC, blocksize, blockSizeSelect, associativitySelect, associativity;
	char fileName[15] = "trace.din";
	string line;
	int offset, tag, index;

	cout << "\nSelect\n1. Split cache\n2. Combined Cache\n\t";
	cin >> sOrC;
	cout << "\nBlock size\n1. 8B\n2. 32B\n3. 128B\n\t";
	cin >> blockSizeSelect;
	switch (blockSizeSelect) {
	case 1:
		blocksize = 8;
		break;
	case 2:
		blocksize = 32;
		break;
	case 3:
		blocksize = 128;
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
	if (sOrC == 1) {

		while (getline(in, line)) {
			dOrI = convertTypeToNumber(line);
			data = convertDataToNumber(line);
			if (dOrI == 0) {					// Instuction Fetch
				dataToInstuctionCacheLRU(data);
			}
			else if (dOrI == 1) { 				// Data write
				dataToDataWriteCacheLRU(data);
			}
			else if (dOrI == 0) {				// Data read
				dataToDataReadCacheLRU(data);
			}
		}
	}
//	cout << "Tag = " << tag << "\t\t" << "Index = " << index << "Offset = " << offset;
	cout << "\nInstuction Hit Rate = " << instructionCacheHitRate << "\t\t" << "Instruction Miss Rate = " << instructionCacheMissRate << endl;
	cout << "Data Hit Rate = " << dataCacheHitRate << "\t\t" << "Data Miss Rate = " << dataCacheMissRate << endl;
	cout << "Combined Hit Rate = " << combinedCacheHitRate << "\t\t" << "Combined Miss Rate = " << combiledCacheMissRate << endl;
	in.close();
	return 0;
}