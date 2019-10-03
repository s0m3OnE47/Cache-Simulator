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

	int data, dOrI;
	char fileName[15] = "trace.din";
	string line;

	ifstream in(fileName);
	if (!in) {
		cout << "\nFile does not exist";
	}
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

	cout<<"Instuction Hit Rate = "<<instructionCacheHitRate<<endl;
	cout<<"Instruction Miss Rate = "<<instructionCacheMissRate<<endl;
	cout<<"Data Hit Rate = "<<dataCacheHitRate<<endl;
	cout<<"Data Miss Rate = "<<dataCacheMissRate<<endl;
	cout<<"Combined Hit Rate = "<<combinedCacheHitRate<<endl;
	cout<<"Combined Miss Rate = "<<combiledCacheMissRate<<endl;
	in.close();
	return 0;
}