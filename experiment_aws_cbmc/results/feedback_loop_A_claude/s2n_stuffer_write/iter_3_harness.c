#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

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

int s2n_stuffer_write(struct s2n_stuffer *stuffer, const struct s2n_blob *in);

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

/* Nondet helpers */
uint32_t nondet_uint32_t(void);
bool nondet_bool(void);

void s2n_stuffer_write_harness(void) {
    /* Set up stuffer */
    struct s2n_stuffer stuffer;

    /* Allocate blob data for stuffer */
    uint32_t blob_size = nondet_uint32_t();
    __CPROVER_assume(blob_size <= MAX_BUFFER_SIZE);
    stuffer.blob.size = blob_size;
    stuffer.blob.allocated = blob_size;
    if (blob_size > 0) {
        stuffer.blob.data = malloc(blob_size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }
    stuffer.blob.growable = nondet_bool() ? 1 : 0;

    /* Set up cursors - must be valid */
    stuffer.read_cursor = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.write_cursor <= blob_size);
    __CPROVER_assume(stuffer.high_water_mark <= blob_size);
    __CPROVER_assume(stuffer.write_cursor <= stuffer.high_water_mark);

    stuffer.alloced = nondet_bool() ? 1 : 0;
    stuffer.growable = nondet_bool() ? 1 : 0;
    stuffer.tainted = nondet_bool() ? 1 : 0;

    /* Set up input blob */
    struct s2n_blob in;
    uint32_t in_size = nondet_uint32_t();
    __CPROVER_assume(in_size <= MAX_BUFFER_SIZE);
    in.size = in_size;
    in.allocated = in_size;
    if (in_size > 0) {
        in.data = malloc(in_size);
        __CPROVER_assume(in.data != NULL);
    } else {
        in.data = NULL;
    }
    in.growable = nondet_bool() ? 1 : 0;

    /* Save old state */
    struct s2n_stuffer old_stuffer = stuffer;
    struct s2n_blob old_in = in;
    uint32_t old_write_cursor = stuffer.write_cursor;
    uint32_t old_read_cursor = stuffer.read_cursor;
    uint32_t old_high_water_mark = stuffer.high_water_mark;

    /* Call function under test */
    int result = s2n_stuffer_write(&stuffer, &in);

    /* Input blob must not be modified */
    assert(in.data == old_in.data);
    assert(in.size == old_in.size);
    assert(in.allocated == old_in.allocated);

    /* Stuffer structural fields must not change */
    assert(stuffer.blob.data == old_stuffer.blob.data);
    assert(stuffer.blob.size == old_stuffer.blob.size);
    assert(stuffer.blob.allocated == old_stuffer.blob.allocated);
    assert(stuffer.alloced == old_stuffer.alloced);
    assert(stuffer.read_cursor == old_read_cursor);

    if (result == 0) {
        /* Success: write_cursor advanced by in->size */
        assert(stuffer.write_cursor == old_write_cursor + in.size);
        /* high_water_mark >= write_cursor */
        assert(stuffer.high_water_mark >= stuffer.write_cursor);
    } else {
        /* Failure: result should be -1 */
        assert(result == -1);
    }

    return 0;
}
