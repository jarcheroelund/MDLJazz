#pragma once 

#include <stdint.h>


typedef struct QSkinTextureHandle {
    uint32_t gl_id;
    uint8_t* texture_data_local;
} QSkinTextureHandle;

uint32_t upload_qpalette(uint8_t palette_lmp[256*3]);

QSkinTextureHandle gl_upload_qskintexture(uint32_t palette_id, uint32_t size_x, uint32_t size_y, uint8_t* skin_data);