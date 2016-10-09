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

#include <iostream>

#include <opencv/cv.h>
#include <opencv/highgui.h>

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
    , m_font() {}

TrafficLightDetector::~TrafficLightDetector() {}

void TrafficLightDetector::setUp() {
    // This method will be call automatically _before_ running body().
    if (m_debug) {
        // Create an OpenCV-window.
        cvNamedWindow("WindowShowImage", CV_WINDOW_AUTOSIZE);
        cvMoveWindow("WindowShowImage", 300, 100);
    }
}

void TrafficLightDetector::tearDown() {
    // This method will be call automatically _after_ return from body().
    if (m_image != NULL) {
        cvReleaseImage(&m_image);
    }

    if (m_debug) {
        cvDestroyWindow("WindowShowImage");
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
                memcpy(m_image->imageData,
                m_sharedImageMemory->getSharedMemory(),
                si.getWidth() * si.getHeight() * numberOfChannels);
            }

            // Mirror the image.
            cvFlip(m_image, 0, -1);

            retVal = true;
        }
    }
    return retVal;
}

void TrafficLightDetector::processImage() {
}

// This method will do the main data processing job.
// Therefore, it tries to open the real camera first. If that fails, the virtual camera images from camgen are used.
odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode TrafficLightDetector::body() {
    // Get configuration data.
    KeyValueConfiguration kv = getKeyValueConfiguration();
    m_debug = kv.getValue< int32_t >("lanefollower.debug") == 1;

    // Initialize fonts.
    const double hscale = 0.4;
    const double vscale = 0.3;
    const double shear = 0.2;
    const int thickness = 1;
    const int lineType = 6;

    cvInitFont(&m_font, CV_FONT_HERSHEY_DUPLEX, hscale, vscale, shear, thickness, lineType);

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

//        // Create container for finally sending the set values for the control algorithm.
//        Container c2(m_actuationRequest);
//        // Send container.
//        getConference().send(c2);
    }

    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}
}
} // opendlv::legacy
