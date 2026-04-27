Looking at the function `aws_array_list_push_front`:

1. **What fields change on success?**
   - `list->length` increases by 1
   - `list->data` contents change (elements shifted, new element at front)
   - `list->current_size` may change (if capacity was expanded)

2. **What fields are unchanged (implicit invariant)?**
   - `list->alloc` (allocator pointer)
   - `list->item_size` (element size)

3. **What happens on failure?**
   - Returns non-zero error code
   - List should remain valid

4. **What validity invariants must hold after the call?**
   - `aws_array_list_is_valid(list)` must hold

```c
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_push_front_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Prepare val: a non-deterministic buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Save old state BEFORE calling */
    struct aws_array_list old_list = list;
    size_t old_length = list.length;

    /* 4. Call function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: length increases by 1 */
        assert(list.length == old_length + 1);
        /* item_size must not change */
        assert(list.item_size == old_list.item_size);
        /* allocator must not change */
        assert(list.alloc == old_list.alloc);
        /* current_size must be >= length * item_size */
        assert(list.current_size >= list.length * list.item_size);
        /* data pointer must be non-null (we have at least one element) */
        assert(list.data != NULL);
        /* The first element should match val */
        if (list.item_size > 0) {
            assert(memcmp(list.data, val, list.item_size) == 0);
        }
    } else {
        /* On failure: length should not have changed */
        assert(list.length == old_length);
        /* item_size must not change */
        assert(list.item_size == old_list.item_size);
        /* allocator must not change */
        assert(list.alloc == old_list.alloc);
        /* current_size must not change on failure */
        assert(list.current_size == old_list.current_size);
    }

    /* 6. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
