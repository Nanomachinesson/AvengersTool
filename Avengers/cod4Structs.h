#pragma once
#ifndef CODSTRUCTS_H
#define CODSTRUCTS_H

#include "windows.h"
#include "vectors.h"

#define PM_NORMAL				0x0
#define PM_NORMAL_LINKED		0x1
#define PM_NOCLIP				0x2
#define PM_UFO					0x3
#define PM_SPEC					0x4
#define PM_INTERMISSION			0x5
#define PM_LASTSTAND			0x6
#define PM_DEAD					0x7
#define PM_DEAD_LINKED			0x8
#define PMF_FOLLOW  			0x2
#define PMF_JUMPING             0x4000

/*https://github.com/kejjjjj/1_kej/blob/master/1_kej/dvar.hpp*/

enum dvar_flags : std::uint16_t
{
    none = 0x0,
    saved = 0x1,
    user_info = 0x2, // sent to server on connect or change
    server_info = 0x4, // sent in response to front end requests
    replicated = 0x8,
    write_protected = 0x10,
    latched = 0x20,
    read_only = 0x40,
    cheat_protected = 0x80,
    temp = 0x100,
    no_restart = 0x400, // do not clear when a cvar_restart is issued
    user_created = 0x4000, // created by a set command
};

union DvarLimits
{
    struct
    {
        int stringCount;
        const char** strings;
    } enumeration;

    struct
    {
        int min;
        int max;
    } integer;

    struct
    {
        float min;
        float max;
    } value;

    struct
    {
        float min;
        float max;
    } vector;
};

union DvarValue
{
    bool enabled;
    int integer;
    unsigned int unsignedInt;
    float value;
    float vector[4];
    const char* string;
    char color[4];
};

enum DvarType
{
    DVAR_TYPE_BOOL = 0x0,
    DVAR_TYPE_FLOAT = 0x1,
    DVAR_TYPE_FLOAT_2 = 0x2,
    DVAR_TYPE_FLOAT_3 = 0x3,
    DVAR_TYPE_FLOAT_4 = 0x4,
    DVAR_TYPE_INT = 0x5,
    DVAR_TYPE_ENUM = 0x6,
    DVAR_TYPE_STRING = 0x7,
    DVAR_TYPE_COLOR = 0x8,
    DVAR_TYPE_COUNT = 0x9,
};

enum class dvar_type : std::int8_t
{
    boolean = 0,
    value = 1,
    vec2 = 2,
    vec3 = 3,
    vec4 = 4,
    integer = 5,
    enumeration = 6,
    string = 7,
    color = 8,
    rgb = 9 // Color without alpha
};

struct cvar_t
{
    const char* name;
    const char* description;
    //unsigned __int16 flags;
    dvar_flags flags;
    //char type;
    dvar_type type;
    bool modified;
    DvarValue current;
    DvarValue latched;
    DvarValue reset;
    DvarLimits domain;
    bool(__cdecl* domainFunc)(cvar_t*, DvarValue);
    cvar_t* hashNext;
};

enum team_t
{
    TEAM_FREE = 0x0,
    TEAM_AXIS = 0x1,
    TEAM_ALLIES = 0x2,
    TEAM_SPECTATOR = 0x3,
    TEAM_NUM_TEAMS = 0x4,
};

enum class  msgtype_t : int {
    MSG_DEFAULT,
    MSG_NA,    //Not defined
    MSG_WARNING,
    MSG_ERROR,
    MSG_NORDPRINT
};
enum class conChannel_t : int
{
    CON_CHANNEL_DONT_FILTER = 0x0,
    CON_CHANNEL_ERROR = 0x1,
    CON_CHANNEL_GAMENOTIFY = 0x2,
    CON_CHANNEL_BOLDGAME = 0x3,
    CON_CHANNEL_SUBTITLE = 0x4,
    CON_CHANNEL_OBITUARY = 0x5,
    CON_CHANNEL_LOGFILEONLY = 0x6,
    CON_CHANNEL_CONSOLEONLY = 0x7,
    CON_CHANNEL_GFX = 0x8,
    CON_CHANNEL_SOUND = 0x9,
    CON_CHANNEL_FILES = 0xA,
    CON_CHANNEL_DEVGUI = 0xB,
    CON_CHANNEL_PROFILE = 0xC,
    CON_CHANNEL_UI = 0xD,
    CON_CHANNEL_CLIENT = 0xE,
    CON_CHANNEL_SERVER = 0xF,
    CON_CHANNEL_SYSTEM = 0x10,
    CON_CHANNEL_PLAYERWEAP = 0x11,
    CON_CHANNEL_AI = 0x12,
    CON_CHANNEL_ANIM = 0x13,
    CON_CHANNEL_PHYS = 0x14,
    CON_CHANNEL_FX = 0x15,
    CON_CHANNEL_LEADERBOARDS = 0x16,
    CON_CHANNEL_LIVE = 0x17,
    CON_CHANNEL_PARSERSCRIPT = 0x18,
    CON_CHANNEL_SCRIPT = 0x19,
    CON_CHANNEL_SPAWNSYSTEM = 0x1A,
    CON_CHANNEL_COOPINFO = 0x1B,
    CON_CHANNEL_SERVERDEMO = 0x1C,
    CON_CHANNEL_DDL = 0x1D,
    CON_CHANNEL_NETWORK = 0x1E,
    CON_CHANNEL_SCHEDULER = 0x1F,
    CON_FIRST_DEBUG_CHANNEL = 0x1F,
    CON_CHANNEL_TASK = 0x20,
    CON_CHANNEL_SPU = 0x21,
    CON_CHANNEL_FILEDL = 0x22,
    CON_BUILTIN_CHANNEL_COUNT = 0x23
};

struct SprintState
{
    int SprintButtonUpRequired; //0x0000
    int SprintDelay; //0x0004
    int LastSprintStart; //0x0008
    int LastSprintEnd; //0x000C
    int SprintStartMaxLength; //0x0010
}; //Size=0x0014

struct MantleState
{
    float Yaw; //0x0000
    int Timer; //0x0004
    int TransIndex; //0x0008
    int Flags; //0x000C
}; //Size=0x0010

