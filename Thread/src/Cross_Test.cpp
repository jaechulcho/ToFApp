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

#define MY_PRIORITY	(99)
#define MAX_SAFE_STACK	(8*1024)

using namespace Voxel;

int32_t frameCount = 1;
int64_t lastTimeStamp = 0;
int thread_count = 0;

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

  while(option->nId >= 0)
  {
    std::cout << option->pszArg << " " << option->helpInfo << std::endl;
    option++;
  }
}

char buf[160*120];
void depthThread()
{
    struct mq_attr attr;
    mqd_t mfd;
    const XYZIPointCloudFrame *d;
    char *bufaddr = buf;

    attr.mq_maxmsg  = 160*120;
    attr.mq_msgsize = 160*120;
    mfd = mq_open("/depthThread", O_RDONLY | O_CREAT, 0777, &attr);

    if(mfd == -1){
        perror("depthThread mq_open error \n");
        exit(-1);
    }
    while(1){
        printf("here\n");
        if(mq_receive(mfd, buf, 160*120, NULL) == -1){
            perror("depthThread Receive error");
            exit(-1);
        }
        printf("depthThread Point ::: %p\n", d);

        d = *(const XYZIPointCloudFrame**)bufaddr;

        for (int y = 58; y < 62; ++y) {
            for (int x = 0; x < 160; ++x) {
                if ((d->points[x+160*y].z < 1.0) && (d->points[x+160*y].i > 0.001)) {
                    printf("Capture frame %05d@%lld", d->id, d->timestamp);
                    printf(" (%7.4f fps) ", 1E6/(d->timestamp - lastTimeStamp));
                    printf("Object Detected : z : %10.7f\n", d->points[x+160*y].z);
                }
            }
        }
    }
	return;
}

int main(int argc, char *argv[])
{

  //pthread_t thread;
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
  CSimpleOpt s(argc, argv, argumentSpecifications);

  logger.setDefaultLogLevel(LOG_INFO);

  uint16_t vid = 0;

  Vector<uint16_t> pids;
  String serialNumber;
  String dumpFileName;

  //int32_t frameCount = 1;

  char *endptr;

  while (s.Next())
  {
    if (s.LastError() != SO_SUCCESS)
    {
      std::cout << s.GetLastErrorText(s.LastError()) << ": '" << s.OptionText() << "' (use -h to get command line help)" << std::endl;
      help();
      return -1;
    }

    //std::cout << s.OptionId() << ": " << s.OptionArg() << std::endl;

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

  if(vid == 0 || pids.size() == 0 || pids[0] == 0)
  {
    logger(LOG_ERROR) << "Required argument missing." << std::endl;
    help();
    return -1;
  }

  if (dumpFileName.size() != 0) {
    std::ofstream f(dumpFileName, std::ios::binary | std::ios::out);
    if(!f.good())
    {
      logger(LOG_ERROR) << "Failed to open '" << dumpFileName << "'" << std::endl;
      return -1;
    }
  }	

  CameraSystem sys;

  // Get all valid detected devices
  const Vector<DevicePtr> &devices = sys.scan();

  DevicePtr toConnect;

  std::cout << "Detected devices: " << std::endl;
  for(auto &d: devices)
  {
    std::cout << d->id() << std::endl;

    if(d->interfaceID() == Device::USB)
    {
      USBDevice &usb = (USBDevice &)*d;

      if(usb.vendorID() == vid && (serialNumber.size() == 0 || usb.serialNumber() == serialNumber))
      {
        for(auto pid: pids)
          if(usb.productID() == pid)
            toConnect = d;
      }
    }
  }

  if(!toConnect)
  {
    logger(LOG_ERROR) << "No valid device found for the specified VID:PID:serialnumber" << std::endl;
    return -1;
  }

  DepthCameraPtr depthCamera = sys.connect(toConnect);

  if(!depthCamera)
  {
    logger(LOG_ERROR) << "Could not load depth camera for device " << toConnect->id() << std::endl;
    return -1;
  }

  if(!depthCamera->isInitialized())
  {
    logger(LOG_ERROR) << "Depth camera not initialized for device " << toConnect->id() << std::endl;
    return -1;
  }

  std::cout << "Successfully loaded depth camera for device " << toConnect->id() << std::endl;

  int count = 0;

    TimeStampType lastTimeStamp = 0;

    std::thread th(depthThread);
    //th.join();

    struct mq_attr attr;
    mqd_t mfd;

    attr.mq_maxmsg   = 160*120;
    attr.mq_msgsize  = 160*120;


depthCamera->registerCallback(DepthCamera::FRAME_XYZI_POINT_CLOUD_FRAME, [&](DepthCamera &dc, const Frame &frame, DepthCamera::FrameType c) {
	const XYZIPointCloudFrame *d = dynamic_cast<const XYZIPointCloudFrame *>(&frame);

  if(!d)
  {
    std::cout << "Null frame captured? or not of type XYZIPointCloudFrame" << std::endl;
    return;
  }

  if(lastTimeStamp != 0)

  lastTimeStamp = d->timestamp;
  count++;
  if(count >= frameCount)
    dc.stop();

  mfd = mq_open("/depthThread", O_WRONLY|O_CREAT, 0777, &attr);
  if(mfd == -1){
      perror("mq open error \n");
      return;
  }

  if(mq_send(mfd, (char*)&d, 160*120, 0) == -1)
  {
      perror("mfd send error \n");

  }else{
      printf("mfd send success \n");
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

  if(depthCamera->start())
  {
    FrameRate r;
    if(depthCamera->getFrameRate(r))
      logger(LOG_INFO) << "Capturing at a frame rate of " << r.getFrameRate() << " fps" << std::endl;
    depthCamera->wait();
  }
  else
    logger(LOG_ERROR) << "Could not start the depth camera " << depthCamera->id() << std::endl;

  th.join();

  return 0;
}
