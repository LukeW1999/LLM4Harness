#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               64U

void aws_array_list_clean_up_harness(void) {
    struct aws_array_list list;
    /* nondet initialization of the list structure */
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Preserve the pre‑state of the list */
    struct aws_array_list old = list;
    uint8_t *old_data = NULL;
    size_t old_data_size = 0;

    if (list.data != NULL) {
        old_data_size = list.current_size;
        old_data = malloc(old_data_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, old_data_size);
    }

    /* Call the function under verification */
    aws_array_list_clean_up(&list);

    /* Post‑condition 1: the list structure must be zeroed */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0U);
    assert(list.length == 0U);
    assert(list.item_size == 0U);

    /* Post‑condition 2: no other memory should be modified */
    if (old_data != NULL) {
        /* The memory that was pointed to by list.data before the call must remain unchanged */
        assert(memcmp(old_data, old_data, old_data_size) == 0);
        free(old_data);
    }

    /* Post‑condition 3: the allocator pointer from the old state is no longer stored in the list */
    assert(old.alloc != list.alloc); /* list.alloc is now NULL, old.alloc was the original allocator */

    return 0;
}
