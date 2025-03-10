#include "global.h"
#include "evolution_graphics.h"
#include "sprite.h"
#include "trig.h"
#include "random.h"
#include "decompress.h"
#include "task.h"
#include "sound.h"
#include "constants/songs.h"
#include "palette.h"
#include "constants/rgb.h"

static void SpriteCB_Sparkle_Dummy(struct Sprite* sprite);

static void Task_Sparkles_SpiralUpward_Init(u8 taskId);
static void Task_Sparkles_SpiralUpward(u8 taskId);
static void Task_Sparkles_SpiralUpward_End(u8 taskId);

static void Task_Sparkles_ArcDown_Init(u8 taskId);
static void Task_Sparkles_ArcDown(u8 taskId);
static void Task_Sparkles_ArcDown_End(u8 taskId);

static void Task_Sparkles_CircleInward_Init(u8 taskId);
static void Task_Sparkles_CircleInward(u8 taskId);
static void Task_Sparkles_CircleInward_End(u8 taskId);

static void Task_Sparkles_SprayAndFlash_Init(u8 taskId);
static void Task_Sparkles_SprayAndFlash(u8 taskId);
static void Task_Sparkles_SprayAndFlashTrade_Init(u8 taskId);
static void Task_Sparkles_SprayAndFlashTrade(u8 taskId);
static void Task_Sparkles_SprayAndFlash_End(u8 taskId);

static void Task_CycleEvolutionMonSprite_Init(u8 taskId);
static void Task_CycleEvolutionMonSprite_TryEnd(u8 taskId);
static void Task_CycleEvolutionMonSprite_UpdateSize(u8 taskId);
static void EndOnPreEvoMon(u8 taskId);
static void EndOnPostEvoMon(u8 taskId);

#define TAG_SPARKLE 1001

static const u16 sEvoSparkle_Pal[] = INCBIN_U16("graphics/misc/evo_sparkle.gbapal");
static const u32 sEvoSparkle_Gfx[] = INCBIN_U32("graphics/misc/evo_sparkle.4bpp.lz");

static const struct CompressedSpriteSheet sEvoSparkleSpriteSheets[] =
{
    {sEvoSparkle_Gfx, 0x20, TAG_SPARKLE},
    {NULL, 0, 0}
};

static const struct SpritePalette sEvoSparkleSpritePals[] =
{
    {sEvoSparkle_Pal, TAG_SPARKLE},
    {NULL, 0}
};

