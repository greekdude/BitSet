#ifndef BITSET
#define BITSET

// probably will get rid of these flags
#define OVERWRITE 0x01
#define RANDOM_CLEAR 0x02

/* Variable Definitions
 * --------------------
 * bits: stores all the data.  Size = number of (bits + 7) / 8 we will have at most 7 bits more than we actually need, but garaunteed to fit all the data
 *
 * size: actual number of bits we are interested in
 *
 * flags: various flags that the program looks at
 */
typedef struct{
	void *bits;
	unsigned long int size;
	unsigned int flags;
} Bitset;

typedef struct Bitset_Mask Bitset_Mask;

/* Variable Definitions
 * --------------------
 *  input_mask: bits that will be bitwise and'd with the data to get only the data we are interested in
 *
 *  rel_out_offset: difference between two data segments, direction is driven by the shift function
 *
 *  data: escrow storage
 *
 *  shift: left or right bit shifting
 *
 *  flow: directs how the data is moving from element to another
 */
struct Bitset_Mask{
	unsigned char input_mask;
	unsigned char rel_out_offset;
	unsigned char data;
	unsigned char (*shift)(unsigned char, unsigned char);
	void (*flow)(Bitset*, unsigned long int, Bitset_Mask, unsigned char, unsigned char);
};

// creates a Bitset of size 'length' and place 'data' starting at 'index'
Bitset* createBitset( unsigned long int length, unsigned int flags );

// sets an overwrite and or destruction flag
void setBitsetFlags( Bitset *bitset, unsigned int flags );

// removes flags
void removeBitsetFlags( Bitset *bitset, unsigned int flags );

// returns a sub-Bitset from 'bitset' starting at 'index' and having a length of 'length'
Bitset* getBits( Bitset *bitset, unsigned long int index, unsigned long int length );

// sets 'data' at 'indexer for 'length' bits
// WARNING:	Since this sets raw data, it is not possible to know how long *data actually is,
//		so if length is longer than data is meant to be, it will not stop until length is met.
void setBits( Bitset *bitset, unsigned long int bitset_index, void *data, unsigned long int data_index, unsigned long int length );

// shifting handlers
unsigned char rshift( unsigned char data, unsigned char shift );
unsigned char lshift( unsigned char data, unsigned char shift );

//overflow handlers
//based on offset values in setBits function
void equalflow( Bitset *bitset, unsigned long int index, Bitset_Mask mask, unsigned char current_byte, unsigned char next_byte ); // when the input and output offsets are equal
void overflow( Bitset *bitset, unsigned long int index, Bitset_Mask mask, unsigned char current_byte, unsigned char next_byte ); // when ouput offset is greater than input offset
void underflow( Bitset *bitset, unsigned long int index, Bitset_Mask mask, unsigned char current_byte, unsigned char next_byte ); // when output offset is less than input offset

void destroyBitset( Bitset *bitset );

#endif
