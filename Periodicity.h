
//The following should be tailored to your processor
//For periodicity counting variables
typedef unsigned int PERIODICITY_COUNTING;
//For periodicity values
typedef unsigned int PERIODICITY_VALUE;
#define PERIODICITY_VALUE_MAXIMUM 0xFFFFFFFF
//For accumulating an average
typedef unsigned long long PERIODICITY_ACCUMULATE;

#define MAXPERIODICITYINDEX 10

typedef struct
{
/* Results */
	PERIODICITY_COUNTING number_of_calls;
	PERIODICITY_COUNTING variance_exceeded;
	PERIODICITY_COUNTING variance_ok;
	PERIODICITY_VALUE minimum;
	PERIODICITY_VALUE maximum;
	PERIODICITY_VALUE average;

/* Internal */
	PERIODICITY_VALUE intn_previous;
	PERIODICITY_VALUE intn_lower_bound;	
	PERIODICITY_VALUE intn_upper_bound;
	PERIODICITY_ACCUMULATE intn_average;
} periodicity_struct;


periodicity_struct * PP_GET_PERIODICITY_REPORT (unsigned int index);

void PP_PERIODICITY_CHECK (unsigned int index);
int PP_PERIODICITY_INITIALIZE (unsigned int index, unsigned int expected, unsigned int max_variance_percent);

