#define STB_IMAGE_IMPLEMENTATION
#include "render_primitives.h"
#include "mesh_utils.h"
#include "basic_objects.h"
#include "asset_loader.h"
#include "dirent.h"
#include "material_insts.h"
#include "model_table.h"
#include "debug.h"

//#include <stb_image.h>

int _Load_Obj(const char* filename, objData* data) 
{

    if (!filename) {
        fprintf(stderr, "null filename\n");
        return 1;
    }

    float* verts = (float*)calloc(3, sizeof(float));
    uint32_t vertcount = 0;
    uint32_t vertcap = 3;

    float* uvs = (float*)calloc(2, sizeof(float));
    uint32_t uvcount = 0;
    uint32_t uvcap = 2;

    float* normals = (float*)calloc(3, sizeof(float));
    uint32_t ncount = 0;
    uint32_t ncap = 3;

    uint32_t* faces = (uint32_t*)calloc(9, sizeof(uint32_t));
    uint32_t facecount = 0;
    uint32_t facecap = 9; 

    subMesh* submeshes = (subMesh*)calloc(1, sizeof(subMesh));
    uint32_t smcount = 0;
    uint32_t smcap = 1;


    FILE* f = fopen(filename, "r");
    fseek(f, 0, SEEK_END);
    long size = ftell(f) + 1;
    fseek(f, 0, SEEK_SET);
    
    char* mtlib = (char*)calloc(64, sizeof(char));
    char* oname = (char*)calloc(64, sizeof(char));
    char buf[64] = {};
    uint32_t scres = 0;
    float x, y, z ,w;
    int v1, t1, n1, v2, t2, n2, v3, t3, n3;
    subMesh sm = {};
    uint32_t sm_wr = 0;
    while (ftell(f) != size - 1)
    {
        fgets(buf, 64, f);
        if (buf[0] == EOF || buf[0] == '\0') break;
        if (buf[0] == '#' || buf[0] == ' ' || buf[0] == '\n') continue;
        if (buf[0] == 'm' && sscanf(buf, "mtllib %s", mtlib)) continue;
        if (buf[0] == 'u' && sscanf(buf, "usemtl %s", sm.matName)) continue;
        if (buf[0] == 's') continue; // smooth shading; not supported now
        if (buf[0] == 'o' && sscanf(buf, "o %s",oname))
        {
            printf("Loading mesh %s\n", oname);
            sm.len = facecount - sm.startInd;
            if (sm_wr)
            {
                if (smcount == smcap)
                {
                    submeshes = (subMesh*)realloc(submeshes, 2 * smcap * sizeof(subMesh));
                    smcap *= 2;
                }
                submeshes[smcount] = sm;
                smcount++;
            }
            sm.len = 0;
            sm.matName = (char*)calloc(32, sizeof(char));
            sm.startInd = facecount;
            sm_wr = 1;
            continue;
        }
        

        // try parse vert
        scres = sscanf(buf, "v %f %f %f %f", &x, &y, &z, &w);
        if (scres >= 3) 
        {
            if (scres == 3) w = 1;

            if (vertcap == vertcount)
            {
                verts = (float*)realloc(verts, vertcap * 2 * sizeof(float));
                vertcap *= 2;
            }
            verts[vertcount] = x * w;
            verts[vertcount+1] = y * w;
            verts[vertcount+2] = z * w;

            assert(x < 10e5);
            assert(y < 10e5);
            assert(z < 10e5);
            
            vertcount += 3;
            continue;
        }
        
        // try parse uv
        scres = sscanf(buf, "vt %f %f %f", &x, &y, &z);
        if (scres >= 2)
        {
            if (uvcap == uvcount)
            {
                uvs = (float*)realloc(uvs, uvcap * 2 * sizeof(float));
                uvcap *= 2;
            }
            uvs[uvcount] = x;
            uvs[uvcount+1] = y;
            uvcount += 2;
            continue;
        }

        // vertex normals
        scres = sscanf(buf, "vn %f %f %f", &x, &y, &z);
        if (scres == 3)
        {
            if (ncap == ncount)
            {
                normals = (float*)realloc(normals, ncap * 2 * sizeof(float));
                ncap *= 2;
            }
            normals[ncount] = x;
            normals[ncount+1] = y;
            normals[ncount+2] = z;
            ncount += 3;
            continue;
        }        

        // faces (triangles)
        scres = sscanf(buf, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
        if (scres == 9)
        {
            if (facecap == facecount)
            {
                faces = (uint32_t*)realloc(faces, facecap * 2 * sizeof(uint32_t));
                facecap *= 2;
            }
            faces[facecount] =   v1 > 0 ? v1 - 1 : vertcount - v1;
            faces[facecount+1] = t1 > 0 ? t1 - 1 : uvcount - t1;
            faces[facecount+2] = n1 > 0 ? n1 - 1 : ncount - n1;
            faces[facecount+3] = v2 > 0 ? v2 - 1 : vertcount - v2;
            faces[facecount+4] = t2 > 0 ? t2 - 1 : uvcount - t2;
            faces[facecount+5] = n2 > 0 ? n2 - 1 : ncount - n2;
            faces[facecount+6] = v3 > 0 ? v3 - 1 : vertcount - v3;
            faces[facecount+7] = t3 > 0 ? t3 - 1 : uvcount - t3;
            faces[facecount+8] = n3 > 0 ? n3 - 1 : ncount - n3;
            facecount += 9;
            continue;
        }      

        printf("Failed to parse obj: %s\n", filename);
        return 1;
    }

    // write last submesh
    if (sm_wr)
    {
        sm.len = facecount - sm.startInd;
        if (smcount == smcap)
        {
            submeshes = (subMesh*)realloc(submeshes, 2 * smcap * sizeof(subMesh));
            smcap *= 2;
        }
        submeshes[smcount] = sm;
        smcount++;
    }

    data->facecount = facecount;
    data->faces = faces;
    data->normals = normals;
    data->ncount = ncount;
    data->submeshes = submeshes;
    data->submeshCount = smcount;
    data->uvs = uvs;
    data->uvcount = uvcount;
    data->pos = verts;
    data->pcount = vertcount;
    data->name = oname;
    data->mtllib = mtlib;

    return 0;
}

void Load_PartModelFromObj(EngineState* engineState, char* file, void** partStructure)
{
    assert(file);
    assert(partStructure);

    objData data = {};
    int res = _Load_Obj(file, &data);
    if (res) return;
    if (data.submeshCount == 0) {printf("!File %s contains no mesh definitions\n", file); return;}

    void* part = NULL;
    InstancedRenderData** rdatas = (InstancedRenderData**)calloc(data.submeshCount, sizeof(InstancedRenderData*));
    

    int ind = 0;
    for (int sm = 0; sm < data.submeshCount; sm++)
    {
        rdatas[sm] = Create_InstanceData();
        Mesh* mesh = Create_Mesh(engineState);
        mesh->vertices = (Vertex*)calloc(data.submeshes[sm].len / 3, sizeof(Vertex));
        mesh->vertexCap = data.submeshes[sm].len / 3;
        mesh->indices = (uint32_t*)calloc(data.submeshes[sm].len / 3, sizeof(uint32_t));
        mesh->indexCap = data.submeshes[sm].len / 3;

        mesh->deleteWithRenderInstance = 1;
        for (int i = 0; i < data.submeshes[sm].len; i+=9)
        {
            for (int k = 0; k < 3; k++)
            {
                ind = i + data.submeshes[sm].startInd;
                uint32_t pind = data.faces[ind + k*3]*3;
                uint32_t tind = data.faces[ind + k*3 + 1]*2;
                uint32_t nind = data.faces[ind + k*3 + 2]*3;
                mesh->vertices[mesh->vertexCount++] = (Vertex){data.pos[pind],      data.pos[pind+1],     data.pos[pind+2],
                                                                data.normals[nind], data.normals[nind+1], data.normals[nind+2],
                                                                data.uvs[tind],     data.uvs[tind+1]}; 
                mesh->indices[mesh->indexCount] = mesh->indexCount;
                mesh->indexCount++;
            }
        }
        rdatas[sm]->mesh = mesh;

        rdatas[sm]->material = GetMaterial_Test();

        //Debug_Print_Mesh(mesh);
        Debug_Validate_Mesh(mesh);
    }

    if (data.submeshCount == 1) 
    {
        part = Create_PartStructureSimpleMesh();
        ((PartStructureSimpleMesh*)part)->renderData = rdatas[0];
        free(rdatas);
    }
    else 
    {
        part = Create_PartStructureMultiMesh();
        ((PartStructureMultiMesh*)part)->renderDatas = rdatas;
        ((PartStructureMultiMesh*)part)->matCount = data.submeshCount;
    }
    *partStructure = part;
}

char fileNameBuf[400];

int Load_Models(EngineState* engineState)
{
    if (Init_ModelTable()) return printf("!Failed to init model table\n"), 1;

    DIR* dr;
    struct dirent* de;

    dr = opendir("assets/models");

    if (!dr) return printf("!Failed to open meshes directory\n"), 1;

    void* model = NULL;
    while (de = readdir(dr))
    {
        printf("reading dir: %s\n",de->d_name);
        if (de->d_name[0] == '.') continue;
        snprintf(fileNameBuf, 320, "assets/models/%s", de->d_name);
        Load_PartModelFromObj(engineState, fileNameBuf, &model);

        AddUpload_ModelTransformArrays(model);
        
        ModelTable_Set_Model(de->d_name, model);
    }
    closedir(dr);
    return 0;
}

int Unload_Models(EngineState* engineState)
{
    CleanUp_ModelTable();
    return 0;
}