struct playerState_t // adress is 0x00794474
{
    int CommandTime; //0x0000
    int PM_Type; //0x0004
    int BobCycle; //0x0008
    int PM_Flags; //0x000C
    int WeaponFlags; //0x0010
    int OtherFlags; //0x0014
    int PM_Time; //0x0018
    vec3<float> Origin; //0x001C
    vec3<float> Velocity; //0x0028
    vec2<float> OldVelocity; //0x0034
    int WeaponTime; //0x003C
    int WeaponDelay; //0x0040
    int GrenadeTimeLeft; //0x0044
    int ThrowbackGrenadeOwner; //0x0048
    int ThrowbackGrenadeTimeLeft; //0x004C
    int WeaponRestrictKickTime; //0x0050
    int FoliageSoundTime; //0x0054
    int Gravity; //0x0058
    float Lean; //0x005C
    int Speed; //0x0060
    vec3<float> DeltaAngles; //0x0064
    int GroundEntityNum; //0x0070
    vec3<float> LadderVec; //0x0074
    int JumpTime; //0x0080
    float JumpOriginZ; //0x0084
    int LegsTimer; //0x0088
    int LegsAnim; //0x008C
    int TorsoTimer; //0x0090
    int TorsoAnim; //0x0094
    int LegsAnimDuration; //0x0098
    int TorsoAnimDuration; //0x009C
    int DamageTimer; //0x00A0
    int DamageDuration; //0x00A4
    int FlinchYawAnim; //0x00A8
    int MovementDir; //0x00AC
    int EFlags; //0x00B0
    int EventSequence; //0x00B4
    int Events[4]; //0x00B8
    int EventParams[4]; //0x00C8
    int OldEventSequence; //0x00D8
    int ClientNum; //0x00DC
    int OffHandIndex; //0x00E0
    int OffhandSecondary; //0x00E4
    int Weapon; //0x00E8
    int WeaponState; //0x00EC
    int WeaponShotCount; //0x00F0
    float WeaponPosFraction; //0x00F4
    int ADSDelayTime; //0x00F8
    int SpreadOverride; //0x00FC
    int SpreadOverrideState; //0x0100
    int ViewModelIndex; //0x0104
    vec3<float> ViewAngles; //0x0108
    int ViewHeightTarget; //0x0114
    float ViewHeightCurrent; //0x0118
    int ViewHeightLerpTime; //0x011C
    int ViewHeightLerpTarget; //0x0120
    int ViewHeightLerpDown; //0x0124
    vec2<float> ViewAngleClampBase; //0x0128
    vec2<float> ViewAngleClampRange; //0x0130
    int DamageEvent; //0x0138
    int DamageYaw; //0x013C
    int DamagePitch; //0x0140
    int DamageCount; //0x0144
    int Stats[5]; //0x0148
    int Ammo[128]; //0x015C
    int AmmoInClip[128]; //0x035C
    int Weapons[4]; //0x055C
    int WeaponsOld[4]; //0x056C
    int WeaponRechamber[4]; //0x057C
    float ProneDirection; //0x058C
    float ProneDirectionPitch; //0x0590
    float ProneTorsoPitch; //0x0594
    int ViewLocked; //0x0598
    int ViewLockedEntNum; //0x059C
    int CursorHint; //0x05A0
    int CursorHintString; //0x05A4
    int CursorHintEntIndex; //0x05A8
    int ICompassPlayerInfo; //0x05AC
    int RadarEnabled; //0x05B0
    int LocationSelectionInfo; //0x05B4
    SprintState SprintState; //0x05B8
    float TorsoPitch; //0x05CC
    float WaistPitch; //0x05D0
    float HoldBreathScale; //0x05D4
    int HoldBreathTimer; //0x05D8
    float MoveSpeedScaleMultiplier; //0x05DC
    MantleState MantleState; //0x05E0
    float MeleeChargeYaw; //0x05F0
    int MeleeChargeDistance; //0x05F4
    int MeleeChargeTime; //0x05F8
    int Perks; //0x05FC
    int ActionSlotType[4]; //0x0600
    int ActionSlotParam[4]; //0x0610
    int EntityEventSequence; //0x0620
    int WeaponAnim; //0x0624
    float AimSpreadScale; //0x0628
    int ShellShockIndex; //0x062C
    int ShellShockTime; //0x0630
    int ShellShockDuration; //0x0634
    float DOFNearStart; //0x0638
    float DOFNearEnd; //0x063C
    float DOFFarStart; //0x0640
    float DOFFarEnd; //0x0644
    float DOFNearBlur; //0x0648
    float DOFFarBlur; //0x064C
    float DOFViewModelStart; //0x0650
    float DOFViewModelEnd; //0x0654
    char _0x0658[0x1C4]; //0x0658
    char WeaponModels[128]; //0x81C
    int DeltaTime; //0x089C
    int KillCamEntity; //0x08A0
    char _0x08A4[0x26C0]; //hudelems cba to reverse rn
}; //Size=0x2F64

struct trajectory_t
{
    int Type; //0x0000
    int Time; //0x0004
    int Duration; //0x0008
    vec3<float> Base; //0x000C
    vec3<float> Delta; //0x0018
}; //Size=0x0024

struct LerpEntityState
{
    unsigned int EntityFlags; //0x0000
    trajectory_t Postion; //0x0004
    trajectory_t APostion; //0x0028
    char _0x004C[0x1C]; //0x004C
}; //Size=0x0068

struct entityState_t
{
    int Number; //0x0000
    int EType; //0x0004
    LerpEntityState Lerp; //0x0008
    int Time2; //0x0070
    int OtherEntityNum; //0x0074
    int AttackerEntityNum; //0x0078
    int GroundEntityNum; //0x007C
    int LoopSound; //0x0080
    int SurfaceType; //0x0084
    int Index; //0x0088
    int ClientNum; //0x008C
    int IHeadIcon; //0x0090
    int IHeadIconTeam; //0x0094
    int Solid; //0x0098
    int EventParam; //0x009C
    int EventSequence; //0x00A0
    int Events[4]; //0x00A4
    int EventParams[4]; //0x00B4
    int Weapon; //0x00C4
    int WeaponModel; //0x00C8
    int LegsAnim; //0x00CC
    int TorsoAnim; //0x00D0
    int Union1; //0x00D4
    int Union2; //0x00D8
    float TorsoPitch; //0x00DC
    float WaistPitch; //0x00E0
    int PartBits[4]; //0x00E4
}; //Size=0x00F4
struct clientState_s
{
    int ClientIndex; //0x0000
    team_t Team; //0x0004
    int Modelindex; //0x0008
    int AttachModelIndex[6]; //0x000C
    int AttachTagIndex[6]; //0x0024
    char Name[16]; //0x003C
    float MaxSprintTimeMultiplier; //0x004C
    int Rank; //0x0050
    int Prestige; //0x0054
    int Perks; //0x0058
    int AttachedVehEntNum; //0x005C
    int AttachedVehSlotIndex; //0x0060
}; //Size: 0x64

struct snapshot_t
{
    int Flags; //0x0000
    int Ping; //0x0004
    int ServerTime; //0x0008
    playerState_t PlayerState; //0x000C
    int  NumEntities; //0x2F70
    int  NumClients; //0x2F74
    entityState_t Entities[512]; //0x2F78
    clientState_s Clients[64]; //0x21778
    int ServerCommandSequence; //0x23078
}; //Size = 0x2307C

struct refdef_t
{
    int ScreenX; //0x0000
    int ScreenY; //0x0004
    int ScreenWidth; //0x0008
    int ScreenHeight; //0x000C
    vec2<float> FOV; //0x0010
    vec3<float> Origin; //0x0018
    vec3<float> ViewAxis[3]; //0x0024
    //vec3f ViewOffset; //0x0048
    char unknown142[0x4050];
    vec3<float> ViewAngles;
};  //0x00797600

struct cg_t // adress 0x0074E338
{
    int ClientNum; //0x0000
    char _0x0004[0x1C]; //0x0004
    snapshot_t* CurrentSnap; //0x0020
    snapshot_t* NextSnap; //0x0024
    char _0x0028[0x460FC]; //0x0028
    int FrameTime; //0x46124
    int Time; //0x46128
    int OldTime; //0x4612C
    int PhysicsTime; //0x46130
    int MapRestart; //0x46134
    int RenderingThirdPerson; //0x46138
    playerState_t PredictedPlayerState; //0x4613C
    char _0x490A0[0x228];
    refdef_t Refdef; //0x492C8
}; //Size=0xFE7F0

enum OffhandSecondaryClass
{
    PLAYER_OFFHAND_SECONDARY_SMOKE = 0x0,
    PLAYER_OFFHAND_SECONDARY_FLASH = 0x1,
    PLAYER_OFFHAND_SECONDARIES_TOTAL = 0x2,
};

enum ViewLockTypes
{
    PLAYERVIEWLOCK_NONE = 0x0,
    PLAYERVIEWLOCK_FULL = 0x1,
    PLAYERVIEWLOCK_WEAPONJITTER = 0x2,
    PLAYERVIEWLOCKCOUNT = 0x3,
};

enum ActionSlotType
{
    ACTIONSLOTTYPE_DONOTHING = 0x0,
    ACTIONSLOTTYPE_SPECIFYWEAPON = 0x1,
    ACTIONSLOTTYPE_ALTWEAPONTOGGLE = 0x2,
    ACTIONSLOTTYPE_NIGHTVISION = 0x3,
    ACTIONSLOTTYPECOUNT = 0x4,
};

struct ActionSlotParam_SpecifyWeapon
{
    unsigned int index;
};

struct ActionSlotParam
{
    ActionSlotParam_SpecifyWeapon specifyWeapon;
};

enum objectiveState_t
{
    OBJST_EMPTY = 0x0,
    OBJST_ACTIVE = 0x1,
    OBJST_INVISIBLE = 0x2,
    OBJST_DONE = 0x3,
    OBJST_CURRENT = 0x4,
    OBJST_FAILED = 0x5,
    OBJST_NUMSTATES = 0x6,
};

