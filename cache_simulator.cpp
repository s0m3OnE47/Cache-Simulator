
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>

using namespace std;

#define splitOrCombinedL1 1	// Combined Cache is selected
#define	cacheSizeL1 65536	// Cache Size 		is set to 65kB
#define	blockSizeL1 64		// Block Size 		is set to 32
#define associativityL1 8	// associativity 	is set to 4

#define splitOrCombinedL2 2	// Combined Cache is selected
#define	cacheSizeL2 524288	// Cache Size 		is set to 512kB
#define	blockSizeL2 64		// Block Size 		is set to 32
#define associativityL2 4	// associativity 	is set to 4

#define fileName "trace.din"
/**
 * Converts string to Number. 0 -> Data Read, 1 -> Data Write, 2 -> Instruction Fetch
 * @param  line String containing line information
 * @return      Returns interger.
 */
int convertTypeToNumber(string line) {
	char dataOrInstructionString[2];
	int dataOrInstruction;
	dataOrInstructionString[0] = line[0];
	dataOrInstruction = (int)strtol(dataOrInstructionString, NULL, 16);
	return dataOrInstruction;
}

/**
 * Converts string to Number.
 * @param  line String containing line information
 * @return      Returns integer data.
 */
int convertDataToNumber(string line) {
	int i = 2, data;
	char temp[100];
	while (i != 10) {
		temp[i - 2] = line[i];
		i++;
	}
	data = (int)strtol(temp, NULL, 16);
	return data;
}




class Cache {
private:
	int dataTag, dataIndex, dataOffset;
	int i, k, flag = 0;
	int splitOrCombined, cacheSize, blockSize, associativity;
	int offsetTemp, indexTemp, tagTemp;
	int offsetLength, indexLength, tagLength;
	vector<vector<int>> combinedCache, instructionCache, dataCache;
	//int combinedCache[2][cacheSize / blockSize], instructionCache[2][cacheSize / blockSize], dataCache[2][cacheSize / blockSize];

public:

	int combinedFetches = 0, combinedHit = 0, instructionFetches = 0, instructionHit = 0, dataFetches = 0, dataHit = 0;

	Cache(int splitOrCombinedL, int cacheSizeL, int blockSizeL, int associativityL) {
		splitOrCombined = splitOrCombinedL;
		cacheSize = cacheSizeL;
		blockSize = blockSizeL;
		associativity = associativityL;

		combinedCache.resize(2,vector<int>(cacheSize / blockSize));
		instructionCache.resize(2,vector<int>(cacheSize / blockSize));
		dataCache.resize(2,vector<int>(cacheSize / blockSize));


		offsetLength = (int)(log2(blockSize));
		indexLength = (int)(log2(cacheSize / (blockSize * associativity)));
		tagLength = (int)(32 - indexLength - offsetLength);

		// Initialization with 0 data. Reset LRU = 3 (associativity - 1)
		for (i = 0; i < cacheSize / blockSize; i++) {
			combinedCache[0][i] = 0;
			combinedCache[1][i] = associativity - 1;
			if (i < cacheSize / blockSize) {
				instructionCache[0][i] = 0;
				instructionCache[1][i] = associativity - 1;
				dataCache[0][i] = 0;
				dataCache[1][i] = associativity - 1;
			}
		}
	}

	int getOffsetLength(){
		return offsetLength;
	}

	int getIndexLength(){
		return indexLength;
	}

	int getTagLength(){
		return tagLength;
	}

	void updateCache(int dataOrInstruction, int data) {

		offsetTemp = (1 << offsetLength) - 1;
		indexTemp = (1 << indexLength) - 1;
		tagTemp = (1 << tagLength) - 1;

		dataOffset = data & offsetTemp;
		dataIndex = (data & (indexTemp << offsetLength)) >> (offsetLength);
		dataTag = (data & (tagTemp << (indexLength + offsetLength))) >> ((indexLength + offsetLength));


		if (splitOrCombined == 2) {
			//cout << "\ndataTag = " << dataTag << "\tdataIndex = " << dataIndex << "\tdataOffset = " << dataOffset << endl;
			combinedFetches++;
			flag = 0;
			for (k = 0; k < associativity; k++) {
				if (combinedCache[0][dataIndex + ((cacheSize / (blockSize * associativity)) * k)] == dataTag) {
					combinedCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * k)] = 0;
					for (i = 0; i < associativity; i++) {
						if (i != k) {
							if (combinedCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * i)] != (associativity - 1))
								combinedCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * i)]++;
						}
					}
					combinedHit++;
					flag = 1;
					break;
				}
			}
			if (flag != 1) {
				for (i = 0; i < associativity; i++) {
					if (combinedCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * i)] == (associativity - 1)) {
						combinedCache[0][dataIndex + ((cacheSize / (blockSize * associativity)) * i)] = dataTag;
						combinedCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * i)] = 0;
						for (k = 0; k < associativity; k++) {
							if (k != i) {
								if (combinedCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * k)] != (associativity - 1)) {
									combinedCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * k)]++;
								}
							}
						}
						break;
					}
				}
			}
		}


		// If Split Cache is selected
		if (splitOrCombined == 1) {
			if (dataOrInstruction == 2) {
				flag = 0;
				instructionFetches++;
				for (k = 0; k < associativity; k++) {
					if (instructionCache[0][dataIndex + ((cacheSize / (blockSize * associativity)) * k)] == dataTag) {
						instructionCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * k)] = 0;
						for (i = 0; i < associativity; i++) {
							if (i != k) {
								if (instructionCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * i)] != (associativity - 1))
									instructionCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * i)]++;
							}
						}
						instructionHit++;
						flag = 1;
						break;
					}
				}
				if (flag != 1) {
					for (i = 0; i < associativity; i++) {
						if (instructionCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * i)] == (associativity - 1)) {
							instructionCache[0][dataIndex + ((cacheSize / (blockSize * associativity)) * i)] = dataTag;
							instructionCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * i)] = 0;
							for (k = 0; k < associativity; k++) {
								if (k != i) {
									if (instructionCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * k)] != (associativity - 1)) {
										instructionCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * k)]++;
									}
								}
							}
							break;
						}
					}
				}
			}

			if (dataOrInstruction == 1 || dataOrInstruction == 0) {
				flag = 0;
				dataFetches++;
				for (k = 0; k < associativity; k++) {
					if (dataCache[0][dataIndex + ((cacheSize / (blockSize * associativity)) * k)] == dataTag) {
						dataCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * k)] = 0;
						for (i = 0; i < associativity; i++) {
							if (i != k) {
								if (dataCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * i)] != (associativity - 1))
									dataCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * i)]++;
							}
						}
						dataHit++;
						flag = 1;
						break;
					}
				}
				if (flag != 1) {
					for (i = 0; i < associativity; i++) {
						if (dataCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * i)] == (associativity - 1)) {
							dataCache[0][dataIndex + ((cacheSize / (blockSize * associativity)) * i)] = dataTag;
							dataCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * i)] = 0;
							for (k = 0; k < associativity; k++) {
								if (k != i) {
									if (dataCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * k)] != (associativity - 1)) {
										dataCache[1][dataIndex + ((cacheSize / (blockSize * associativity)) * k)]++;
									}
								}
							}
							break;
						}
					}
				}
			}
		}


	}
};




