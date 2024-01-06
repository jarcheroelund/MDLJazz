#include <gl_qtex.h>
#include <glad/glad.h>
#include <stdlib.h>
#include <string.h>

typedef struct QPalette {
    uint8_t pal[3*256];
} QPalette;

// Dynamic list of palettes
uint32_t PalListSize = 0;
uint32_t PalListUsed = 0;
QPalette* PalList = NULL;


uint32_t upload_qpalette(uint8_t palette_lmp[256*3]) {
    
    if(!PalList) {
        PalList = malloc(sizeof(QPalette));
        PalListSize = 1;
    }

    if(PalListUsed == PalListSize) {
        PalList = realloc(PalList, sizeof(QPalette) * (PalListSize + 1));
        PalListSize ++;
    }

    memcpy(PalList + PalListUsed, palette_lmp, sizeof(QPalette));

    PalListUsed++;
    return PalListUsed - 1;
}

QSkinTextureHandle gl_upload_qskintexture(uint32_t palette_id, uint32_t size_x, uint32_t size_y, uint8_t* skin_data) {
    uint8_t* texture_buffer = malloc(size_x * size_y * 3);

// OpenGL doesn't have paletted textures.
// this means we need to convert the incoming Quake texture data that uses palettes into RGBA8 so OpenGL can understand it.
    for (int i = 0; i < (size_x * size_y); i ++) {
        texture_buffer[(i*3)+0]   = (PalList[palette_id]).pal[(skin_data[i])*3];
        texture_buffer[(i*3)+1] = (PalList[palette_id]).pal[(skin_data[i])*3+1];
        texture_buffer[(i*3)+2] = (PalList[palette_id]).pal[(skin_data[i])*3+2];
    }
       
    uint32_t skin_texture_id;
    glGenTextures(1, &skin_texture_id);
    glBindTexture(GL_TEXTURE_2D, skin_texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size_x, size_y, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_buffer);
    glGenerateMipmap(GL_TEXTURE_2D);

    QSkinTextureHandle return_handle = {
        .gl_id = skin_texture_id,
        .texture_data_local = texture_buffer
    };

    glBindTexture(GL_TEXTURE_2D, 0);
    return return_handle;
}