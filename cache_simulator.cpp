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
	while (i != 10) {
		temp[i - 2] = line[i];
		i++;
	}
	data = (int)strtol(temp, NULL, 16);
	return data;
}

int main()
{
	int combinedCache[2][4096], instructionCache[2][4096], dataCache[2][4096];		// [0][index] -> tag, [1][index] -> LRU priority
	char fileName[15] = "trace.din";
	string line;
	int i, k, flag = 0;
	int data, dOrI, sOrC, blockSize, blockSizeSelect, associativitySelect, associativity;
	int tagLength, indexLength, offsetLength, cacheSize, offsetTemp, indexTemp, tagTemp;
	int dataTag, dataIndex, dataOffset;
	int combinedFetches = 0, combinedHit = 0, instructionFetches = 0, dataFetches = 0, instructionHit = 0, dataHit = 0, combinedMiss = 0;
//***************************************************************************************
// Menu to input data																	*
//***************************************************************************************

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


	offsetLength = (int)(log2(blockSize));
	indexLength = (int)(log2(cacheSize / (blockSize * associativity)));
	tagLength = (int)(32 - indexLength - offsetLength);

	cout << "\nTag = " << tagLength << "\tIndex = " << indexLength << "\tOffset = " << offsetLength << endl;

	offsetTemp = (1 << offsetLength) - 1;
	indexTemp = (1 << indexLength) - 1;
	tagTemp = (1 << tagLength) - 1;
	//cout<<offsetTemp<<" " <<indexTemp<<"  "<<tagTemp<<endl;
	/*
		offsetTemp = blockSize - 1;
		indexTemp = ((cacheSize / (blockSize * associativity)) - 1) << offsetLength;
		tagTemp = ((1 << tagLength) - 1) << (indexLength + offsetLength);
	*/
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

	if (sOrC == 2) {
		/*		(getline(in, line));
				dOrI = convertTypeToNumber(line);
					data = convertDataToNumber(line);
				cout<<data<<endl;
				(getline(in, line));
				dOrI = convertTypeToNumber(line);
					data = convertDataToNumber(line);
				cout<<data<<endl;
				(getline(in, line));
				dOrI = convertTypeToNumber(line);
					data = convertDataToNumber(line);
				cout<<data<<endl;
				(getline(in, line));
				dOrI = convertTypeToNumber(line);
					data = convertDataToNumber(line);
				cout<<data<<endl;
				(getline(in, line));
				dOrI = convertTypeToNumber(line);
					data = convertDataToNumber(line);
				cout<<data<<endl;
				(getline(in, line));
				dOrI = convertTypeToNumber(line);
					data = convertDataToNumber(line);
				cout<<data<<endl;
				(getline(in, line));
				dOrI = convertTypeToNumber(line);
					data = convertDataToNumber(line);
				cout<<data<<endl;
				(getline(in, line));
				dOrI = convertTypeToNumber(line);
					data = convertDataToNumber(line);
				cout<<data<<endl;
				(getline(in, line));
				dOrI = convertTypeToNumber(line);
					data = convertDataToNumber(line);
				cout<<data<<endl;
				(getline(in, line));
				dOrI = convertTypeToNumber(line);
					data = convertDataToNumber(line);
				cout<<data<<endl;
		*/
		while (getline(in, line)) {
			dOrI = convertTypeToNumber(line);
			data = convertDataToNumber(line);
			//cout << data << endl;
			/*
			dataTag = ((data & tagTemp) >> (indexLength + offsetLength));
			dataIndex = ((data & indexTemp) >> offsetLength);
			dataOffset = (data & offsetTemp);
			*/
			dataOffset = data & offsetTemp;
			dataIndex = (data & (indexTemp << offsetLength)) >> (offsetLength);
			dataTag = (data & (tagTemp << (indexLength + offsetLength))) >> ((indexLength + offsetLength));

			//cout<<data<<endl;
			//cout << "\ndataTag = " << dataTag << "\tdataIndex = " << dataIndex << "\tdataOffset = " << dataOffset << endl;
			combinedFetches++;
			if (associativity == 1) {

				if (combinedCache[0][dataIndex] == dataTag) {
					combinedHit++;
				}
				else {
					combinedCache[0][dataIndex] = dataTag;
					combinedMiss++;
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
								//cout << "HI" << endl;
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
		cout << "\nfetches = " << combinedFetches << "\thit = " << combinedHit << "\tmisses = " << (combinedFetches - combinedHit) << "\tMiss Counter = " << combinedMiss << endl;
	}


	if (sOrC == 1) {
		while (getline(in, line)) {
			dOrI = convertTypeToNumber(line);
			data = convertDataToNumber(line);
			//cout << data << endl;
			/*
			dataTag = ((data & tagTemp) >> (indexLength + offsetLength));
			dataIndex = ((data & indexTemp) >> offsetLength);
			dataOffset = (data & offsetTemp);
			*/
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
									//cout << "HI" << endl;
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
									//cout << "HI" << endl;
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
		cout << "\nI fetches = " << instructionFetches << "\tI hit = " << instructionHit << "\tmisses = " << (instructionFetches - instructionHit) << "\tD Fetches = " << dataFetches << "\tD Hit = " << dataHit << "\tD Misses = " << (dataFetches - dataHit) << endl;
	}


	return 0;
}