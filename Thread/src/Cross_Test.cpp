/*
 * TI Voxel Lib component.
 *
 * Copyright (c) 2014 Texas Instruments Inc.
 */


#include "CameraSystem.h"

#include "SimpleOpt.h"
#include "Common.h"
#include "Logger.h"
#include "UVCStreamer.h"
#include <iomanip>
#include <fstream>
#include <sched.h>
#include <sys/mman.h>
#include <cstdio>
#include <thread>
#include <unistd.h>
#include <mqueue.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include "fgpio.h"
#include "AreaData.h"
#include <cstdio>
#include <cstring>
#include <math.h>

#define MY_PRIORITY	(99)
#define MAX_SAFE_STACK	(8*1024)
#define GPIO_OUTPIN_NUM 4
#define GPIO_INPIN_NUM  3

const int FRAME_WIDTH  = 160;
const int FRAME_HEIGHT = 120;
const int IMAGESIZE  = FRAME_WIDTH*FRAME_HEIGHT;
const double Y_MIN   = 0.0;
const double Y_MAX   = 0.1;
const double Y_MIN_2 = 0.5;
const double Y_MAX_2 = 0.6;
const double I_MIN   = 0.001;
const double X_MIN   = -0.4;
const double X_MAX   = 0.4;
const double Z_MAX   = 5.0;
const double AREA_1  = 3.0;
const double AREA_2  = 1.0;

using namespace Voxel;

enum Options
{
    VENDOR_ID = 0,
    PRODUCT_ID = 1,
    SERIAL_NUMBER = 2,
    DUMP_FILE = 3,
    NUM_OF_FRAMES = 4
};

Vector<CSimpleOpt::SOption> argumentSpecifications =
{
    { VENDOR_ID,    "-v", SO_REQ_SEP, "Vendor ID of the USB device (hexadecimal)"}, // Only worker count is needed here
    { PRODUCT_ID,   "-p", SO_REQ_SEP, "Comma separated list of Product IDs of the USB devices (hexadecimal)"},
    { SERIAL_NUMBER,"-s", SO_REQ_SEP, "Serial number of the USB device (string)"},
    { DUMP_FILE,    "-f", SO_REQ_SEP, "Name of the file to dump extracted frames"},
    { NUM_OF_FRAMES,"-n", SO_REQ_SEP, "Number of frames to dump [default = 1]"},
    SO_END_OF_OPTIONS
};

void help()
{
    std::cout << "CameraSystemTest v1.0" << std::endl;

    CSimpleOpt::SOption *option = argumentSpecifications.data();

    while (option->nId >= 0) {
        std::cout << option->pszArg << " " << option->helpInfo << std::endl;
        option++;
    }
}

void gpio_init(){
    fgpio gpio;
    gpio.gpio_use_sel(GPIO_OUTPIN_NUM);
    gpio.gpio_in_out_sel("out", GPIO_OUTPIN_NUM);
    return;
}

int func(const XYZIPointCloudFrame *d,
                        AreaData &ad1,
                        double y_min, double y_max,
                        int location){
    int ret = 0xff;
    char name[10];

    for (int i = 0 ; i < IMAGESIZE ; i++) {
        if ((y_min <= d->points[i].y) && (y_max >= d->points[i].y) && (I_MIN < d->points[i].i)) {
            ret &= ad1.isInside(d->points[i].x, d->points[i].z);
        }
    }

    if (location == 1) {
        sprintf(name, "becle");
    } else {
        sprintf(name, "||| obstacle");
    }

    if (ret == 7) {
        printf("%s No Object\n", name);
    } else if ( ret == 6) {
        printf("%s Area 1\n", name);
    } else if ( ret == 5) {
        printf("%s Area 2\n", name);
    } else if ( ret == 4) {
        printf("%s Area 2, Area 1\n", name);
    } else if ( ret == 3) {
        printf("%s Area 3\n", name);
    } else if ( ret == 2) {
        printf("%s Area 1, Are 3\n", name);
    } else if ( ret == 1) {
        printf("%s Area 1, Area 2\n", name);
    } else if ( ret == 0) {
        printf("%s Area 1, Area 2, Area 3\n", name);
    }

    return ret;

}


