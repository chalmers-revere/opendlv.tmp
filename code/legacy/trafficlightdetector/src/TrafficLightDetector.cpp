/**
 * trafficlightdetector - Sample application to detect traffic lights.
 * Copyright (C) 2016 Muhanad Nabeel, David Ustarbowski, Christian Berger
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <string>
#include <vector>

#include <opencv/highgui.h>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/contrib/contrib.hpp>

#include <opendavinci/GeneratedHeaders_OpenDaVINCI.h>
#include <opendavinci/odcore/base/KeyValueConfiguration.h>
#include <opendavinci/odcore/base/Lock.h>
#include <opendavinci/odcore/data/Container.h>
#include <opendavinci/odcore/io/conference/ContainerConference.h>
#include <opendavinci/odcore/wrapper/SharedMemoryFactory.h>

#include "TrafficLightDetector.h"

namespace opendlv {
namespace legacy {

using namespace std;
using namespace odcore::base;
using namespace odcore::data;
using namespace odcore::data::image;

TrafficLightDetector::TrafficLightDetector(const int32_t &argc, char **argv)
    : TimeTriggeredConferenceClientModule(argc, argv, "trafficlightdetector")
    , m_hasAttachedToSharedImageMemory(false)
    , m_sharedImageMemory()
    , m_image(NULL)
    , m_debug(false)
    , m_cascadeClassifier(NULL)
    , m_frame() {
    // Calling the xml trained haar.
    m_cascadeClassifier = new cv::CascadeClassifier("./haar_xml_07_19.xml");
}

TrafficLightDetector::~TrafficLightDetector() {
    if (m_cascadeClassifier != NULL) {
        delete m_cascadeClassifier;
    }
}

void TrafficLightDetector::setUp() {
    // This method will be call automatically _before_ running body().
    if (m_debug) {
        // Create an OpenCV-window.
        cv::namedWindow("WindowShowImage", CV_WINDOW_AUTOSIZE);
        cv::moveWindow("WindowShowImage", 300, 100);
    }
}

void TrafficLightDetector::tearDown() {
    // This method will be call automatically _after_ return from body().
    if (m_image != NULL) {
        cvReleaseImage(&m_image);
    }

    if (m_debug) {
        cv::destroyWindow("WindowShowImage");
    }
}

bool TrafficLightDetector::readSharedImage(Container &c) {
    bool retVal = false;

    if (c.getDataType() == odcore::data::image::SharedImage::ID()) {
        SharedImage si = c.getData< SharedImage >();

        // Check if we have already attached to the shared memory.
        if (!m_hasAttachedToSharedImageMemory) {
            m_sharedImageMemory = odcore::wrapper::SharedMemoryFactory::attachToSharedMemory(
            si.getName());
        }

        // Check if we could successfully attach to the shared memory.
        if (m_sharedImageMemory->isValid()) {
            // Lock the memory region to gain exclusive access using a scoped lock.
            Lock l(m_sharedImageMemory);
            const uint32_t numberOfChannels = 3;
            // For example, simply show the image.
            if (m_image == NULL) {
                m_image = cvCreateImage(cvSize(si.getWidth(), si.getHeight()), IPL_DEPTH_8U, numberOfChannels);
            }

            // Copying the image data is very expensive...
            if (m_image != NULL) {
                memcpy(m_image->imageData, m_sharedImageMemory->getSharedMemory(), si.getWidth() * si.getHeight() * numberOfChannels);
                m_frame = cv::cvarrToMat(m_image);
            }

            retVal = true;
        }
    }
    return retVal;
}

void TrafficLightDetector::processImage() {
    if (m_cascadeClassifier != NULL) {
        // Detect TrafficLights through cascade classifier.
        vector<cv::Rect> trLights;
        m_cascadeClassifier->detectMultiScale(m_frame, trLights, 1.1, 0, CV_HAAR_SCALE_IMAGE | CV_HAAR_FEATURE_MAX, cv::Size(24, 24));
        cv::Mat trafficTemplate;
        m_frame.copyTo(trafficTemplate);

        // Circle detection after detecting a traffic light.
        {
            vector<cv::Vec3f> VectorCir;
            cv::Mat resultImg;

            //Apply color map to search for certian color
            cv::applyColorMap(trafficTemplate, trafficTemplate, cv::COLORMAP_SUMMER);
            cv::inRange(trafficTemplate, cv::Scalar(0, 90, 90), cv::Scalar(204, 255, 255), resultImg);
            cv::GaussianBlur(resultImg, resultImg, cv::Size(9, 9), 0.5, 0.5);
            cv::HoughCircles(resultImg,
                VectorCir,
                CV_HOUGH_GRADIENT,
                2,
                90,
                50,
                20,
                4,
                10);

            for(auto iterCircles = VectorCir.begin(); iterCircles !=VectorCir.end(); iterCircles++) {
                cv::circle(trafficTemplate,
                    cv::Point((int)(*iterCircles)[0],(int)(*iterCircles)[1]),
                    3,
                    cv::Scalar(255,0,0),
                    CV_FILLED);

                cv::circle(trafficTemplate,cv::Point((int)(*iterCircles)[0],(int)(*iterCircles)[1]),
                    (int)(*iterCircles)[2],
                    cv::Scalar(0,0,255),
                    3);
            }
            if (m_debug) {
                cv::imshow("WindowShowImage", resultImg);
            }
        }
    }
}

// This method will do the main data processing job.
// Therefore, it tries to open the real camera first. If that fails, the virtual camera images from camgen are used.
odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode TrafficLightDetector::body() {
    // Get configuration data.
    KeyValueConfiguration kv = getKeyValueConfiguration();
    m_debug = kv.getValue< int32_t >("trafficlightdetector.debug") == 1;

    // Overall state machine handler.
    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
        bool has_next_frame = false;

        // Get the most recent available container for a SharedImage.
        Container c = getKeyValueDataStore().get(odcore::data::image::SharedImage::ID());

        if (c.getDataType() == odcore::data::image::SharedImage::ID()) {
            // Example for processing the received container.
            has_next_frame = readSharedImage(c);
        }

        // Process the read image and calculate regular lane following set values for control algorithm.
        if (true == has_next_frame) {
            processImage();
        }
    }

    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}
}
} // opendlv::legacy
