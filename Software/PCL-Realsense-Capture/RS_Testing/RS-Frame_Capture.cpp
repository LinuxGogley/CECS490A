/***********************************************************
 * Author:  Daniel Tran
 *          Liam Gogley
 * 
 * Project: 3D-ROMAP Senior Project
 * Purpose: The following .cpp file will utilize the Intel
 *          realsense camera to stream and capture frame
 *          data of the environment. Color is then applied
 *          and a point cloud is generated and saved to
 *          a point cloud data format (.pcd).
 * 
 * Rev:     Color data to pointer.
 * 
 * 
 * Version 0.07 - Last Editted 6/24/18
 ***********************************************************/

#include <iostream>

// Intel Realsense Headers
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API

// PCL Headers
#include <pcl/io/pcd_io.h>

#include <pcl/point_types.h>
#include <pcl/filters/passthrough.h>

#include <pcl/common/common_headers.h>
#include <pcl/features/normal_3d.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/console/parse.h>
#include <boost/thread/thread.hpp>

#include <pcl/io/io.h>
#include <pcl/visualization/cloud_viewer.h>

using namespace std;

void Load_PCDFile(void);

int main(int argc, char * argv[])
{
    // Object Definitions
    pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZRGBA>);
    pcl::PointCloud<pcl::PointXYZRGBA>::Ptr newCloud (new pcl::PointCloud<pcl::PointXYZRGBA>);

    //Point Cloud Object for PC calculation/Texture rendering
    rs2::pointcloud pc;

    // Points Object to store data points & color
    rs2::points points;

    // Declare RealSense Pipeline (Device/Sensors) and stream with default config
    rs2::pipeline pipe;
    pipe.start();

    // Wait for frames from the camera to settle
    for (auto i = 0; i < 29; ++i) pipe.wait_for_frames();

    // Depth and color capture
    auto frames = pipe.wait_for_frames();
    auto depth  = frames.get_depth_frame(); // Obtain depth from captured frame
    auto color  = frames.get_color_frame(); // Obtain color from captured frame

    // Point cloud object maps color from frame
    pc.map_to(color);
    
    // Generate point cloud mapping
    points = pc.calculate(depth);

    //================================
    // Realsense Data ---> PCL Format
    //================================
    // Convert data captured from Realsense camera to PCL
    auto sp = points.get_profile().as<rs2::video_stream_profile>();
    cloud->width = sp.width();
    cloud->height = sp.height();
    cloud->is_dense = false;
    cloud->points.resize(points.size());
    
    auto d_ptr = points.get_vertices();             // Get Vertices
    
    for (auto& p : cloud->points)
    {
        p.x = d_ptr->x;
        p.y = d_ptr->y;
        p.z = d_ptr->z;
        p.a = color.get_bits_per_pixel();        // Bits per pixel 'a' from color
        d_ptr++;
    }
    
    //========================================
    // Filter PointCloud (PassThrough Method)
    //========================================
    pcl::PassThrough<pcl::PointXYZRGBA> Cloud_Filter; // Create the filtering object
    Cloud_Filter.setInputCloud (cloud);           // Input generated cloud to filter
    Cloud_Filter.setFilterFieldName ("z");        // Set field name to Z-coordinate
    Cloud_Filter.setFilterLimits (0.0, 1.0);      // Set accepted interval values
    Cloud_Filter.filter (*newCloud);              // Filtered Cloud Outputted

    //==============================
    // Write PC to .pcd File Format
    //==============================
    // Take Cloud Data and write to .PCD File Format
    cout << "Generating PCD Point Cloud File... " << endl;
    pcl::io::savePCDFileASCII("Captured_Frame.pcd", *newCloud);
    cout << "Captured_Frame.pcd successfully generated. " << endl; 
    
    //==============================
    // Read .pcd File and visualize
    //==============================
    Load_PCDFile(); // Load the generated .pcd and display in viewer

    cout << "Exitting Program..." << endl; 
    return 0;
}

// Function will load PCD file generated and display data points of cloud
void Load_PCDFile(void)
{
    // Generate object to store cloud in .pcd file
    pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloudView (new pcl::PointCloud<pcl::PointXYZRGBA>);
    
    pcl::io::loadPCDFile ("Captured_Frame.pcd", *cloudView); // Load .pcd File
    
    //==========================
    // Pointcloud Visualization
    //==========================
    // Create viewer object titled "Room Map"
    boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer (new pcl::visualization::PCLVisualizer ("Room Map"));
    // Set background of viewer to black
    viewer->setBackgroundColor (0, 0, 0); 
    // Add generated point cloud and identify with string "Room Cloud"
    viewer->addPointCloud<pcl::PointXYZRGBA> (cloudView, "Cloud");
    // Default size for rendered points
    viewer->setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "Cloud");
    // Viewer Properties
    //viewer->addCoordinateSystem (1.0); // Display XYZ axis
    viewer->initCameraParameters();  // Camera Parameters for ease of viewing
    
    cout << "Loaded " << cloudView->width * cloudView->height
         << " data points from Captured_Frame.pcd with the following fields: "
         << std::endl;
    for (size_t i = 0; i < cloudView->points.size (); ++i)
    {
        cout << "    " << cloudView->points[i].x
             << " "    << cloudView->points[i].y
             << " "    << cloudView->points[i].z 
             << std::endl;
    }
    
    while (!viewer->wasStopped ())
    {
        viewer->spinOnce (100);
        boost::this_thread::sleep (boost::posix_time::microseconds (100000));
    }
}
