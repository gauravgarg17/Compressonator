//===============================================================================
// Copyright (c) 2007-2016  Advanced Micro Devices, Inc. All rights reserved.
// Copyright (c) 2004-2006 ATI Technologies Inc.
//===============================================================================
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <math.h>
#include "bc7_definitions.h"
#include "bc7_utils.h"
#include "debug.h"

//
// Write one bit to a buffer at a specified offset
//
// Used by BC7_Encode

void    WriteBit(BYTE   *base,
                 int  offset,
                 BYTE   bitVal)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    int byteLocation;
    int remainder;
    BYTE    val;
    BYTE   mask;

    byteLocation = offset/8;
    remainder = offset%8;
    mask = ~(1 << remainder);
    bitVal &= 1;

    val = base[byteLocation];
    val = val & mask;
    val |= bitVal << remainder;

    base[byteLocation] = val & 0xff;
}


// Used by BC7_Decode
const double  rampLerpWeights[5][1<<MAX_INDEX_BITS] =
{
#if USE_FINAL_BC7_WEIGHTS
    {0.0},  // 0 bit index
    {0.0, 1.0}, // 1 bit index
    {0.0, 21.0/64.0, 43.0/64.0, 1.0}, // 2 bit index
    {0.0, 9.0/64.0, 18.0/64.0, 27.0/64.0, 37.0/64.0, 46.0/64.0, 55.0/64.0, 1.0}, // 3 bit index
    {0.0, 4.0/64.0, 9.0/64.0, 13.0/64.0, 17.0/64.0, 21.0/64.0, 26.0/64.0, 30.0/64.0,
     34.0/64.0, 38.0/64.0, 43.0/64.0, 47.0/64.0, 51.0/64.0, 55.0/64.0, 60.0/64.0, 1.0} // 4 bit index
#else
    // Pure linear weights
    {0.0},  // 0 bit index
    {0.0, 1.0}, // 1 bit index
    {0.0, 1.0/3.0, 2.0/3.0, 1.0}, // 2 bit index
    {0.0, 1.0/7.0, 2.0/7.0, 3.0/7.0, 4.0/7.0, 5.0/7.0, 6.0/7.0, 1.0}, // 3 bit index
    {0.0, 1.0/15.0, 2.0/15.0, 3.0/15.0, 4.0/15.0, 5.0/15.0, 6.0/15.0, 7.0/15.0,
     8.0/15.0, 9.0/15.0, 10.0/15.0, 11.0/15.0, 12.0/15.0, 13.0/15.0, 14.0/15.0, 1.0} // 4 bit index
#endif
};


//
// This routine generates the ramp colours with correct rounding
//
//
//
// Used by BC7_Decode

void GetRamp(DWORD endpoint[][MAX_DIMENSION_BIG],
             double ramp[MAX_DIMENSION_BIG][(1<<MAX_INDEX_BITS)],
             DWORD clusters[2],
             DWORD componentBits[MAX_DIMENSION_BIG])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    double ep[2][MAX_DIMENSION_BIG];
    DWORD i;

    // Expand each endpoint component to 8 bits by shifting the MSB to bit 7
    // and then replicating the high bits to the low bits revealed by
    // the shift
    for(i=0; i<MAX_DIMENSION_BIG; i++)
    {
        ep[0][i] = 0.;
        ep[1][i] = 0.;
        if(componentBits[i])
        {
            ep[0][i] = (double)(endpoint[0][i] << (8 - componentBits[i]));
            ep[1][i] = (double)(endpoint[1][i] << (8 - componentBits[i]));
            ep[0][i] += (double)((DWORD)ep[0][i] >> componentBits[i]);
            ep[1][i] += (double)((DWORD)ep[1][i] >> componentBits[i]);

            ep[0][i] = min(255., max(0., ep[0][i]));
            ep[1][i] = min(255., max(0., ep[1][i]));
        }
    }

    // If this block type has no explicit alpha channel
    // then make sure alpha is 1.0 for all points on the ramp
    if(!componentBits[COMP_ALPHA])
    {
        ep[0][COMP_ALPHA] = ep[1][COMP_ALPHA] = 255.;
    }

    DWORD   rampIndex = clusters[0];

    rampIndex = (DWORD)(log((double)rampIndex) / log(2.0));

    // Generate colours for the RGB ramp
    for(i=0; i < clusters[0]; i++)
    {
        ramp[COMP_RED][i] = floor((ep[0][COMP_RED] * (1.0-rampLerpWeights[rampIndex][i])) +
                                  (ep[1][COMP_RED] * rampLerpWeights[rampIndex][i]) + 0.5);
        ramp[COMP_RED][i] = min(255.0, max(0., ramp[COMP_RED][i]));
        ramp[COMP_GREEN][i] = floor((ep[0][COMP_GREEN] * (1.0-rampLerpWeights[rampIndex][i])) +
                                  (ep[1][COMP_GREEN] * rampLerpWeights[rampIndex][i]) + 0.5);
        ramp[COMP_GREEN][i] = min(255.0, max(0., ramp[COMP_GREEN][i]));
        ramp[COMP_BLUE][i] = floor((ep[0][COMP_BLUE] * (1.0-rampLerpWeights[rampIndex][i])) +
                                  (ep[1][COMP_BLUE] * rampLerpWeights[rampIndex][i]) + 0.5);
        ramp[COMP_BLUE][i] = min(255.0, max(0., ramp[COMP_BLUE][i]));
    }

    rampIndex = clusters[1];
    rampIndex = (DWORD)(log((double)rampIndex) / log(2.0));

    if(!componentBits[COMP_ALPHA])
    {
        for(i=0; i < clusters[1]; i++)
        {
            ramp[COMP_ALPHA][i] = 255.;
        }
    }
    else
    {
        // Generate alphas
        for(i=0; i < clusters[1]; i++)
        {
            ramp[COMP_ALPHA][i] = floor((ep[0][COMP_ALPHA] * (1.0-rampLerpWeights[rampIndex][i])) +
                                        (ep[1][COMP_ALPHA] * rampLerpWeights[rampIndex][i]) + 0.5);
            ramp[COMP_ALPHA][i] = min(255.0, max(0., ramp[COMP_ALPHA][i]));
        }
    }
}