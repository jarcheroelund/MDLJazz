#include <stdint.h>
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <stdio.h>
#include <pak.h>
#include <gl_qtex.h>


/*
    This code isn't very elegant.
    There are very few comments.
    Only the texture pak file code have their own translation units.
    I would probably change these things if this wasn't just a learning exersise
    For OpenGL and Quake formats.
*/

/* 
    You can read up on the Quake MDL and PAK specs here:

    PAK: https://quakewiki.org/wiki/.pak
    MDL: http://tfc.duke.free.fr/coding/mdl-specs-en.html
*/


// Basically none of these fields are used but I need them for fread() to read the right amount of bytes
typedef struct MdlHeader {
    int ident;            /* magic number: "IDPO" */
    int version;          /* version: 6 */

    float scale[3];         /* scale factor */
    float translate[3];     /* translation vector */
    float bounding_radius;
    float eye_position[3];   /* eyes' position */

    int num_skins;        /* number of textures */
    int skinwidth;        /* texture width */
    int skinheight;       /* texture height */

    int num_verts;        /* number of vertices */
    int num_tris;         /* number of triangles */
    int num_frames;       /* number of frames */

    int synctype;         /* 0 = synchron, 1 = random */
    int flags;            /* state flag */
    float size;

    int skin_group;
} MdlHeader;


float triangle_vertices[] = {
    /*positions*/-0.5f, -0.5f, 0.0f,  /*UVs*/   0.0f, 0.0f,
                  0.5f, -0.5f, 0.0f,            1.0f, 0.0f,
                  0.0f,  0.5f, 0.0f,            0.5f, 1.0f 
};  

const char* test_vert_shader_source = "#version 450 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "   TexCoord = aTexCoord;\n"
    "}\n";

const char* test_frag_shader_source = "#version 450 core\n"
    "out vec4 FragColor;\n"
    "\n"
    "in vec2 TexCoord;\n"
    "\n"
    "layout(binding=0) uniform sampler2D ourTexture;\n"
    "void main()\n"
    "{\n"
    "    FragColor = texture(ourTexture, TexCoord);\n"
    "}\n";

int main(int argc, char** argv) {
    if(argc != 3) {
        printf("Wrong Arguments. Aborting!");
        return 0;
    }
    const char* pak_name = argv[1];
    const char* pak_mdl_name = argv[2];
    printf("Opening PAK: %s\n", pak_name);
    PakArchiveHandle pak_file = open_pak_archive_read(pak_name);

    if (!pak_file) {
        printf("Invalid Pak\n");
        return 0;
    }

    PakFileEntryDataHandle qpalette = open_pak_file_entry_read(pak_file, "gfx/palette.lmp");
    printf("Opening MDL: %s\n", pak_mdl_name);
    PakFileEntryDataHandle new_dog_mdl = open_pak_file_entry_read(pak_file, pak_mdl_name);

    uint8_t qpalette_buffer[256*3];
    pak_file_entry_get_bytes(qpalette, 0, 256*3, qpalette_buffer);

    MdlHeader new_dog_mdl_header;
    
    pak_file_entry_get_bytes(new_dog_mdl, 0, sizeof(MdlHeader), &new_dog_mdl_header);


    if(new_dog_mdl_header.skin_group != 0) {
        printf("Cannot Read this type of model \n");
        return 1;
    }

    if(new_dog_mdl_header.ident != 'OPDI') {
        printf("Failed to read new_dog_mdl \n");
    }


    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("%s", SDL_GetError());
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    SDL_Window* mdljazz_window = SDL_CreateWindow("MDLJazz", 300, 300, 1280, 720, SDL_WINDOW_OPENGL);
    if(!SDL_GL_CreateContext(mdljazz_window)) {
        printf("Could not create OpenGL context. SDL Error: %s\n", SDL_GetError());
    }

    gladLoadGLLoader(SDL_GL_GetProcAddress);

    uint32_t qpalette_id = upload_qpalette(qpalette_buffer);

    uint8_t* new_dog_mdl_skin_data = malloc(new_dog_mdl_header.skinwidth * new_dog_mdl_header.skinheight);
    pak_file_entry_get_bytes(new_dog_mdl, sizeof(MdlHeader), new_dog_mdl_header.skinwidth * new_dog_mdl_header.skinheight, new_dog_mdl_skin_data);

    QSkinTextureHandle new_dog_skin = gl_upload_qskintexture(qpalette_id, new_dog_mdl_header.skinwidth, new_dog_mdl_header.skinheight, new_dog_mdl_skin_data);


    uint32_t test_vao;
    glGenVertexArrays(1, &test_vao);  

    uint32_t test_vbo;
    glGenBuffers(1, &test_vbo);

    glBindVertexArray(test_vao);
    glBindBuffer(GL_ARRAY_BUFFER, test_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);  

    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertex_shader, 1, &test_vert_shader_source, NULL);
    glCompileShader(vertex_shader);

    int  success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %s\n", info_log);
    }

    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragment_shader, 1, &test_frag_shader_source, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n %s\n", info_log);
    }

    uint32_t shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        printf("ERROR::SHADER::LINKING_FAILED\n %s\n", info_log);
    }
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    int running = 1;
    while(running) {
        SDL_GL_SwapWindow(mdljazz_window);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, new_dog_skin.gl_id);
        glBindTextureUnit(0, new_dog_skin.gl_id);

        glBindVertexArray(test_vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {  // poll until all events are handled!
            if(event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                    running = 0;
                }
            }
        }   
    }

    SDL_Quit();
    return 0;
}