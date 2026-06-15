/* CBMC harness for aws_array_list_capacity */

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/array_list.h>
#include <aws/common/memory.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"

/* Nondeterministic size_t generator for CBMC */
size_t nondet_size_t(void);
int nondet_int(void);

/* Compute a simple checksum over a memory region */
static size_t checksum_bytes(const uint8_t *buf, size_t size) {
    size_t sum = 0;
    for (size_t i = 0; i < size; ++i) {
        sum += buf[i];
    }
    return sum;
}

void harness(void) {
    struct aws_array_list list;
    struct aws_array_list old_list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* --- nondeterministic initialization of item size (must be > 0) --- */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);

    /* --- nondeterministic initial allocation count --- */
    size_t init_count = nondet_size_t();

    /* Ensure multiplication does not overflow for the init call */
    size_t allocation_size = 0;
    if (aws_mul_size_checked(init_count, item_size, &allocation_size)) {
        /* overflow – force init to fail, but we only continue on success */
        allocation_size = 0;
    }

    /* --- initialize the list dynamically --- */
    int init_result = aws_array_list_init_dynamic(&list, alloc, init_count, item_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);

    /* Snapshot the whole list structure before the call */
    old_list = list;

    /* Compute a checksum of the data buffer (if any) */
    size_t pre_checksum = 0;
    if (list.current_size > 0) {
        pre_checksum = checksum_bytes((const uint8_t *)list.data, list.current_size);
    }

    /* --- set length to a nondeterministic value within capacity --- */
    size_t capacity = list.current_size / list.item_size;
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= capacity);
    list.length = len;

    /* --- call the function under test --- */
    size_t cap = aws_array_list_capacity(&list);

    /* ------------------- postconditions ------------------- */

    /* 1. Return value / error code correctness */
    assert(cap == list.current_size / list.item_size);   /* definition */
    assert(cap >= list.length);                         /* capacity must cover length */

    /* 2. Output buffer length/capacity invariants are already covered above */

    /* 3. Memory not modified beyond the function's contract (frame condition) */
    /* The list structure must be unchanged */
    assert(memcmp(&list, &old_list, sizeof(struct aws_array_list)) == 0);

    /* The data buffer must be unchanged */
    if (list.current_size > 0) {
        size_t post_checksum = checksum_bytes((const uint8_t *)list.data, list.current_size);
        assert(post_checksum == pre_checksum);
    }

    /* The list must remain valid */
    assert(aws_array_list_is_valid(&list));
}
