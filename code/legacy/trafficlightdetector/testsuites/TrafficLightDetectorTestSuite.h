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

#ifndef TRAFFICLIGHTDETECTORTESTSUITE_H_
#define TRAFFICLIGHTDETECTORTESTSUITE_H_

#include "cxxtest/TestSuite.h"

#include <memory>

#include <opendavinci/odcore/data/Container.h>
#include <opendavinci/odcore/wrapper/SharedMemory.h>
#include <opendavinci/odcore/wrapper/SharedMemoryFactory.h>

#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"

// Include local header files.
#include "../include/TrafficLightDetector.h"

using namespace std;
using namespace odcore::data;
using namespace odcore::data::image;
using namespace opendlv::legacy;

/**
 * This class derives from TrafficLightDetector to allow access to protected methods.
 */
class TrafficLightDetectorTestling : public TrafficLightDetector {
   private:
    TrafficLightDetectorTestling();

   public:
    TrafficLightDetectorTestling(const int32_t &argc, char **argv)
        : TrafficLightDetector(argc, argv) {}

    // Here, you need to add all methods which are protected in TrafficLightDetector and which are needed for the test cases.

    /**
     * This method calls the inherited but protected method readSharedImage from TrafficLightDetector.
     *
     * @param c Container to be processed.
     * @return result from inherited method.
     */
    bool callReadSharedImage(Container &c) {
        return readSharedImage(c);
    }
};

/**
 * The actual testsuite starts here.
 */
class TrafficLightDetectorTest : public CxxTest::TestSuite {
   private:
    TrafficLightDetectorTestling *ldt;

   public:
    /**
         * This method will be called before each testXYZ-method.
         */
    void setUp() {
        // Prepare the data that would be available from commandline.
        string argv0("trafficlightdetector");
        string argv1("--cid=100");
        int32_t argc = 2;
        char **argv;
        argv = new char *[2];
        argv[0] = const_cast< char * >(argv0.c_str());
        argv[1] = const_cast< char * >(argv1.c_str());

        // Create an instance of sensorboard through TrafficLightDetectorTestling which will be deleted in tearDown().
        ldt = new TrafficLightDetectorTestling(argc, argv);
    }

    /**
         * This method will be called after each testXYZ-method.
         */
    void tearDown() {
        delete ldt;
        ldt = NULL;
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Below this line the actual testcases are defined.
    ////////////////////////////////////////////////////////////////////////////////////

    void testTrafficLightDetectorSuccessfullyCreated() {
        TS_ASSERT(ldt != NULL);
    }

    void testTrafficLightDetectorProcessingMethod() {
        // First, create a shared memory region for the TrafficLightDetector.
        const uint32_t WIDTH = 3;
        const uint32_t HEIGHT = 4;
        const uint32_t BYTESPERPIXEL = 3;
        std::shared_ptr< odcore::wrapper::SharedMemory > imageProducer = odcore::wrapper::SharedMemoryFactory::createSharedMemory("ImageProducer", WIDTH * HEIGHT * BYTESPERPIXEL);
        TS_ASSERT(imageProducer->isValid());
        TS_ASSERT(imageProducer->getSize() == WIDTH * HEIGHT * BYTESPERPIXEL);
        imageProducer->lock();
        for (uint32_t i = 0; i < imageProducer->getSize(); i++) {
            *(((char *)(imageProducer->getSharedMemory())) + i) = ('A' + i);
        }
        imageProducer->unlock();

        // Wrap the shared memory region into a SharedImage.
        SharedImage si;
        si.setName(imageProducer->getName());
        si.setWidth(WIDTH);
        si.setHeight(HEIGHT);
        si.setBytesPerPixel(BYTESPERPIXEL);
        si.setSize(si.getWidth() * si.getHeight() * si.getBytesPerPixel());

        // Create a container.
        Container c(si);

        TS_ASSERT(ldt->callReadSharedImage(c) == true);
    }

    void testTrafficLightDetectorProcessingMethodInvalidSharedMemory() {
        // First, create a shared memory region for the TrafficLightDetector.
        const uint32_t WIDTH = 3;
        const uint32_t HEIGHT = 4;
        const uint32_t BYTESPERPIXEL = 3;

        // Wrap the shared memory region into a SharedImage.
        SharedImage si;
        si.setName("InvalidSharedMemory");
        si.setWidth(WIDTH);
        si.setHeight(HEIGHT);
        si.setBytesPerPixel(BYTESPERPIXEL);
        si.setSize(si.getWidth() * si.getHeight() * si.getBytesPerPixel());

        // Create a container.
        Container c(si);

        TS_ASSERT(ldt->callReadSharedImage(c) == false);
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Below this line the necessary constructor for initializing the pointer variables,
    // and the forbidden copy constructor and assignment operator are declared.
    //
    // These functions are normally not changed.
    ////////////////////////////////////////////////////////////////////////////////////

   public:
    /**
         * This constructor is only necessary to initialize the pointer variable.
         */
    TrafficLightDetectorTest()
        : ldt(NULL) {}

   private:
    /**
     * "Forbidden" copy constructor. Goal: The compiler should warn
     * already at compile time for unwanted bugs caused by any misuse
     * of the copy constructor.
     *
     * @param obj Reference to an object of this class.
     */
    TrafficLightDetectorTest(const TrafficLightDetectorTest & /*obj*/);

    /**
     * "Forbidden" assignment operator. Goal: The compiler should warn
     * already at compile time for unwanted bugs caused by any misuse
     * of the assignment operator.
     *
     * @param obj Reference to an object of this class.
     * @return Reference to this instance.
     */
    TrafficLightDetectorTest &operator=(const TrafficLightDetectorTest & /*obj*/);
};

#endif /*TRAFFICLIGHTDETECTORTESTSUITE_H_*/
