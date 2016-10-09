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

#ifndef TRAFFICLIGHTDETECTOR_H_
#define TRAFFICLIGHTDETECTOR_H_

#include <opencv/cv.h>

#include <memory>

#include <opendavinci/odcore/base/module/DataTriggeredConferenceClientModule.h>
#include <opendavinci/odcore/data/TimeStamp.h>
#include <opendavinci/odcore/wrapper/SharedMemory.h>

namespace opendlv {
namespace legacy {

using namespace std;

/**
 * This class is an exemplary skeleton for processing video data.
 */
class TrafficLightDetector : public odcore::base::module::DataTriggeredConferenceClientModule {
   private:
    /**
     * "Forbidden" copy constructor. Goal: The compiler should warn
     * already at compile time for unwanted bugs caused by any misuse
     * of the copy constructor.
     *
     * @param obj Reference to an object of this class.
     */
    TrafficLightDetector(const TrafficLightDetector & /*obj*/);

    /**
     * "Forbidden" assignment operator. Goal: The compiler should warn
     * already at compile time for unwanted bugs caused by any misuse
     * of the assignment operator.
     *
     * @param obj Reference to an object of this class.
     * @return Reference to this instance.
     */
    TrafficLightDetector &operator=(const TrafficLightDetector & /*obj*/);

   public:
    /**
     * Constructor.
     *
     * @param argc Number of command line arguments.
     * @param argv Command line arguments.
     */
    TrafficLightDetector(const int32_t &argc, char **argv);

    virtual ~TrafficLightDetector();

    virtual void nextContainer(odcore::data::Container &c);

   protected:
    /**
     * This method is called to process an incoming container.
     *
     * @param c Container to process.
     * @return true if c was successfully processed.
     */
    bool readSharedImage(odcore::data::Container &c);

   private:
    virtual void setUp();
    virtual void tearDown();
    void processImage();

//    void detectAndDisplay(Mat& frame, int nr);
//    void DetectCircles(cv::Mat &traffic_template, int nr);

   private:
    bool m_hasAttachedToSharedImageMemory;
    std::shared_ptr< odcore::wrapper::SharedMemory > m_sharedImageMemory;
    IplImage *m_image;
    bool m_debug;

    cv::CascadeClassifier m_cascadeClassifier;
    cv::Mat m_frame;
};
}
} // opendlv::legacy

#endif /*TRAFFICLIGHTDETECTOR_H_*/