struct objective_t
{
    objectiveState_t state;
    float origin[3];
    int entNum;
    int teamNum;
    int icon;
};

enum he_type_t
{
    HE_TYPE_FREE = 0x0,
    HE_TYPE_TEXT = 0x1,
    HE_TYPE_VALUE = 0x2,
    HE_TYPE_PLAYERNAME = 0x3,
    HE_TYPE_MAPNAME = 0x4,
    HE_TYPE_GAMETYPE = 0x5,
    HE_TYPE_MATERIAL = 0x6,
    HE_TYPE_TIMER_DOWN = 0x7,
    HE_TYPE_TIMER_UP = 0x8,
    HE_TYPE_TENTHS_TIMER_DOWN = 0x9,
    HE_TYPE_TENTHS_TIMER_UP = 0xA,
    HE_TYPE_CLOCK_DOWN = 0xB,
    HE_TYPE_CLOCK_UP = 0xC,
    HE_TYPE_WAYPOINT = 0xD,
    HE_TYPE_COUNT = 0xE,
};

struct $C96EA5EC2ACBB9C0BF22693F316ACC67
{
    char r;
    char g;
    char b;
    char a;
};

union hudelem_color_t
{
    $C96EA5EC2ACBB9C0BF22693F316ACC67 __s0;
    int rgba;
};

struct hudelem_s
{
    he_type_t type;
    float x;
    float y;
    float z;
    int targetEntNum;
    float fontScale;
    int font;
    int alignOrg;
    int alignScreen;
    hudelem_color_t color;
    hudelem_color_t fromColor;
    int fadeStartTime;
    int fadeTime;
    int label;
    int width;
    int height;
    int materialIndex;
    int offscreenMaterialIdx;
    int fromWidth;
    int fromHeight;
    int scaleStartTime;
    int scaleTime;
    float fromX;
    float fromY;
    int fromAlignOrg;
    int fromAlignScreen;
    int moveStartTime;
    int moveTime;
    int time;
    int duration;
    float value;
    int text;
    float sort;
    hudelem_color_t glowColor;
    int fxBirthTime;
    int fxLetterTime;
    int fxDecayStartTime;
    int fxDecayDuration;
    int soundID;
    int flags;
};

struct $15067B6A14D88D7E1E730369692C3A81
{
    hudelem_s current[31];
    hudelem_s archival[31];
};

struct playerState_s
{
    int commandTime;
    int pm_type;
    int bobCycle;
    int pm_flags;
    int weapFlags;
    int otherFlags;
    int pm_time;
    float origin[3];
    float velocity[3];
    float oldVelocity[2];
    int weaponTime;
    int weaponDelay;
    int grenadeTimeLeft;
    int throwBackGrenadeOwner;
    int throwBackGrenadeTimeLeft;
    int weaponRestrictKickTime;
    int foliageSoundTime;
    int gravity;
    float leanf;
    int speed;
    float delta_angles[3];
    int groundEntityNum;
    float vLadderVec[3];
    int jumpTime;
    float jumpOriginZ;
    int legsTimer;
    int legsAnim;
    int torsoTimer;
    int torsoAnim;
    int legsAnimDuration;
    int torsoAnimDuration;
    int damageTimer;
    int damageDuration;
    int flinchYawAnim;
    int movementDir;
    int eFlags;
    int eventSequence;
    int events[4];
    unsigned int eventParms[4];
    int oldEventSequence;
    int clientNum;
    int offHandIndex;
    OffhandSecondaryClass offhandSecondary;
    unsigned int weapon;
    int weaponstate;
    unsigned int weaponShotCount;
    float fWeaponPosFrac;
    int adsDelayTime;
    int spreadOverride;
    int spreadOverrideState;
    int viewmodelIndex;
    float viewangles[3];
    int viewHeightTarget;
    float viewHeightCurrent;
    int viewHeightLerpTime;
    int viewHeightLerpTarget;
    int viewHeightLerpDown;
    float viewAngleClampBase[2];
    float viewAngleClampRange[2];
    int damageEvent;
    int damageYaw;
    int damagePitch;
    int damageCount;
    int stats[5];
    int ammo[128];
    int ammoclip[128];
    unsigned int weapons[4];
    unsigned int weaponold[4];
    unsigned int weaponrechamber[4];
    float proneDirection;
    float proneDirectionPitch;
    float proneTorsoPitch;
    ViewLockTypes viewlocked;
    int viewlocked_entNum;
    int cursorHint;
    int cursorHintString;
    int cursorHintEntIndex;
    int iCompassPlayerInfo;
    int radarEnabled;
    int locationSelectionInfo;
    SprintState sprintState;
    float fTorsoPitch;
    float fWaistPitch;
    float holdBreathScale;
    int holdBreathTimer;
    float moveSpeedScaleMultiplier;
    MantleState mantleState;
    float meleeChargeYaw;
    int meleeChargeDist;
    int meleeChargeTime;
    int perks;
    ActionSlotType actionSlotType[4];
    ActionSlotParam actionSlotParam[4];
    int entityEventSequence;
    int weapAnim;
    float aimSpreadScale;
    int shellshockIndex;
    int shellshockTime;
    int shellshockDuration;
    float dofNearStart;
    float dofNearEnd;
    float dofFarStart;
    float dofFarEnd;
    float dofNearBlur;
    float dofFarBlur;
    float dofViewmodelStart;
    float dofViewmodelEnd;
    int hudElemLastAssignedSoundID;
    objective_t objective[16];
    char weaponmodels[128];
    int deltaTime;
    int killCamEntity;
    $15067B6A14D88D7E1E730369692C3A81 hud;
};

struct usercmd_s
{
    int Commandtime; //0x0000
    int buttons; //0x0004
    int viewangles[3]; //0x0008
    short weapon; // 0x14
    BYTE forward; //0x16
    BYTE side; //0x17
    BYTE unk1; // 0x18
    BYTE unn[3];
    float unk2; // 0x20
};

struct pmove_t
{
    playerState_s* ps;
    usercmd_s cmd;
    usercmd_s oldcmd;
    int tracemask;
    int numtouch;
    int touchents[32];
    float mins[3];
    float maxs[3];
    float xyspeed;
    int proneChange;
    float maxSprintTimeMultiplier;
    bool mantleStarted;
    float mantleEndPos[3];
    int mantleDuration;
    int viewChangeTime;
    float viewChange;
    char handler;
};

struct input_s // don't really know the type
{
    usercmd_s usercmds[128]; // 0x0
    int currentCmdNum; // 0x1000

    usercmd_s* GetUserCmd(int cmdNum)
    {
        int id = cmdNum & 0x7F;
        return &usercmds[id];
    }
};

enum MapType
{
    MAPTYPE_NONE = 0x0,
    MAPTYPE_INVALID1 = 0x1,
    MAPTYPE_INVALID2 = 0x2,
    MAPTYPE_2D = 0x3,
    MAPTYPE_3D = 0x4,
    MAPTYPE_CUBE = 0x5,
    MAPTYPE_COUNT = 0x6,
};

struct GfxImageLoadDef
{
    char levelCount;
    char flags;
    __int16 dimensions[3];
    int format;
    int resourceSize;
    char data[1];
};
union GfxTexture
{
    /*IDirect3DBaseTexture9 *basemap;
    IDirect3DTexture9 *map;
    IDirect3DVolumeTexture9 *volmap;
    IDirect3DCubeTexture9 *cubemap;*/
    GfxImageLoadDef* loadDef;
    void* data;
};

struct Picmip
{
    char platform[2];
};

struct CardMemory
{
    int platform[2];
};

struct GfxImage
{
    MapType mapType;
    GfxTexture texture;
    Picmip picmip;
    bool noPicmip;
    char semantic;
    char track;
    CardMemory cardMemory;
    unsigned __int16 width;
    unsigned __int16 height;
    unsigned __int16 depth;
    char category;
    bool delayLoadPixels;
    const char* name;
};

