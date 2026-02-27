#include <stdio.h>
#include <math.h>
#include "render_passes.h"
#include "image_utils.h"
#include "materials/materials.h"
#include "mesh_utils.h"
#include "transform_utils.h"
#include "math_util.h"
#include "tests.h"

// Testing
Mesh* testmesh = NULL;
InstancedRenderData testdata = {};
Ship* testship = NULL;

Mesh* get_testmesh(EngineState* engineState)
{
    Mesh* mesh = (Mesh*)calloc(1, sizeof(Mesh));
    mesh->engineState = engineState;
    mesh->vertices = (Vertex*)calloc(4, sizeof(Vertex));
    mesh->vertexCount = 4;
    mesh->indexCount = 6;
    mesh->indices = (uint32_t*)calloc(6, sizeof(uint32_t));
    mesh->vertices[0] = (Vertex){-1.0f,-0.5f,2.0f};
    mesh->vertices[1] = (Vertex){0.0f,0.5f,2.0f};
    mesh->vertices[2] = (Vertex){0.0f,-0.5f,2.0f};
    mesh->vertices[3] = (Vertex){1.0f,0.5f,2.0f};
    mesh->indices[0] = 0;
    mesh->indices[1] = 1;
    mesh->indices[2] = 2;
    mesh->indices[3] = 3;
    mesh->indices[4] = 1;
    mesh->indices[5] = 2;
    Mesh_UploadData(mesh);
    return mesh;
}

void _Testing(EngineState* engineState)
{
    Init_MaterialInstances(engineState);
    printf("Init mat instances\n");
    _Init_Cube();
    printf("Init cube\n");
    create_testshipbp(engineState);
    printf("Created test ship BP\n");
    testship = Create_ShipFromBP(get_testbp());
    printf("Created ship from BP\n");
}

