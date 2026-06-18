#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <proof_helpers/make_common_data_structures.h>

/* Nondeterministic primitives */
uint64_t nondet_uint64_t(void);
uint8_t nondet_uint8_t(void);
_Bool nondet_bool(void);

/* Helper to nondeterministically choose a size > 0 */
static size_t nondet_size_gt_zero(void) {
    size_t sz = (size_t)nondet_uint64_t();
    __CPROVER_assume(sz > 0);
    return sz;
}

/* Helper to nondeterministically choose any size */
static size_t nondet_size(void) {
    return (size_t)nondet_uint64_t();
}

/* Harness for aws_array_list_push_back */
void harness(void) {
    struct aws_array_list list;
    struct aws_array_list old_list;
    uint8_t *old_data = NULL;
    uint8_t *val = NULL;
    size_t item_size;
    size_t old_len;
    size_t old_current_size;
    int rc;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Choose item size (>0) */
    item_size = nondet_size_gt_zero();

    /* Choose between dynamic and static list */
    if (nondet_bool()) {
        /* ---------- Dynamic list ---------- */
        size_t initial_item_allocation = nondet_size(); /* may be zero */
        assert(aws_array_list_init_dynamic(&list,
                                           allocator,
                                           initial_item_allocation,
                                           item_size) == AWS_OP_SUCCESS);
        /* Choose a length that is <= capacity */
        size_t capacity = list.current_size / item_size;
        old_len = nondet_size();
        __CPROVER_assume(old_len <= capacity);
        list.length = old_len;
    } else {
        /* ---------- Static list ---------- */
        size_t item_count = nondet_size(); /* may be zero */
        __CPROVER_assume(item_count > 0);
        __CPROVER_assume(item_size <= SIZE_MAX / item_count);
        size_t total_bytes = item_count * item_size;
        uint8_t *raw_array = malloc(total_bytes);
        __CPROVER_assume(raw_array != NULL);
        aws_array_list_init_static(&list, raw_array, item_count, item_size);
        /* Choose a length that is <= item_count */
        old_len = nondet_size();
        __CPROVER_assume(old_len <= item_count);
        list.length = old_len;
    }

    /* Preserve old state */
    old_list = list;
    old_current_size = list.current_size;
    if (list.data && old_current_size > 0) {
        old_data = malloc(old_current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, old_current_size);
    }

    /* Allocate and nondet-initialize value to push */
    val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < item_size; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* Call the function under test */
    rc = aws_array_list_push_back(&list, val);

    /* General postcondition: list must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* Determine expected length after call */
    if (rc == AWS_OP_SUCCESS) {
        /* Length must increase by exactly one */
        assert(list.length == old_len + 1);
        /* New element must equal the input value */
        uint8_t *new_elem = (uint8_t *)list.data + (old_len * item_size);
        assert(memcmp(new_elem, val, item_size) == 0);
        /* Existing elements must be unchanged */
        if (old_len > 0) {
            assert(memcmp(list.data, old_data, old_len * item_size) == 0);
        }
    } else {
        /* On error, length must be unchanged */
        assert(list.length == old_len);
        /* All previously stored elements must be unchanged */
        if (old_len > 0) {
            assert(memcmp(list.data, old_data, old_len * item_size) == 0);
        }
        /* If the list is static and full, the error should be LIST_EXCEEDS_MAX_SIZE */
        if (!list.alloc) {
            size_t capacity = list.current_size / item_size;
            if (old_len == capacity) {
                assert(rc == AWS_ERROR_LIST_EXCEEDS_MAX_SIZE);
            }
        }
    }

    /* Frame condition: memory outside the list's current allocation must be unchanged */
    if (old_current_size > 0) {
        size_t used_bytes = old_len * item_size;
        if (old_current_size > used_bytes) {
            size_t padding = old_current_size - used_bytes;
            uint8_t *pad_start = (uint8_t *)list.data + used_bytes;
            assert(memcmp(pad_start, old_data + used_bytes, padding) == 0);
        }
    }

    /* Clean up */
    if (old_data) {
        free(old_data);
    }
    if (val) {
        free(val);
    }
    aws_array_list_clean_up(&list);
}