struct __declspec(align(8)) GfxCmdBufInput
{
    float consts[58][4];
    GfxImage* codeImages[27];
    char codeImageSamplerStates[27];
    void* data;
};

struct GfxMatrix
{
    float m[4][4];
};

struct GfxCodeMatrices
{
    GfxMatrix matrix[32];
};

struct GfxViewParms
{
    GfxMatrix viewMatrix;
    GfxMatrix projectionMatrix;
    GfxMatrix viewProjectionMatrix;
    GfxMatrix inverseViewProjectionMatrix;
    float origin[4];
    float axis[3][3];
    float depthHackNearClip;
    float zNear;
    float zFar;
};

#pragma warning( push )
#pragma warning( disable : 4324 )
struct __declspec(align(16)) GfxCmdBufSourceState
{
    GfxCodeMatrices matrices;
    GfxCmdBufInput input;
    GfxViewParms viewParms;
    GfxMatrix shadowLookupMatrix;
};
#pragma warning( pop )

//collision//////////// https://github.com/xoxor4d/iw3xo-dev/blob/develop/src/game/structs.hpp

struct __declspec(align(4)) ShowCollisionBrushPt
{
    vec3<float> xyz;
    __int16 sideIndex[3];
};

struct cplane_s
{
    float normal[3];
    float dist;
    char type;
    char signbits;
    char pad[2];
};

#pragma pack(push, 2)
struct cbrushside_t
{
    cplane_s* plane;
    unsigned int materialNum;
    __int16 firstAdjacentSideOffset;
    char edgeCount;
};
#pragma pack(pop)

#pragma pack(push, 16)
struct cbrush_t
{
    vec3<float> mins;
    int contents;
    vec3<float> maxs;
    unsigned int numsides;
    cbrushside_t* sides;
    __int16 axialMaterialNum[2][3];
    char* baseAdjacentSide;
    __int16 firstAdjacentSideOffsets[2][3];
    char edgeCount[2][3];
    __int16 colorCounter;
    __int16 cmBrushIndex;
    //float distFromCam;
    __int16 cmSubmodelIndex;
    bool isSubmodel;
    bool pad;
};
#pragma pack(pop)

#pragma pack(push, 4)
struct cLeaf_t
{
    unsigned __int16 firstCollAabbIndex;
    unsigned __int16 collAabbCount;
    int brushContents;
    int terrainContents;
    float mins[3];
    float maxs[3];
    int leafBrushNode;
    __int16 cluster;
};
#pragma pack(pop)

struct cmodel_t
{
    float mins[3];
    float maxs[3];
    float radius;
    cLeaf_t leaf;
};

struct TriggerModel
{
    int contents;
    unsigned __int16 hullCount;
    unsigned __int16 firstHull;
};

struct Bounds
{
    vec3_t midPoint;
    vec3_t halfSize;
};

struct TriggerHull
{
    Bounds bounds;
    int contents;
    unsigned __int16 slabCount;
    unsigned __int16 firstSlab;
};

struct TriggerSlab
{
    float dir[3];
    float midPoint;
    float halfSize;
};

struct MapTriggers
{
    unsigned int count;
    TriggerModel* models;
    unsigned int hullCount;
    TriggerHull* hulls;
    unsigned int slabCount;
    TriggerSlab* slabs;
};

struct MapEnts
{
    const char* name;
    char* entityString;
    int numEntityChars;
    MapTriggers trigger;
    // this goes on for a while but we don't need any of it
};

struct cLeafBrushNodeLeaf_t
{
    unsigned __int16* brushes;
};

struct cLeafBrushNodeChildren_t
{
    float dist;
    float range;
    unsigned __int16 childOffset[2];
};

union cLeafBrushNodeData_t
{
    cLeafBrushNodeLeaf_t leaf;
    cLeafBrushNodeChildren_t children;
};

struct cLeafBrushNode_s
{
    char axis;
    __int16 leafBrushCount;
    int contents;
    cLeafBrushNodeData_t data;
};

struct clipMap_t
{
    const char* name;
    int isInUse;
    int planeCount;
    cplane_s* planes;
    unsigned int numStaticModels;
    byte* staticModelList;
    unsigned int numMaterials;
    byte* materials;
    unsigned int numBrushSides;
    cbrushside_t* brushsides;
    unsigned int numBrushEdges;
    char* brushEdges;
    unsigned int numNodes;
    void* nodes;
    unsigned int numLeafs;
    cLeaf_t* leafs;
    unsigned int leafbrushNodesCount;
    cLeafBrushNode_s* leafbrushNodes;
    unsigned int numLeafBrushes;
    unsigned __int16* leafbrushes;
    unsigned int numLeafSurfaces;
    unsigned int* leafsurfaces;
    unsigned int vertCount;
    float(*verts)[3];
    int triCount;
    unsigned __int16* triIndices;
    char* triEdgeIsWalkable;
    int borderCount;
    byte* borders;
    int partitionCount;
    byte* partitions;
    int aabbTreeCount;
    byte* aabbTrees;
    unsigned int numSubModels;
    cmodel_t* cmodels;
    unsigned __int16 numBrushes;
    cbrush_t* brushes;
    int numClusters;
    int clusterBytes;
    char* visibility;
    int vised;
    MapEnts* mapEnts;
    cbrush_t* box_brush;
    cmodel_t box_model;
    unsigned __int16 dynEntCount[2];
    byte* dynEntDefList[2];
    /*DynEntityPose*/ void* dynEntPoseList[2];
    /*DynEntityClient*/ void* dynEntClientList[2];
    /*DynEntityColl*/ void* dynEntCollList[2];
    unsigned int checksum;
};

struct brushmodel_entity_s
{
    int cm_submodel_index;
    cmodel_t* cm_submodel;
    float cm_submodel_origin[3];
    int cm_brush_index;
    cbrush_t* cm_brush;
    std::vector<std::string> brush_sides;
};

struct winding_t
{
    int numpoints;
    float p[4][3];
};

union GfxColor
{
    unsigned int packed;
    char array[4];
};

struct MaterialArgumentCodeConst
{
    unsigned __int16 index;
    char firstRow;
    char rowCount;
};

union MaterialArgumentDef
{
    const float* literalConst;
    MaterialArgumentCodeConst codeConst;
    unsigned int codeSampler;
    unsigned int nameHash;
};

/* MaterialShaderArgument->type */
#define MTL_ARG_MATERIAL_VERTEX_CONST	0x0
#define MTL_ARG_LITERAL_VERTEX_CONST	0x1
#define MTL_ARG_MATERIAL_PIXEL_SAMPLER	0x2
#define MTL_ARG_CODE_VERTEX_CONST	    0x3
#define MTL_ARG_CODE_PIXEL_SAMPLER	    0x4
#define MTL_ARG_CODE_PIXEL_CONST	    0x5
#define MTL_ARG_MATERIAL_PIXEL_CONST	0x6
#define MTL_ARG_LITERAL_PIXEL_CONST	    0x7

struct MaterialShaderArgument
{
    unsigned __int16 type;
    unsigned __int16 dest;
    MaterialArgumentDef u;
};

