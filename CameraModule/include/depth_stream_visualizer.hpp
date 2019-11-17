#ifndef VISUALIZER_H
#define VISUALIZER_H
#include <librealsense2/rs.hpp>

namespace Visualizer 
{
	// Visualize Depth Camera
	int imshow_colorizer(rs2::pipeline pipe);
}

#endif
