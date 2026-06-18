#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

/* Minimal s2n struct definitions needed for the harness */
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

/* Forward declaration of the function under test */
int s2n_stuffer_write_uint32(struct s2n_stuffer *stuffer, const uint32_t u);

/* Nondet helpers */
uint32_t nondet_uint32_t(void);
bool nondet_bool(void);

void s2n_stuffer_write_uint32_harness(void) {
    /* 1. Declare and set up the stuffer */
    struct s2n_stuffer stuffer;

    /* Non-deterministically initialize stuffer fields */
    stuffer.read_cursor = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    stuffer.alloced = nondet_bool() ? 1 : 0;
    stuffer.growable = nondet_bool() ? 1 : 0;
    stuffer.tainted = nondet_bool() ? 1 : 0;

    /* Set up the blob */
    stuffer.blob.size = nondet_uint32_t();
    stuffer.blob.allocated = nondet_uint32_t();
    stuffer.blob.growable = nondet_bool() ? 1 : 0;

    /* Bound the blob size to keep verification tractable */
    __CPROVER_assume(stuffer.blob.size <= 16);
    __CPROVER_assume(stuffer.blob.allocated >= stuffer.blob.size);

    /* Allocate memory for the blob data */
    if (stuffer.blob.size > 0) {
        stuffer.blob.data = malloc(stuffer.blob.size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    /* Ensure cursors are within valid range */
    __CPROVER_assume(stuffer.write_cursor <= stuffer.blob.size);
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);

    /* The value to write */
    uint32_t u = nondet_uint32_t();

    /* 2. Save old state */
    struct s2n_stuffer old = stuffer;
    uint32_t old_write_cursor = stuffer.write_cursor;
    uint32_t old_read_cursor = stuffer.read_cursor;
    uint32_t old_high_water_mark = stuffer.high_water_mark;

    /* 3. Call the function under test */
    int result = s2n_stuffer_write_uint32(&stuffer, u);

    /* 4. Assert postconditions */

    /* read_cursor must never change */
    assert(stuffer.read_cursor == old_read_cursor);

    /* blob pointer and size fields must not change */
    assert(stuffer.blob.data == old.blob.data);
    assert(stuffer.blob.size == old.blob.size);
    assert(stuffer.blob.allocated == old.blob.allocated);
    assert(stuffer.blob.growable == old.blob.growable);

    /* alloced, growable, tainted flags must not change */
    assert(stuffer.alloced == old.alloced);
    assert(stuffer.growable == old.growable);
    assert(stuffer.tainted == old.tainted);

    if (result == 0) {
        /* Success: write_cursor advanced by 4 bytes */
        assert(stuffer.write_cursor == old_write_cursor + sizeof(uint32_t));

        /* high_water_mark is at least as large as write_cursor */
        assert(stuffer.high_water_mark >= stuffer.write_cursor);

        /* write_cursor must still be within blob size */
        assert(stuffer.write_cursor <= stuffer.blob.size);

        /* The 4 bytes were written in network (big-endian) order */
        if (stuffer.blob.data != NULL && old_write_cursor + 4 <= stuffer.blob.size) {
            uint8_t expected_b0 = (uint8_t)((u >> 24) & 0xFF);
            uint8_t expected_b1 = (uint8_t)((u >> 16) & 0xFF);
            uint8_t expected_b2 = (uint8_t)((u >> 8) & 0xFF);
            uint8_t expected_b3 = (uint8_t)(u & 0xFF);

            assert(stuffer.blob.data[old_write_cursor + 0] == expected_b0);
            assert(stuffer.blob.data[old_write_cursor + 1] == expected_b1);
            assert(stuffer.blob.data[old_write_cursor + 2] == expected_b2);
            assert(stuffer.blob.data[old_write_cursor + 3] == expected_b3);
        }
    } else {
        /* Failure: write_cursor should be unchanged */
        assert(stuffer.write_cursor == old_write_cursor);

        /* high_water_mark should be unchanged on failure */
        assert(stuffer.high_water_mark == old_high_water_mark);
    }
}
