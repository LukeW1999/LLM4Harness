#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               64U

void aws_array_list_clean_up_harness(void) {
    struct aws_array_list list;

    /* Assume the list is bounded and valid according to the model. */
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Use a concrete, well‑behaved allocator. */
    list.alloc = aws_default_allocator();
    __CPROVER_assume(list.alloc != NULL);
    __CPROVER_assume(list.alloc->mem_release != NULL);
    __CPROVER_assume(list.alloc->mem_acquire != NULL);

    /* If the list currently owns a buffer, make sure it was allocated with the
       same allocator so that the free operation is well‑defined. */
    if (list.data != NULL) {
        size_t sz = list.current_size;
        uint8_t *buf = list.alloc->mem_acquire(list.alloc, sz);
        __CPROVER_assume(buf != NULL);
        /* The contents are irrelevant for the clean‑up verification. */
        list.data = buf;
    }

    struct aws_array_list old = list;

    aws_array_list_clean_up(&list);

    /* After clean‑up the list must be empty and its allocator cleared. */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0U);
    assert(list.length == 0U);
    assert(list.item_size == 0U);

    /* The allocator before clean‑up must differ from the one after. */
    assert(old.alloc != list.alloc);
}
