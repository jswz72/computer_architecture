#include "memory.h"

#define N 25
#define A 0
#define B A+N*N
#define C B+N*N

int main()
{
  int ii;

  printf ("Smart matrix multiplication (N = %d)\n", N);

  // iterate over cache organization: direct, twoway, fully
  for (ii = DIRECT; ii <= TWOWAY; ii++) {
    cache_org = ii; // set cache organization 
    printCacheOrg (cache_org);
    resetClock(); // reset clock and numMisses

    for ( int i = 0; i < N; i++ )
      for ( int k = 0; k < N; k++ )
	for ( int j = 0; j < N; j++ ) {
	  int c = getData ( C + i*N + j );
	  int a = getData ( A + i*N + k );
	  int b = getData ( B + k*N + j );
	  c += a * b;
	  putData ( C + i*N + j, c );
	}
    
    float missrate = (float)numMisses/(float)(N*N*N*4)*100.0;
    printf ("\nClock: %d,  Cache misses: %d, Miss rate: %f\n\n", clockX, numMisses, missrate);
    // print cache contents
    showCacheAddress();
    printf ("======================================\n");
  }
 
  return 0;
}
