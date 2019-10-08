/*===============================================================*/
/*                                                               */
/*                       check_result.cpp                        */
/*                                                               */
/*      Compare result and expected label to get error rate      */
/*                                                               */
/*===============================================================*/

#include <cstdio>
#include "typedefs.h"

// compare the results with the expected labels, print the errors
// and calculate the overall error rate
void check_results(bit4_t * result, const bit4_t * expected, int cnt)
{
  int correct_cnt = 0;

  for (int i = 0; i < cnt; i ++ )
  {
    if (result[i] != expected[i])
      printf("Test %d: expected = %d, result = %d\n", i, (int)expected[i], (int)result[i]);
    else
      correct_cnt ++;
  }

  printf("\n\t %d / %d correct!\n", correct_cnt, cnt);  

}
