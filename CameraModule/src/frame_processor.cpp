#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>
#include <thread>
#include <future>
#include <unordered_map>
#include <chrono>

#include "bt_transmitter.hpp"
#include "frame_processor.hpp"
#include "init_constants.hpp"

float Helpers::meters_to_feet(float meters)
{
	return meters * FEET_PER_METER;
}

float Helpers::feet_to_inches(float feet)
{
	return feet * INCHES_PER_FEET;
}

bool Processors::detect_object(const cv::Mat depth_mat)
{
	// ROI dimensions.
	int nrows = depth_mat.rows;
	int ncols = depth_mat.cols;

	// Keep track of object pixels.
	int num_object_pixels = 0;
	float pixel_distance = 0.0;

	// average distance.
	float avg_distance = 0.0;

	// Check each pixel in the ROI if they belong to the object.
	for (int i = 0; i < nrows; i++)
	{
		for (int j = 0; j < ncols; j++)
		{
			pixel_distance = depth_mat.at<float>(i, j);
			pixel_distance  = Helpers::meters_to_feet(pixel_distance);
			pixel_distance = Helpers::feet_to_inches(pixel_distance);

			avg_distance += pixel_distance;

			if (pixel_distance <= DISTANCE_THRESHOLD)
			{
				num_object_pixels++;
			}

		}
	}

	// Check if the number of object pixels meet the threshold.
	if (num_object_pixels >= NUM_OBJ_PIX_THRESHOLD)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void Processors::process_frame(
	const rs2::pipeline& pipe,
	rs2::frame_queue queue,
	BTTransmitter bt_transmitter
)
{
	// Get the depth scale of the depth sensor.
	float depth_scale = pipe.get_active_profile()
		.get_device()
		.first<rs2::depth_sensor>()
		.get_depth_scale();

        // Initialize object counter for each ROI.
        bool object_detected;
        int num_frames_with_obj_lroi = 0;
        int num_frames_with_obj_rroi = 0;

	// Process each frame.
	while (true)
	{
		rs2::frame frame;
		if (queue.poll_for_frame(&frame))
		{
			// Get depth frame.
			frame.get_data();
			rs2::depth_frame depth(frame);

			// Convert rs2 depth frame into openCV Mat of 32-bit float.
			cv::Mat depth_mat(
				cv::Size(DEPTH_FRAME_WIDTH, DEPTH_FRAME_HEIGHT),
				CV_16UC1,
				(void*)depth.get_data(),
				cv::Mat::AUTO_STEP
			);

			// Convert to 32-bit floating point matrix.
			depth_mat.convertTo(
				depth_mat,
				CV_32F
			);

			// Scale depth matrix to appropriate distance
			// measurement in meters.
			depth_mat = depth_mat * depth_scale;


			// Creating processing thread.
                        std::unordered_map<std::string, std::future<bool>> roi_jobs;

			//  Left ROI job.
                        roi_jobs["left"] = (std::future<bool>)std::async (
				Processors::detect_object,
                                depth_mat(
                                        cv::Range(LROI_ROW_START,  LROI_ROW_END),
                                        cv::Range(LROI_COL_START,  LROI_COL_END)
                                )
                        );

                        // Right ROI job.
                        roi_jobs["right"] = (std::future<bool>)std::async (
                                Processors::detect_object,
                                depth_mat(
                                        cv::Range(RROI_ROW_START,  RROI_ROW_END),
                                        cv::Range(RROI_COL_START,  RROI_COL_END)
                                )
                        );

                        // Wait for ROI object detection threads to complete.
                        for (auto& roi_job : roi_jobs)
                        {
                                roi_job.second.wait();
                        }

                        // Record how many times an object appeared in the
                        // (1) left ROI consecutively.
                        if (roi_jobs["left"].get())
                        {
                                num_frames_with_obj_lroi++;
                        }
                        else
                        {
                                num_frames_with_obj_lroi = 0;
                        }
                        // (2) right ROI consecutively.
                        if (roi_jobs["right"].get())
                        {
                                num_frames_with_obj_rroi++;
                        }
			else
                        {
                                num_frames_with_obj_rroi = 0;
                        }


			// SEND signal to device.
                        if (num_frames_with_obj_lroi >= NUM_FRAMES_WITH_OBJ_THRESHOLD &&
                                num_frames_with_obj_rroi >= NUM_FRAMES_WITH_OBJ_THRESHOLD)
                        {
                                bt_transmitter.send(IncomingObj::BOTH);
                                std::cout << "Incoming object: ON BOTH SIDES" << std::endl;
                        }
                        else if (num_frames_with_obj_lroi >= NUM_FRAMES_WITH_OBJ_THRESHOLD)
                        {
                                bt_transmitter.send(IncomingObj::LEFT);
                                std::cout << "Incoming object: ON YOUR LEFT" << std::endl;

                        }
                        else if (num_frames_with_obj_rroi >= NUM_FRAMES_WITH_OBJ_THRESHOLD)
                        {
                                bt_transmitter.send(IncomingObj::RIGHT);
                                std::cout << "Incoming object: ON YOUR RIGHT" << std::endl;
                        }
                        else
                        {
                                bt_transmitter.send(IncomingObj::NONE);
                                std::cout << "Incoming object: NONE" << std::endl;
                        }
		}
	}

	return;
}


void Processors::poll_frame(
	rs2::pipeline pipe,
	rs2::frame_queue queue
)
{
	while (true)
	{
		rs2::frameset frames = pipe.wait_for_frames();
		queue.enqueue(frames.get_depth_frame());
		std::this_thread::sleep_for(std::chrono::milliseconds(SAMPLING_PERIOD));
	}

}