enum ShaderCodeConstants
{
    CONST_SRC_CODE_MAYBE_DIRTY_PS_BEGIN = 0x0,
    CONST_SRC_CODE_LIGHT_POSITION = 0x0,
    CONST_SRC_CODE_LIGHT_DIFFUSE = 0x1,
    CONST_SRC_CODE_LIGHT_SPECULAR = 0x2,
    CONST_SRC_CODE_LIGHT_SPOTDIR = 0x3,
    CONST_SRC_CODE_LIGHT_SPOTFACTORS = 0x4,
    CONST_SRC_CODE_NEARPLANE_ORG = 0x5,
    CONST_SRC_CODE_NEARPLANE_DX = 0x6,
    CONST_SRC_CODE_NEARPLANE_DY = 0x7,
    CONST_SRC_CODE_SHADOW_PARMS = 0x8,
    CONST_SRC_CODE_SHADOWMAP_POLYGON_OFFSET = 0x9,
    CONST_SRC_CODE_RENDER_TARGET_SIZE = 0xA,
    CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT = 0xB,
    CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR = 0xC,
    CONST_SRC_CODE_DOF_EQUATION_SCENE = 0xD,
    CONST_SRC_CODE_DOF_LERP_SCALE = 0xE,
    CONST_SRC_CODE_DOF_LERP_BIAS = 0xF,
    CONST_SRC_CODE_DOF_ROW_DELTA = 0x10,
    CONST_SRC_CODE_PARTICLE_CLOUD_COLOR = 0x11,
    CONST_SRC_CODE_GAMETIME = 0x12,
    CONST_SRC_CODE_MAYBE_DIRTY_PS_END = 0x13,
    CONST_SRC_CODE_ALWAYS_DIRTY_PS_BEGIN = 0x13,
    CONST_SRC_CODE_PIXEL_COST_FRACS = 0x13,
    CONST_SRC_CODE_PIXEL_COST_DECODE = 0x14,
    CONST_SRC_CODE_FILTER_TAP_0 = 0x15,
    CONST_SRC_CODE_FILTER_TAP_1 = 0x16,
    CONST_SRC_CODE_FILTER_TAP_2 = 0x17,
    CONST_SRC_CODE_FILTER_TAP_3 = 0x18,
    CONST_SRC_CODE_FILTER_TAP_4 = 0x19,
    CONST_SRC_CODE_FILTER_TAP_5 = 0x1A,
    CONST_SRC_CODE_FILTER_TAP_6 = 0x1B,
    CONST_SRC_CODE_FILTER_TAP_7 = 0x1C,
    CONST_SRC_CODE_COLOR_MATRIX_R = 0x1D,
    CONST_SRC_CODE_COLOR_MATRIX_G = 0x1E,
    CONST_SRC_CODE_COLOR_MATRIX_B = 0x1F,
    CONST_SRC_CODE_ALWAYS_DIRTY_PS_END = 0x20,
    CONST_SRC_CODE_NEVER_DIRTY_PS_BEGIN = 0x20,
    CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION = 0x20,
    CONST_SRC_CODE_SHADOWMAP_SCALE = 0x21,
    CONST_SRC_CODE_ZNEAR = 0x22,
    CONST_SRC_CODE_SUN_POSITION = 0x23,
    CONST_SRC_CODE_SUN_DIFFUSE = 0x24,
    CONST_SRC_CODE_SUN_SPECULAR = 0x25,
    CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE = 0x26,
    CONST_SRC_CODE_DEBUG_BUMPMAP = 0x27,
    CONST_SRC_CODE_MATERIAL_COLOR = 0x28,
    CONST_SRC_CODE_FOG = 0x29,
    CONST_SRC_CODE_FOG_COLOR = 0x2A,
    CONST_SRC_CODE_GLOW_SETUP = 0x2B,
    CONST_SRC_CODE_GLOW_APPLY = 0x2C,
    CONST_SRC_CODE_COLOR_BIAS = 0x2D,
    CONST_SRC_CODE_COLOR_TINT_BASE = 0x2E,
    CONST_SRC_CODE_COLOR_TINT_DELTA = 0x2F,
    CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS = 0x30,
    CONST_SRC_CODE_ENVMAP_PARMS = 0x31,
    CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST = 0x32,
    CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE = 0x33,
    CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET = 0x34,
    CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX = 0x35,
    CONST_SRC_CODE_DEPTH_FROM_CLIP = 0x36,
    CONST_SRC_CODE_CODE_MESH_ARG_0 = 0x37,
    CONST_SRC_CODE_CODE_MESH_ARG_1 = 0x38,
    CONST_SRC_CODE_CODE_MESH_ARG_LAST = 0x38,
    CONST_SRC_CODE_BASE_LIGHTING_COORDS = 0x39,
    CONST_SRC_CODE_NEVER_DIRTY_PS_END = 0x3A,
    CONST_SRC_CODE_COUNT_FLOAT4 = 0x3A,
    CONST_SRC_FIRST_CODE_MATRIX = 0x3A,
    CONST_SRC_CODE_WORLD_MATRIX = 0x3A,
    CONST_SRC_CODE_INVERSE_WORLD_MATRIX = 0x3B,
    CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX = 0x3C,
    CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX = 0x3D,
    CONST_SRC_CODE_VIEW_MATRIX = 0x3E,
    CONST_SRC_CODE_INVERSE_VIEW_MATRIX = 0x3F,
    CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX = 0x40,
    CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX = 0x41,
    CONST_SRC_CODE_PROJECTION_MATRIX = 0x42,
    CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX = 0x43,
    CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX = 0x44,
    CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX = 0x45,
    CONST_SRC_CODE_WORLD_VIEW_MATRIX = 0x46,
    CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX = 0x47,
    CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX = 0x48,
    CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX = 0x49,
    CONST_SRC_CODE_VIEW_PROJECTION_MATRIX = 0x4A,
    CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX = 0x4B,
    CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0x4C,
    CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0x4D,
    CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX = 0x4E,
    CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX = 0x4F,
    CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX = 0x50,
    CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX = 0x51,
    CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX = 0x52,
    CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX = 0x53,
    CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0x54,
    CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0x55,
    CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x56,
    CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x57,
    CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x58,
    CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x59,
    CONST_SRC_TOTAL_COUNT = 0x5A,
    CONST_SRC_NONE = 0x5B,
};

struct MaterialStreamRouting
{
    char source;
    char dest;
};

struct MaterialVertexStreamRouting
{
    MaterialStreamRouting data[16];
    void* decl[16];
};

struct MaterialVertexDeclaration
{
    char streamCount;
    bool hasOptionalSource;
    bool isLoaded;
    MaterialVertexStreamRouting routing;
};

struct GfxVertexShaderLoadDef
{
    unsigned int* program;
    unsigned __int16 programSize;
    unsigned __int16 loadForRenderer;
};

struct MaterialVertexShaderProgram
{
    void* vs;
    GfxVertexShaderLoadDef loadDef;
};

struct MaterialVertexShader
{
    const char* name;
    MaterialVertexShaderProgram prog;
};

struct GfxPixelShaderLoadDef
{
    unsigned int* program;
    unsigned __int16 programSize;
    unsigned __int16 loadForRenderer;
};

struct MaterialPixelShaderProgram
{
    void* ps;
    GfxPixelShaderLoadDef loadDef;
};

struct MaterialPixelShader
{
    const char* name;
    MaterialPixelShaderProgram prog;
};

#pragma pack(push, 4)
struct MaterialPass
{
    MaterialVertexDeclaration* vertexDecl;
    MaterialVertexShader* vertexShader;
    MaterialPixelShader* pixelShader;
    char perPrimArgCount;
    char perObjArgCount;
    char stableArgCount;
    char customSamplerFlags;
    MaterialShaderArgument* args;
};
#pragma pack(pop)

struct MaterialTechnique
{
    const char* name;
    unsigned __int16 flags;
    unsigned __int16 passCount;
    MaterialPass passArray[1];	// count = passCount
};

/* MaterialTechniqueSet->worldVertFormat */
enum MaterialWorldVertexFormat : char
{
    MTL_WORLDVERT_TEX_1_NRM_1 = 0x0,
    MTL_WORLDVERT_TEX_2_NRM_1 = 0x1,
    MTL_WORLDVERT_TEX_2_NRM_2 = 0x2,
    MTL_WORLDVERT_TEX_3_NRM_1 = 0x3,
    MTL_WORLDVERT_TEX_3_NRM_2 = 0x4,
    MTL_WORLDVERT_TEX_3_NRM_3 = 0x5,
    MTL_WORLDVERT_TEX_4_NRM_1 = 0x6,
    MTL_WORLDVERT_TEX_4_NRM_2 = 0x7,
    MTL_WORLDVERT_TEX_4_NRM_3 = 0x8,
    MTL_WORLDVERT_TEX_5_NRM_1 = 0x9,
    MTL_WORLDVERT_TEX_5_NRM_2 = 0xA,
    MTL_WORLDVERT_TEX_5_NRM_3 = 0xB,
};

