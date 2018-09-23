#ifndef __CBITSET_H_
#define __CBITSET_H_

#include <string.h>

/**
 * This is our bitset class for storing which face has already been drawn.
 * The bitset functionality isn't really taken advantage of in this version
 * since we aren't rendering by leafs and nodes.
 */

class CBitset
{

public:

    // Initialize all the data members
    CBitset () : m_size(0), m_bits(NULL) { ;}

    // This is our deconstructor
    ~CBitset ()
    {
        // If we have valid memory, get rid of it
        if (m_bits)
        {
            delete [] m_bits;
            m_bits = NULL;
        }
    }

    /*
     * This resizes our bitset to a size so each face has a bit associated with it
     */
    void Resize (size_t count)
    {
        // Get the size of integers we need
        m_size = count / 32 + 1;

        // Make sure we haven't already allocated memory for the bits
        if (m_bits)
        {
            delete [] m_bits;
            m_bits = NULL;
        }

        // Allocate the bits and initialize them
        m_bits = new unsigned int[m_size];
        ClearAll ();
    }

    /**
     * This does the binary math to set the desired bit
     */
    void Set (size_t i)
    {
        m_bits[i >> 5] |= (1 << (i & 31));
    }

    /**
     * This returns if the desired bit slot is a 1 or a 0
        */
    int On (int i) const
    {
        return m_bits[i >> 5] & (1 << (i & 31));
    }

    /**
     * This clears a bit to 0
     */
    void Clear (int i)
    {
        m_bits[i >> 5] &= ~(1 << (i & 31));
    }

    /**
     * This initializes the bits to 0
     */
    void ClearAll ()
    {
        memset (m_bits, 0, sizeof (unsigned int) * m_size);
    }

    int m_size;

private:

    /**
     * Our private bit data that holds the bits and size
     */
    unsigned int *m_bits;

};

#endif // __CBITSET_H_
