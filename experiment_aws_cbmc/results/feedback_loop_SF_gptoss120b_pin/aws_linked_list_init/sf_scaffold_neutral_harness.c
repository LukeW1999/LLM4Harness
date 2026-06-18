#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include "make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_init_harness(void) {
    struct aws_linked_list *list = malloc(sizeof(*list));
    __CPROVER_assume(list != NULL);

    /* Snapshot of the original state */
    struct aws_linked_list old = *list;

    /* Call the function under verification */
    aws_linked_list_init(list);

    
}