/*struct MaterialTechniqueSet // org
{
    char *name;
    MaterialWorldVertexFormat worldVertFormat;
    MaterialTechnique *techniques[34];
};*/

struct MaterialTechniqueSet
{
    char* name;
    MaterialWorldVertexFormat worldVertFormat;
    bool hasBeenUploaded;
    char unused[1];
    MaterialTechniqueSet* remappedTechniqueSet;
    MaterialTechnique* techniques[34];
};

struct GfxDrawSurfFields
{
    unsigned __int64 objectId : 16;
    unsigned __int64 reflectionProbeIndex : 8;
    unsigned __int64 customIndex : 5;
    unsigned __int64 materialSortedIndex : 11;
    unsigned __int64 prepass : 2;
    unsigned __int64 primaryLightIndex : 8;
    unsigned __int64 surfType : 4;
    unsigned __int64 primarySortKey : 6;
    unsigned __int64 unused : 4;
};

union GfxDrawSurf
{
    GfxDrawSurfFields fields;
    unsigned long long packed;
};

#pragma pack(push, 4)
struct MaterialInfo
{
    const char* name;
    char gameFlags;
    char sortKey;
    char textureAtlasRowCount;
    char textureAtlasColumnCount;
    GfxDrawSurf drawSurf;
    unsigned int surfaceTypeBits;
    unsigned __int16 hashIndex;
};
#pragma pack(pop)

enum MaterialTechniqueType
{
    TECHNIQUE_DEPTH_PREPASS = 0x0,
    TECHNIQUE_BUILD_FLOAT_Z = 0x1,
    TECHNIQUE_BUILD_SHADOWMAP_DEPTH = 0x2,
    TECHNIQUE_BUILD_SHADOWMAP_COLOR = 0x3,
    TECHNIQUE_UNLIT = 0x4,
    TECHNIQUE_EMISSIVE = 0x5,
    TECHNIQUE_EMISSIVE_SHADOW = 0x6,
    TECHNIQUE_LIT_BEGIN = 0x7,
    TECHNIQUE_LIT = 0x7,
    TECHNIQUE_LIT_SUN = 0x8,
    TECHNIQUE_LIT_SUN_SHADOW = 0x9,
    TECHNIQUE_LIT_SPOT = 0xA,
    TECHNIQUE_LIT_SPOT_SHADOW = 0xB,
    TECHNIQUE_LIT_OMNI = 0xC,
    TECHNIQUE_LIT_OMNI_SHADOW = 0xD,
    TECHNIQUE_LIT_INSTANCED = 0xE,
    TECHNIQUE_LIT_INSTANCED_SUN = 0xF,
    TECHNIQUE_LIT_INSTANCED_SUN_SHADOW = 0x10,
    TECHNIQUE_LIT_INSTANCED_SPOT = 0x11,
    TECHNIQUE_LIT_INSTANCED_SPOT_SHADOW = 0x12,
    TECHNIQUE_LIT_INSTANCED_OMNI = 0x13,
    TECHNIQUE_LIT_INSTANCED_OMNI_SHADOW = 0x14,
    TECHNIQUE_LIT_END = 0x15,
    TECHNIQUE_LIGHT_SPOT = 0x15,
    TECHNIQUE_LIGHT_OMNI = 0x16,
    TECHNIQUE_LIGHT_SPOT_SHADOW = 0x17,
    TECHNIQUE_FAKELIGHT_NORMAL = 0x18,
    TECHNIQUE_FAKELIGHT_VIEW = 0x19,
    TECHNIQUE_SUNLIGHT_PREVIEW = 0x1A,
    TECHNIQUE_CASE_TEXTURE = 0x1B,
    TECHNIQUE_WIREFRAME_SOLID = 0x1C,
    TECHNIQUE_WIREFRAME_SHADED = 0x1D,
    TECHNIQUE_SHADOWCOOKIE_CASTER = 0x1E,
    TECHNIQUE_SHADOWCOOKIE_RECEIVER = 0x1F,
    TECHNIQUE_DEBUG_BUMPMAP = 0x20,
    TECHNIQUE_DEBUG_BUMPMAP_INSTANCED = 0x21,
    //TECHNIQUE_COUNT = 0x22
};

struct infoParm_t
{
    const char* name;
    int clearSolid;
    int surfaceFlags;
    int contents;
    int toolFlags;
};

struct WaterWritable
{
    float floatTime;
};

struct complex_s
{
    float real;
    float imag;
};

struct water_t
{
    WaterWritable writable;
    complex_s* H0;
    float* wTerm;
    int M;
    int N;
    float Lx;
    float Lz;
    float gravity;
    float windvel;
    float winddir[2];
    float amplitude;
    float codeConstant[4];
    GfxImage* image;
};

union MaterialTextureDefInfo
{
    GfxImage* image;	// MaterialTextureDef->semantic != TS_WATER_MAP
    water_t* water;		// MaterialTextureDef->semantic == TS_WATER_MAP
};

struct MaterialTextureDef
{
    unsigned int nameHash;
    char nameStart;
    char nameEnd;
    char samplerState;
    char semantic;
    MaterialTextureDefInfo u;
};

struct MaterialConstantDef
{
    int nameHash;
    char name[12];
    vec4_t literal;
};

struct GfxStateBits
{
    unsigned int loadBits[2];
};

struct Material
{
    MaterialInfo info;
    char stateBitsEntry[34];
    char textureCount;
    char constantCount;
    char stateBitsCount;
    char stateFlags;
    char cameraRegion;
    MaterialTechniqueSet* techniqueSet;
    MaterialTextureDef* textureTable;
    MaterialConstantDef* constantTable;
    GfxStateBits* stateBitsTable;
};

enum MaterialVertexDeclType
{
    VERTDECL_GENERIC = 0x0,
    VERTDECL_PACKED = 0x1,
    VERTDECL_WORLD = 0x2,
    VERTDECL_WORLD_T1N0 = 0x3,
    VERTDECL_WORLD_T1N1 = 0x4,
    VERTDECL_WORLD_T2N0 = 0x5,
    VERTDECL_WORLD_T2N1 = 0x6,
    VERTDECL_WORLD_T2N2 = 0x7,
    VERTDECL_WORLD_T3N0 = 0x8,
    VERTDECL_WORLD_T3N1 = 0x9,
    VERTDECL_WORLD_T3N2 = 0xA,
    VERTDECL_WORLD_T4N0 = 0xB,
    VERTDECL_WORLD_T4N1 = 0xC,
    VERTDECL_WORLD_T4N2 = 0xD,
    VERTDECL_POS_TEX = 0xE,
    VERTDECL_STATICMODELCACHE = 0xF,
    VERTDECL_COUNT = 0x10,
};

struct gfxVertexSteamsUnk
{
    unsigned int stride;
    int* vb; // IDirect3DVertexBuffer9
    unsigned int offset;
};

struct GfxCmdBufPrimState
{
    IDirect3DDevice9* device; // IDirect3DDevice9
    int* indexBuffer; // IDirect3DIndexBuffer9
    MaterialVertexDeclType vertDeclType;
    gfxVertexSteamsUnk streams[2];
    int* vertexDecl; // IDirect3DVertexDeclaration9
};

struct GfxViewport
{
    int x;
    int y;
    int width;
    int height;
};

enum GfxRenderTargetId
{
    R_RENDERTARGET_SAVED_SCREEN = 0x0,
    R_RENDERTARGET_FRAME_BUFFER = 0x1,
    R_RENDERTARGET_SCENE = 0x2,
    R_RENDERTARGET_RESOLVED_POST_SUN = 0x3,
    R_RENDERTARGET_RESOLVED_SCENE = 0x4,
    R_RENDERTARGET_FLOAT_Z = 0x5,
    R_RENDERTARGET_DYNAMICSHADOWS = 0x6,
    R_RENDERTARGET_PINGPONG_0 = 0x7,
    R_RENDERTARGET_PINGPONG_1 = 0x8,
    R_RENDERTARGET_SHADOWCOOKIE = 0x9,
    R_RENDERTARGET_SHADOWCOOKIE_BLUR = 0xA,
    R_RENDERTARGET_POST_EFFECT_0 = 0xB,
    R_RENDERTARGET_POST_EFFECT_1 = 0xC,
    R_RENDERTARGET_SHADOWMAP_SUN = 0xD,
    R_RENDERTARGET_SHADOWMAP_SPOT = 0xE,
    R_RENDERTARGET_COUNT = 0xF,
    R_RENDERTARGET_NONE = 0x10,
};

