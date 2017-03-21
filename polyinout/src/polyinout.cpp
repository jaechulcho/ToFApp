//============================================================================
// Name        : polyinout.cpp
// Author      : jccho@fine-tec.com
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "AreaData.h"

using namespace std;

int main() {
	AreaData ad1(1.0, 0.5);

	ad1[0] = {0.1, 0.0};
    ad1[1] = {0.32, 0.35};
    ad1[2] = {0.32, 5.0};
    ad1[3] = {0.0, 5.0};
    ad1[4] = {-0.32, 5.0};
    ad1[5] = {-0.32, 0.35};
    ad1[6] = {-0.1, 0.0};

    int ret = ad1.isInside(0.319, 0.499);
    //printf("result[%08X]\n", ret);
    ad1.setPartitionShape(1);
    ret = ad1.isInside(0.319, 0.499);
    //printf("result[%08X]\n", ret);

	return 0;
}
