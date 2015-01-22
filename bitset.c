#include "bitset.h"
#include <stdlib.h>
#include <stdio.h>

// creates a Bitset of size 'length', in bits, and place 'data' starting at 'index'
Bitset* createBitset( unsigned long int length, unsigned int flags ){
	Bitset *bitset = calloc( 1, sizeof(Bitset) );

	if( bitset != NULL ){
		bitset->size = length;
		bitset->flags = flags;

		// convert bits to bytes so we can calloc the appropriate amount of data
		length = (length + 7) / 8;
		bitset->bits = calloc( length, sizeof(unsigned char) );

		if( bitset->bits == NULL ){
			free( bitset );
			bitset = NULL;
		}
	}

	return bitset;
}

// sets flags
void setBitsetFlags( Bitset *bitset, unsigned int flags ){
	bitset->flags |= flags;
}

// removes flags
void removeBitsetFlags( Bitset *bitset, unsigned int flags ){
	bitset->flags &= (~flags);
}

Bitset* getBits( Bitset *bitset, unsigned long int index, unsigned long int length ){
	Bitset *bitset_tmp = NULL;

	if( index + length <= bitset->size ){
		bitset_tmp = calloc( 1, sizeof(Bitset) );

		if( bitset_tmp != NULL ){
			bitset_tmp->size = length;
			bitset_tmp->flags = 0;

			bitset_tmp->bits = calloc( (length + 7) / 8, sizeof(unsigned char) );

			if( bitset_tmp->bits == NULL ){
				free( bitset_tmp );
				bitset_tmp = NULL;
			}else{
				setBits( bitset_tmp, 0, bitset->bits, index, length );
			}
		}
	}

	return bitset_tmp;
}

/* Sets data into a bitset
 * 
 * bitset:		user supplied bitset
 * bitset_index:	where in the bitset to start placing data
 * data:		data stream
 * data_index:		where in the data stream to start taking date from
 * length:		how many bits to take from the data stream
 *
 * WARNING:		Since this sets raw data, it is not possible to know how long *data actually is,
 * 			so if length is longer than data is meant to be, it will not stop until length is met.
 */
void setBits( Bitset *bitset, unsigned long int bitset_index, void *data, unsigned long int data_index, unsigned long int length ){
	if( bitset_index < bitset->size ){
		if( length <= bitset->size - bitset_index ){
			unsigned char data_offset = (unsigned char)(data_index) % 8;
			unsigned char bitset_offset = (unsigned char)(bitset_index) % 8;

			Bitset_Mask mask;

			mask.input_mask = (unsigned char)(( 2 << (unsigned int)( 8 - data_offset ) ) - 1);
			mask.input_mask = mask.input_mask << data_offset;

			// optimizes mask for smooth running by reducing the ammount of checks needs during looping
			if( (int)bitset_offset - (int)data_offset > -1 ){
				mask.rel_out_offset = bitset_offset - data_offset;
				mask.shift = &lshift;

				if( data_offset == bitset_offset ){
					mask.flow = &equalflow;
				}else{
					mask.flow = &overflow;
				}
			}else{
				mask.rel_out_offset = data_offset - bitset_offset;
				mask.shift = &rshift;

				mask.flow = &underflow;
			}

			// add data one byte at a time
			unsigned long int i;
			for( i = 0; i < length / 8; i++ ){
				unsigned char current_byte = (*(unsigned char*)(data + i + (data_index/8)));
				unsigned char next_byte = (*(unsigned char*)(data + i + 1 + (data_index/8)));

				*(unsigned char*)(bitset->bits + i + (bitset_index / 8)) |= mask.shift( ( current_byte & mask.input_mask ), mask.rel_out_offset );
				mask.flow( bitset, i + (bitset_index / 8), mask, current_byte, next_byte );
			}

			// if length is not divisible by 8, then we need to add the final bits
			unsigned char left_overs = length & 7;
			if( left_overs != 0 ){
				unsigned char mask_size = __builtin_popcount( mask.input_mask );
				unsigned char final_mask;

				// if the left overs are smaller than the mask, we need to create a new final mask that is the needed size
				if( left_overs > mask_size ){
					unsigned char current_byte = (*(unsigned char*)(data + i + (data_index/8)));

					*(unsigned char*)(bitset->bits + i + (bitset_index / 8)) |= mask.shift( ( current_byte & mask.input_mask ), mask.rel_out_offset );
					final_mask = ( ( 2 << (left_overs - mask_size) ) - 1 );
					mask.input_mask = ~final_mask;
				}else{
					final_mask = ( 2 << left_overs ) - 1;

					if( mask.flow == &overflow ){
						final_mask >>= mask.rel_out_offset;
					}else if( mask.flow == &underflow ){
						final_mask <<= mask.rel_out_offset;
					}else{
						final_mask <<= __builtin_ctz( mask.input_mask );
					}

					unsigned char current_byte = (*(unsigned char*)(data + i + (data_index/8)));
					*(unsigned char*)(bitset->bits + i + (bitset_index / 8)) |= mask.shift( ( current_byte & final_mask ), mask.rel_out_offset );
				}
			}
		}
	}
}

// used when the offset of the original source and the new source are equal
void equalflow( Bitset *bitset, unsigned long int index, Bitset_Mask mask, unsigned char current_byte, unsigned char next_byte ){
	next_byte = next_byte & (~mask.input_mask);
	*(unsigned char*)(bitset->bits + index + 1) |= next_byte;
}

// receiving data set's offset is greater than the source's
void overflow( Bitset *bitset, unsigned long int index, Bitset_Mask mask, unsigned char current_byte, unsigned char next_byte ){
	unsigned char tmp = ( 2 << mask.rel_out_offset ) - 1;
	tmp = tmp << ( 8 - mask.rel_out_offset );
	current_byte = current_byte & tmp;
	*(unsigned char*)(bitset->bits + index + 1) |= rshift(current_byte, 8-mask.rel_out_offset);
	next_byte = next_byte & (~mask.input_mask);
	*(unsigned char*)(bitset->bits + index + 1) |= lshift(next_byte, mask.rel_out_offset);
}

// receiving data set's offset is less than the source's
void underflow( Bitset *bitset, unsigned long int index, Bitset_Mask mask, unsigned char current_byte, unsigned char next_byte ){
	unsigned char tmp_mask = ( 2 << mask.rel_out_offset ) - 1;

	*(unsigned char*)(bitset->bits + index) |= (next_byte & tmp_mask) << (8 - mask.rel_out_offset);

	unsigned char data = next_byte & ~mask.input_mask;
	data = data >> mask.rel_out_offset;

	*(unsigned char*)(bitset->bits + index + 1) |= data;
}

// rshift and lshift exist so we don't have to keep doing lookups to perform shifting operations
unsigned char rshift( unsigned char data, unsigned char shift ){
	return data >> shift;
}

unsigned char lshift( unsigned char data, unsigned char shift ){
	return data << shift;
}

void destroyBitset( Bitset *bitset ){
	free( bitset->bits );
}

