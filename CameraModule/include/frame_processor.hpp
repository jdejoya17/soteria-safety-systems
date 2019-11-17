#ifndef FRAME_PROCESSOR_H
#define FRAME_PROCESSOR_H

#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>
#include "bt_transmitter.hpp"

namespace Helpers
{
	float meters_to_feet(float meters);
	float feet_to_inches(float feet);
}

namespace Processors
{
	// Region processing.
	// Conditions to detect an object in the ROI.
	// (1) 50% of the pixels meet the distance threshold
	// (2) The distance threshold is 10ft. But for the purposes
	//              of testing, we will scale it to 10 inches.
	// (3) The object appears in the ROI for 10 consequetive frames.
	bool detect_object(const cv::Mat depth_mat);


	// Processing block.
	// This function will:
	// (1) Start a thread to detect if there is an object
	// in the defined left or right ROI. The threads return a boolean
	// indicating the presence of an object in the ROI.
	// (2) Send a signal to the pair bluetooth device if the object
	// appears for more than the set number of frames.
	void process_frame(
		const rs2::pipeline& pipe,
		rs2::frame_queue queue,
		BTTransmitter bt_transmitter
	);


	// Frame polling loop.
	void poll_frame(
		rs2::pipeline pipe,
		rs2::frame_queue queue
	);
}


#endif
