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
//
//  File Name:   CodecBuffer_RGBA8888.cpp
//  Description: implementation of the CCodecBuffer_RGBA8888 class
//
//////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "CodecBuffer_RGBA8888.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

const int nChannelCount = 4;
const int nPixelSize = nChannelCount * sizeof(CMP_BYTE);

CCodecBuffer_RGBA8888::CCodecBuffer_RGBA8888(CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_DWORD dwPitch, CMP_BYTE* pData)
: CCodecBuffer(dwWidth, dwHeight, dwPitch, pData)
{
    assert((m_dwPitch == 0) || (m_dwPitch >= GetWidth() * nPixelSize));
    if(m_dwPitch <= GetWidth() * nPixelSize)
        m_dwPitch = GetWidth() * nPixelSize;

    if(m_pData == NULL)
    {
        CMP_DWORD dwSize = m_dwPitch * GetHeight();
        m_pData = (CMP_BYTE*) malloc(dwSize);
    }
}

CCodecBuffer_RGBA8888::~CCodecBuffer_RGBA8888()
{

}

void CCodecBuffer_RGBA8888::Copy(CCodecBuffer& srcBuffer)
{
    if(GetWidth() != srcBuffer.GetWidth() || GetHeight() != srcBuffer.GetHeight())
        return;

    const CMP_DWORD dwBlocksX = ((GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((GetHeight() + 3) >> 2);

    for(CMP_DWORD j = 0; j < dwBlocksY; j++)
    {
        for(CMP_DWORD i = 0; i < dwBlocksX; i++)
        {
            CMP_BYTE block[BLOCK_SIZE_4X4X4];
            srcBuffer.ReadBlockRGBA(i*4, j*4, 4, 4, block);
            WriteBlockRGBA(i*4, j*4, 4, 4, block);
        }
    }
}

bool CCodecBuffer_RGBA8888::ReadBlock(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[], CMP_DWORD dwChannelOffset)
{
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = min(w, (GetWidth() - x));

    CMP_DWORD i, j;
    for(j = 0; j < h && (y + j) < GetHeight(); j++)
    {
        CMP_DWORD* pData = (CMP_DWORD*) (GetData() + ((y + j) * m_dwPitch) + (x * sizeof(CMP_DWORD)));
        for(i = 0; i < dwWidth; i++)
            block[(j * w) + i] = static_cast<CMP_BYTE>(((*pData++) >> dwChannelOffset) & BYTE_MASK);

        // Pad line with previous values if necessary
        if(i < w)
            PadLine(i, w, 1, &block[j * w]);
    }

    // Pad block with previous values if necessary
    if(j < h)
        PadBlock(j, w, h, 1, block);

    return true;
}

bool CCodecBuffer_RGBA8888::WriteBlock(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[], CMP_DWORD dwChannelOffset)
{
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwChannelMask = ~((CMP_DWORD) BYTE_MASK << dwChannelOffset);
    CMP_DWORD dwWidth = min(w, (GetWidth() - x));

    for(CMP_DWORD j = 0; j < h && (y + j) < GetHeight(); j++)
    {
        CMP_DWORD* pData = (CMP_DWORD*) (GetData() + ((y + j) * m_dwPitch) + (x * sizeof(CMP_DWORD)));
        for(CMP_DWORD i = 0; i < dwWidth; i++)
      {
            *pData = (*pData & dwChannelMask) | (((CMP_DWORD) block[(j * w) + i]) << dwChannelOffset);
         pData++;
      }
    }
    return true;
}

bool CCodecBuffer_RGBA8888::ReadBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[])
{
    return ReadBlock(x, y, w, h, block, RGBA8888_OFFSET_A);
}

bool CCodecBuffer_RGBA8888::ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[])
{
    return ReadBlock(x, y, w, h, block, RGBA8888_OFFSET_R);
}

