#include "ship.h"

void Set_OwnPartTransform(Ship* ship, Part* part, mat4 TRS)
{
    glm_mat4_copy(TRS, ship->modelTransform.matrixes[part->matrixIndex]);
    //self.mdoeltransform.matrixes[part.matrixindex] = ...
}

