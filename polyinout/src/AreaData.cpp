/*
 * AreaData.cpp
 *
 *  Created on: 2017. 3. 17.
 *      Author: josco
 */

#include <cstdio>
#include <cstring>
#include <math.h>
#include "AreaData.h"

AreaData::AreaData() {
	memset(m_point, 0, sizeof(m_point));
	m_output2 = 0;
	m_output3 = 0;
	m_partitionshape = 0;
	m_twoscanmode = 0;
	m_mirrorreflectionmode = 0;
	m_halfhysteresis = 0;
}

AreaData::~AreaData() {
	// TODO Auto-generated destructor stub
}

AreaPoint& AreaData::operator[](int index) {
	return m_point[index];
}

int AreaData::isInside(double x, double z) const
{
    int ret = 0x07;
    int crosses = 0;
    for (int i = 0; i < MAX_AP_NUM; i++) {
        int j = (i+1)%MAX_AP_NUM;
        if ((m_point[i].z > z) != (m_point[j].z > z)) {
            double atX = (((m_point[j].x-m_point[i].x)*(z-m_point[i].z))/(m_point[j].z-m_point[i].z))
                            + m_point[i].x;
            if (x < atX) {
                crosses++;
            }
        }
    }
    if (crosses & 0x01) {
        if (0 == m_partitionshape) {
            if (z <= m_output3) {
                ret = 0x03;
            }
            else if (z <= m_output2) {
                ret = 0x05;
            }
            else {
                ret = 0x06;
            }
        }
        else if (1 == m_partitionshape) {
            double r = sqrt(x*x+z*z);
            if (r <= m_output3) {
                ret = 0x03;
            }
            else if (r <= m_output2) {
                ret = 0x05;
            }
            else {
                ret = 0x06;
            }
        }
        else {
            ret =  0x07;
        }
    }
    return ret;
}
