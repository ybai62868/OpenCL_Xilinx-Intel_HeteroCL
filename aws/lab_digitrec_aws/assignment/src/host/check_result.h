/*===============================================================*/
/*                                                               */
/*                        check_result.h                         */
/*                                                               */
/*      Compare result and expected label to get error rate      */
/*                                                               */
/*===============================================================*/


// compare the results with the expected labels, print the errors
// and calculate the overall error rate
void check_results(bit4_t * result, const bit4_t * expected, int cnt);