enum GfxDepthRangeType
{
    GFX_DEPTH_RANGE_SCENE = 0x0,
    GFX_DEPTH_RANGE_VIEWMODEL = 0x2,
    GFX_DEPTH_RANGE_FULL = 0xFFFFFFFF,
};

struct GfxCmdBufState
{
    char refSamplerState[16];
    unsigned int samplerState[16];
    GfxTexture* samplerTexture[16];
    GfxCmdBufPrimState prim;
    Material* material;
    MaterialTechniqueType techType;
    MaterialTechnique* technique;
    MaterialPass* pass;
    unsigned int passIndex;
    GfxDepthRangeType depthRangeType;
    float depthRangeNear;
    float depthRangeFar;
    unsigned __int64 vertexShaderConstState[32];
    unsigned __int64 pixelShaderConstState[256];
    char alphaRef;
    unsigned int refStateBits[2];
    unsigned int activeStateBits[2];
    MaterialPixelShader* pixelShader;
    MaterialVertexShader* vertexShader;
    GfxViewport viewport;
    GfxRenderTargetId renderTargetId;
    Material* origMaterial;
    MaterialTechniqueType origTechType;
};

struct __declspec(align(4)) GfxLightImage
{
    GfxImage* image;
    char samplerState;
};

struct GfxLightDef
{
    const char* name;
    GfxLightImage attenuation;
    int lmapLookupStart;
};

struct GfxWorldStreamInfo
{
    int aabbTreeCount;
    // 			GfxStreamingAabbTree *aabbTrees;
    // 			int leafRefCount;
    // 			int *leafRefs;
};

union PackedUnitVec
{
    unsigned int packed;
    char array[4];
};

struct GfxWorldVertex
{
    float xyz[3];
    float binormalSign;
    GfxColor color;
    float texCoord[2];
    float lmapCoord[2];
    PackedUnitVec normal;
    PackedUnitVec tangent;
};

struct GfxWorldVertexData
{
    GfxWorldVertex* vertices;
    void/*IDirect3DVertexBuffer9*/* worldVb;
};

struct GfxWorldVertexLayerData
{
    char* data;
    void/*IDirect3DVertexBuffer9*/* layerVb;
};

struct SunLightParseParams
{
    char name[64];
    float ambientScale;
    float ambientColor[3];
    float diffuseFraction;
    float sunLight;
    float sunColor[3];
    float diffuseColor[3];
    char diffuseColorHasBeenSet;
    float angles[3];
};

struct GfxLight
{
    char type;
    char canUseShadowMap;
    char unused[2];
    float color[3];
    float dir[3];
    float origin[3];
    float radius;
    float cosHalfFovOuter;
    float cosHalfFovInner;
    int exponent;
    unsigned int spotShadowIndex;
    GfxLightDef* def;
};

struct GfxReflectionProbe
{
    float origin[3];
    GfxImage* reflectionImage;
};

struct GfxWorldDpvsPlanes
{
    int cellCount;
    cplane_s* planes;
    unsigned __int16* nodes;
    unsigned int* sceneEntCellBits;
};

struct GfxAabbTree
{
    float mins[3];
    float maxs[3];
    unsigned __int16 childCount;
    unsigned __int16 surfaceCount;
    unsigned __int16 startSurfIndex;
    unsigned __int16 surfaceCountNoDecal;
    unsigned __int16 startSurfIndexNoDecal;
    unsigned __int16 smodelIndexCount;
    unsigned __int16* smodelIndexes;
    int childrenOffset;
};

struct GfxPortal;

struct GfxPortalWritable
{
    char isQueued;
    char isAncestor;
    char recursionDepth;
    char hullPointCount;
    float(*hullPoints)[2];
    GfxPortal* queuedParent;
};

struct DpvsPlane
{
    float coeffs[4];
    char side[3];
    char pad;
};

struct GfxCell;

struct GfxPortal
{
    GfxPortalWritable writable;
    DpvsPlane plane;
    GfxCell* cell;
    float(*vertices)[3];
    char vertexCount;
    float hullAxis[2][3];
};

struct GfxCell
{
    float mins[3];
    float maxs[3];
    int aabbTreeCount;
    GfxAabbTree* aabbTree;
    int portalCount;
    GfxPortal* portals;
    int cullGroupCount;
    int* cullGroups;
    char reflectionProbeCount;
    char* reflectionProbes;
};

struct GfxLightmapArray
{
    GfxImage* primary;
    GfxImage* secondary;
};

struct GfxLightGridEntry
{
    unsigned __int16 colorsIndex;
    char primaryLightIndex;
    char needsTrace;
};

struct GfxLightGridColors
{
    char rgb[56][3];
};

struct GfxLightGrid
{
    char hasLightRegions;
    unsigned int sunPrimaryLightIndex;
    unsigned __int16 mins[3];
    unsigned __int16 maxs[3];
    unsigned int rowAxis;
    unsigned int colAxis;
    unsigned __int16* rowDataStart;
    unsigned int rawRowDataSize;
    char* rawRowData;
    unsigned int entryCount;
    GfxLightGridEntry* entries;
    unsigned int colorCount;
    GfxLightGridColors* colors;
};

struct GfxBrushModelWritable
{
    float mins[3];
    float maxs[3];
};

struct __declspec(align(4)) GfxBrushModel
{
    GfxBrushModelWritable writable;
    float bounds[2][3];
    unsigned __int16 surfaceCount;
    unsigned __int16 startSurfIndex;
    unsigned __int16 surfaceCountNoDecal;
};

struct MaterialMemory
{
    Material* material;
    int memory;
};

struct sunflare_t
{
    char hasValidData;
    Material* spriteMaterial;
    Material* flareMaterial;
    float spriteSize;
    float flareMinSize;
    float flareMinDot;
    float flareMaxSize;
    float flareMaxDot;
    float flareMaxAlpha;
    int flareFadeInTime;
    int flareFadeOutTime;
    float blindMinDot;
    float blindMaxDot;
    float blindMaxDarken;
    int blindFadeInTime;
    int blindFadeOutTime;
    float glareMinDot;
    float glareMaxDot;
    float glareMaxLighten;
    int glareFadeInTime;
    int glareFadeOutTime;
    float sunFxPosition[3];
};

struct XModelDrawInfo
{
    unsigned __int16 lod;
    unsigned __int16 surfId;
};

struct GfxSceneDynModel
{
    XModelDrawInfo info;
    unsigned __int16 dynEntId;
};

struct BModelDrawInfo
{
    unsigned __int16 surfId;
};

struct GfxSceneDynBrush
{
    BModelDrawInfo info;
    unsigned __int16 dynEntId;
};

struct GfxShadowGeometry
{
    unsigned __int16 surfaceCount;
    unsigned __int16 smodelCount;
    unsigned __int16* sortedSurfIndex;
    unsigned __int16* smodelIndex;
};

struct GfxLightRegionAxis
{
    float dir[3];
    float midPoint;
    float halfSize;
};

struct GfxLightRegionHull
{
    float kdopMidPoint[9];
    float kdopHalfSize[9];
    unsigned int axisCount;
    GfxLightRegionAxis* axis;
};

struct GfxLightRegion
{
    unsigned int hullCount;
    GfxLightRegionHull* hulls;
};

struct GfxPackedPlacement
{
    float origin[3];
    vec3_t axis[3];
    float scale;
};

struct XModelLodInfo
{
    float dist;
    unsigned __int16 numsurfs;
    unsigned __int16 surfIndex;
    int partBits[4];
    char lod;
    char smcIndexPlusOne;
    char smcAllocBits;
    char unused;
};

struct XModelStreamInfo
{
    int* highMipBounds;
};

