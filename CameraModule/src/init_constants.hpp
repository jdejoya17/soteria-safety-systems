#ifndef INITIALIZATION_H
#define INITIALIZATION_H
#include <cmath>
#include <string>

// Signal Type to send to bluetooth device.
enum IncomingObj
{
        NONE = '0',
        LEFT = '1',
        RIGHT = '2',
        BOTH = '3',
	DUMMY = '4'
};

// Return Code enumeration.
enum ReturnCode
{
        NO_DETECTED_DEVICES = 1,
		BLUETOOTH_NOT_CONNECTED = 2
};

// Throw exception if no device are detected.
class NoDevicesDetected : public std::exception
{
        private:
                const char* msg  = "No device detected. Are any device plugged in?";
                ReturnCode code = ReturnCode::NO_DETECTED_DEVICES;

        public:
                virtual const char* what() const throw()
                {
                        return msg;
                }

                ReturnCode return_code()
                {
                        return code;
                }
};
//
// Throw exception if no device are detected.
class BluetoothNotConnected : public std::exception
{
        private:
                const char* msg  = "Unable to connect Camera module and LED module via bluetooth.";
                ReturnCode code = ReturnCode::BLUETOOTH_NOT_CONNECTED;

        public:
                virtual const char* what() const throw()
                {
                        return msg;
                }

                ReturnCode return_code()
                {
                        return code;
                }
};

// Constants.

// Startup delay to let the bluetooth connection settle.
const int START_UP_DELAY = 10;

// Number of dummy signals sent to the LED module via
// bluetooth to stabilize connection.
const int NUM_DUMMY_SIGNAL = 100;

// 90 frames per second.
const float FRAMERATE = 30;

// Time between frame polls (milliseconds).
const int SAMPLING_PERIOD = round((1.0 / FRAMERATE) * 1000);

// Convertion rate feet/meters
const float FEET_PER_METER = 3.28;
//
// Convertion rate inches/feet
const float INCHES_PER_FEET = 12.0;

// Define frame queue capacity
const int QUEUE_CAPACITY = 5;


// Camera resolution
const int DEPTH_FRAME_HEIGHT = 480;
const int DEPTH_FRAME_WIDTH = 640;

// ROI Window size
const int ROI_SIZE = 151; // pixels

// Distance threshold - 10ft or 10in
const float DISTANCE_THRESHOLD = 15.0;

// Number of object pixels threshold
const float NUM_OBJ_PIX_THRESHOLD = pow((ROI_SIZE - 1), 2) / 4.0;

// Number of frames an object appears in the ROI threshold.
const int NUM_FRAMES_WITH_OBJ_THRESHOLD = 10;

// Left ROI row range
const int LROI_ROW_START =  (DEPTH_FRAME_HEIGHT / 2) - ((ROI_SIZE - 1) / 2);
const int LROI_ROW_END =  (DEPTH_FRAME_HEIGHT / 2) + ((ROI_SIZE - 1) / 2);
const int LROI_COL_START =  (DEPTH_FRAME_WIDTH / 4) - ((ROI_SIZE - 1) / 2);
const int LROI_COL_END =  (DEPTH_FRAME_WIDTH / 4) + ((ROI_SIZE - 1) / 2);

// Right ROI row range
const int RROI_ROW_START =  (DEPTH_FRAME_HEIGHT / 2) - ((ROI_SIZE - 1) / 2);
const int RROI_ROW_END =  (DEPTH_FRAME_HEIGHT / 2) + ((ROI_SIZE - 1) / 2);
const int RROI_COL_START =  (3 * DEPTH_FRAME_WIDTH / 4) - ((ROI_SIZE - 1) / 2);
const int RROI_COL_END =  (3 * DEPTH_FRAME_WIDTH / 4) + ((ROI_SIZE - 1) / 2);

// Bluetooth information.
// filepath associated to rfcomm serial port
const std::string PORT_NAME = "/dev/rfcomm0";
const std::string BT_MAC_ADDR = "00:14:03:06:19:EF";

#endif
