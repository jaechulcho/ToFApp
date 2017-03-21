/*
* profile.cpp
 *
 *  Created on: 2017. 3. 20.
 *      Author: han
 */

#include "profile.h"
#include "AreaData.h"

AreaData profile_sel(int num, double area2, double area3){

    AreaData AD(area2, area3);

    switch(num){
        case 1 :
            AD = Ubg_vpf1();
            break;
        case 2 :
            AD = Ubg_vpf2();
            break;
        case 3 :
            AD = Ubg_vpf3();
            break;
        case 4 :
            AD = Ubg_vpf4();
            break;
        case 5 :
            AD = Ubg_vpf5();
            break;
        case 6 :
            AD = Ubg_vpf6();
            break;
        case 7 :
            AD = Pbs_vpf1();
            break;
        case 8 :
            AD = Pbs_vpf2();
            break;
        case 9 :
            AD = Pbs_vpf3();
            break;
        case 10 :
            AD = Pbs_vpf4();
            break;
    }

    return AD;
}


AreaData Ubg_vpf1(){

    AreaData ubg_vpf(1.0, 1.0);
    ubg_vpf[0] = {0.125, 0.15};
    ubg_vpf[1] = {0.125, 0.3};
    ubg_vpf[2] = {0.125, 0.3};
    ubg_vpf[3] = {0.0, 3.0};
    ubg_vpf[4] = {-0.125, 3.0};
    ubg_vpf[5] = {-0.125, 3.0};
    ubg_vpf[6] = {-0.125, 0.15};

    return ubg_vpf;
}

AreaData Ubg_vpf2(){

    AreaData ubg_vpf(1.0, 0.5);
    ubg_vpf[0] = {0.1, 0.0};
    ubg_vpf[1] = {0.320, 0.350};
    ubg_vpf[2] = {0.320, 3.0};
    ubg_vpf[3] = {0.0, 3.0};
    ubg_vpf[4] = {-0.320, 3.0};
    ubg_vpf[5] = {-0.320, 0.350};
    ubg_vpf[6] = {-0.100, 0.0};

    return ubg_vpf;
}

AreaData Ubg_vpf3(){

    AreaData ubg_vpf(1.0, 1.0);
    ubg_vpf[0] = {0.7, 0.0};
    ubg_vpf[1] = {0.7, 1.0};
    ubg_vpf[2] = {0.32, 1.5};
    ubg_vpf[3] = {0.32, 3.0};
    ubg_vpf[4] = {0.0, 3.0};
    ubg_vpf[5] = {-0.32, 3.0};
    ubg_vpf[6] = {-0.32, 0.35};

    return ubg_vpf;
}

AreaData Ubg_vpf4(){

    AreaData ubg_vpf(1.0, 0.5);
    ubg_vpf[0] = {0.1, 0.0};
    ubg_vpf[1] = {0.32, 0.35};
    ubg_vpf[2] = {0.32, 1.0};
    ubg_vpf[3] = {0.0, 1.0};
    ubg_vpf[4] = {-0.32, 1.0};
    ubg_vpf[5] = {-0.32, 0.35};
    ubg_vpf[6] = {-0.1, 0.0};

    return ubg_vpf;
}

AreaData Ubg_vpf5(){

    AreaData ubg_vpf(1.0, 0.7);
    ubg_vpf[0] = {0.550, 0.0};
    ubg_vpf[1] = {0.600, 1.0};
    ubg_vpf[2] = {0.300, 1.5};
    ubg_vpf[3] = {0.013, 3.0};
    ubg_vpf[4] = {-0.300, 1.5};
    ubg_vpf[5] = {-0.600, 1.0};
    ubg_vpf[6] = {-0.550, 0.0};

    return ubg_vpf;
}

AreaData Ubg_vpf6(){

    AreaData ubg_vpf(1.0, 0.7);
    ubg_vpf[0] = {0.475, 0.0};
    ubg_vpf[1] = {0.550, 1.0};
    ubg_vpf[2] = {0.300, 1.5};
    ubg_vpf[3] = {0.013, 3.0};
    ubg_vpf[4] = {-0.300, 1.5};
    ubg_vpf[5] = {-0.550, 1.0};
    ubg_vpf[6] = {-0.475, 0.0};

    return ubg_vpf;
}

AreaData Pbs_vpf1(){

    AreaData pbs_vpf(2.0, 1.0);
    pbs_vpf[0] = {0.125, 0.15};
    pbs_vpf[1] = {0.125, 3.0};
    pbs_vpf[2] = {0.125, 3.0};
    pbs_vpf[3] = {0.0, 3.0};
    pbs_vpf[4] = {-0.125, 3.0};
    pbs_vpf[5] = {-0.125, 3.0};
    pbs_vpf[6] = {-0.125, 0.15};

    return pbs_vpf;
}

AreaData Pbs_vpf2(){

    AreaData pbs_vpf(0.030, 0.020);
    pbs_vpf[0] = {0.125, 0.150};
    pbs_vpf[1] = {0.125, 0.200};
    pbs_vpf[2] = {0.125, 0.346};
    pbs_vpf[3] = {0.0, 0.4};
    pbs_vpf[4] = {-0.200, 0.346};
    pbs_vpf[5] = {-0.346, 0.200};
    pbs_vpf[6] = {-0.400, 0.070};

    return pbs_vpf;
}

AreaData Pbs_vpf3(){

    AreaData pbs_vpf(0.300, 0.200);
    pbs_vpf[0] = {0.400, 0.070};
    pbs_vpf[1] = {0.346, 0.200};
    pbs_vpf[2] = {0.200, 0.346};
    pbs_vpf[3] = {0.0, 0.4};
    pbs_vpf[4] = {-0.125, 0.346};
    pbs_vpf[5] = {-0.125, 0.200};
    pbs_vpf[6] = {-0.125, 0.150};

    return pbs_vpf;
}

AreaData Pbs_vpf4(){

    AreaData pbs_vpf(0.3, 0.2);
    pbs_vpf[0] = {0.256, 0.298};
    pbs_vpf[1] = {0.180, 0.354};
    pbs_vpf[2] = {0.092, 0.386};
    pbs_vpf[3] = {0.0, 0.400};
    pbs_vpf[4] = {-0.092, 0.386};
    pbs_vpf[5] = {-0.180, 0.354};
    pbs_vpf[6] = {-0.256, 0.298};

    return pbs_vpf;
}


