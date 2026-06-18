#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

extern size_t nondet_size_t(void);
extern void *nondet_void_pointer(void);

void aws_array_list_clean_up_harness(void) {
    struct aws_array_list list;

    /* allocate a default allocator */
    list.alloc = aws_default_allocator();

    /* nondeterministic fields */
    list.current_size = nondet_size_t();
    list.length       = nondet_size_t();
    list.item_size    = nondet_size_t();
    list.data         = nondet_void_pointer();

    /* assume the list is in a valid state before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* snapshot of pre‑state */
    struct aws_allocator *old_alloc        = list.alloc;
    void                 *old_data         = list.data;
    size_t                old_current_size = list.current_size;
    size_t                old_length        = list.length;
    size_t                old_item_size     = list.item_size;

    /* call the function under verification */
    aws_array_list_clean_up(&list);

    /* ASSERT_POSTCONDITIONS_HERE */
}
