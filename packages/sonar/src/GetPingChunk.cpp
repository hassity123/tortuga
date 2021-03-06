/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Michael Levashov
 * All rights reserved.
 *
 * Author: Michael Levashov
 * File:  packages/sonar/src/GetPingChunk.cpp
 */

// STD Includes
#include <complex>
//#include <iostream>

// Project Includes
#include "sonar/include/PingDetect.h"
#include "sonar/include/Sonar.h"
#include "sonar/include/GetPingChunk.h"

#include "drivers/bfin_spartan/include/spartan.h"
#include "drivers/bfin_spartan/include/dataset.h"

using namespace std;

namespace ram {
namespace sonar {

getPingChunk::getPingChunk(const int* kBands)
	: pdetect(PD_THRESHOLDS, NCHANNELS, kBands, PING_DETECT_FRAME)
{}

getPingChunk::~getPingChunk()
{}

int
getPingChunk::getChunk(adcdata_t** data, int* locations, struct dataset* dataSet)
{
    //Initialize some things
    last_detected=0;
    pdetect.purge(); //re-zero the parameters, even if I already have done it again.  Doesn't hurt that much, since it was done once
    for(int channel=0; channel<NCHANNELS; channel++)
    {
        last_ping_index[channel]=0;
        last_value[channel]=0;
    }

    for(int i=0; i<dataSet->size; i++)
    {
        sample[0] = getSample(dataSet, 0, i);
        sample[1] = getSample(dataSet, 1, i);
        sample[2] = getSample(dataSet, 2, i);
        sample[3] = getSample(dataSet, 3, i);

        detected=pdetect.p_update(sample);

        if(i<DFT_FRAME) //The DFT initializes to 0, so I ignore all points before it
            continue;
        else if(i==DFT_FRAME)
            pdetect.reset_minmax();

        if(i-last_detected>MAX_PING_SEP)
        {
            for(int j=0; j<NCHANNELS; j++)
                last_value[j]=0;
            last_detected=0;
        }

        if(detected !=0)
        {
            last_detected=i;
            if(((detected & 1) != 0) && (last_value[0]!=1))
            {
                last_value[0]=1;
                last_ping_index[0]=i;
            }
            if(((detected & 2) != 0) && (last_value[1]!=1))
            {
                last_value[1]=1;
                last_ping_index[1]=i;
            }
            if(((detected & 4) != 0) && (last_value[2]!=1))
            {
                last_value[2]=1;
                last_ping_index[2]=i;
            }
            if(((detected & 8) != 0) && (last_value[3]!=1))
            {
                last_value[3]=1;
                last_ping_index[3]=i;
            }
        }

        if((last_value[0]==1) &&
           (last_value[1]==1) &&
           (last_value[2]==1) &&
           (last_value[3]==1))
        {
            if((last_ping_index[0]<ENV_CALC_FRAME) || //too close to the start, skip it
               (last_ping_index[1]<ENV_CALC_FRAME) || 
               (last_ping_index[2]<ENV_CALC_FRAME) || 
               (last_ping_index[3]<ENV_CALC_FRAME))
            {
                pdetect.reset_minmax();
                for(int channel=0; channel<NCHANNELS; channel++)
                    last_value[channel]=0;
            }
            else
            {
                for(int channel=0; channel<NCHANNELS; channel++)
                {
                    for(int k=0; k<ENV_CALC_FRAME; k++)
                        data[channel][k]=getSample(dataSet, channel, k+last_ping_index[channel]-ENV_CALC_FRAME+1+DFT_FRAME/2); //might need to be tweaked
                    locations[channel]=last_ping_index[channel]-ENV_CALC_FRAME+1;
                }
                //cout<<"Ping Detected at "<<locations[0]<<endl;

                return 1;
            }
        }
    }
    return 0;
}

}//sonar
}//ram
