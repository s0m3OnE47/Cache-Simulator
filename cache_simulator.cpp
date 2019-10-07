#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

using namespace std;

/**
 * Converts string to Number. 0 -> Data Read, 1 -> Data Write, 2 -> Instruction Fetch
 * @param  line String containing line information
 * @return      Returns interger.
 */
int convertTypeToNumber(string line) {
	char dOrIString[2];
	int dOrI;
	dOrIString[0] = line[0];
	dOrI = (int)strtol(dOrIString, NULL, 16);
	return dOrI;
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

int main()
{
	//4096 is worst case. Cache_Size/Block_Size = Array_Length. 32768/8 = 4096;
	int combinedCache[2][4096], instructionCache[2][4096], dataCache[2][4096];		// [0][index] -> tag, [1][index] -> LRU priority
	char fileName[15] = "trace.din";												// File name of input addresses
	string line;
	int i, k, flag = 0;
	int data, dOrI, sOrC, blockSize, blockSizeSelect, associativitySelect, associativity;
	int tagLength, indexLength, offsetLength, cacheSize, offsetTemp, indexTemp, tagTemp;
	int dataTag, dataIndex, dataOffset;
	int combinedFetches = 0, combinedHit = 0, instructionFetches = 0, dataFetches = 0, instructionHit = 0, dataHit = 0, combinedMiss = 0;

//***************************************************************************************************
// Menu to input data from user																		*
//***************************************************************************************************

	cout << "\nSelect\n1. Split cache\n2. Combined Cache\n\t";
	cin >> sOrC;
	switch (sOrC) {
	case 1:
		cacheSize = 16384;
		break;
	case 2:
		cacheSize = 32768;
		break;
	default:
		cout << "Enter valid choice. Defaulting to Combined Cache." << endl;
		sOrC = 2;
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
	default:
		cout << "Enter valid choice. Defaulting to 8B Block Size." << endl;
		blockSizeSelect = 1;
		blockSize = 8;
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
	default:
		cout << "Enter valid choice. Defaulting to Direct Mapped Cache." << endl;
		associativitySelect = 1;
		associativity = 1;
		break;
	}
	ifstream in(fileName);
	if (!in) {
		cout << "\nFile does not exist";
	}

	offsetLength = (int)(log2(blockSize));
	indexLength = (int)(log2(cacheSize / (blockSize * associativity)));
	tagLength = (int)(32 - indexLength - offsetLength);

	cout << "\nTag uses " << tagLength << " bits," << "\tIndex uses " << indexLength << " bits," << "\tOffset uses " << offsetLength << " bits" << endl;

	offsetTemp = (1 << offsetLength) - 1;
	indexTemp = (1 << indexLength) - 1;
	tagTemp = (1 << tagLength) - 1;

	// Reset LRU = 3, data = 0;
	for (i = 0; i < 4096; i++) {
		combinedCache[0][i] = 0;
		combinedCache[1][i] = 3;
		if (i < 4096) {
			instructionCache[0][i] = 0;
			instructionCache[1][i] = 3;
			dataCache[0][i] = 0;
			dataCache[1][i] = 3;
		}
	}

	// If Combined Cache is selected
	if (sOrC == 2) {
		while (getline(in, line)) {
			dOrI = convertTypeToNumber(line);
			data = convertDataToNumber(line);

			dataOffset = data & offsetTemp;
			dataIndex = (data & (indexTemp << offsetLength)) >> (offsetLength);
			dataTag = (data & (tagTemp << (indexLength + offsetLength))) >> ((indexLength + offsetLength));

			//cout << "\ndataTag = " << dataTag << "\tdataIndex = " << dataIndex << "\tdataOffset = " << dataOffset << endl;
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
				flag = 0;
				for (k = 0; k < associativity; k++) {
					if (combinedCache[0][dataIndex + (1024 * k)] == dataTag) {
						combinedCache[1][dataIndex + (1024 * k)] = 0;
						for (i = 0; i < associativity; i++) {
							if (i != k) {
								if (combinedCache[1][dataIndex + (1024 * i)] != (associativity - 1))
									combinedCache[1][dataIndex + (1024 * i)]++;
							}
						}
						combinedHit++;
						flag = 1;
						break;
					}
				}
				if (flag != 1) {
					for (i = 0; i < associativity; i++) {
						if (combinedCache[1][dataIndex + (1024 * i)] == (associativity - 1)) {
							combinedCache[0][dataIndex + (1024 * i)] = dataTag;
							combinedCache[1][dataIndex + (1024 * i)] = 0;
							for (k = 0; k < associativity; k++) {
								if (k != i) {
									if (combinedCache[1][dataIndex + (1024 * k)] != (associativity - 1)) {
										combinedCache[1][dataIndex + (1024 * k)]++;
									}
								}
							}
							break;
						}
					}
				}
			}
		}
		cout << "\nCombined Fetches = " << combinedFetches << "\tCombined Hits = " << combinedHit << "\t\tCombined Misses = " << (combinedFetches - combinedHit) << endl;
		cout << "\nCombined Hit Rate = " << (((float)combinedHit) * 100) / ((float)combinedFetches) << "%" << "\tInstruction Miss Rate = " << 100.0 - (((float)combinedHit) * 100) / ((float)combinedFetches) << "%" << endl;
	}

// If Split Cache is selected
	if (sOrC == 1) {
		while (getline(in, line)) {
			dOrI = convertTypeToNumber(line);
			data = convertDataToNumber(line);

			dataOffset = data & offsetTemp;
			dataIndex = (data & (indexTemp << offsetLength)) >> (offsetLength);
			dataTag = (data & (tagTemp << (indexLength + offsetLength))) >> ((indexLength + offsetLength));

			//cout<<"\ndataTag = "<<dataTag<<"\tdataIndex = "<<dataIndex<<"\tdataOffset = "<<dataOffset<<endl;
			if (associativity == 1) {
				if (dOrI == 2) {
					instructionFetches++;
					if (instructionCache[0][dataIndex] == dataTag) {
						instructionHit++;
					}
					else {
						instructionCache[0][dataIndex] = dataTag;
					}
				}
				if (dOrI == 1) {
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

			if (associativity == 4) {
				if (dOrI == 2) {
					instructionFetches++;
					flag = 0;
					for (k = 0; k < associativity; k++) {
						if (instructionCache[0][dataIndex + (1024 * k)] == dataTag) {
							instructionCache[1][dataIndex + (1024 * k)] = 0;
							for (i = 0; i < associativity; i++) {
								if (i != k) {
									if (instructionCache[1][dataIndex + (1024 * i)] != (associativity - 1))
										instructionCache[1][dataIndex + (1024 * i)]++;
								}
							}
							instructionHit++;
							flag = 1;
							break;
						}
					}
					if (flag != 1) {
						for (i = 0; i < associativity; i++) {
							if (instructionCache[1][dataIndex + (1024 * i)] == (associativity - 1)) {
								instructionCache[0][dataIndex + (1024 * i)] = dataTag;
								instructionCache[1][dataIndex + (1024 * i)] = 0;
								for (k = 0; k < associativity; k++) {
									if (k != i) {
										if (instructionCache[1][dataIndex + (1024 * k)] != (associativity - 1)) {
											instructionCache[1][dataIndex + (1024 * k)]++;
										}
									}
								}
								break;
							}
						}
					}
				}
				if (dOrI == 1 || dOrI == 0) {
					dataFetches++;
					flag = 0;
					for (k = 0; k < associativity; k++) {
						if (dataCache[0][dataIndex + (1024 * k)] == dataTag) {
							dataCache[1][dataIndex + (1024 * k)] = 0;
							for (i = 0; i < associativity; i++) {
								if (i != k) {
									if (dataCache[1][dataIndex + (1024 * i)] != (associativity - 1))
										dataCache[1][dataIndex + (1024 * i)]++;
								}
							}
							dataHit++;
							flag = 1;
							break;
						}
					}
					if (flag != 1) {
						for (i = 0; i < associativity; i++) {
							if (dataCache[1][dataIndex + (1024 * i)] == (associativity - 1)) {
								dataCache[0][dataIndex + (1024 * i)] = dataTag;
								dataCache[1][dataIndex + (1024 * i)] = 0;
								for (k = 0; k < associativity; k++) {
									if (k != i) {
										if (dataCache[1][dataIndex + (1024 * k)] != (associativity - 1)) {
											dataCache[1][dataIndex + (1024 * k)]++;
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
		cout << "\nInstruction Fetches = " << instructionFetches << "\tInstruction Hits = " << instructionHit << "\tInstruction Misses = " << (instructionFetches - instructionHit) << endl;
		cout << "Data Fetches = " << dataFetches << "\t\tData Hits = " << dataHit << "\t\tData Misses = " << (dataFetches - dataHit) << endl;
		cout << "\nInstruction Hit Rate = " << (((float)instructionHit) * 100) / ((float)instructionFetches) << "%" << "\t\tInstruction Miss Rate = " << 100.0 - ((((float)instructionHit) * 100) / ((float)instructionFetches)) << "%" << endl;
		cout << "Data Hit Rate = " << (((float)dataHit) * 100) / ((float)dataFetches) << "%" << "\t\tData Miss Rate = " << 100.0 - ((((float)dataHit) * 100) / ((float)dataFetches)) << "%" << endl;
	}

	return 0;
}