bool CCodecBuffer_RGBA8888::ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[])
{
    return ReadBlock(x, y, w, h, block, RGBA8888_OFFSET_G);
}

bool CCodecBuffer_RGBA8888::ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[])
{
    return ReadBlock(x, y, w, h, block, RGBA8888_OFFSET_B);
}

bool CCodecBuffer_RGBA8888::WriteBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[])
{
    return WriteBlock(x, y, w, h, block, RGBA8888_OFFSET_A);
}

bool CCodecBuffer_RGBA8888::WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[])
{
    return WriteBlock(x, y, w, h, block, RGBA8888_OFFSET_R);
}

bool CCodecBuffer_RGBA8888::WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[])
{
    return WriteBlock(x, y, w, h, block, RGBA8888_OFFSET_G);
}

bool CCodecBuffer_RGBA8888::WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[])
{
    return WriteBlock(x, y, w, h, block, RGBA8888_OFFSET_B);
}

bool CCodecBuffer_RGBA8888::ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[])
{
    assert(x < GetWidth());
    assert(y < GetHeight());
    assert(x % w == 0);
    assert(y % h == 0);

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    DWORD* pdwBlock = (DWORD*) block;
    if (w==4 && h==4 && (x+w)<=GetWidth() && (y+h)<=GetHeight())
    {
        // Fastpath for the key case to alleviate the drag this code puts on the really fast DXTC
        CMP_DWORD* pData = (CMP_DWORD*) (GetData() + (y * m_dwPitch) + (x * sizeof(CMP_DWORD)));
        pdwBlock[0]  = pData[0];
        pdwBlock[1]  = pData[1];
        pdwBlock[2]  = pData[2];
        pdwBlock[3]  = pData[3];
        pData += (m_dwPitch>>2);
        pdwBlock[4]  = pData[0];
        pdwBlock[5]  = pData[1];
        pdwBlock[6]  = pData[2];
        pdwBlock[7]  = pData[3];
        pData += (m_dwPitch>>2);
        pdwBlock[8]  = pData[0];
        pdwBlock[9]  = pData[1];
        pdwBlock[10] = pData[2];
        pdwBlock[11] = pData[3];
        pData += (m_dwPitch>>2);
        pdwBlock[12] = pData[0];
        pdwBlock[13] = pData[1];
        pdwBlock[14] = pData[2];
        pdwBlock[15] = pData[3];
    }
    else
    {
        CMP_DWORD dwWidth = min(w, (GetWidth() - x));
        CMP_DWORD i, j;
        for(j = 0; j < h && (y + j) < GetHeight(); j++)
        {
            CMP_DWORD* pData = (CMP_DWORD*) (GetData() + ((y + j) * m_dwPitch) + (x * sizeof(CMP_DWORD)));
            for(i = 0; i < dwWidth; i++)
                pdwBlock[(j * w) + i] = *pData++;

            // Pad block with previous values if necessary
            if(i < w)
                PadLine(i, w, 4, (CMP_BYTE*) &pdwBlock[j * w]);
        }

        // Pad block with previous values if necessary
        if(j < h)
            PadBlock(j, w, h, 4, (CMP_BYTE*) pdwBlock);
    }

    return true;
}

bool CCodecBuffer_RGBA8888::WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[])
{
    assert(x < GetWidth());
    assert(y < GetHeight());
    assert(x % 4 == 0);
    assert(y % 4 == 0);

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = min(w, (GetWidth() - x));
    DWORD* pdwBlock = (DWORD*) block;

    for(CMP_DWORD j = 0; j < h && (y + j) < GetHeight(); j++)
    {
        CMP_DWORD* pData = (CMP_DWORD*) (GetData() + ((y + j) * m_dwPitch) + (x * sizeof(CMP_DWORD)));
        for(CMP_DWORD i = 0; i < dwWidth; i++)
            *pData++ = pdwBlock[(j * w) + i]; 
    }
    return true;
}