int main(int argc, char *argv[])
{

	//	if (argc == 1) {
	//		bitCounter = (int)strtol(argv[1], NULL, 10);
	//	}

	//4096 is worst case. Cache_Size/Block_Size = Array_Length. 32768/8 = 4096;
	string line;
	int i;
	int data, dataOrInstruction;
	Cache L1(splitOrCombinedL1, cacheSizeL1, blockSizeL1, associativityL1);
	Cache L2(splitOrCombinedL2, cacheSizeL2, blockSizeL2, associativityL2);

	ifstream in(fileName);
	if (!in) {
		cout << "\nFile does not exist";
	}

	cout << "###########################################################################################" << endl;
	cout << "-------------------------------------------------------------------------------------------" << endl;
	cout << "Cache Size = " << cacheSizeL1 << " B" << "\tBlock Size = " << blockSizeL1 << " B" << "\t\tassociativity = " << associativityL1 << " way" << endl;
	cout << "Cache Size = " << cacheSizeL2 << " B" << "\tBlock Size = " << blockSizeL2 << " B" << "\t\tassociativity = " << associativityL2 << " way" << endl;
	cout << "___________________________________________________________________________________________" << endl;

	cout << "\nL1 Tag uses " << L1.getTagLength() << " bits," << "\tL1 Index uses " << L1.getIndexLength() << " bits," << "\tL1 Offset uses " << L1.getOffsetLength() << " bits" << endl;
	cout << "\nL2 Tag uses " << L2.getTagLength() << " bits," << "\tL2 Index uses " << L2.getIndexLength() << " bits," << "\tL2 Offset uses " << L2.getOffsetLength() << " bits" << endl;


	// If Combined Cache is selected
	while (getline(in, line)) {

		dataOrInstruction = convertTypeToNumber(line);
		data = convertDataToNumber(line);

		L1.updateCache(dataOrInstruction, data);


	}

	if (splitOrCombinedL1 == 1 ) {
		cout << "\nInstruction Fetches = " << L1.instructionFetches << "\tInstruction Hits = " << L1.instructionHit << "\tInstruction Misses = " << (L1.instructionFetches - L1.instructionHit) << endl;
		cout << "Data Fetches = " << L1.dataFetches << "\t\tData Hits = " << L1.dataHit << "\t\tData Misses = " << (L1.dataFetches - L1.dataHit) << endl;
		cout << "\nInstruction Hit Rate = " << (((float)L1.instructionHit) * 100) / ((float)L1.instructionFetches) << "%" << "\t\tInstruction Miss Rate = " << 100.0 - ((((float)L1.instructionHit) * 100) / ((float)L1.instructionFetches)) << "%" << endl;
		cout << "Data Hit Rate = " << (((float)L1.dataHit) * 100) / ((float)L1.dataFetches) << "%" << "\t\tData Miss Rate = " << 100.0 - ((((float)L1.dataHit) * 100) / ((float)L1.dataFetches)) << "%" << endl;

	}

	if (splitOrCombinedL1 == 2 ) {
		cout << "\nCombined Fetches = " << L1.combinedFetches << "\tCombined Hits = " << L1.combinedHit << "\t\tCombined Misses = " << (L1.combinedFetches - L1.combinedHit) << endl;
		cout << "\nCombined Hit Rate = " << (((float)L1.combinedHit) * 100) / ((float)L1.combinedFetches) << "%" << "\tCombined Miss Rate = " << 100.0 - (((float)L1.combinedHit) * 100) / ((float)L1.combinedFetches) << "%" << endl;
	}

	cout << "###########################################################################################" << endl;
	return 0;
}