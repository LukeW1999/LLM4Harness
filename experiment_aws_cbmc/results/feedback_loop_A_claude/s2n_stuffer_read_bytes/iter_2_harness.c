#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Minimal s2n definitions needed */
#define S2N_SUCCESS 0
#define S2N_FAILURE -1

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

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

int s2n_stuffer_read_bytes(struct s2n_stuffer *stuffer, uint8_t *data, uint32_t size);

/* nondet functions */
uint32_t nondet_uint32(void);
bool nondet_bool(void);

void s2n_stuffer_read_bytes_harness(void) {
    struct s2n_stuffer stuffer;
    uint32_t blob_size;
    __CPROVER_assume(blob_size <= MAX_BUFFER_SIZE);
    stuffer.blob.size = blob_size;
    stuffer.blob.allocated = blob_size;
    stuffer.blob.growable = nondet_bool() ? 1 : 0;
    if (blob_size > 0) {
        stuffer.blob.data = malloc(blob_size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    uint32_t read_cursor;
    uint32_t write_cursor;
    uint32_t high_water_mark;
    __CPROVER_assume(read_cursor <= write_cursor);
    __CPROVER_assume(write_cursor <= high_water_mark);
    __CPROVER_assume(high_water_mark <= blob_size);
    stuffer.read_cursor = read_cursor;
    stuffer.write_cursor = write_cursor;
    stuffer.high_water_mark = high_water_mark;
    stuffer.alloced = nondet_bool() ? 1 : 0;
    stuffer.growable = nondet_bool() ? 1 : 0;
    stuffer.tainted = nondet_bool() ? 1 : 0;

    uint32_t size;
    uint8_t *out_data = NULL;
    if (size > 0) {
        out_data = malloc(size);
        __CPROVER_assume(out_data != NULL);
    } else {
        out_data = malloc(1);
        __CPROVER_assume(out_data != NULL);
    }

    struct s2n_stuffer old_stuffer = stuffer;

    int result = s2n_stuffer_read_bytes(&stuffer, out_data, size);

    if (result == S2N_SUCCESS) {
        assert(stuffer.read_cursor == old_stuffer.read_cursor + size);
        assert(stuffer.write_cursor == old_stuffer.write_cursor);
        assert(stuffer.high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer.blob.data == old_stuffer.blob.data);
        assert(stuffer.blob.size == old_stuffer.blob.size);
        assert(stuffer.blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer.blob.growable == old_stuffer.blob.growable);
        assert(stuffer.alloced == old_stuffer.alloced);
        assert(stuffer.growable == old_stuffer.growable);
        assert(stuffer.tainted == old_stuffer.tainted);
    } else {
        assert(stuffer.read_cursor == old_stuffer.read_cursor);
        assert(stuffer.write_cursor == old_stuffer.write_cursor);
        assert(stuffer.high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer.blob.data == old_stuffer.blob.data);
        assert(stuffer.blob.size == old_stuffer.blob.size);
        assert(stuffer.blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer.blob.growable == old_stuffer.blob.growable);
        assert(stuffer.alloced == old_stuffer.alloced);
        assert(stuffer.growable == old_stuffer.growable);
        assert(stuffer.tainted == old_stuffer.tainted);
    }

    assert(stuffer.read_cursor <= stuffer.write_cursor);
    assert(stuffer.write_cursor <= stuffer.high_water_mark);
    assert(stuffer.high_water_mark <= stuffer.blob.size);
}
