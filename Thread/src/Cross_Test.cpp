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

#define MY_PRIORITY	(99)
#define MAX_SAFE_STACK	(8*1024)
#define GPIO_OUTPIN_NUM 4
#define GPIO_INPIN_NUM  3

using namespace Voxel;

int32_t frameCount = 1;
int64_t thread_lastTimeStamp = 0;

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

char buf[160*120];
void depthThread()
{
    fgpio gpio;
    mqd_t mfd;
    const XYZIPointCloudFrame *d;
    char *bufaddr = buf;
    mfd = mq_open("/depthThread", O_RDONLY, 0777);

    if (mfd == -1) {
        perror("depthThread mq_open error \n");
        exit(-1);
    }
    while (1) {
        if (mq_receive(mfd, buf, sizeof(buf), NULL) == -1) {
            perror("depthThread Receive error");
            sleep(1);
            continue;
        }
        //printf("depthThread Point ::: %p\n", d);

        d = *(const XYZIPointCloudFrame**)bufaddr;

        //for (int y = 58; y < 62; ++y) {
            //for (int x = 0; x < 160; ++x) {
                //if ((d->points[x+160*y].z < 1.0) && (d->points[x+160*y].i > 0.001)) {
                    printf("Capture frame %05d@%ld", d->id, d->timestamp);
                    printf(" (%7.4f fps) ", 1E6/(d->timestamp - thread_lastTimeStamp));
                    //printf("Object Detected : z : %10.7f\n", d->points[x+160*y].z);
                    printf("Object Detected : x : %10.7f , y : %10.7f, z : %10.7f :\n", d->points[0].x, d->points[0].y, d->points[0].z);
                    gpio.gpio_outmode_value(GPIO_OUTPIN_NUM, 1);
                //}
            //}
        //}

        thread_lastTimeStamp = d->timestamp;

    }
	return;
}

void trinity_thread(){

    while (1) {
        printf("trinity_thread in\n");
        sleep(1);
    }
    return;
}

void gpio_init(){
    fgpio gpio;
    gpio.gpio_use_sel(GPIO_OUTPIN_NUM);
    gpio.gpio_in_out_sel("out", GPIO_OUTPIN_NUM);
    return;
}

void MessageQue_init(){
    mq_unlink("/depthThread");
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
    MessageQue_init();
    gpio_init();
    CSimpleOpt s(argc, argv, argumentSpecifications);

    logger.setDefaultLogLevel(LOG_INFO);

    uint16_t vid = 0;
    Vector<uint16_t> pids;
    String serialNumber;
    String dumpFileName;
    int32_t frameCount = 1;
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
                frameCount = (int32_t)strtol(s.OptionArg(), &endptr, 10);
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

    struct mq_attr attr;//, attr_trinity;
    mqd_t mfd;//, mfd_trinity;

    attr.mq_maxmsg   = 10;
    attr.mq_msgsize  = 2000;

    mfd = mq_open("/depthThread", O_WRONLY|O_CREAT, 0777, &attr);
    if (mfd == -1) {
        perror("mq open error \n");
        return 0;
    }
 /*
    mfd_trinity = mq_open("/trinityThread", O_WRONLY|O_CREAT, 0777, &attr_trinity);
    if(mfd_trinity == -1){
        perror("mq open error \n");
        return 0;
    }
*/
    std::thread th(depthThread);
    std::thread th2(trinity_thread);

    depthCamera->registerCallback(DepthCamera::FRAME_XYZI_POINT_CLOUD_FRAME, [&](DepthCamera &dc, const Frame &frame, DepthCamera::FrameType c) {
        const XYZIPointCloudFrame *d = dynamic_cast<const XYZIPointCloudFrame *>(&frame);
        if (!d) {
            std::cout << "Null frame captured? or not of type XYZIPointCloudFrame" << std::endl;
            return;
        }

        if (lastTimeStamp != 0) {
            std::cout << " (" << 1E6/(d->timestamp - lastTimeStamp) << " fps)";
        }

        if(mq_send(mfd, (char*)&d, sizeof(d), 0) == -1) {
            perror("mfd send error \n");
        }
        else {
            printf("mfd send success \n");
        }
        /*
        if(mq_send(mfd_trinity, (char*)&d, sizeof(d), 0) == -1)
        {
        perror("mfd_trinity send error \n");

        }else{
        printf("mfd_trinity send success \n");
        }
        */
        lastTimeStamp = d->timestamp;
        count++;
        if(count >= frameCount) {
            dc.stop();
        }
    });

    auto pfname = depthCamera->getCameraProfileNames();
    Map<int, String>::iterator iter;
    for (iter = pfname.begin(); iter != pfname.end(); ++iter) {
        std::cout << "[" << (*iter).first << "] " << (*iter).second << std::endl;
    }

    depthCamera->setCameraProfile(128);
    depthCamera->setCameraProfile(5);

    FrameSize fs;
    fs.width = 160;
    fs.height = 120;
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

    th.join();
    th2.join();
    mq_close(mfd);


    return 0;
}
