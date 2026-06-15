#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               64U

void aws_array_list_clean_up_harness(void) {
    struct aws_array_list list;

    /* Non‑deterministically initialize the list structure. */
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Preserve the original state for frame condition checks if needed. */
    struct aws_array_list old = list;

    /* Call the function under verification. */
    aws_array_list_clean_up(&list);

    /* Post‑condition: the list must be zeroed. */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);

    /* Optional frame condition: fields that were not part of the struct
       should remain unchanged (none in this case). */
    (void)old; /* suppress unused‑variable warning */

    return 0;
}