static const struct OamData sOamData_EvoSparkle =
{
    .y = DISPLAY_HEIGHT,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = 0,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(8x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(8x8),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_EvoSparkle[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_EvoSparkle[] =
{
    sSpriteAnim_EvoSparkle,
};

static const struct SpriteTemplate sEvoSparkleSpriteTemplate =
{
    .tileTag = TAG_SPARKLE,
    .paletteTag = TAG_SPARKLE,
    .oam = &sOamData_EvoSparkle,
    .anims = sSpriteAnimTable_EvoSparkle,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_Sparkle_Dummy
};

static const s16 sEvoSparkleMatrices[] =
{
    0x3C0, 0x380, 0x340, 0x300, 0x2C0, 0x280, 0x240, 0x200, 0x1C0,
    0x180, 0x140, 0x100, -4, 0x10, -3, 0x30, -2, 0x50,
    -1, 0x70, 0x1, 0x70, 0x2, 0x50, 0x3, 0x30, 0x4, 0x10
};

static void SpriteCB_Sparkle_Dummy(struct Sprite *sprite)
{

}

static void SetEvoSparklesMatrices(void)
{
    u16 i;
    for (i = 0; i < 12; i++)
    {
        SetOamMatrix(20 + i, sEvoSparkleMatrices[i], 0, 0, sEvoSparkleMatrices[i]);
    }
}

static void SpriteCB_Sparkle_SpiralUpward(struct Sprite* sprite)
{
    if (sprite->pos1.y > 8)
    {
        u8 matrixNum;

        sprite->pos1.y = 88 - (sprite->data[7] * sprite->data[7]) / 80;
        sprite->pos2.y = Sin((u8)(sprite->data[6]), sprite->data[5]) / 4;
        sprite->pos2.x = Cos((u8)(sprite->data[6]), sprite->data[5]);
        sprite->data[6] += 4;
        if (sprite->data[7] & 1)
            sprite->data[5]--;
        sprite->data[7]++;
        if (sprite->pos2.y > 0)
            sprite->subpriority = 1;
        else
            sprite->subpriority = 20;
        matrixNum = sprite->data[5] / 4 + 20;
        if (matrixNum > 31)
            matrixNum = 31;
        sprite->oam.matrixNum = matrixNum;
    }
    else
        DestroySprite(sprite);
}

static void CreateSparkle_SpiralUpward(u8 arg0)
{
    u8 spriteID = CreateSprite(&sEvoSparkleSpriteTemplate, 120, 88, 0);
    if (spriteID != MAX_SPRITES)
    {
        gSprites[spriteID].data[5] = 48;
        gSprites[spriteID].data[6] = arg0;
        gSprites[spriteID].data[7] = 0;
        gSprites[spriteID].oam.affineMode = ST_OAM_AFFINE_NORMAL;
        gSprites[spriteID].oam.matrixNum = 31;
        gSprites[spriteID].callback = SpriteCB_Sparkle_SpiralUpward;
    }
}

static void SpriteCB_Sparkle_ArcDown(struct Sprite* sprite)
{
    if (sprite->pos1.y < 88)
    {
        sprite->pos1.y = 8 + (sprite->data[7] * sprite->data[7]) / 5;
        sprite->pos2.y = Sin((u8)(sprite->data[6]), sprite->data[5]) / 4;
        sprite->pos2.x = Cos((u8)(sprite->data[6]), sprite->data[5]);
        sprite->data[5] = 8 + Sin((u8)(sprite->data[7] * 4), 40);
        sprite->data[7]++;
    }
    else
        DestroySprite(sprite);
}

static void CreateSparkle_ArcDown(u8 arg0)
{
    u8 spriteID = CreateSprite(&sEvoSparkleSpriteTemplate, 120, 8, 0);
    if (spriteID != MAX_SPRITES)
    {
        gSprites[spriteID].data[5] = 8;
        gSprites[spriteID].data[6] = arg0;
        gSprites[spriteID].data[7] = 0;
        gSprites[spriteID].oam.affineMode = ST_OAM_AFFINE_NORMAL;
        gSprites[spriteID].oam.matrixNum = 25;
        gSprites[spriteID].subpriority = 1;
        gSprites[spriteID].callback = SpriteCB_Sparkle_ArcDown;
    }
}

static void SpriteCB_Sparkle_CircleInward(struct Sprite* sprite)
{
    if (sprite->data[5] > 8)
    {
        sprite->pos2.y = Sin((u8)(sprite->data[6]), sprite->data[5]);
        sprite->pos2.x = Cos((u8)(sprite->data[6]), sprite->data[5]);
        sprite->data[5] -= sprite->data[3];
        sprite->data[6] += 4;
    }
    else
        DestroySprite(sprite);
}

static void CreateSparkle_CircleInward(u8 arg0, u8 arg1)
{
    u8 spriteID = CreateSprite(&sEvoSparkleSpriteTemplate, 120, 56, 0);
    if (spriteID != MAX_SPRITES)
    {
        gSprites[spriteID].data[3] = arg1;
        gSprites[spriteID].data[5] = 120;
        gSprites[spriteID].data[6] = arg0;
        gSprites[spriteID].data[7] = 0;
        gSprites[spriteID].oam.affineMode = ST_OAM_AFFINE_NORMAL;
        gSprites[spriteID].oam.matrixNum = 31;
        gSprites[spriteID].subpriority = 1;
        gSprites[spriteID].callback = SpriteCB_Sparkle_CircleInward;
    }
}

static void SpriteCB_Sparkle_Spray(struct Sprite* sprite)
{
    if (!(sprite->data[7] & 3))
        sprite->pos1.y++;
    if (sprite->data[6] < 128)
    {
        u8 matrixNum;

        sprite->pos2.y = -Sin((u8)(sprite->data[6]), sprite->data[5]);
        sprite->pos1.x = 120 + (sprite->data[3] * sprite->data[7]) / 3;
        sprite->data[6]++;
        matrixNum = 31 - (sprite->data[6] * 12 / 128);
        if (sprite->data[6] > 64)
            sprite->subpriority = 1;
        else
        {
            sprite->invisible = FALSE;
            sprite->subpriority = 20;
            if (sprite->data[6] > 112 && sprite->data[6] & 1)
                sprite->invisible = TRUE;
        }
        if (matrixNum < 20)
            matrixNum = 20;
        sprite->oam.matrixNum = matrixNum;
        sprite->data[7]++;
    }
    else
        DestroySprite(sprite);
}

static void CreateSparkle_Spray(u8 id)
{
    u8 spriteID = CreateSprite(&sEvoSparkleSpriteTemplate, 120, 56, 0);
    if (spriteID != MAX_SPRITES)
    {
        gSprites[spriteID].data[3] = 3 - (Random() % 7);
        gSprites[spriteID].data[5] = 48 + (Random() & 0x3F);
        gSprites[spriteID].data[7] = 0;
        gSprites[spriteID].oam.affineMode = ST_OAM_AFFINE_NORMAL;
        gSprites[spriteID].oam.matrixNum = 31;
        gSprites[spriteID].subpriority = 20;
        gSprites[spriteID].callback = SpriteCB_Sparkle_Spray;
    }
}

void LoadEvoSparkleSpriteAndPal(void)
{
    LoadCompressedSpriteSheetUsingHeap(&sEvoSparkleSpriteSheets[0]);
    LoadSpritePalettes(sEvoSparkleSpritePals);
}

#define tPalNum data[1]
#define tTimer  data[15]

u8 EvolutionSparkles_SpiralUpward(u16 palNum)
{
    u8 taskId = CreateTask(Task_Sparkles_SpiralUpward_Init, 0);
    gTasks[taskId].tPalNum = palNum;
    return taskId;
}

static void Task_Sparkles_SpiralUpward_Init(u8 taskId)
{
    SetEvoSparklesMatrices();
    gTasks[taskId].tTimer = 0;
    BeginNormalPaletteFade(3 << gTasks[taskId].tPalNum, 0xA, 0, 0x10, RGB_WHITE);
    gTasks[taskId].func = Task_Sparkles_SpiralUpward;
    PlaySE(SE_M_MEGA_KICK); // 'Charging up' sound for the sparkles as they spiral upwards
}

static void Task_Sparkles_SpiralUpward(u8 taskId)
{
    if (gTasks[taskId].tTimer < 64)
    {
        if (!(gTasks[taskId].tTimer & 7))
        {
            u8 i;
            for (i = 0; i < 4; i++)
                CreateSparkle_SpiralUpward((0x78 & gTasks[taskId].tTimer) * 2 + i * 64);
        }
        gTasks[taskId].tTimer++;
    }
    else
    {
        gTasks[taskId].tTimer = 96;
        gTasks[taskId].func = Task_Sparkles_SpiralUpward_End;
    }
}

static void Task_Sparkles_SpiralUpward_End(u8 taskId)
{
    if (gTasks[taskId].tTimer != 0)
        gTasks[taskId].tTimer--;
    else
        DestroyTask(taskId);
}

u8 EvolutionSparkles_ArcDown(void)
{
    return CreateTask(Task_Sparkles_ArcDown_Init, 0);
}

static void Task_Sparkles_ArcDown_Init(u8 taskId)
{
    SetEvoSparklesMatrices();
    gTasks[taskId].tTimer = 0;
    gTasks[taskId].func = Task_Sparkles_ArcDown;
    PlaySE(SE_M_BUBBLE_BEAM2);
}

static void Task_Sparkles_ArcDown(u8 taskId)
{
    if (gTasks[taskId].tTimer < 96)
    {
        if (gTasks[taskId].tTimer < 6)
        {
            u8 i;
            for (i = 0; i < 9; i++)
                CreateSparkle_ArcDown(i * 16);
        }
        gTasks[taskId].tTimer++;
    }
    else
        gTasks[taskId].func = Task_Sparkles_ArcDown_End;
}

static void Task_Sparkles_ArcDown_End(u8 taskId)
{
    DestroyTask(taskId);
}

u8 EvolutionSparkles_CircleInward(void)
{
    return CreateTask(Task_Sparkles_CircleInward_Init, 0);
}

static void Task_Sparkles_CircleInward_Init(u8 taskId)
{
    SetEvoSparklesMatrices();
    gTasks[taskId].tTimer = 0;
    gTasks[taskId].func = Task_Sparkles_CircleInward;
    PlaySE(SE_SHINY);
}

static void Task_Sparkles_CircleInward(u8 taskId)
{
    if (gTasks[taskId].tTimer < 48)
    {
        if (gTasks[taskId].tTimer == 0)
        {
            u8 i;
            for (i = 0; i < 16; i++)
                CreateSparkle_CircleInward(i * 16, 4);
        }
        if (gTasks[taskId].tTimer == 32)
        {
            u8 i;
            for (i = 0; i < 16; i++)
                CreateSparkle_CircleInward(i * 16, 8);
        }
        gTasks[taskId].tTimer++;
    }
    else
        gTasks[taskId].func = Task_Sparkles_CircleInward_End;
}

static void Task_Sparkles_CircleInward_End(u8 taskId)
{
    DestroyTask(taskId);
}

#define tSpecies data[2] // Never read

u8 EvolutionSparkles_SprayAndFlash(u16 species)
{
    u8 taskId = CreateTask(Task_Sparkles_SprayAndFlash_Init, 0);
    gTasks[taskId].tSpecies = species;
    return taskId;
}

static void Task_Sparkles_SprayAndFlash_Init(u8 taskId)
{
    SetEvoSparklesMatrices();
    gTasks[taskId].tTimer = 0;
    CpuSet(&gPlttBufferFaded[0x20], &gPlttBufferUnfaded[0x20], 0x30);
    BeginNormalPaletteFade(0xFFF9041C, 0, 0, 0x10, RGB_WHITE); // was 0xFFF9001C in R/S
    gTasks[taskId].func = Task_Sparkles_SprayAndFlash;
    PlaySE(SE_M_PETAL_DANCE);
}

static void Task_Sparkles_SprayAndFlash(u8 taskId)
{
    if (gTasks[taskId].tTimer < 128)
    {
        u8 i;
        switch (gTasks[taskId].tTimer)
        {
        default:
            if (gTasks[taskId].tTimer < 50)
                CreateSparkle_Spray(Random() & 7);
            break;
        case 0:
            for (i = 0; i < 8; i++)
                CreateSparkle_Spray(i);
            break;
        case 32:
            BeginNormalPaletteFade(0xFFFF041C, 0x10, 0x10, 0, RGB_WHITE); // was 0xFFF9001C in R/S
            break;
        }
        gTasks[taskId].tTimer++;
    }
    else
        gTasks[taskId].func = Task_Sparkles_SprayAndFlash_End;
}

static void Task_Sparkles_SprayAndFlash_End(u8 taskId)
{
    if (!gPaletteFade.active)
        DestroyTask(taskId);
}

// Separate from EvolutionSparkles_SprayAndFlash for difference in fade color
u8 EvolutionSparkles_SprayAndFlash_Trade(u16 species)
{
    u8 taskId = CreateTask(Task_Sparkles_SprayAndFlashTrade_Init, 0);
    gTasks[taskId].tSpecies = species;
    return taskId;
}

static void Task_Sparkles_SprayAndFlashTrade_Init(u8 taskId)
{
    SetEvoSparklesMatrices();
    gTasks[taskId].tTimer = 0;
    CpuSet(&gPlttBufferFaded[0x20], &gPlttBufferUnfaded[0x20], 0x30);
    BeginNormalPaletteFade(0xFFF90400, 0, 0, 0x10, RGB_WHITE); // was 0xFFFF0001 in R/S
    gTasks[taskId].func = Task_Sparkles_SprayAndFlashTrade;
    PlaySE(SE_M_PETAL_DANCE);
}

static void Task_Sparkles_SprayAndFlashTrade(u8 taskId)
{
    if (gTasks[taskId].tTimer < 128)
    {
        u8 i;
        switch (gTasks[taskId].tTimer)
        {
        default:
            if (gTasks[taskId].tTimer < 50)
                CreateSparkle_Spray(Random() & 7);
            break;
        case 0:
            for (i = 0; i < 8; i++)
                CreateSparkle_Spray(i);
            break;
        case 32:
            BeginNormalPaletteFade(0xFFFF0400, 0x10, 0x10, 0, RGB_WHITE); // was 0xFFFF0001 in R/S
            break;
        }
        gTasks[taskId].tTimer++;
    }
    else
        gTasks[taskId].func = Task_Sparkles_SprayAndFlash_End;
}

#undef tTimer
#undef tSpecies

static void SpriteCB_EvolutionMonSprite(struct Sprite* sprite)
{

}

#define tPreEvoSpriteId     data[1]
#define tPostEvoSpriteId    data[2]
#define tPreEvoScale        data[3]
#define tPostEvoScale       data[4]
#define tShowingPostEvo     data[5]
#define tScaleSpeed         data[6]
#define tEvoStopped         data[8]

#define MATRIX_PRE_EVO  30
#define MATRIX_POST_EVO 31

#define MON_MAX_SCALE 256
#define MON_MIN_SCALE 16

// Alternately grow/shrink the pre-evolution and post-evolution mon sprites
u8 CycleEvolutionMonSprite(u8 preEvoSpriteId, u8 postEvoSpriteId)
{
    u16 i;
    u16 stack[16];
    u8 taskId;
    s32 toDiv;

    for (i = 0; i < ARRAY_COUNT(stack); i++)
        stack[i] = 0x7FFF;

    taskId = CreateTask(Task_CycleEvolutionMonSprite_Init, 0);
    gTasks[taskId].tPreEvoSpriteId = preEvoSpriteId;
    gTasks[taskId].tPostEvoSpriteId = postEvoSpriteId;
    gTasks[taskId].tPreEvoScale = MON_MAX_SCALE;
    gTasks[taskId].tPostEvoScale = MON_MIN_SCALE;

    toDiv = 65536;
    SetOamMatrix(MATRIX_PRE_EVO, MON_MAX_SCALE, 0, 0, MON_MAX_SCALE);
    SetOamMatrix(MATRIX_POST_EVO, toDiv / gTasks[taskId].tPostEvoScale, 0, 0, toDiv / gTasks[taskId].tPostEvoScale);

    // Set up sprites but do not cycle them
    gSprites[preEvoSpriteId].callback = SpriteCB_EvolutionMonSprite;
    gSprites[preEvoSpriteId].oam.affineMode = ST_OAM_AFFINE_NORMAL;
    gSprites[preEvoSpriteId].oam.matrixNum = MATRIX_PRE_EVO;
    gSprites[preEvoSpriteId].invisible = FALSE;
    CpuSet(stack, &gPlttBufferFaded[0x100 + (gSprites[preEvoSpriteId].oam.paletteNum * 16)], 16);

    gSprites[postEvoSpriteId].callback = SpriteCB_EvolutionMonSprite;
    gSprites[postEvoSpriteId].oam.affineMode = ST_OAM_AFFINE_NORMAL;
    gSprites[postEvoSpriteId].oam.matrixNum = MATRIX_POST_EVO;
    gSprites[postEvoSpriteId].invisible = FALSE;
    CpuSet(stack, &gPlttBufferFaded[0x100 + (gSprites[postEvoSpriteId].oam.paletteNum * 16)], 16);

    // Set task properties but don't cycle sprites
    gTasks[taskId].tEvoStopped = FALSE;
    return taskId;
}

static void Task_CycleEvolutionMonSprite_Init(u8 taskId)
{
    gTasks[taskId].tShowingPostEvo = FALSE;
    gTasks[taskId].tScaleSpeed = 8;
    gTasks[taskId].func = Task_CycleEvolutionMonSprite_TryEnd;
}

// Try to end sprite cycling without ever cycling the sprite
// Simply ends on the pre-evo sprite if evo hasn't stopped yet
static void Task_CycleEvolutionMonSprite_TryEnd(u8 taskId)
{
    // Directly check if the evolution has been stopped and handle accordingly
    if (gTasks[taskId].tEvoStopped)
        EndOnPreEvoMon(taskId);
    else
        // Attempt to end directly, skipping the cycling of sprites
        EndOnPostEvoMon(taskId);
}

static void Task_CycleEvolutionMonSprite_UpdateSize(u8 taskId)
{
    if (gTasks[taskId].tEvoStopped)
        gTasks[taskId].func = EndOnPreEvoMon;
    else
    {
        u16 oamMatrixArg;
        u8 numSpritesFinished = 0;
        if (!gTasks[taskId].tShowingPostEvo)
        {
            // Set pre-evo sprite growth
            if (gTasks[taskId].tPreEvoScale < MON_MAX_SCALE - gTasks[taskId].tScaleSpeed)
                gTasks[taskId].tPreEvoScale += gTasks[taskId].tScaleSpeed;
            else
            {
                gTasks[taskId].tPreEvoScale = MON_MAX_SCALE;
                numSpritesFinished++;
            }

            // Set post-evo sprite shrink
            if (gTasks[taskId].tPostEvoScale > MON_MIN_SCALE + gTasks[taskId].tScaleSpeed)
                gTasks[taskId].tPostEvoScale  -= gTasks[taskId].tScaleSpeed;
            else
            {
                gTasks[taskId].tPostEvoScale = MON_MIN_SCALE;
                numSpritesFinished++;
            }
        }
        else
        {
            // Set post-evo sprite growth
            if (gTasks[taskId].tPostEvoScale < MON_MAX_SCALE - gTasks[taskId].tScaleSpeed)
                gTasks[taskId].tPostEvoScale += gTasks[taskId].tScaleSpeed;
            else
            {
                gTasks[taskId].tPostEvoScale = MON_MAX_SCALE;
                numSpritesFinished++;
            }

            // Set pre-evo sprite shrink
            if (gTasks[taskId].tPreEvoScale > MON_MIN_SCALE + gTasks[taskId].tScaleSpeed)
                gTasks[taskId].tPreEvoScale  -= gTasks[taskId].tScaleSpeed;
            else
            {
                gTasks[taskId].tPreEvoScale = MON_MIN_SCALE;
                numSpritesFinished++;
            }
        }

        // Grow/shrink pre-evo sprite
        oamMatrixArg = 65536 / gTasks[taskId].tPreEvoScale;
        SetOamMatrix(MATRIX_PRE_EVO, oamMatrixArg, 0, 0, oamMatrixArg);

        // Grow/shrink post-evo sprite
        oamMatrixArg = 65536 / gTasks[taskId].tPostEvoScale;
        SetOamMatrix(MATRIX_POST_EVO, oamMatrixArg, 0, 0, oamMatrixArg);

        // Both sprites have reached their size extreme
        if (numSpritesFinished == 2)
            gTasks[taskId].func = Task_CycleEvolutionMonSprite_TryEnd;
    }
}

static void EndOnPostEvoMon(u8 taskId)
{
    gSprites[gTasks[taskId].tPreEvoSpriteId].oam.affineMode = ST_OAM_AFFINE_OFF;
    gSprites[gTasks[taskId].tPreEvoSpriteId].oam.matrixNum = 0;
    gSprites[gTasks[taskId].tPreEvoSpriteId].invisible = TRUE;

    gSprites[gTasks[taskId].tPostEvoSpriteId].oam.affineMode = ST_OAM_AFFINE_OFF;
    gSprites[gTasks[taskId].tPostEvoSpriteId].oam.matrixNum = 0;
    gSprites[gTasks[taskId].tPostEvoSpriteId].invisible = FALSE;

    DestroyTask(taskId);
}

static void EndOnPreEvoMon(u8 taskId)
{
    gSprites[gTasks[taskId].tPreEvoSpriteId].oam.affineMode = ST_OAM_AFFINE_OFF;
    gSprites[gTasks[taskId].tPreEvoSpriteId].oam.matrixNum = 0;
    gSprites[gTasks[taskId].tPreEvoSpriteId].invisible = FALSE;

    gSprites[gTasks[taskId].tPostEvoSpriteId].oam.affineMode = ST_OAM_AFFINE_OFF;
    gSprites[gTasks[taskId].tPostEvoSpriteId].oam.matrixNum = 0;
    gSprites[gTasks[taskId].tPostEvoSpriteId].invisible = TRUE;

    DestroyTask(taskId);
}
