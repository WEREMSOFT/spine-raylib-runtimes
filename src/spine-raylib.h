//
// Created by Pablo Weremczuk on 1/13/20.
//

#ifndef RAYLIBTEST_SPINE_RAYLIB_H
#define RAYLIBTEST_SPINE_RAYLIB_H
#ifndef SP_LAYER_SPACING
#define SP_LAYER_SPACING 0
#endif

#ifndef SP_LAYER_SPACING_BASE
#define SP_LAYER_SPACING_BASE 0
#endif

#include <spine/extension.h>
#include <rlgl.h>
#include <stdio.h>

float anti_z_fighting_index = SP_LAYER_SPACING_BASE;


#define MAX_TEXTURES 10
static Texture2D tm_textures[MAX_TEXTURES] = {0};
static int texture_index = 0;

char *_spUtil_readFile(const char *path, int *length) {
    return _spReadFile(path, length);
}

void _spAtlasPage_disposeTexture(spAtlasPage *self) {
    if (self->rendererObject == NULL) return;
    Texture2D *t2d = self->rendererObject;
    UnloadTexture(*t2d);
}

typedef struct Vertex {
    // Position in x/y plane
    float x, y;

    // UV coordinates
    float u, v;

    // Color, each channel in the range from 0-1
    // (Should really be a 32-bit RGBA packed color)
    float r, g, b, a;
} Vertex;

#define MAX_VERTICES_PER_ATTACHMENT 2048
float worldVerticesPositions[MAX_VERTICES_PER_ATTACHMENT];

Vertex vertices[MAX_VERTICES_PER_ATTACHMENT];

void addVertex(float x, float y, float u, float v, float r, float g, float b, float a, int *index) {
    Vertex *vertex = &vertices[*index];
    vertex->x = x;
    vertex->y = y;
    vertex->u = u;
    vertex->v = v;
    vertex->r = r;
    vertex->g = g;
    vertex->b = b;
    vertex->a = a;
    *index += 1;
}

void engine_draw_region(Vertex* vertices, Texture* texture, Vector3 position, int* vertex_order){
    Vertex vertex;
    rlEnableTexture(texture->id);
    rlPushMatrix();
    {
        rlBegin(RL_QUADS);
        {
            rlNormal3f(0.0f, 0.0f, 1.0f);
            for (int i = 0; i < 4; i++){
                vertex = vertices[vertex_order[i]];
                rlTexCoord2f(vertex.u, vertex.v);
                rlColor4f(vertex.r, vertex.g, vertex.b, vertex.a);
                rlVertex3f( position.x + vertex.x, position.y + vertex.y, position.z + anti_z_fighting_index);
            }
        }rlEnd();

#ifdef SP_DRAW_DOUBLE_FACED
        rlBegin(RL_QUADS);
        {
            rlNormal3f(0.0f, 0.0f, 1.0f);
            for (int i = 3; i >= 0; i--){
                vertex = vertices[vertex_order[i]];
                rlTexCoord2f(vertex.u, vertex.v);
                rlColor4f(vertex.r, vertex.g, vertex.b, vertex.a);
                rlVertex3f( position.x + vertex.x, position.y + vertex.y, position.z - anti_z_fighting_index);
            }
        }rlEnd();
#endif

    }rlPopMatrix();
    rlDisableTexture();
}
void engine_drawMesh(Vertex *vertices, int start, int count, Texture *texture, Vector3 position, int *vertex_order) {
    Vertex vertex;
    rlPushMatrix();
    {
        for (int vertexIndex = start; vertexIndex < count; vertexIndex += 3) {
            rlEnableTexture(texture->id);
            rlBegin(RL_QUADS);
            {
                int i;
                for (i = 2; i > -1; i--) {
                    vertex = vertices[vertexIndex + i];
                    rlTexCoord2f(vertex.u, vertex.v);
                    rlColor4f(vertex.r, vertex.g, vertex.b, vertex.a);
                    rlVertex3f(position.x + vertex.x, position.y + vertex.y, position.z + anti_z_fighting_index);
                }
                rlVertex3f(position.x + vertex.x, position.y + vertex.y, position.z + anti_z_fighting_index);
            }
            rlEnd();
#ifdef SP_DRAW_DOUBLE_FACED
            fprintf(stderr, "double sided not supported for mesh based spine files\n");
            exit(-1);
#endif

#ifdef SP_RENDER_WIREFRAME
                DrawTriangleLines((Vector2) {vertices[vertexIndex].x + position.x, vertices[vertexIndex].y + position.y},
                                  (Vector2) {vertices[vertexIndex + 1].x + position.x, vertices[vertexIndex + 1].y + position.y},
                                  (Vector2) {vertices[vertexIndex + 2].x + position.x, vertices[vertexIndex + 2].y + position.y}, vertexIndex == 0 ? RED : GREEN);
#endif

        }



    }
    rlPopMatrix();
    rlDisableTexture();
}

