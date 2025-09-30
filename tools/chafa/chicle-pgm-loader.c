#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>

#include <chafa.h>
#include "chicle-pgm-loader.h"
#include "chicle-util.h"

struct PgmLoader
{
    ChicleFileMapping *mapping;
    const guint8 *file_data;
    size_t file_data_len;
    gpointer frame_data;
    gint width, height, rowstride;
};

static PgmLoader *
chicle_pgm_loader_new (void)
{
    return g_new0 (PgmLoader, 1);
}

void
chicle_pgm_loader_destroy (PgmLoader *loader)
{
    if (!loader) return;

    if (loader->frame_data)
        free(loader->frame_data);

    if (loader->mapping)
        chicle_file_mapping_destroy (loader->mapping);

    g_free (loader);
}

static const guint8 *
find_header_end(const guint8 *data, size_t data_len, int *header_len)
{
    const guint8 *current = data;
    int lines_found = 0;
    *header_len = 0;

    while (*header_len < data_len && lines_found < 3) {
        if (current[*header_len] == '\n') {
            lines_found++;
            if (lines_found == 1 && (*header_len + 1 < data_len)) {
                if (current[*header_len + 1] == '#') {
                    while (*header_len < data_len && current[*header_len] != '\n') {
                        (*header_len)++;
                    }
                    continue;
                }
            }
        }
        (*header_len)++;
    }

    return current + (*header_len);
}

PgmLoader* chicle_pgm_loader_new_from_mapping(ChicleFileMapping *mapping)
{
    PgmLoader *loader = NULL;
    g_return_val_if_fail (mapping != NULL, NULL);

    loader = chicle_pgm_loader_new ();
    loader->mapping = mapping;
    loader->file_data = chicle_file_mapping_get_data (loader->mapping, &loader->file_data_len);

    if (loader->file_data_len < 10) {
        chicle_pgm_loader_destroy(loader);
        return NULL;
    }

    char format[3];
    int width, height, max_value;
    int header_length;
    const guint8 *pixel_data;

    if (sscanf((const char*)loader->file_data, "%2s %d %d %d",
        format, &width, &height, &max_value) != 4) {
        chicle_pgm_loader_destroy(loader);
    return NULL;
        }

        if (strcmp(format, "P5") != 0) {
            chicle_pgm_loader_destroy(loader);
            return NULL;
        }


        if (max_value > 255) {
            chicle_pgm_loader_destroy(loader);
            return NULL;
        }

        pixel_data = find_header_end(loader->file_data, loader->file_data_len, &header_length);

        size_t expected_pixel_data_size = width * height;
        size_t actual_pixel_data_size = loader->file_data_len - header_length;

        if (actual_pixel_data_size < expected_pixel_data_size) {
            chicle_pgm_loader_destroy(loader);
            return NULL;
        }

        loader->width = width;
        loader->height = height;
        loader->rowstride = width * 3;
        loader->frame_data = malloc(loader->width * loader->height * 3);

        if (!loader->frame_data) {
            chicle_pgm_loader_destroy(loader);
            return NULL;
        }

        guint8 *rgb_data = (guint8 *)loader->frame_data;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int src_idx = y * width + x;
                int dst_idx = (y * width + x) * 3;

                guint8 gray_value = pixel_data[src_idx];

                rgb_data[dst_idx] = gray_value;     // R
                rgb_data[dst_idx + 1] = gray_value; // G
                rgb_data[dst_idx + 2] = gray_value; // B
            }
        }

        return loader;
}

gboolean
chicle_pgm_loader_get_is_animation (PgmLoader *loader)
{
    g_return_val_if_fail (loader != NULL, FALSE);
    return FALSE;
}

gconstpointer
chicle_pgm_loader_get_frame_data (PgmLoader *loader,
                                  ChafaPixelType *pixel_type_out,
                                  gint *width_out,
                                  gint *height_out,
                                  gint *rowstride_out)
{
    g_return_val_if_fail (loader != NULL, NULL);

    if (pixel_type_out)
        *pixel_type_out = CHAFA_PIXEL_RGB8;
    if (width_out)
        *width_out = loader->width;
    if (height_out)
        *height_out = loader->height;
    if (rowstride_out)
        *rowstride_out = loader->rowstride;

    return loader->frame_data;
}

gint
chicle_pgm_loader_get_frame_delay (PgmLoader *loader)
{
    g_return_val_if_fail (loader != NULL, 0);
    return 0;
}

void
chicle_pgm_loader_goto_first_frame (PgmLoader *loader)
{
    g_return_if_fail (loader != NULL);
}

gboolean
chicle_pgm_loader_goto_next_frame (PgmLoader *loader)
{
    g_return_val_if_fail (loader != NULL, FALSE);
    return FALSE;
}
