#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

/* s2n struct definitions */
struct s2n_blob {
    uint8_t *data;
    uint32_t size;
    uint32_t allocated;
    unsigned growable : 1;
};

struct s2n_stuffer {
    struct s2n_blob blob;
    uint32_t read_cursor;
    uint32_t write_cursor;
    uint32_t high_water_mark;
    unsigned int alloced : 1;
    unsigned int growable : 1;
    unsigned int tainted : 1;
};

int s2n_stuffer_copy(struct s2n_stuffer *from, struct s2n_stuffer *to, uint32_t len);

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

void s2n_stuffer_copy_harness(void) {
    struct s2n_stuffer from;
    struct s2n_stuffer to;
    uint32_t len;

    /* Set up 'from' stuffer with allocated backing memory */
    uint32_t from_size = nondet_uint32_t();
    __CPROVER_assume(from_size <= MAX_BUFFER_SIZE);
    from.blob.size = from_size;
    from.blob.allocated = from_size;
    if (from_size > 0) {
        from.blob.data = malloc(from_size);
        __CPROVER_assume(from.blob.data != NULL);
    } else {
        from.blob.data = NULL;
    }
    from.blob.growable = nondet_bool() ? 1 : 0;
    __CPROVER_assume(from.read_cursor <= from_size);
    __CPROVER_assume(from.write_cursor <= from_size);
    __CPROVER_assume(from.read_cursor <= from.write_cursor);
    from.high_water_mark = nondet_uint32_t();
    __CPROVER_assume(from.high_water_mark <= from_size);
    from.alloced = nondet_bool() ? 1 : 0;
    from.growable = nondet_bool() ? 1 : 0;
    from.tainted = nondet_bool() ? 1 : 0;

    /* Set up 'to' stuffer with allocated backing memory */
    uint32_t to_size = nondet_uint32_t();
    __CPROVER_assume(to_size <= MAX_BUFFER_SIZE);
    to.blob.size = to_size;
    to.blob.allocated = to_size;
    if (to_size > 0) {
        to.blob.data = malloc(to_size);
        __CPROVER_assume(to.blob.data != NULL);
    } else {
        to.blob.data = NULL;
    }
    to.blob.growable = nondet_bool() ? 1 : 0;
    __CPROVER_assume(to.read_cursor <= to_size);
    __CPROVER_assume(to.write_cursor <= to_size);
    __CPROVER_assume(to.read_cursor <= to.write_cursor);
    to.high_water_mark = nondet_uint32_t();
    __CPROVER_assume(to.high_water_mark <= to_size);
    to.alloced = nondet_bool() ? 1 : 0;
    to.growable = nondet_bool() ? 1 : 0;
    to.tainted = nondet_bool() ? 1 : 0;

    /* Bound len */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Save original cursors */
    uint32_t orig_from_read_cursor = from.read_cursor;
    uint32_t orig_to_write_cursor = to.write_cursor;

    /* Save other fields that should not change */
    uint32_t orig_from_write_cursor = from.write_cursor;
    uint32_t orig_from_high_water_mark = from.high_water_mark;
    uint32_t orig_to_read_cursor = to.read_cursor;
    uint32_t orig_to_high_water_mark = to.high_water_mark;

    /* Call function under test */
    int result = s2n_stuffer_copy(&from, &to, len);

    if (result == 0) {
        /* Success: read_cursor of from advances by len */
        assert(from.read_cursor == orig_from_read_cursor + len);
        /* Success: write_cursor of to advances by len */
        assert(to.write_cursor == orig_to_write_cursor + len);
    } else {
        /* Failure: cursors are restored */
        assert(from.read_cursor == orig_from_read_cursor);
        assert(to.write_cursor == orig_to_write_cursor);
    }

    /* Fields that must not change regardless of result */
    assert(from.write_cursor == orig_from_write_cursor);
    assert(from.high_water_mark == orig_from_high_water_mark);
    assert(to.read_cursor == orig_to_read_cursor);
    assert(to.high_water_mark == orig_to_high_water_mark);

    /* Blob fields must not change */
    assert(from.blob.size == from_size);
    assert(from.blob.allocated == from_size);
    assert(to.blob.size == to_size);
    assert(to.blob.allocated == to_size);
}
