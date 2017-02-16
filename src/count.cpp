#include <iostream>
#include <cstdlib>
#include <papi.h>
#define NUM_EVENTS 1

using namespace std;

const int N=10000;
const int T=N/8;

int main() {
	long long values[NUM_EVENTS];	
	unsigned int Events[NUM_EVENTS]={PAPI_BR_MSP};
	int retval = PAPI_library_init(PAPI_VER_CURRENT);
	cout << "retval " << retval << endl;

  int large=0,X[N];
  for (int i=0; i<N; i++)
    X[i]=rand() % N;

	PAPI_start_counters((int*)Events,NUM_EVENTS);
	PAPI_read_counters(values,NUM_EVENTS);

  for (int r=0; r<100; r++)
    for (int i=0; i<N; i++)
      if (X[i]>T) large+=1;

  cout << large << endl;
	
  retval = PAPI_stop_counters(values,NUM_EVENTS);
  cout << "retval2 " << retval << endl;
  for(int k=0; k < NUM_EVENTS; k++){
	cout << values[k] << endl;
			
}

  return 0;
};

