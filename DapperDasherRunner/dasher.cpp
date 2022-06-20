#include "raylib.h"

struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int animationFrame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowsHeight)   //true if is on ground
{
    return data.pos.y >= windowsHeight - data.rec.height;
}

AnimData updateAnimationFrame(AnimData data, float dT, int maxFrame)    //update Animation
{
    data.runningTime += dT;   //update runningTime
    if(data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        //update animation frame
        data.rec.x = data.animationFrame * data.rec.width;
        data.animationFrame++;
 
        if(data.animationFrame > maxFrame)
        data.animationFrame = 0;            
    }
    return data;
}

int main()
{
    // array with window dimensions
    int windowDimensions[2];
    windowDimensions[0] = 800;
    windowDimensions[1] = 450;

    // initialize the window
    InitWindow(windowDimensions[0], windowDimensions[1], "Dapper Dasher!");

    // acceleration due to gravity (pixels/s)/s
    const int gravity{1'000};

    // nebula variables
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    const int sizeOfNebulae{15};
    AnimData nebulae[sizeOfNebulae]{};

    for (int i = 0; i < sizeOfNebulae; i++)
    {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width/8;
        nebulae[i].rec.height = nebula.height/8;
        nebulae[i].pos.x =  windowDimensions[0] + i * 350;
        nebulae[i].pos.y = windowDimensions[1] - nebula.height/8;
        nebulae[i].animationFrame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = 1.0/12.0;
    }

    float finishLine{nebulae[sizeOfNebulae -1].pos.x}; //win if collision

    // nebula X velocity (pixels/second)
    int nebVel{-200};

    // scarfy variables
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData
    {
        {0, 0, scarfy.width/6, scarfy.height},
        {windowDimensions[0]/2 - scarfyData.rec.width/2, windowDimensions[1] - scarfyData.rec.height},
        0,
        1.0/12.0,
        0.0
    };
    
    const int jumpVel{-600};    // jump velocity (pixels/second)
    int scarfyVelocity{0};      //scarfy velocity

    //far buildings
    Texture2D backGround = LoadTexture("textures/far-buildings.png");
    float backGroundX{};
    Vector2 backGroundPos[2]{};
    //BackBuildings
    Texture2D backBuildings = LoadTexture("textures/back-buildings.png");
    float backBuildingsX{};
    Vector2 backBuildingsPos[2]{};
    //ForeGround
    Texture2D foreGround = LoadTexture("textures/foreground.png");
    float foreGroundX{};
    Vector2 foreGroundPos[2]{};

    bool collision {};   //true if collision

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        // delta time (time since last frame)
        const float dT{GetFrameTime()};

        // start drawing
        BeginDrawing();
        ClearBackground(WHITE);

        //scroll backGround
        backGroundX -= 20 * dT; 

        if(backGroundX <= -backGround.width * 4)
            backGroundX = 0.0; 

        //scroll backBuildings
        backBuildingsX -= 40 * dT; 

        if(backBuildingsX <= -backBuildings.width * 4)
            backBuildingsX = 0.0;

        //scroll foreGround
        foreGroundX -= 100 * dT;  

        if(foreGroundX <= -foreGround.width * 4)
            foreGroundX = 0.0;
              
        //draw background
        for (int i = 0; i < 2; i++)
        {                               
            DrawTextureEx(backGround, backGroundPos[i], 0.0, 4.0, WHITE);   //draw backGround
            backGroundPos[i] = {backGroundX + ((backGround.width * 4) * i), 0.0};   //update backgroundPos
        }
        //draw backbuildings
        for (int i = 0; i < 2; i++)
        {
            DrawTextureEx(backBuildings, backBuildingsPos[i], 0.0, 4.0, WHITE); //draw backbuildings
            backBuildingsPos[i] = {backBuildingsX + ((backBuildings.width * 4) * i), 0.0};  //update backBuildingsPos            
        }
        //draw foreGround
        for (int i = 0; i < 2; i++)
        {
            DrawTextureEx(foreGround, foreGroundPos[i], 0.0, 4.0, WHITE);    //draw foreGround
            foreGroundPos[i] = {foreGroundX + ((foreGround.width * 4) * i), 0.0};   //update foreGroundPos
        }
        
        //ground check
        if(isOnGround(scarfyData, windowDimensions[1]))
        {
            //scarfy is on the ground
            scarfyVelocity = 0;
        }
        else
        {
            //scarfy is in the air
            scarfyVelocity += gravity * dT;
        }

        // jump check
        if (IsKeyPressed(KEY_SPACE) && isOnGround(scarfyData, windowDimensions[1]))
        {
            scarfyVelocity += jumpVel;
        }

        // update the position of each nebula
        for (int i = 0; i < sizeOfNebulae; i++) 
        {
            nebulae[i].pos.x += nebVel * dT;
        }

        //finish line update
        finishLine += nebVel * dT;        

        // update scarfy position
        scarfyData.pos.y += scarfyVelocity * dT;

        for (AnimData nebula : nebulae)     //check collision between nebula and scarfy
        {
            float pad{50};
            Rectangle nebRec    //initialize nebRec
            {
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2 * pad,
                nebula.rec.height - 2 * pad
            };
            Rectangle scarfyRec
            {
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height
            };
            //check if collision
            if(CheckCollisionRecs(nebRec, scarfyRec))
            {
                collision = true;
            }
        }

        //scarfy animation
        if(isOnGround(scarfyData, windowDimensions[1]))
        {
            scarfyData = updateAnimationFrame(scarfyData, dT, 5);
        }       

        //update animations frames for nebula
        for (int i = 0; i < sizeOfNebulae; i++)
        {
            nebulae[i] = updateAnimationFrame(nebulae[i], dT, 7);          
        }

        if(collision)
        {
           DrawText("You Lose!", 150, 225, 100, RED);
        }
        else if (scarfyData.pos.x > finishLine)
        {
            DrawText("You Win!", 150, 225, 100, GREEN);

            for (int i = 0; i < sizeOfNebulae; i++)
            {
                // draw nebula
                   DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }
            // draw scarfy
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);  
        } 
        else
        {
            for (int i = 0; i < sizeOfNebulae; i++)
            {
                // draw nebula
                   DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }
            // draw scarfy
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);            
        }                
        EndDrawing();
    }
    UnloadTexture(scarfy);
    UnloadTexture(nebula);

    UnloadTexture(backGround);
    UnloadTexture(backBuildings);
    UnloadTexture(foreGround);

    CloseWindow();
}