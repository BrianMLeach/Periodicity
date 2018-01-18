
#include "Periodicity.h"

#include "windows.h"
#include "stdio.h"
unsigned long rdtsc () 
{
	//return *(unsigned long*)0xffe0A004 ;
	return GetTickCount();
}

static unsigned predictable_array[] =
{
	1000, 1984, 3030, 3920, 5030, 5999, 7100, 8100, 9100, 10000
};
int main(int argc, const char **argv)
{
	DWORD Count = GetTickCount(), Iter;
	periodicity_struct *pp;

	PP_PERIODICITY_INITIALIZE (0, 1000, 5);
	
	PP_PERIODICITY_CHECK (0);

	for(Iter = 0 ; Iter < sizeof(predictable_array) / sizeof(*predictable_array) ; Iter++)
	{
		while((GetTickCount() - Count) < predictable_array[Iter])
			;
		PP_PERIODICITY_CHECK (0);
	}

	pp = PP_GET_PERIODICITY_REPORT (0);
	printf("#calls   = %u\n", pp->number_of_calls);
	printf("Exceeded = %u\n", pp->variance_exceeded);
	printf("OK       = %u\n", pp->variance_ok);
	printf("Max      = %u\n", pp->maximum);
	printf("Min      = %u\n", pp->minimum);
	printf("Average  = %u\n", pp->average);
	printf("Lo bound = %u\n", pp->intn_lower_bound);
	printf("Up bound = %u\n", pp->intn_upper_bound);
}

periodicity_struct periodicity_data [ MAXPERIODICITYINDEX ] = {0};

//Index - The index for the periodicity test (0 - MAXPERIODICITYINDEX).
//Expected - The change in the hardware counter over the expected period.
//Max_variance_percent. The maximum allowed variance in percent.
//						Maximum value is 20;

int PP_PERIODICITY_INITIALIZE (unsigned int index, PERIODICITY_VALUE expected,
							   unsigned int max_variance_percent)
{
    periodicity_struct * _this;
	int nRet = 0;
    if (index < MAXPERIODICITYINDEX)
    {
        _this = periodicity_data + index;
        _this->number_of_calls = 0;
        _this->variance_exceeded = 0;
        _this->variance_ok = 0;  
        _this->minimum = PERIODICITY_VALUE_MAXIMUM;
        _this->maximum = 0;
        _this->intn_average = 0;
        _this->intn_previous = 0;
		//Below 200 there is a lack of resolution. Above avoid computation overflow.
		if((expected >= 200) && (expected <= (PERIODICITY_VALUE_MAXIMUM / 20)))
		{
			PERIODICITY_VALUE variance = (expected * max_variance_percent) / 100;
			_this->intn_lower_bound = expected - variance;
			_this->intn_upper_bound = expected + variance;
			nRet = 1;
		}
    }
	return nRet;
}


void PP_PERIODICITY_CHECK (unsigned int index)
{
    PERIODICITY_VALUE diff;
    periodicity_struct *_this;

    if(index < MAXPERIODICITYINDEX)
    {
        _this = periodicity_data + index;
        if(_this->number_of_calls)
        {
			PERIODICITY_VALUE now = rdtsc();
            diff = now - _this->intn_previous;
            if((diff < _this->intn_lower_bound) || (diff > _this->intn_upper_bound))
                _this->variance_exceeded++;       
			else
				_this->variance_ok++; 
  
            if(diff < _this->minimum)
                _this->minimum = diff; /* minimum diff seen */
            if(diff > _this->maximum)
                _this->maximum = diff; /* maximum diff seen */

            _this->intn_average += (PERIODICITY_ACCUMULATE)diff;
			_this->intn_previous = now;
        }
		else
			_this->intn_previous = rdtsc();
        _this->number_of_calls++;
    }
}

periodicity_struct * PP_GET_PERIODICITY_REPORT (unsigned int index)
{
    periodicity_struct *_this;
    if (index < MAXPERIODICITYINDEX)
	{
		_this = periodicity_data + index;
		if(_this->number_of_calls < 2)
			_this->average = 0;
		else
			_this->average = (PERIODICITY_VALUE) 
				(_this->intn_average / (PERIODICITY_ACCUMULATE) (_this->number_of_calls - 1));
	}
	else
		_this = (periodicity_struct *) 0;
     return _this ;
} 

