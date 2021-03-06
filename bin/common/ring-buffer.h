#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <glib.h>

G_BEGIN_DECLS

typedef struct {
    guchar  *data;
    gsize    block_size;
    guint    n_blocks_total;
    guint    n_blocks_used;
    guint    current_index;
} RingBuffer;

RingBuffer * ring_buffer_new                  (gsize       block_size,
                                               gsize       n_blocks);
void         ring_buffer_free                 (RingBuffer *buffer);
void         ring_buffer_reset                (RingBuffer *buffer);
gsize        ring_buffer_get_block_size       (RingBuffer *buffer);
gpointer     ring_buffer_get_current_pointer  (RingBuffer *buffer);
void         ring_buffer_set_current_pointer  (RingBuffer *buffer,
                                               guint       index);
gpointer     ring_buffer_get_pointer          (RingBuffer *buffer,
                                               guint       index);
guint        ring_buffer_get_num_blocks       (RingBuffer *buffer);
void         ring_buffer_proceed              (RingBuffer *buffer);

G_END_DECLS

#endif
