#include "raylib.h"

// compound data types
struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame)
{
    // update running time
    data.runningTime += deltaTime;

    if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        // update animation frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;

        if (data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }

    return data;
}

int main()
{

    // dimensions of the window drawn to the screen (Array with the size of 2)
    int windowDimensions[1];

    windowDimensions[0] = 512;
    windowDimensions[1] = 380;

    InitWindow(windowDimensions[0], windowDimensions[1], "Runner");

    // acceleration due to gravity (pixels / s) / s
    const int gravity{1000};

    // hero variables
    Texture2D hero = LoadTexture("textures/1.png");
    AnimData heroData;
    heroData.rec.width = hero.width / 3;
    heroData.rec.height = hero.height / 4;
    heroData.rec.x = 0;
    heroData.rec.y = 0;
    heroData.pos.x = windowDimensions[0] / 2 - heroData.rec.width / 2;
    heroData.pos.y = windowDimensions[1] - heroData.rec.height;
    heroData.frame = 0;
    heroData.updateTime = 1.0 / 12.0;
    heroData.runningTime = 0;

    // monster variables
    Texture2D monster = LoadTexture("textures/2.png");

    const int numberOfMonsters{10};
    AnimData monsters[numberOfMonsters]{};

    for (int i = 0; i < numberOfMonsters; i++)
    {
        monsters[i].rec.x = 0.0;
        monsters[i].rec.y = 0.0;
        monsters[i].rec.width = monster.width / 3;
        monsters[i].rec.height = monster.height / 4;
        monsters[i].pos.y = windowDimensions[1] - monster.height / 4;
        monsters[i].frame = 0;
        monsters[i].runningTime = 0.0;
        monsters[i].updateTime = 0.0;

        if (i > 0)
        {
            windowDimensions[0] += 300;
            monsters[i].pos.x = windowDimensions[0];
        }
        else
        {
            monsters[0].pos.x = windowDimensions[0];
        }
    }

    float finishLine{monsters[numberOfMonsters - 1].pos.x};

    int monsterVelocity{-200};

    // jump velocity (Pixels/second)
    const int jumpVelocity{-600};

    int heroVelocity{0};

    bool isInAir{false};

    bool collision{};

    // background variables
    Texture2D background = LoadTexture("textures/Background.png");
    float backgroundX{};

    SetTargetFPS(120);
    while (!WindowShouldClose())
    {

        // delta time
        const float deltaTime{GetFrameTime()};

        // updates running time
        heroData.runningTime += deltaTime;

        if (heroData.runningTime >= heroData.updateTime && !isInAir)
        {
            heroData = updateAnimData(heroData, deltaTime, 2);
        }

        for (int i = 0; i < numberOfMonsters; i++)
        {
            monsters[i] = updateAnimData(monsters[i], deltaTime, 2);
        }

        // start drawing
        BeginDrawing();
        ClearBackground(WHITE);

        backgroundX -= 20 * deltaTime;

        if (backgroundX <= -background.width)
        {
            backgroundX = 0;
        }

        // draw background
        Vector2 backgroundPos1{backgroundX, 0.0};
        DrawTextureEx(background, backgroundPos1, 0.0, 0.8, WHITE);
        Vector2 backgroundPos2{backgroundX + background.width - 220, 0.0};
        DrawTextureEx(background, backgroundPos2, 0.0, 0.8, WHITE);

        // check is currently on ground
        if (isOnGround(heroData, 380))
        {
            // rectangle is on the ground
            heroVelocity = 0;
            isInAir = false;
        }
        else
        {
            // rectangle is in the air apply gravity
            heroVelocity += gravity * deltaTime;
            isInAir = true;
        }

        // check for jumping
        if (IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            heroVelocity += jumpVelocity;
        }

        for (int i = 0; i < numberOfMonsters; i++)
        {
            // update monsters position
            monsters[i].pos.x += monsterVelocity * deltaTime;
        }

        // update finishline position
        finishLine += monsterVelocity * deltaTime;

        // update hero position
        heroData.pos.y += heroVelocity * deltaTime;

        // detect collisions
        for (AnimData monster : monsters)
        {
            float padding{20};
            Rectangle monsterRec{
                monster.pos.x + padding,
                monster.pos.y + padding,
                monster.rec.width - 2 * padding,
                monster.rec.height - 2 * padding};

            Rectangle heroRec{
                heroData.pos.x + padding,
                heroData.pos.y + padding,
                heroData.rec.width - 2 * padding,
                heroData.rec.height - 2 * padding};

            if (CheckCollisionRecs(monsterRec, heroRec))
            {
                collision = true;
            }
        }

        if (collision == false && heroData.pos.x <= finishLine)
        {
            for (int i = 0; i < numberOfMonsters; i++)
            {
                // draw monster
                DrawTextureRec(monster, monsters[i].rec, monsters[i].pos, WHITE);
            }

            // draw hero
            DrawTextureRec(hero, heroData.rec, heroData.pos, WHITE);
        }
        else if (heroData.pos.x >= finishLine)
        {
            DrawText("You Win!", 120, 190, 40, RED);
        }
        else if (collision == true)
        {
            DrawText("You Lose!", 120, 190, 40, RED);
        }
        // stop drawing
        EndDrawing();
    }
    UnloadTexture(hero);
    UnloadTexture(monster);
    UnloadTexture(background);
    CloseWindow();
}