struct XModel
{
    const char* name;
    char numBones;
    char numRootBones;
    unsigned char numsurfs;
    char lodRampType;
    unsigned __int16* boneNames;
    char* parentList;
    __int16* quats;
    float* trans;
    char* partClassification;
    int* baseMat;
    int* surfs;
    Material** materialHandles;
    XModelLodInfo lodInfo[4];
    int* collSurfs;
    int numCollSurfs;
    int contents;
    int* boneInfo;
    float radius;
    float mins[3];
    float maxs[3];
    __int16 numLods;
    __int16 collLod;
    XModelStreamInfo streamInfo;
    int memUsage;
    char flags;
    bool bad;
    int* physPreset;
    int* physGeoms;
};

struct __declspec(align(4)) GfxStaticModelDrawInst
{
    float cullDist;
    GfxPackedPlacement placement;
    XModel* model;
    unsigned __int16 smodelCacheIndex[4];
    char reflectionProbeIndex;
    char primaryLightIndex;
    unsigned __int16 lightingHandle;
    char flags;
};

struct srfTriangles_t
{
    int vertexLayerData;
    int firstVertex;
    unsigned __int16 vertexCount;
    unsigned __int16 triCount;
    int baseIndex;
};

struct GfxSurface
{
    srfTriangles_t tris;
    Material* material;
    char lightmapIndex;
    char reflectionProbeIndex;
    char primaryLightIndex;
    char flags;
    float bounds[2][3];
};

struct GfxStaticModelInst
{
    float mins[3];
    float maxs[3];
    GfxColor groundLighting;
};

struct GfxCullGroup
{
    float mins[3];
    float maxs[3];
    int surfaceCount;
    int startSurfIndex;
};

struct GfxWorldDpvsStatic
{
    unsigned int smodelCount;
    unsigned int staticSurfaceCount;
    unsigned int staticSurfaceCountNoDecal;
    unsigned int litSurfsBegin;
    unsigned int litSurfsEnd;
    unsigned int decalSurfsBegin;
    unsigned int decalSurfsEnd;
    unsigned int emissiveSurfsBegin;
    unsigned int emissiveSurfsEnd;
    unsigned int smodelVisDataCount;
    unsigned int surfaceVisDataCount;
    char* smodelVisData[3];
    char* surfaceVisData[3];
    unsigned int* lodData;
    unsigned __int16* sortedSurfIndex;
    GfxStaticModelInst* smodelInsts;
    GfxSurface* surfaces;
    GfxCullGroup* cullGroups;
    GfxStaticModelDrawInst* smodelDrawInsts;
    GfxDrawSurf* surfaceMaterials;
    unsigned int* surfaceCastsSunShadow;
    volatile int usageCount;
};

struct GfxWorldDpvsDynamic
{
    unsigned int dynEntClientWordCount[2];
    unsigned int dynEntClientCount[2];
    unsigned int* dynEntCellBits[2];
    char* dynEntVisData[2][3];
};

struct GfxWorld
{
    const char* name;
    const char* baseName;
    int planeCount;
    int nodeCount;
    int indexCount;
    unsigned __int16* indices;
    int surfaceCount;
    GfxWorldStreamInfo streamInfo;
    int skySurfCount;
    int* skyStartSurfs;
    GfxImage* skyImage;
    char skySamplerState;
    unsigned int vertexCount;
    GfxWorldVertexData vd;
    unsigned int vertexLayerDataSize;
    GfxWorldVertexLayerData vld;
    SunLightParseParams sunParse;
    GfxLight* sunLight;
    float sunColorFromBsp[3];
    unsigned int sunPrimaryLightIndex;
    unsigned int primaryLightCount;
    int cullGroupCount;
    unsigned int reflectionProbeCount;
    GfxReflectionProbe* reflectionProbes;
    GfxTexture* reflectionProbeTextures;
    GfxWorldDpvsPlanes dpvsPlanes;
    int cellBitsCount;
    GfxCell* cells;
    int lightmapCount;
    GfxLightmapArray* lightmaps;
    GfxLightGrid lightGrid;
    GfxTexture* lightmapPrimaryTextures;
    GfxTexture* lightmapSecondaryTextures;
    int modelCount;
    GfxBrushModel* models;
    float mins[3];
    float maxs[3];
    unsigned int checksum;
    int materialMemoryCount;
    MaterialMemory* materialMemory;
    sunflare_t sun;
    float outdoorLookupMatrix[4][4];
    GfxImage* outdoorImage;
    unsigned int* cellCasterBits;
    GfxSceneDynModel* sceneDynModel;
    GfxSceneDynBrush* sceneDynBrush;
    unsigned int* primaryLightEntityShadowVis;
    unsigned int* primaryLightDynEntShadowVis[2];
    char* nonSunPrimaryLightForModelDynEnt;
    GfxShadowGeometry* shadowGeom;
    GfxLightRegion* lightRegion;
    GfxWorldDpvsStatic dpvs;
    GfxWorldDpvsDynamic dpvsDyn;
};

struct GfxVertex
{
    float xyzw[4];
    GfxColor color;
    float texCoord[2];
    PackedUnitVec normal;
};

struct materialCommands_t
{
    GfxVertex verts[5450];
    unsigned __int16 indices[1048576];
    MaterialVertexDeclType vertDeclType;
    unsigned int vertexSize;
    int indexCount;
    int vertexCount;
    int firstVertex;
    int lastVertex;
    bool finishedFilling;
    char pad[7];
};

#pragma warning( push )
#pragma warning( disable : 4324 )
struct __declspec(align(128)) r_global_permanent_t
{
    Material* sortedMaterials[2048];
    int needSortMaterials;
    int materialCount;
    GfxImage* whiteImage;
    GfxImage* blackImage;
    GfxImage* blackImage3D;
    GfxImage* blackImageCube;
    GfxImage* grayImage;
    GfxImage* identityNormalMapImage;
    GfxImage* specularityImage;
    GfxImage* outdoorImage;
    GfxImage* pixelCostColorCodeImage;
    GfxLightDef* dlightDef;
    Material* defaultMaterial;
    Material* whiteMaterial;
    Material* additiveMaterial;
    Material* pointMaterial;
    Material* lineMaterial;
    Material* lineMaterialNoDepth;
    Material* clearAlphaStencilMaterial;
    Material* shadowClearMaterial;
    Material* shadowCookieOverlayMaterial;
    Material* shadowCookieBlurMaterial;
    Material* shadowCasterMaterial;
    Material* shadowOverlayMaterial;
    Material* depthPrepassMaterial;
    Material* glareBlindMaterial;
    Material* pixelCostAddDepthAlwaysMaterial;
    Material* pixelCostAddDepthDisableMaterial;
    Material* pixelCostAddDepthEqualMaterial;
    Material* pixelCostAddDepthLessMaterial;
    Material* pixelCostAddDepthWriteMaterial;
    Material* pixelCostAddNoDepthWriteMaterial;
    Material* pixelCostColorCodeMaterial;
    Material* stencilShadowMaterial;
    Material* stencilDisplayMaterial;
    Material* floatZDisplayMaterial;
    Material* colorChannelMixerMaterial;
    Material* frameColorDebugMaterial;
    Material* frameAlphaDebugMaterial;
    GfxImage* rawImage;
    GfxWorld* world;
    Material* feedbackReplaceMaterial;
    Material* feedbackBlendMaterial;
    Material* feedbackFilmBlendMaterial;
    Material* cinematicMaterial;
    Material* dofDownsampleMaterial;
    Material* dofNearCocMaterial;
    Material* smallBlurMaterial;
    Material* postFxDofMaterial;
    Material* postFxDofColorMaterial;
    Material* postFxColorMaterial;
    Material* postFxMaterial;
    Material* symmetricFilterMaterial[8];
    Material* shellShockBlurredMaterial;
    Material* shellShockFlashedMaterial;
    Material* glowConsistentSetupMaterial;
    Material* glowApplyBloomMaterial;
    int savedScreenTimes[4];
};
#pragma warning( pop )

///////////////////////

#endif
