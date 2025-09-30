#ifndef __CHICLE_PGM_LOADER_H__
#define __CHICLE_PGM_LOADER_H__

#include <glib.h>
#include "chicle-file-mapping.h"

G_BEGIN_DECLS

typedef struct PgmLoader PgmLoader;

PgmLoader *chicle_pgm_loader_new_from_mapping (ChicleFileMapping *mapping);
void chicle_pgm_loader_destroy (PgmLoader *loader);

gboolean chicle_pgm_loader_get_is_animation (PgmLoader *loader);

gconstpointer chicle_pgm_loader_get_frame_data (PgmLoader *loader,
                                                 ChafaPixelType *pixel_type_out,
                                                 gint *width_out,
                                                 gint *height_out,
                                                 gint *rowstride_out);




//ok
void chicle_pgm_loader_goto_first_frame (PgmLoader *loader);
gboolean chicle_pgm_loader_goto_next_frame (PgmLoader *loader);
gint chicle_pgm_loader_get_frame_delay (PgmLoader *loader);

G_END_DECLS

#endif /* __CHICLE_PGM_LOADER_H__ */
