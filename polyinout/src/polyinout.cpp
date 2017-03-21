//============================================================================
// Name        : polyinout.cpp
// Author      : jccho@fine-tec.com
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include "AreaData.h"

using namespace std;

void print_area(int val);

vector<AreaData> vad;

int main() {
	// outpu2,output3 = 1.0m, 0.5m
	AreaData ad1(3.0, 1.0);
	// points
	ad1[0] = {0.1, 0.0};
    ad1[1] = {0.32, 0.35};
    ad1[2] = {0.32, 5.0};
    ad1[3] = {0.0, 5.0};
    ad1[4] = {-0.32, 5.0};
    ad1[5] = {-0.32, 0.35};
    ad1[6] = {-0.1, 0.0};

    int ret = ad1.isInside(-0.7493770, 0.9963353);
    print_area(ret);

	return 0;
}

void print_area(int val)
{
	for (int i = 2; i >= 0; i--) {
		if (!(val & (0x01 << i))) {
			printf("%9s%d", "area", (i+1));
		}
		else {
			printf("%9s", " ");
		}
	}
	printf("\n");
}

int info_file_parcer(string path)
{
	return 0;
}
