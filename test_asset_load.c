#include "dirent.h"
#include "stdio.h"

int main()
{
    DIR* dr;
    struct dirent* de;

    dr = opendir("assets/models");

    if (!dr) return printf("!Failed to open meshes directory\n"), 1;

    void* model = NULL;
    while (de = readdir(dr))
    {
        printf("%s\n",de->d_name);
        ///Load_PartModelFromObj(engineState, de->d_name, &model);
        //ModelTable_Set_Model();
    }
    closedir(dr);
    
}