int Run_MainLoop(EngineState* engineState, Uint64 frameCount)
{
    int frame_ind = frameCount % _BufferCount;

    int error = 0;
    if ((error = vkWaitForFences(engineState->device, 1, &engineState->frameData.fence[frame_ind], true, 10e9)) != VK_SUCCESS)
    {
        return -printf("Waiting for render fence failed: %d\n",error);
    }
    if (vkResetFences(engineState->device, 1, &engineState->frameData.fence[frame_ind]) != VK_SUCCESS)
    {
        return -printf("Reseting render fence failed\n");
    }
    uint32_t scImageIndex = 0;
    if (VK_SUCCESS != vkAcquireNextImageKHR(engineState->device, engineState->swapchainState.swapchain, 10e9, engineState->frameData.swapchainSemaphore[frame_ind], NULL, &scImageIndex))
    {
        return -printf("Failed to acquire next swapchain image\n");
    }

    // start command buffer
    VkCommandBuffer Cmnd = engineState->commandsHandle.commandBuffers[frame_ind];
    ImageData DrawImage = engineState->frameData.drawImage; // warning we are not changing layout in the original struct
    ImageData ScImage = (ImageData){
        .image = engineState->swapchainState.images[scImageIndex],
        .imageView = engineState->swapchainState.imageViews[scImageIndex],
        .layout = VK_IMAGE_LAYOUT_UNDEFINED,
        .imageExtent = engineState->swapchainState.extent};

    if (vkResetCommandBuffer(Cmnd, 0) != VK_SUCCESS) return -printf("!Failed to reset command buffer\n");

    VkCommandBufferBeginInfo bInfo = {};
    bInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bInfo.pNext = NULL;
    bInfo.pInheritanceInfo = NULL;
    bInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;   

    VkRenderingAttachmentInfo raInfo = Get_RenderAttachmentInfo(DrawImage.imageView);
    VkRenderingAttachmentInfo draInfo = Get_RenderAttachmentInfo(engineState->frameData.depthImage.imageView);
    VkRenderingInfo rInfo = Get_MainRenderPassInfo(frameCount, &raInfo, &draInfo, engineState);

    //Start first render pass
    if (vkBeginCommandBuffer(Cmnd, &bInfo) != VK_SUCCESS) return -printf("!Failed to begin command buffer\n");

    //Clear image
    Change_ImageLayout(Cmnd, &DrawImage, VK_IMAGE_LAYOUT_GENERAL);
    Clear_Image(Cmnd, DrawImage, (VkClearColorValue){0.4f,0.0f,0.0f,1.0f});

    Change_ImageLayout(Cmnd, &engineState->frameData.depthImage, VK_IMAGE_LAYOUT_GENERAL);
    Clear_Depth(Cmnd, engineState->frameData.depthImage, (VkClearDepthStencilValue){-999.0,0});

    Change_ImageLayout(Cmnd, &DrawImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    Change_ImageLayout(Cmnd, &engineState->frameData.depthImage, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
    //Main rendering
    vkCmdBeginRendering(Cmnd, &rInfo);


    //Bind_Material(Cmnd, gradient);

    VkExtent3D _drawExtent = engineState->frameData.drawImage.imageExtent;

    VkViewport viewport = {};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = _drawExtent.width;
	viewport.height = _drawExtent.height;
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;

	vkCmdSetViewport(Cmnd, 0, 1, &viewport);

	VkRect2D scissor = {};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent.width = _drawExtent.width;
	scissor.extent.height = _drawExtent.height;

	vkCmdSetScissor(Cmnd, 0, 1, &scissor);

    //vkCmdDraw(Cmnd, 3, 1, 0, 0);s
    mat4 mat;
    mat4 proj;
    Projection_Matrix(proj, (float)_drawExtent.height/(float)_drawExtent.width, 0.01f, 100.0f, GLM_PI * 0.3f);
    //printf("%f %f\n",proj[0][0],proj[1][1]);
    float phi = (float)frameCount / 400.0f;
    glm_mat4_copy((mat4){{cos(phi),0,-sin(phi),0},{0,1,0,0},{sin(phi),0,cos(phi),0},{0,0,15,1}}, mat);
    glm_mat4_mul(proj, mat, mat);
    //printf("Start render ship\n");
    Render_Ship(testship, mat);
    //printf("%f %f %f %f\n",vec[0],vec[1],vec[2],vec[3]);
    Render_InstancedMeshes(engineState, Cmnd);
    //RenderMesh(Cmnd, testmesh, basicmesh);

    vkCmdEndRendering(Cmnd);

    //Copy draw image to swapchain
    Change_ImageLayout(Cmnd, &DrawImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    Change_ImageLayout(Cmnd, &ScImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    Copy_ImageToImage(Cmnd, DrawImage, ScImage);
    
    //Prepare image for presentation
    Change_ImageLayout(Cmnd, &ScImage, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    vkEndCommandBuffer(Cmnd);


    Submit_CommandBuffer(engineState, frame_ind, Cmnd);

    VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = NULL;
	presentInfo.pSwapchains = &engineState->swapchainState.swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &engineState->frameData.computeSemaphore[frame_ind];
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &scImageIndex;

    vkQueueWaitIdle(engineState->queueHandles._Present); // Questionable solution for the following problem:
    /*
    1. first time command buffer is submitted (sets a fence)
    2. presentation starts (no fence is set)
    -- next frame (actually frame after the next because everything alternates between 2 frames)
    3. fence is waited for
    4(error). second time command buffer is submitted, BUT presentation from previous frame may not have finished
    */
	vkQueuePresentKHR(engineState->queueHandles._Present, &presentInfo);

    return 0;
}

int main(int argc, char** argv)
{
    int running = 1;

    EngineState* engineState;
    AllocInfo* allocInfo = (AllocInfo*)calloc(1, sizeof(AllocInfo));
    int res = 0;
    if ((res = Init_MainEngine(&engineState, &allocInfo))) return -printf("Main engine initialization failed, exit code: %d\n", res);

    printf("Starting main loop\n");
    SDL_Event event;

    _Testing(engineState);

    Uint64 frameCount = 0;
    while (running)
    {
        while (SDL_PollEvent(&event)) // process all input before main update
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                printf("Quit event\n");
                running = 0;
            }
        }

        if (Run_MainLoop(engineState, frameCount) != VK_SUCCESS) running = 0;
        frameCount++;
    }

    Delete_Ship(testship);
    Destroy_BlockModels();
    Destroy_MaterialInstances();
    printf("%ld\n",frameCount);
    printf("Closing\n");
    Cleanup_MainEngine(engineState, allocInfo);
}
