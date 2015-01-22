#include <stdio.h>
#include <stdlib.h>
#include "bitset.h"

void printData( Bitset *data );

int main( int argc, char *argv [] ){
	Bitset *a = createBitset( 24, OVERWRITE );
	Bitset *b = createBitset( 7, OVERWRITE );
	Bitset *c = createBitset( 13, OVERWRITE );


	printf( "---- EQUAL FLOW TESTS ----\n" );
	printf( "--------------------------\n" );
	printf( "1: Expected: 97, 98, 99\n" );
	setBits( a, 0, "abc", 0, 24 );
	printData( a );


	printf( "2: Expected: 68\n" );
	setBits( b, 2, "G", 2, 5 );
	printData( b );


	printf( "3: Expected: 116, 8 \n" );
	setBits( c, 1, "atH", 9, 12 );
	printData( c );


	Bitset *d = createBitset( 24, OVERWRITE );
	Bitset *e = createBitset( 15, OVERWRITE );
	Bitset *f = createBitset( 24, OVERWRITE );


	printf( "---- UNDER FLOW TESTS ----\n" );
	printf( "--------------------------\n" );
	printf( "4: Expected: 76, 12, 0\n" );
	setBits( d, 0, "ab", 3, 13);
	printData( d );


	printf( "5: Expected: 32, 28\n" );
	setBits( e, 5, "aW8", 14, 10 );
	printData( e );


	printf( "6: Expected: 188, 49, 49\n" );
	setBits( f, 1, "ycb", 2, 22 );
	printData( f );


	Bitset *g = createBitset( 24, OVERWRITE );
	Bitset *h = createBitset( 24, OVERWRITE );
	Bitset *i = createBitset( 24, OVERWRITE );


	printf( "----  OVER FLOW TESTS ----\n" );
	printf( "--------------------------\n" );
	printf( "7: Expected: 194, 196, 198\n" );
	setBits( g, 1, "abc", 0, 23 );
	printData( g );


	printf( "8: Expected: 0, 19, 27\n" );

	setBits( h, 4, "abc", 1, 20 );
	printData( h );


	printf( "9: Expected: 0, 0, 19\n" );
	setBits( i, 15, "abc", 4, 8 );
	printData( i );

	destroyBitset( a );
	destroyBitset( b );
	destroyBitset( c );
	destroyBitset( d );
	destroyBitset( e );
	destroyBitset( f );
	destroyBitset( g );
	destroyBitset( h );
	destroyBitset( i );

	free(a);
	free(b);
	free(c);
	free(d);
	free(e);
	free(f);
	free(g);
	free(h);
	free(i);

	Bitset *j = createBitset( 24, OVERWRITE );
	setBits( j, 0, "abc", 0, 24 );

	Bitset *k = getBits( j, 8, 16 );
	printf( "----   GET BITS TEST  ----\n" );
	printf( "--------------------------\n" );
	printf( "7: Expected: 98, 99\n" );
	printData( k );

	Bitset *l = createBitset( 24, OVERWRITE );
	setBits( l, 0, "abc", 0, 24 );

	Bitset *m = getBits( l, 5, 19 );
	if( m != NULL ){
		printf( "----   GET BITS TEST  ----\n" );
		printf( "--------------------------\n" );
		printf( "7: Expected: 19, 27, 3\n" );
		printData( m );
	}else{
		printf( "Error allocating space for bitset m.\n" );
	}
	

	return 0;
}

void printData( Bitset *data ){
	unsigned int i = 0;
	for( i = 0; i < ( (data->size + 7) / 8 ); i++ ){
		printf( "%c\t  %u\n", *(unsigned char*)(data->bits + i), *(unsigned char*)(data->bits + i));
		unsigned int j = 0;
		for( j; j < 8; j++ ){
			printf("%u", ( *(unsigned char*)(data->bits + i) >> j ) % 2 );
		}

		printf("\n\n");
	}

	printf("\n");
}