int main(int argc, char *argv[])
{

  //pthread_t thread;
/*
  struct sched_param param;

  param.sched_priority = MY_PRIORITY;
  if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
    perror("sched_setscheduler failed");
    exit(-1);
  }
  if (mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
    perror("mlockall failed");
    exit(-2);
  }
*/
    gpio_init();
    CSimpleOpt s(argc, argv, argumentSpecifications);

    logger.setDefaultLogLevel(LOG_INFO);

    uint16_t vid = 0;
    Vector<uint16_t> pids;
    String serialNumber;
    String dumpFileName;
    //int32_t frameCount = 1;
    char *endptr;

    while (s.Next()) {
        if (s.LastError() != SO_SUCCESS) {
            std::cout << s.GetLastErrorText(s.LastError()) << ": '" << s.OptionText() << "' (use -h to get command line help)" << std::endl;
            help();
            return -1;
        }

        Vector<String> splits;
        switch (s.OptionId())
        {
            case VENDOR_ID:
                vid = (uint16_t)strtol(s.OptionArg(), &endptr, 16);
                break;

            case PRODUCT_ID:
                split(s.OptionArg(), ',', splits);
                for(auto &s1: splits)
                    pids.push_back((uint16_t)strtol(s1.c_str(), &endptr, 16));
                break;

            case SERIAL_NUMBER:
                serialNumber = s.OptionArg();
                break;

            case DUMP_FILE:
                dumpFileName = s.OptionArg();
                break;

            case NUM_OF_FRAMES:
                //frameCount = (int32_t)strtol(s.OptionArg(), &endptr, 10);
                break;

            default:
                help();
                break;
        };
    }

    if (vid == 0 || pids.size() == 0 || pids[0] == 0) {
        logger(LOG_ERROR) << "Required argument missing." << std::endl;
        help();
        return -1;
    }

    if (dumpFileName.size() != 0) {
        std::ofstream f(dumpFileName, std::ios::binary | std::ios::out);
        if (!f.good()) {
            logger(LOG_ERROR) << "Failed to open '" << dumpFileName << "'" << std::endl;
            return -1;
        }
    }

    CameraSystem sys;

    // Get all valid detected devices
    const Vector<DevicePtr> &devices = sys.scan();
    DevicePtr toConnect;
    std::cout << "Detected devices: " << std::endl;

    for (auto &d: devices) {
        std::cout << d->id() << std::endl;
        if (d->interfaceID() == Device::USB) {
            USBDevice &usb = (USBDevice &)*d;
            if (usb.vendorID() == vid && (serialNumber.size() == 0 || usb.serialNumber() == serialNumber)) {
                for(auto pid: pids)
                if (usb.productID() == pid) {
                    toConnect = d;
                }
            }
        }
    }

    if (!toConnect) {
        logger(LOG_ERROR) << "No valid device found for the specified VID:PID:serialnumber" << std::endl;
        return -1;
    }

    DepthCameraPtr depthCamera = sys.connect(toConnect);

    if (!depthCamera) {
        logger(LOG_ERROR) << "Could not load depth camera for device " << toConnect->id() << std::endl;
        return -1;
    }

    if (!depthCamera->isInitialized()) {
        logger(LOG_ERROR) << "Depth camera not initialized for device " << toConnect->id() << std::endl;
        return -1;
    }

    std::cout << "Successfully loaded depth camera for device " << toConnect->id() << std::endl;

    int count = 0;
    TimeStampType lastTimeStamp = 0;

    AreaData ad1(1.0, 0.5);
    ad1[0] = {0.1, 0.0};
    ad1[1] = {0.32, 0.35};
    ad1[2] = {0.32, 5.0};
    ad1[3] = {0.0, 5.0};
    ad1[4] = {-0.32, 5.0};
    ad1[5] = {-0.32, 0.35};
    ad1[6] = {-0.1, 0.0};

    depthCamera->registerCallback(DepthCamera::FRAME_XYZI_POINT_CLOUD_FRAME, [&](DepthCamera &dc, const Frame &frame, DepthCamera::FrameType c) {
        const XYZIPointCloudFrame *d = dynamic_cast<const XYZIPointCloudFrame *>(&frame);
        //int AreaNum, AreaNum2;
        if (!d) {
            std::cout << "Null frame captured? or not of type XYZIPointCloudFrame" << std::endl;
            return;
        }

        if (lastTimeStamp != 0) {
            std::cout << " (" << 1E6/(d->timestamp - lastTimeStamp) << " fps) ||| ";
        }

        func(d, ad1, Y_MIN, Y_MAX, 1);
        func(d, ad1, Y_MIN_2, Y_MAX_2, 2);

/*
        AreaNum2 = func(d, Y_MIN_2, Y_MAX_2, X_MIN, X_MAX);
        if(AreaNum2 == 3){
            printf("Area2 3 |||");
        } else if (AreaNum2 == 2) {
            printf("Area2 2 |||");
        } else if (AreaNum2 == 1) {
            printf("Area2 1 |||");
        } else {
            printf("No Object |||");
        }

        AreaNum = func(d, Y_MIN, Y_MAX, X_MIN, X_MAX);
        if(AreaNum == 3){
            printf("Area 3\n");
        } else if (AreaNum == 2) {
            printf("Area 2\n");
        } else if (AreaNum == 1) {
            printf("Area 1\n");
        } else {
            printf("No Object\n");
        }
*/
/*
        for (int i = 0 ; i < IMAGESIZE ; i++) {
            if ((Y_MIN <= d->points[i].y) && (Y_MAX >= d->points[i].y) && (I_MIN < d->points[i].i)) {
                if ((X_MIN <= d->points[i].x) && (X_MAX >= d->points[i].x) && (Z_MAX >= d->points[i].z)) {
                    if (AREA_1 < d->points[i].z) {
                        printf("Area 1");
                    } else if ((AREA_2 < d->points[i].z) && AREA_1 >= d->points[i].z) {
                        printf("Area 2");
                    } else {
                        printf("Area 3");
                    }
                } else {
                    printf("No Object\n");
                }
            }
        }

        for (int i = 0 ; i < IMAGESIZE ; i++) {
            if ((Y_MIN_2 <= d->points[i].y) && (Y_MAX_2 >= d->points[i].y) && (I_MIN < d->points[i].i)) {
                if ((X_MIN <= d->points[i].x) && (X_MAX >= d->points[i].x) && (Z_MAX >= d->points[i].z)) {
                    if (AREA_1 < d->points[i].z) {
                        printf("Area 1");
                    } else if ((AREA_2 < d->points[i].z) && AREA_1 >= d->points[i].z) {
                        printf("Area 2");
                    } else {
                        printf("Area 3");
                    }
                } else {
                    printf("No Object\n");
                }
            }
        }
*/



#ifdef _CALLBACK_INFO
        printf("Capture frame %05d@%ld", d->id, d->timestamp);
        printf(" (%7.4f fps) ", 1E6/(d->timestamp - lastTimeStamp));
        printf("Object Detected : z : %10.7f \n", d->points[0].z);
#endif

       lastTimeStamp = d->timestamp;
        //count++;
        //if(count >= frameCount) {
        //    dc.stop();
        //}
    });

    auto pfname = depthCamera->getCameraProfileNames();
    Map<int, String>::iterator iter;
    for (iter = pfname.begin(); iter != pfname.end(); ++iter) {
        std::cout << "[" << (*iter).first << "] " << (*iter).second << std::endl;
    }

    depthCamera->setCameraProfile(128);
    depthCamera->setCameraProfile(5);

    FrameSize fs;
    fs.width = FRAME_WIDTH;
    fs.height = FRAME_HEIGHT;
    depthCamera->setFrameSize(fs);
  
    printf("Current Camera Profile : %d\n", depthCamera->getCurrentCameraProfileID());

    if(depthCamera->start()) {
        FrameRate r;
        if (depthCamera->getFrameRate(r)) {
            logger(LOG_INFO) << "Capturing at a frame rate of " << r.getFrameRate() << " fps" << std::endl;
        }
        depthCamera->wait();
    }
    else {
        logger(LOG_ERROR) << "Could not start the depth camera " << depthCamera->id() << std::endl;
    }
    return 0;
}
