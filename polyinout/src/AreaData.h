/*
 * AreaData.h
 *
 *  Created on: 2017. 3. 17.
 *      Author: josco
 */

#ifndef AREADATA_H_
#define AREADATA_H_

#define MAX_AP_NUM		(7)

struct AreaPoint {
    double x;
    double z;
};

class AreaData {
public:
    AreaData();
    AreaData(double o2=0.0, double o3=0.0, int ps=0,int tsm=0,int mrm=0,int hh=0):
        m_output2(o2), m_output3(o3), m_partitionshape(ps),
        m_twoscanmode(tsm), m_mirrorreflectionmode(mrm),
        m_halfhysteresis(hh) {}
    virtual ~AreaData();
    AreaPoint& operator[](int index);
    void setOutput2(double length) {m_output2 = length;}
    void setOutput3(double length) {m_output3 = length;}
    void setPartitionShape(int mode) {m_partitionshape = mode;}
    void setScanMode(int mode) { m_twoscanmode = mode;}
    void setMRFmode(int mode) { m_mirrorreflectionmode = mode;}
    void setHHysteresis(int mode) { m_halfhysteresis = mode;}
    int isInside(double x, double z) const;
private:
    AreaPoint m_point[MAX_AP_NUM];
    double m_output2;                  // 0.0 - 5.0 m
    double m_output3;                  // 0.0 - 5.0 m
    int m_partitionshape;           // 0:straight 1:fan
    int m_twoscanmode;              // not implemented
    int m_mirrorreflectionmode;     // not implemented
    int m_halfhysteresis;           // not implemented
};

#endif /* AREADATA_H_ */