Texture2D *texture_2d_create(char *path) {
    tm_textures[texture_index] = LoadTexture(path);
    Texture2D *t = &tm_textures[texture_index];
    SetTextureFilter(*t, FILTER_BILINEAR);
    texture_index++;
    return t;
}

void texture_2d_destroy() {
    while (texture_index--) UnloadTexture(tm_textures[texture_index]);
}

void _spAtlasPage_createTexture(spAtlasPage *self, const char *path) {

    Texture2D *t = texture_2d_create((char *) path);

    self->rendererObject = t;
    self->width = t->width;
    self->height = t->height;
}


#define MAX_VERTICES_PER_ATTACHMENT 2048
float worldVerticesPositions[MAX_VERTICES_PER_ATTACHMENT];
Vertex vertices[MAX_VERTICES_PER_ATTACHMENT];
int VERTEX_ORDER_NORMAL[] = {0, 1, 2, 4};
int VERTEX_ORDER_INVERSE[] = {4, 2, 1, 0};

#include <glad.h>

void drawSkeleton(spSkeleton *skeleton, Vector3 position, bool PMA) {
    int blend_mode = 4; //This mode doesnt exist
    int *vertex_order = (skeleton->scaleX * skeleton->scaleY < 0) ? VERTEX_ORDER_NORMAL : VERTEX_ORDER_INVERSE;
    // For each slot in the draw order array of the skeleton
    anti_z_fighting_index = SP_LAYER_SPACING_BASE;
    for (int i = 0; i < skeleton->slotsCount; ++i) {
        anti_z_fighting_index -= SP_LAYER_SPACING;
        spSlot *slot = skeleton->drawOrder[i];

        // Fetch the currently active attachment, continue
        // with the next slot in the draw order if no
        // attachment is active on the slot
        spAttachment *attachment = slot->attachment;
        if (!attachment) continue;


        // Fill the vertices array depending on the type of attachment
        Texture *texture = 0;
        int vertexIndex = 0;
        if (attachment->type == SP_ATTACHMENT_REGION) {
            // Cast to an spRegionAttachment so we can get the rendererObject
            // and compute the world vertices
            spRegionAttachment *regionAttachment = (spRegionAttachment *) attachment;
            // Calculate the tinting color based on the skeleton's color,
            // the slot's color and attachment's color. Each color channel is
            // given in the range [0-1], you may have to multiply by 255 and
            // cast to and int if your engine uses integer ranges for color channels.
            float tintA = skeleton->color.a * slot->color.a * regionAttachment->color.a;
            float alpha = PMA ? tintA : 1;
            float tintR = skeleton->color.r * slot->color.r * regionAttachment->color.r * alpha;
            float tintG = skeleton->color.g * slot->color.g * regionAttachment->color.g * alpha;
            float tintB = skeleton->color.b * slot->color.b * regionAttachment->color.b * alpha;

            // Our engine specific Texture is stored in the spAtlasRegion which was
            // assigned to the attachment on load. It represents the texture atlas
            // page that contains the image the region attachment is mapped to
            texture = (Texture *) ((spAtlasRegion *) regionAttachment->rendererObject)->page->rendererObject;

            // Computed the world vertices positions for the 4 vertices that make up
            // the rectangular region attachment. This assumes the world transform of the
            // bone to which the slot (and hence attachment) is attached has been calculated
            // before rendering via spSkeleton_updateWorldTransform
            spRegionAttachment_computeWorldVertices(regionAttachment, slot->bone, worldVerticesPositions, 0, 2);

            // Create 2 triangles, with 3 vertices each from the region's
            // world vertex positions and its UV coordinates (in the range [0-1]).
            addVertex(worldVerticesPositions[0], worldVerticesPositions[1],
                      regionAttachment->uvs[0], regionAttachment->uvs[1],
                      tintR, tintG, tintB, tintA, &vertexIndex);

            addVertex(worldVerticesPositions[2], worldVerticesPositions[3],
                      regionAttachment->uvs[2], regionAttachment->uvs[3],
                      tintR, tintG, tintB, tintA, &vertexIndex);

            addVertex(worldVerticesPositions[4], worldVerticesPositions[5],
                      regionAttachment->uvs[4], regionAttachment->uvs[5],
                      tintR, tintG, tintB, tintA, &vertexIndex);

            addVertex(worldVerticesPositions[4], worldVerticesPositions[5],
                      regionAttachment->uvs[4], regionAttachment->uvs[5],
                      tintR, tintG, tintB, tintA, &vertexIndex);

            addVertex(worldVerticesPositions[6], worldVerticesPositions[7],
                      regionAttachment->uvs[6], regionAttachment->uvs[7],
                      tintR, tintG, tintB, tintA, &vertexIndex);

            addVertex(worldVerticesPositions[0], worldVerticesPositions[1],
                      regionAttachment->uvs[0], regionAttachment->uvs[1],
                      tintR, tintG, tintB, tintA, &vertexIndex);

            if (slot->data->blendMode != blend_mode)
            {
                EndBlendMode(); //Need this line for blending to work for some reason
                blend_mode = slot->data->blendMode;
                switch (blend_mode)
                {
                    default: //Normal
                        rlSetBlendMode(PMA ? GL_ONE : GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD);
                        break;
                    case 1: //Additive
                        rlSetBlendMode(PMA ? GL_ONE : GL_SRC_ALPHA, GL_ONE, GL_FUNC_ADD);
                        break;
                    case 2: //Multiply
                        rlSetBlendMode(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD);
                        break;
                    case 3: //Screen
                        rlSetBlendMode(GL_ONE, GL_ONE_MINUS_SRC_COLOR, GL_FUNC_ADD);
                        break;
                }
                BeginBlendMode(BLEND_CUSTOM); 
            }
            
            engine_draw_region(vertices, texture, position, vertex_order);
        } else if (attachment->type == SP_ATTACHMENT_MESH) {
            // Cast to an spMeshAttachment so we can get the rendererObject
            // and compute the world vertices
            spMeshAttachment *mesh = (spMeshAttachment *) attachment;
            // Calculate the tinting color based on the skeleton's color,
            // the slot's color and attachment's color. Each color channel is
            // given in the range [0-1], you may have to multiply by 255 and
            // cast to and int if your engine uses integer ranges for color channels.
            float tintA = skeleton->color.a * slot->color.a * mesh->color.a;
            float alpha = PMA ? tintA : 1;
            float tintR = skeleton->color.r * slot->color.r * mesh->color.r * alpha;
            float tintG = skeleton->color.g * slot->color.g * mesh->color.g * alpha;
            float tintB = skeleton->color.b * slot->color.b * mesh->color.b * alpha;

            // Check the number of vertices in the mesh attachment. If it is bigger
            // than our scratch buffer, we don't render the mesh. We do this here
            // for simplicity, in production you want to reallocate the scratch buffer
            // to fit the mesh.
            if (mesh->super.worldVerticesLength > MAX_VERTICES_PER_ATTACHMENT) continue;

            // Our engine specific Texture is stored in the spAtlasRegion which was
            // assigned to the attachment on load. It represents the texture atlas
            // page that contains the image the mesh attachment is mapped to
            texture = (Texture *) ((spAtlasRegion *) mesh->rendererObject)->page->rendererObject;

            // Computed the world vertices positions for the vertices that make up
            // the mesh attachment. This assumes the world transform of the
            // bone to which the slot (and hence attachment) is attached has been calculated
            // before rendering via spSkeleton_updateWorldTransform
            spVertexAttachment_computeWorldVertices(SUPER(mesh), slot, 0, mesh->super.worldVerticesLength,
                                                    worldVerticesPositions, 0, 2);

            // Mesh attachments use an array of vertices, and an array of indices to define which
            // 3 vertices make up each triangle. We loop through all triangle indices
            // and simply emit a vertex for each triangle's vertex.
            for (int i = 0; i < mesh->trianglesCount; ++i) {
                int index = mesh->triangles[i] << 1;
                addVertex(worldVerticesPositions[index], worldVerticesPositions[index + 1],
                          mesh->uvs[index], mesh->uvs[index + 1],
                          tintR, tintG, tintB, tintA, &vertexIndex);
            }
            if (slot->data->blendMode != blend_mode)
            {
                EndBlendMode();
                blend_mode = slot->data->blendMode;
                switch (blend_mode)
                {
                    default: //Normal
                        rlSetBlendMode(PMA ? GL_ONE : GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD);
                        break;
                    case 1: //Additive
                        rlSetBlendMode(PMA ? GL_ONE : GL_SRC_ALPHA, GL_ONE, GL_FUNC_ADD);
                        break;
                    case 2: //Multiply
                        rlSetBlendMode(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD);
                        break;
                    case 3: //Screen
                        rlSetBlendMode(GL_ONE, GL_ONE_MINUS_SRC_COLOR, GL_FUNC_ADD);
                        break;
                }
                BeginBlendMode(BLEND_CUSTOM); 
            }
            // Draw the mesh we created for the attachment
            engine_drawMesh(vertices, 0, vertexIndex, texture, position, vertex_order);
        }
    }
    EndBlendMode(); //Exit out
}


#endif //RAYLIBTEST_SPINE_RAYLIB_H
