#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include "depth_stream_visualizer.hpp"
#include "init_constants.hpp"


// Visualize Depth Camera
int Visualizer::imshow_colorizer(rs2::pipeline pipe)
{
        //  Window name
        const std::string window_name  = "Depth Stream";
        cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);

        // Declare depth colorizer.
        rs2::colorizer color_map;


        while (cv::waitKey(1) < 0 && cv::getWindowProperty(window_name, cv::WND_PROP_AUTOSIZE) >= 0)
        {
                // Wait for frames.
                rs2::frameset data = pipe.wait_for_frames();

                // Apply colorizer.
                rs2::frame depth = data.get_depth_frame().apply_filter(color_map);

                // Get image dimensions.
                const int h = depth.as<rs2::video_frame>().get_height();
                const int w = depth.as<rs2::video_frame>().get_width();

                // Create CV matrix.
                cv::Mat img(cv::Size(w, h), CV_8UC3, (void*)depth.get_data(), cv::Mat::AUTO_STEP);

                // Draw region of interests.
                int side = 201;
                int wing = (side - 1) / 2;
                int thickness = 5;

                // Left ROI.
                //cv::Rect rect_left((w/4)-wing, (h/2)-wing, side, side);
                cv::Rect rect_left(LROI_COL_START, LROI_ROW_START, ROI_SIZE, ROI_SIZE);
                cv::rectangle(img, rect_left, cv::Scalar(0, 255, 0), thickness);
                // Right ROI.
                cv::Rect rect_right(RROI_COL_START, RROI_ROW_START, ROI_SIZE, ROI_SIZE);
                cv::rectangle(img, rect_right, cv::Scalar(0, 255, 0), thickness);

                // Show image.
                cv::imshow(window_name, img);
        }

        return EXIT_SUCCESS;

}

