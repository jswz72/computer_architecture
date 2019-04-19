#include <iostream>
#include "memory.hh"

using namespace std;

const int N = 25, A = 0, B = A+N*N, C = B+N*N;

int main ()
{
  int ii;
  Memory m;

  cout << "Smart matrix multiplication (N = " << N << ")\n";

  // iterate over cache organization: direct, twoway, fully
  for (ii = DIRECT; ii <= TWOWAY; ii++) {
    cache_org = ii; // set cache organization 
    printCacheOrg (cache_org);
    resetClock(); // reset clock and numMisses

    for ( int i = 0; i < N; i++ )
      for ( int k = 0; k < N; k++ )
	for ( int j = 0; j < N; j++ ) {
	  int c = m.getData ( C + i*N + j );
	  int a = m.getData ( A + i*N + k );
	  int b = m.getData ( B + k*N + j );
	  c += a * b;
	  m.putData ( C + i*N + j, c );
	}
    
    float missrate = (float)numMisses/(float)(N*N*N*4)*100.0;
    cout << "\nClock : " << clockX
	 << " Cache misses: " << numMisses;
    cout.setf(ios::fixed);
    cout.precision(2);
    cout << " Cache miss rate(%): " << missrate << endl;
  
    // print cache contents
    m.showCacheAddress();
    printf ("======================================\n");
  }

  return 0;
}
