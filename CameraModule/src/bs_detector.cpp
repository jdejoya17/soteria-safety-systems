#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <thread>
#include <future>
#include <chrono>
#include <iostream>
#include <exception>
#include <string>
#include <vector>
#include <unordered_map>

#include "init_constants.hpp"
#include "depth_stream_visualizer.hpp"
#include "bt_transmitter.hpp"
#include "frame_processor.hpp"

void debug(std::string str) 
{
	std::cout << str << std::endl;
	return;
}



int main() try
{
	// Connect to LED module via bluetooth.
	std::cout << "Establishing bluetooth connection..." << std::flush;
	BTTransmitter bt_transmitter(PORT_NAME, BT_MAC_ADDR);
	bool connected = bt_transmitter.connect();
	if (not connected) 
	{
		throw BluetoothNotConnected();
	}
	std::this_thread::sleep_for(std::chrono::seconds(START_UP_DELAY));
	std::cout << "done." << std::endl;



	// Stabilize bluetooth connection.
	std::cout << "Stabilize bluetooth connection..." << std::flush;
	std::this_thread::sleep_for(std::chrono::seconds(START_UP_DELAY));
	for (int i = 0; i < NUM_DUMMY_SIGNAL; i++) 
	{
		bt_transmitter.send(IncomingObj::DUMMY);
	}
	bt_transmitter.send(IncomingObj::DUMMY);
	std::cout << "done." << std::endl;



	// Detect device.
	std::cout << "Detecting camera..." << std::flush;
	rs2::context ctx;

	// Detect the list of currently connected realsense device.
	rs2::device_list list = ctx.query_devices();

	// Throw a runtime error if there are no devices detected.
	if (list.size() == 0) 
	{
		throw NoDevicesDetected();
	}

	// Gets the reference to the first detected device.
	// Assuming there are multiple devices then, list
	// contains all detected devices.
	rs2::device dev = list.front();
	std::cout << "camera ready." << std::endl;



	// Create pipeline.
	std::cout << "Starting pipeline..." << std::flush;
	rs2::pipeline pipe;

	// Create pipeline configuration.
	rs2::config cfg;
	cfg.disable_all_streams();
	cfg.enable_stream(
		rs2_stream::RS2_STREAM_DEPTH, 
		DEPTH_FRAME_WIDTH,
		DEPTH_FRAME_HEIGHT,
		rs2_format::RS2_FORMAT_Z16,
		FRAMERATE
	);
	
	// Start streaming with default configurations.
	std::cout << "configured and running." << std::endl;
	pipe.start(cfg);

	// Create frame queue. To keep polling frames while
	// processing is being done. Allow a max of 5 frames
	// to be stored in the queue.
	rs2::frame_queue queue(QUEUE_CAPACITY);



	// Creating processing thread.
	std::cout << "Starting threads..." << std::flush;
	std::vector<std::thread> jobs;

	std::cout << "polling frames..." << std::flush;
	jobs.push_back(std::thread(Processors::poll_frame, pipe, queue));

	std::cout << "processing frames..." << std::flush;
	jobs.push_back(std::thread(Processors::process_frame, pipe, queue, bt_transmitter));

	std::cout << "visualizing frames..." << std::flush;
	jobs.push_back(std::thread(Visualizer::imshow_colorizer, pipe));

	// Join threads.
	std::cout << "joining threads" << std::endl;
	for (std::thread& job : jobs) 
	{
		job.join();
	}
	
	return 0;
}

// Catch runtime error.
catch (NoDevicesDetected& e) 
{
	std::cerr << "Error: " << e.what() << std::endl;
	return e.return_code();
}

// Catch runtime error.
catch (BluetoothNotConnected& e) 
{
	std::cerr << "Error: " << e.what() << std::endl;
	return e.return_code();
}

