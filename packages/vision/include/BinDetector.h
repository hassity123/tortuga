/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/BinDetector.h
 */

#ifndef RAM_BIN_DETECTOR_H_06_23_2007
#define RAM_BIN_DETECTOR_H_06_23_2007

// STD Includes
#include <list>

// Project Includes
#include "core/include/ConfigNode.h"
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "vision/include/BlobDetector.h"
#include "vision/include/TrackedBlob.h"
#include "vision/include/SuitDetector.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
    
class RAM_EXPORT BinDetector : public Detector
{
  public:
    class Bin : public TrackedBlob
    {
    public:
        Bin();
        Bin(BlobDetector::Blob blob, double x, double y,
            math::Degree rotation, int id,  Suit::SuitType suit);

        Suit::SuitType getSuit() { return m_suit; }

        /** Draws the bounds of the bin in green, and its ID */
        void draw(Image* image);
        

    private:
        Suit::SuitType m_suit;
    };

    typedef std::list<Bin> BinList;
    typedef BinList::iterator BinListIter;

    
    BinDetector(core::ConfigNode config,
                core::EventHubPtr eventHub = core::EventHubPtr());
    ~BinDetector();

    void processImage(Image* input, Image* output= 0);
    
    void drawBinImage(Image* imgToShow, int binNumber, Image* output = 0);

    /** redSuit and rotatedRedSuit must be the same size, rotates redSuit,
        which is the image containing the center of the bin into rotatedRedSuit
    */
    void unrotateBin(math::Radian bin, Image* redSuit, 
		     Image* rotatedRedSuit);
    bool found();

    /** X cord of the bin closest to the center of the screen */
    float getX();
    
    /** Y cord of the bin closest to the center of the screen */
    float getY();

    /** angle of the bin closest to the center of the screen*/
    math::Degree getAngle();

    /** Gets the suit of the bin cloest to the center of the screen */
    Suit::SuitType getSuit();
    
    /** Gets our current set of bins, sorted close to farther from center */
    BinDetector::BinList getBins();
    
    void setSuitDetectionOn(bool);

    /** Turns on and off the logging of properly extracted suit images to disk
     *
     *  This places all the suit images in <log_dir>/suits/<suit_type>
     *  directory.  Where <log_dir> is the runs log directory, and <suit_type>
     *  is heart, spade, club, diamond, or unknown.
     *
     *  This only works when you are display debug images as well.
     *
     *  @param value
     *      True if you wish to log, false if not.
     */
    void setSuitImageLogging(bool value);
    
  private:
    IplImage* scaledRedSuit;

  
    void init(core::ConfigNode config);

    /** Processes the bin and fires off found event
     *
     *  It adds each process bin to the list of current bins.
     *
     *  @param bin
     *      The blob which bounds the black box of the bin
     *  @oaram detectoSuit
     *      True if we are to find the suit for suit detection
     *  @param output
     *      Our debug output image
     */
    void processBin(BlobDetector::Blob bin, bool detectSuit, BinList& newBins, 
		    int binNum, Image* ouput = 0);

    /** Called by process bin, if suit detection is request is true
     *
     *  @param bin
     *      The blob which bounds the black box of the bin
     *  @param input
     *      The image to extract the suit from
     *  @param outptu
     *      Our debug output image
     */
    Suit::SuitType determineSuit(Image* input,
                                 Image* output = 0);
    
    
    /** Called by process bin, must be called regardless of whether we plan
     *  to detect suits, as this function sets the angle of the bin.
     */
    math::Radian calculateAngleOfBin(BlobDetector::Blob bin, Image* input, Image* output = 0);

    /** Extracts the image of the suit an into a binImage of #binNum
     *
     *  The result is 128x128 pixels, and should be bounded around all of the
     *  red in the center of the bin.
     */
    bool cropImage(IplImage* rotatedRedSuit, int binNum);

    /** Logs the image of the suit to file based on the suit type */
    void logSuitImage(Image* image, Suit::SuitType suit);
    
    bool m_found;
/*    bool foundHeart;
    bool foundSpade;
    bool foundDiamond;
    bool foundClub;
    bool foundEmpty;*/
    /** Determines if we should try and determine the suit of the bin */
    bool m_runSuitDetector;
    /** Determines whether or not we should write suit images to disk */
    bool m_logSuitImages;
    int numBinsFound;
    IplImage* binFrame;
    IplImage* rotated;
    IplImage* bufferFrame;
    IplImage* whiteMaskedFrame;
    IplImage* blackMaskedFrame;
    SuitDetector suitDetector;
    BlobDetector blobDetector;

    IplImage* binImages[4];
    /** Maximum distance for the bin to be considred "centered" */
    double m_centeredLimit;
    
    /** Whether or not we are centered */
    bool m_centered;
    
    /** The max distance between bins on different frames */
    double m_sameBinThreshold;

    /** The maximum aspect ratio a blob can have and still be called a bin*/
    double m_maxAspectRatio;
    
    /** Our current set of bins */
    BinList m_bins;

    Suit suitCenteredOver;

    /** Current bin ids */
    int m_binID;
    
    int m_blackMaskMinimumPercent;
    int m_blackMaskMaxTotalIntensity;        
    int m_whiteMaskMinimumPercent;
    int m_whiteMaskMinimumIntensity;
    bool m_incrediblyWashedOutImages;
};

} // namespace vision
} // namespace ram

#endif // RAM_BIN_DETECTOR_H_06_23_2007
