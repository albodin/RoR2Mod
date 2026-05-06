#pragma once
#include "utils/Math.hpp"
#include <cstdint>
#include <string>
#include <vector>

struct LocalUser;
struct PlayerStatsComponent;
struct CharacterMaster;
struct CharacterBody;
struct CameraRigController;
struct CameraTargetParams;
struct RendererVisiblity;
struct EntityStateMachine;
struct ExpansionDef;
struct ProjectileController;
struct UnlockableDef;
struct BodyAnimatorSmoothingParameters;
struct CharacterEmoteDefinitions;
struct HealthComponent;
struct RailMotor;
struct RigidbodyDirection;
struct CharacterEmoteDefinitions;
struct RigidbodyMotor;
struct TeamComponent;
struct ModelLocator;
struct HurtBoxGroup;
struct NetworkUser;
struct GenericSkill;
struct SkillDef;
struct HuntressPrimarySkillDef;
struct SkillLocator;
struct SkillFamily;
struct MultiShopController;
struct PickupDisplay;
struct CharacterMotor;
struct ChestBehavior;
struct DroneMask;
struct ChildLocator;
struct Component;
struct OnSpawnedWithDirectorServer;
struct EncounterHealthThresholdController;
struct BossShrineCounter;
struct Event;
struct UniquePickupUnityEvent;
struct ExplicitPickupDropTable;
struct DetailedPurchaseEvent;
struct SpecialObjectAttributes;
struct NetworkInstanceId;
struct Inventory;

typedef void* GameObject;
typedef void* NetworkIdentity;
typedef void* UnityEvent;
typedef void* BaseAI;
typedef void* MinionOwnership;
typedef void* Loadout;
typedef void* Xoroshiro128Plus;
typedef void* DevotionInventoryController;
typedef void* Interactor;
typedef void* Player;
typedef void* MPEventSystem;
typedef void* UserProfile;
typedef void* PauseScreenController;
typedef void* Action;
typedef void* StatSheet;
typedef void* Transform;
typedef void* Camera;
typedef void* ParticleSystem;
typedef void* CameraModeBase;
typedef void* HUD;
typedef void* ICameraStateProvider;
typedef void* HurtBox;
typedef void* InputBankTest;
typedef void* NetworkedViewAngles;
typedef void* CharacterCameraParams;
typedef void* PingerController;
typedef void* BuffIndex;
typedef void* JumpDelegate;
typedef void* Rigidbody;
typedef void* CharacterDirection;
typedef void* EquipmentSlot;
typedef void* SfxLocator;
typedef void* EffectManagerHelper;
typedef void* IncreaseDamageOnMultiKillItemDisplayUpdater;
typedef void* ScreenDamageCalculatorDisabledSkills;
typedef void* BoostAllStatsDisplay;
typedef void* HelfireController;
typedef void* DamageTrail;
typedef void* TemporaryVisualEffect;
typedef void* AnimationCurve;
typedef void* DamageInfo;
typedef void* OnShardSpawned;
typedef void* Texture;
typedef void* VehicleSeat;
typedef void* IPainAnimationHandler;
typedef void* IOnIncomingDamageServerReceiver;
typedef void* IOnTakeDamageServerReceiver;
typedef void* ScreenDamageCalculator;
typedef void* RepeatHealComponent;
typedef void* EquipmentDef;
typedef void* Collider;
typedef void* NetworkConnection;
typedef void* NodeGraph;
typedef void* Animator;
typedef void* Material;
typedef void* LimbFlagSet;
typedef void* ItemMask;
typedef void* SkinnedMeshRenderer;
typedef void* MaterialPropertyBlock;
typedef void* LightInfo;
typedef void* RendererInfo;
typedef void* ItemDisplayRuleSet;
typedef void* String;
typedef void* ObjectScaleCurve;
typedef void* TextMeshPro;
typedef void* SpriteRenderer;
typedef void* SceneCollection;
typedef void* MusicTrackDef;
typedef void* AssetReferenceScene;
typedef void* CalcColorDelegate;
typedef void* CalcAccumulatedChargeDelegate;
typedef void* CalcPercentageOfPlayersInRadiusDelegate;
typedef void* CalcChargeRateDelegate;
typedef void* CalcRadiusDelegate;
typedef void* HoldoutZoneControllerChargedUnityEvent;
typedef void* Renderer;
typedef void* Sprite;
typedef void* IEnumerator;
typedef void* ModifyNextStateDelegate;
typedef void* OnSpawnedServer;
typedef void* NetworkLoadout;
typedef void* BaseSkillInstanceData;
typedef void* SkillOverride;
typedef void* StateMachineResolver;
typedef void* Indicator;
typedef void* Variant;
typedef void* Entry;
typedef void* Boolean;
typedef void* NetworkUIPromptController;
typedef void* InspectDef;
typedef void* IInspectInfoProvider;
typedef void* GameObjectUnlockableFilter;
typedef void* PurchaseEvent;
typedef void* InteractorUnityEvent;
typedef void* IDisplayNameProvider;
typedef void* AttemptGrantDelegate;
typedef void* AssetReferenceTexture;
typedef void* AssetReferenceGameObject;
typedef void* NetworkRuleBook;
typedef void* EquipmentMask;
typedef void* CapsuleCollider;
typedef void* HitGroundDelegate;
typedef void* MovementHitDelegate;
typedef void* SerializableLoadout;
typedef void* ItemCountPair;
typedef void* PickupIndexUnityEvent;
typedef void* PickupPickerPanel;
typedef void* Option;
typedef void* EventFunctions;

typedef int32_t Int32;
typedef unsigned char Byte;
typedef int32_t PickupIndex_Value;
typedef uint8_t TeamMask_Value;
typedef uint32_t UInt32;
typedef uint64_t UInt64;

struct MonoObject_Internal {
    void* klass;     // pointer to the array’s element class
    void* gc_header; // GC‐bookkeeping (locks, color bits, etc)
};

struct MonoArrayBounds {
    uint32_t length;
    int32_t lower_bound;
};

struct MonoArray_Internal {
    MonoObject_Internal object;
    MonoArrayBounds* bounds;
    uintptr_t max_length;
    uintptr_t vector_arr[0];
};

template <typename T> static T* mono_array_addr(MonoArray_Internal* arr) {
    return reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(arr) + offsetof(MonoArray_Internal, vector_arr));
}
/*
    uint32_t len = static_cast<uint32_t>((reinterpret_cast<MonoArray_Internal*>(itemDefsArray))->max_length);
    ItemDef** data = mono_array_addr<ItemDef*>(reinterpret_cast<MonoArray_Internal*>(itemDefsArray));
    for (uint32_t i = 0; i < len; ++i) {
        ItemDef* obj = data[i];
        // ... inspect obj, e.g. via mono_object_get_class(obj) etc.
    }
*/

#pragma pack(push, 1)

enum class TeamIndex_Value : int8_t { None = -1, Neutral = 0, Player = 1, Monster = 2, Lunar = 3, Void = 4, Count = 5 };

enum class BodyFlags_Value : uint32_t {
    None = 0,
    IgnoreFallDamage = 1,
    Mechanical = 2,
    Masterless = 4,
    ImmuneToGoo = 8,
    ImmuneToExecutes = 16,     // 0x00000010
    SprintAnyDirection = 32,   // 0x00000020
    ResistantToAOE = 64,       // 0x00000040
    HasBackstabPassive = 128,  // 0x00000080
    HasBackstabImmunity = 256, // 0x00000100
    OverheatImmune = 512,      // 0x00000200
    Void = 1024,               // 0x00000400
    ImmuneToVoidDeath = 2048,  // 0x00000800
    IgnoreItemUpdates = 4096,  // 0x00001000
    Devotion = 8192,           // 0x00002000
    IgnoreKnockback = 16384,   // 0x00004000
    ImmuneToLava = 32768,      // 0x00008000
    UsesAmbientLevel = 65536,  // 0x00010000
};

enum class EquipmentIndex_Value : int32_t {
    None = -1, // 0xFFFFFFFF
};

enum class HullClassification_Value : int32_t {
    Human = 0,
    Golem = 1,
    BeetleQueen = 2,
    Count = 3,
};

enum class ItemTier_Value {
    Tier1 = 0,
    Tier2 = 1,
    Tier3 = 2,
    Lunar = 3,
    Boss = 4,
    NoTier = 5,
    VoidTier1 = 6,
    VoidTier2 = 7,
    VoidTier3 = 8,
    VoidBoss = 9,
    AssignedAtRuntime = 10,
};

enum class ItemTag_Value {
    Any = 0,
    Damage = 1,
    Healing = 2,
    Utility = 3,
    AIBlacklist = 4,
    Cleansable = 5,
    OnKillEffect = 6,
    EquipmentRelated = 7,
    SprintRelated = 8,
    WorldUnique = 9,
    Scrap = 10,
    BrotherBlacklist = 11,
    CannotSteal = 12,
    CannotCopy = 13,
    PriorityScrap = 14,
    CannotDuplicate = 15,
    LowHealth = 16,
    HoldoutZoneRelated = 17,
    InteractableRelated = 18,
    ObliterationRelated = 19,
    OnStageBeginEffect = 20,
    HalcyoniteShrine = 21,
    RebirthBlacklist = 22,
    DevotionBlacklist = 23,
    SacrificeBlacklist = 24,
    Count = 25,
};

enum class MonoOrStereoscopicEye : int32_t {
    Left = 0,
    Right = 1,
    Mono = 2,
};

enum class QueryTriggerInteraction_Value : int32_t { UseGlobal = 0, Ignore = 1, Collide = 2 };

enum class InterruptPriority_Value : int32_t { Any = 0, Skill = 1, PrioritySkill = 2, Pain = 3, Stun = 4, Frozen = 5, Vehicle = 6, Death = 7 };

enum class CostTypeIndex_Value : int32_t {
    None = 0,
    Money = 1,
    PercentHealth = 2,
    LunarCoin = 3,
    WhiteItem = 4,
    GreenItem = 5,
    RedItem = 6,
    Equipment = 7,
    VolatileBattery = 8,
    LunarItemOrEquipment = 9,
    BossItem = 10,
    ArtifactShellKillerItem = 11,
    TreasureCacheItem = 12,
    TreasureCacheVoidItem = 13,
    VoidCoin = 14,
    SoulCost = 15,
    Count = 16
};

enum class SortMode_Value : int32_t { None = 0, Distance = 1, Angle = 2, DistanceAndAngle = 3 };

enum class Interactability_Value : int32_t { Disabled = 0, ConditionsNotMet = 1, Available = 2 };

enum class HighlightColor_Value : int32_t { interactive = 0, teleporter = 1, pickup = 2, custom = 3, unavailable = 4 };

enum class MiscPickupIndex_Value : int32_t { None = -1 };

enum class ColorIndex_Value : int32_t {
    None = 0,
    Tier1Item = 1,
    Tier2Item = 2,
    Tier3Item = 3,
    LunarItem = 4,
    Equipment = 5,
    Interactable = 6,
    Teleporter = 7,
    Money = 8,
    Blood = 9,
    Unaffordable = 10,
    Unlockable = 11,
    LunarCoin = 12,
    BossItem = 13,
    Error = 14,
    EasyDifficulty = 15,
    NormalDifficulty = 16,
    HardDifficulty = 17,
    Tier1ItemDark = 18,
    Tier2ItemDark = 19,
    Tier3ItemDark = 20,
    LunarItemDark = 21,
    BossItemDark = 22,
    WIP = 23,
    Artifact = 24,
    VoidItem = 25,
    VoidItemDark = 26,
    VoidCoin = 27,
    Count = 28
};

enum class PickupRules_Value : int32_t { Default = 0, ConfirmFirst = 1, ConfirmAll = 2 };

enum class GameModeIndex_Value : int32_t { Invalid = -1 };

enum class DeployableSlot_Value : int32_t {
    EngiMine = 0,
    EngiTurret = 1,
    BeetleGuardAlly = 2,
    EngiBubbleShield = 3,
    LoaderPylon = 4,
    EngiSpiderMine = 5,
    RoboBallMini = 6,
    ParentPodAlly = 7,
    ParentAlly = 8,
    PowerWard = 9,
    CrippleWard = 10,
    DeathProjectile = 11,
    RoboBallRedBuddy = 12,
    RoboBallGreenBuddy = 13,
    GummyClone = 14,
    RailgunnerBomb = 15,
    LunarSunBomb = 16,
    VendingMachine = 17,
    VoidMegaCrabItem = 18,
    DroneWeaponsDrone = 19,
    MinorConstructOnKill = 20,
    CaptainSupplyDrop = 21,
    None = 22
};

enum class PickupArtifactFlag_Value : int32_t {
    NONE = 0,
    COMMAND = 1,
    DELUSION = 2,
};

enum class ExitState_Value : int32_t {
    Idle = 0,
    ExtractExp = 1,
    TeleportOut = 2,
    Finished = 3,
};

/* No Header Structs */

struct Quaternion {
    float x;
    float y;
    float z;
    float w;
}; // Size: 16

struct Rect {
    float m_XMin;
    float m_YMin;
    float m_Width;
    float m_Height;
}; // Size: 16

// Generated from RoR2.CameraState
struct CameraState_Value {
    Vector3 position;
    Quaternion rotation;
    float fov;
}; // Size: 32

// Generated from .AimAssistInfo
struct AimAssistInfo_Value {
    HurtBox* aimAssistHurtbox;
    Vector3 localPositionOnHurtbox;
    Vector3 worldPosition;
}; // Size: 32

struct BlendableFloat_Value {
    float value;
    float alpha;
};

struct BlendableVector3_Value {
    Vector3 value;
    float alpha;
};

struct BlendableBool_Value {
    bool value;
    char padding0[3]; // Padding
    float alpha;
};

// Generated from .CameraInfo
struct CameraInfo_Value {
    CameraRigController* cameraRigController;
    Camera* sceneCam;
    ICameraStateProvider* overrideCam;
    CameraState_Value previousCameraState;
    float baseFov;
};

// Generated from .TargetInfo
struct TargetInfo_Value {
    GameObject* target;
    CharacterBody* body;
    InputBankTest* inputBank;
    CameraTargetParams* targetParams;
    TeamIndex_Value teamIndex;
    bool isSprinting;
    bool skillsAreDisabled;
    bool showSecondaryElectricity;
    bool isViewerControlled;
    char padding9[3];
    CharacterMaster* master;
    NetworkUser* networkUser;
    NetworkedViewAngles* networkedViewAngles;
}; // Size: 64

// Generated from .ViewerInfo
struct ViewerInfo_Value {
    LocalUser* localUser;
    UserProfile* userProfile;
    Player* inputPlayer;
    MPEventSystem* eventSystem;
    bool hasCursor;
    bool isUIFocused;
}; // Size: 40

// Generated from .CameraModeContext
struct CameraModeContext_Value {
    CameraInfo_Value cameraInfo;
    char padding1[4];
    TargetInfo_Value targetInfo;
    ViewerInfo_Value viewerInfo;
}; // Size: 168

// Generated from RoR2.CharacterCameraParamsData
struct CharacterCameraParamsData_Value {
    BlendableFloat_Value minPitch;
    BlendableFloat_Value maxPitch;
    BlendableFloat_Value wallCushion;
    BlendableFloat_Value pivotVerticalOffset;
    BlendableVector3_Value idealLocalCameraPos;
    BlendableFloat_Value fov;
    BlendableBool_Value isFirstPerson;
    float overrideFirstPersonFadeDuration;
};

struct NetworkInstanceId_Value {
    uint32_t m_Value;
}; // Size: 4

struct BodyIndex_Value {
    uint32_t value__;
}; // Size: 4

struct CharacterMaster_SEEKER_REVIVE_STATUS_Value {
    uint32_t value__;
}; // Size: 4

struct ItemIndex_Value {
    uint32_t value__;
}; // Size: 4

struct Type_Value {
    int32_t value__;
}; // Size: 4

struct SerializableEntityStateType_Value {
    Type_Value* _stateType;
    void* _typeName;
}; // Size: 16

// Generated from .ItemAvailability
struct ItemAvailability_Value {
    int32_t helfireCount;
    bool hasFireTrail;
    bool hasAffixLunar;
    bool hasAffixPoison;
}; // Size: 7

struct Color_Value {
    float r;
    float g;
    float b;
    float a;
}; // Size: 8

struct FixedTimeStamp_Value {
    float t;
}; // Size: 4

// Generated from .ItemCounts
struct ItemCounts_Value {
    int32_t bear;
    int32_t armorPlate;
    int32_t goldOnHit;
    int32_t goldOnHurt;
    int32_t phasing;
    int32_t thorns;
    int32_t invadingDoppelganger;
    int32_t medkit;
    int32_t parentEgg;
    int32_t fragileDamageBonus;
    int32_t minHealthPercentage;
    int32_t noxiousThorn;
    int32_t elusiveAntlers;
    int32_t ultimateMeal;
    int32_t increaseHealing;
    int32_t barrierOnOverHeal;
    int32_t repeatHeal;
    int32_t novaOnHeal;
    int32_t adaptiveArmor;
    int32_t healingPotion;
    int32_t infusion;
    int32_t missileVoid;
    int32_t unstableTransmitter;
};

// Generated from .KeyValueSplitter
struct KeyValueSplitter_Value {
    void* baseKey;
    int32_t maxKeyLength;
    int32_t maxValueLength;
    void* currentSubKeys;
    void* keyValueSetter;
};

struct GraphType_Value {
    int32_t value__;
};

struct NodeFlags_Value {
    uint8_t value__;
};

struct EliteRules_Value {
    int32_t value__;
};

struct ExpansionIndex_Value {
    int32_t value__;
};

struct RuleCategoryType_Value {
    int32_t value__;
};

struct EntitlementIndex_Value {
    int32_t value__;
};

struct DifficultyIndex_Value {
    int32_t value__;
};

struct ArtifactIndex_Value {
    int32_t value__;
};

struct UnlockableIndex_Value {
    int32_t value__;
};

struct MonsterSpawnDistance_Value {
    int32_t value__;
};

struct EliteIndex_Value {
    int32_t value__;
};

struct Color32_Value {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct DevotionEvolutionLevel_Value {
    int32_t value__;
};

// Generated from .CommonComponentCache
struct CommonComponentCache_Value {
    Transform* transform;
    CharacterBody* characterBody;
    CharacterMotor* characterMotor;
    CharacterDirection* characterDirection;
    Rigidbody* rigidbody;
    RigidbodyMotor* rigidbodyMotor;
    RigidbodyDirection* rigidbodyDirection;
    RailMotor* railMotor;
    ModelLocator* modelLocator;
    InputBankTest* inputBank;
    TeamComponent* teamComponent;
    HealthComponent* healthComponent;
    SkillLocator* skillLocator;
    CharacterEmoteDefinitions* characterEmoteDefinitions;
    CameraTargetParams* cameraTargetParams;
    SfxLocator* sfxLocator;
    BodyAnimatorSmoothingParameters* bodyAnimatorSmoothingParameters;
    ProjectileController* projectileController;
};

struct NetworkSoundEventIndex_Value {
    int32_t value__;
};

struct BuffIndex_Value {
    int32_t value__;
};

struct Flags_Value {
    int32_t value__;
};

struct BuffWardShape_Value {
    int32_t value__;
};

struct HoldoutZoneShape_Value {
    int32_t value__;
};

struct SceneIndex_Value {
    int32_t value__;
};

struct DroneIndex_Value {
    int32_t value__;
};

struct StackingDisplayMethod_Value {
    int32_t value__;
};

struct SceneType_Value {
    int32_t value__;
};

// Generated from .SmoothingParameters
struct SmoothingParameters_Value {
    float walkMagnitudeSmoothDamp;
    float walkAngleSmoothDamp;
    float forwardSpeedSmoothDamp;
    float rightSpeedSmoothDamp;
    float intoJumpTransitionTime;
    float turnAngleSmoothDamp;
};

struct DistanceClass_Value {
    int32_t value__;
};

struct ProcChainMask_Value {
    uint32_t mask;
};

struct NodeIndex_Value {
    int32_t nodeIndex;
};

struct LinkIndex_Value {
    int32_t linkIndex;
};

// Generated from RoR2.WwiseUtils.RtpcSetter
struct RtpcSetter_Value {
    void* name;
    uint32_t id;
    char padding2[4];
    GameObject* gameObject;
    float expectedEngineValue;
    float value;
    bool gameObjectIsNull;
};

struct VisibilityLevel_Value {
    int32_t value__;
};

struct PhysicsScene_Value {
    int32_t m_Handle;
};

// Generated from UnityEngine.Ray
struct Ray_Value {
    Vector3 m_Origin;
    Vector3 m_Direction;
};

// Generated from UnityEngine.RaycastHit
struct RaycastHit_Value {
    Vector3 m_Point;
    Vector3 m_Normal;
    uint32_t m_FaceID;
    float m_Distance;
    Vector2 m_UV;
    int32_t m_Collider;
};

// Generated from RoR2.NetworkUserId
struct NetworkUserId_Value {
    uint64_t value;
    void* strValue;
    uint8_t subId;
};

// Generated from RoR2.PassiveSkill
struct PassiveSkill_Value {
    bool enabled;
    char padding1[7];
    void* skillNameToken;
    void* skillDescriptionToken;
    void* keywordToken;
    Sprite* icon;
};

struct SerializablePickupIndex_Value {
    void* pickupName;
};

// Generated from RoR2.UniquePickup
struct UniquePickup_Value {
    PickupIndex_Value pickupIndex;
    float decayValue;
    int32_t upgradeValue;
};

// Generated from RoR2.RangeFloat
struct RangeFloat_Value {
    float min;
    float max;
};

// Generated from RoR2.ItemCollection
// ItemCollection is a value type wrapping SparseArrayStruct<int32, SparseArrayImpl>.
// inner is a pointer to the sparseValues int32[] (first field of SparseArrayStruct),
// which is a MonoArray_Internal containing per-item stack counts indexed by ItemIndex.
struct ItemCollection_Value {
    void* inner; // sparseValues: MonoArray_Internal* of int32, indexed by ItemIndex
};

// Generated from .TempItemsStorage
struct TempItemsStorage_Value {
    Inventory* inventory;
    void* decayToZeroTimeStamps;
    char padding2[24];
    float decayDuration;
    float invDecayDuration;
    ItemCollection_Value tempItemStacks;
};

// Generated from .DroneIndexEmissionColorPair
struct DroneIndexEmissionColorPair_Value {
    int32_t rampIndex;
    Color_Value emissionColor;
};

// Generated from RoR2.Wave
struct Wave_Value {
    float amplitude;
    float frequency;
    float cycleOffset;
};

struct RunStopwatch_Value {
    float offsetFromFixedTime;
    bool isPaused;
};

// Generated from RoR2.Networking.NetworkGuid
struct NetworkGuid_Value {
    uint64_t _a;
    uint64_t _b;
};

// Generated from RoR2.Networking.NetworkDateTime
struct NetworkDateTime_Value {
    int64_t _binaryValue;
};

// Generated from RoR2.CharacterGravityParameters
struct CharacterGravityParameters_Value {
    int32_t environmentalAntiGravityGranterCount;
    int32_t antiGravityNeutralizerCount;
    int32_t channeledAntiGravityGranterCount;
};

// Generated from RoR2.CharacterFlightParameters
struct CharacterFlightParameters_Value {
    int32_t channeledFlightGranterCount;
};

// Generated from .CreatePickupInfo
struct CreatePickupInfo_Value {
    Vector3 position;
    Quaternion rotation;
    void* _pickupState;
    char padding3[12];
    Option* pickerOptions;
    GameObject* prefabOverride;
    ChestBehavior* chest;
    PickupArtifactFlag_Value artifactFlag;
    ItemIndex_Value delusionItemIndex;
    ItemIndex_Value falseChoice1;
    ItemIndex_Value falseChoice2;
    bool duplicated;
    bool recycled;
};

/* Normal Class Objects with headers */

// Generated from RoR2.HealthComponent
struct HealthComponent {
    char padding0[48];                                          // Padding
    CharacterBody* body;                                        // Offset: 48
    ModelLocator* modelLocator;                                 // Offset: 56
    IPainAnimationHandler* painAnimationHandler;                // Offset: 64
    IOnIncomingDamageServerReceiver* onIncomingDamageReceivers; // Offset: 72
    IOnTakeDamageServerReceiver* onTakeDamageReceivers;         // Offset: 80
    ScreenDamageCalculator* screenDamageCalculator;             // Offset: 88
    GameObject* lastHitAttacker_backing;                        // Offset: 96
    RepeatHealComponent* repeatHealComponent;                   // Offset: 104
    float health;                                               // Offset: 112
    float shield;                                               // Offset: 116
    float echoDamage;                                           // Offset: 120
    uint32_t _killingDamageTypeExt;                             // Offset: 124
    float barrier;                                              // Offset: 128
    float magnetiCharge;                                        // Offset: 132
    bool dontShowHealthbar;                                     // Offset: 136
    bool isDefaultGodMode;                                      // Offset: 137
    char padding16[2];                                          // Padding
    float recentlyTookDamageCoyoteTimer;                        // Offset: 140
    float globalDeathEventChanceCoefficient;                    // Offset: 144
    uint32_t _killingDamageType;                                // Offset: 148
    bool destroyModelOnDeath;                                   // Offset: 152
    char padding20[3];                                          // Padding
    FixedTimeStamp_Value lastHitTime_backing;                   // Offset: 156
    FixedTimeStamp_Value lastHealTime_backing;                  // Offset: 160
    bool godMode_backing;                                       // Offset: 164
    char padding23[3];                                          // Padding
    float potionReserve_backing;                                // Offset: 168
    bool isInFrozenState_backing;                               // Offset: 172
    bool distributeDamage_backing;                              // Offset: 173
    bool isRepairing;                                           // Offset: 174
    char padding27[1];                                          // Padding
    float devilOrbHealPool;                                     // Offset: 176
    float devilOrbTimer;                                        // Offset: 180
    float regenAccumulator;                                     // Offset: 184
    bool wasAlive;                                              // Offset: 188
    char padding31[3];                                          // Padding
    float adaptiveArmorValue;                                   // Offset: 192
    bool isShieldRegenForced;                                   // Offset: 196
    bool forceCulled;                                           // Offset: 197
    bool forceHideBody;                                         // Offset: 198
    bool hadBarrier;                                            // Offset: 199
    float ospTimer;                                             // Offset: 200
    float serverDamageTakenThisUpdate;                          // Offset: 204
    ItemCounts_Value itemCounts;                                // Offset: 208
    EquipmentIndex_Value currentEquipmentIndex;                 // Offset: 300
};

// Generated from RoR2.EquipmentState
struct EquipmentState {
    char padding0[16];                     // Padding
    EquipmentIndex_Value equipmentIndex;   // Offset: 16
    FixedTimeStamp_Value chargeFinishTime; // Offset: 20
    uint8_t charges;                       // Offset: 24
    bool isDisabled;                       // Offset: 25
    bool dirty;                            // Offset: 26
    char padding5[5];                      // Padding
    EquipmentDef* equipmentDef;            // Offset: 32
};

// Generated from RoR2.Inventory
struct Inventory {
    char padding0[48];                        // Padding
    ItemCollection_Value permanentItemStacks; // Offset: 48
    char padding1[24];                        // Padding
    ItemCollection_Value channeledItemStacks; // Offset: 80
    char padding2[24];                        // Padding
    TempItemsStorage_Value tempItemsStorage;  // Offset: 112
    char padding3[24];                        // Padding
    ItemCollection_Value effectiveItemStacks; // Offset: 192
    char padding4[24];                        // Padding
    void* itemAcquisitionOrder;               // Offset: 224
    Boolean* itemAcquisitionSet;              // Offset: 232
    Action* onInventoryChanged;               // Offset: 240
    Action* onEquipmentExternalRestockServer; // Offset: 248
    Byte* _activeEquipmentSet;                // Offset: 256
    void* _equipmentStateSlots;               // Offset: 264
    void* onItemAddedClient;                  // Offset: 272
    void* onEquipmentChangedClient;           // Offset: 280
    int32_t _lastExtraEquipmentCount;         // Offset: 288
    uint8_t activeEquipmentSlot_backing;      // Offset: 292
    bool _activeEquipmentDirty;               // Offset: 293
    bool wasRecentlyExtraEquipmentSwapped;    // Offset: 294
    bool wasRecentlyCreated;                  // Offset: 295
    bool equipmentDisabled;                   // Offset: 296
    char padding18[3];                        // Padding
    int32_t inventoryDisablerCountServer;     // Offset: 300
    bool _inventoryDisabled;                  // Offset: 304
    char padding20[3];                        // Padding
    int32_t inventoryChangeScopeCounter;      // Offset: 308
    float beadAppliedHealth;                  // Offset: 312
    float beadAppliedShield;                  // Offset: 316
    float beadAppliedRegen;                   // Offset: 320
    float beadAppliedDamage;                  // Offset: 324
    uint32_t infusionBonus_backing;           // Offset: 328
};

// Generated from RoR2.TeamComponent
struct TeamComponent {
    char padding0[48];                  // Padding
    CharacterBody* body_backing;        // Offset: 48
    GameObject* defaultIndicatorPrefab; // Offset: 56
    GameObject* indicator;              // Offset: 64
    bool hideAllyCardDisplay;           // Offset: 72
    TeamIndex_Value _teamIndex;         // Offset: 73
    TeamIndex_Value oldTeamIndex;       // Offset: 74
};

// Generated from .DestructionNotifier
struct DestructionNotifier {
    char padding0[24];                // Padding
    ModelLocator* subscriber_backing; // Offset: 24
};

// Generated from RoR2.CharacterMotor
struct CharacterMotor {
    char padding0[56];                                   // Padding
    CharacterDirection* characterDirection;              // Offset: 56
    Rigidbody* ignoredRigidbodies;                       // Offset: 64
    NetworkIdentity* networkIdentity;                    // Offset: 72
    CharacterBody* body;                                 // Offset: 80
    CapsuleCollider* capsuleCollider;                    // Offset: 88
    Collider* currentGround_backing;                     // Offset: 96
    HitGroundDelegate* onHitGroundAuthority;             // Offset: 104
    void* onMotorStart;                                  // Offset: 112
    MovementHitDelegate* onMovementHit;                  // Offset: 120
    void* walkSpeedPenaltyModifiers;                     // Offset: 128
    float walkSpeedPenaltyCoefficient;                   // Offset: 136
    float gravityScale;                                  // Offset: 140
    bool muteWalkMotion;                                 // Offset: 144
    char padding13[3];                                   // Padding
    float mass;                                          // Offset: 148
    float airControl;                                    // Offset: 152
    bool disableAirControlUntilCollision;                // Offset: 156
    bool generateParametersOnAwake;                      // Offset: 157
    bool doNotTriggerJumpVolumes;                        // Offset: 158
    bool alive;                                          // Offset: 159
    float restStopwatch;                                 // Offset: 160
    Vector3 previousPosition;                            // Offset: 164
    bool hasEffectiveAuthority_backing;                  // Offset: 176
    bool isAirControlForced;                             // Offset: 177
    char padding23[2];                                   // Padding
    int32_t jumpCount;                                   // Offset: 180
    bool netIsGrounded;                                  // Offset: 184
    char padding25[3];                                   // Padding
    Vector3 netGroundNormal;                             // Offset: 188
    Vector3 velocity;                                    // Offset: 200
    Vector3 lastVelocity;                                // Offset: 212
    Vector3 rootMotion;                                  // Offset: 224
    Vector3 _moveDirection;                              // Offset: 236
    float initialCapsuleHeight_backing;                  // Offset: 248
    float initialCapsuleRadius_backing;                  // Offset: 252
    FixedTimeStamp_Value lastGroundedTime_backing;       // Offset: 256
    int32_t _safeCollisionEnableCount;                   // Offset: 260
    float minWalkSpeedPenaltyCoefficient;                // Offset: 264
    CharacterGravityParameters_Value _gravityParameters; // Offset: 268
    bool useGravity_backing;                             // Offset: 280
    bool useHangtimeGravity;                             // Offset: 281
    bool useCustomGravity;                               // Offset: 282
    char padding39[1];                                   // Padding
    float CustomGravity;                                 // Offset: 284
    CharacterFlightParameters_Value _flightParameters;   // Offset: 288
    bool isFlying_backing;                               // Offset: 292
};

// Generated from RoR2.ModelLocator
struct ModelLocator {
    char padding0[24];                             // Padding
    Transform* _modelTransform;                    // Offset: 24
    RendererVisiblity* modelVisibility;            // Offset: 32
    DestructionNotifier* modelDestructionNotifier; // Offset: 40
    Transform* modelBaseTransform;                 // Offset: 48
    void* onModelChanged;                          // Offset: 56
    Transform* modelParentTransform;               // Offset: 64
    void* logbookOverrideBodyName;                 // Offset: 72
    ChildLocator* cachedModelChildLocator;         // Offset: 80
    CharacterMotor* characterMotor;                // Offset: 88
    float modelScaleCompensation;                  // Offset: 96
    Vector3 initialScale;                          // Offset: 100
    Vector3 initialPosition;                       // Offset: 112
    Quaternion initialRotation;                    // Offset: 124
    bool autoUpdateModelTransform;                 // Offset: 140
    bool dontDetatchFromParent;                    // Offset: 141
    bool noCorpse;                                 // Offset: 142
    bool dontReleaseModelOnDeath;                  // Offset: 143
    bool preserveModel;                            // Offset: 144
    bool forceCulled;                              // Offset: 145
    bool normalizeToFloor;                         // Offset: 146
    char padding20[1];                             // Padding
    float normalSmoothdampTime;                    // Offset: 148
    float normalMaxAngleDelta;                     // Offset: 152
    bool shouldOverrideLogbookModel;               // Offset: 156
    char padding23[3];                             // Padding
    Vector3 normalSmoothdampVelocity;              // Offset: 160
    Vector3 targetNormal;                          // Offset: 172
    Vector3 currentNormal;                         // Offset: 184
};

// Generated from RoR2.CharacterBody
struct CharacterBody {
    char padding0[48];                                                                        // Padding
    void* onRecalculateStats;                                                                 // Offset: 48
    void* baseNameToken;                                                                      // Offset: 56
    void* subtitleNameToken;                                                                  // Offset: 64
    JumpDelegate* onJump;                                                                     // Offset: 72
    BuffIndex* activeBuffsList;                                                               // Offset: 80
    Int32* buffs;                                                                             // Offset: 88
    void* timedBuffs;                                                                         // Offset: 96
    GameObject* warCryEffectInstance;                                                         // Offset: 104
    GameObject* _masterObject;                                                                // Offset: 112
    CharacterMaster* _master;                                                                 // Offset: 120
    Inventory* inventory_backing;                                                             // Offset: 128
    Action* onInventoryChanged;                                                               // Offset: 136
    Transform* transform;                                                                     // Offset: 144
    Rigidbody* rigidbody_backing;                                                             // Offset: 152
    NetworkIdentity* networkIdentity_backing;                                                 // Offset: 160
    CharacterMotor* characterMotor_backing;                                                   // Offset: 168
    CharacterDirection* characterDirection_backing;                                           // Offset: 176
    TeamComponent* teamComponent_backing;                                                     // Offset: 184
    HealthComponent* healthComponent_backing;                                                 // Offset: 192
    EquipmentSlot* equipmentSlot_backing;                                                     // Offset: 200
    InputBankTest* inputBank_backing;                                                         // Offset: 208
    SkillLocator* skillLocator_backing;                                                       // Offset: 216
    SfxLocator* sfxLocator_backing;                                                           // Offset: 224
    ModelLocator* modelLocator_backing;                                                       // Offset: 232
    HurtBoxGroup* hurtBoxGroup_backing;                                                       // Offset: 240
    HurtBox* mainHurtBox_backing;                                                             // Offset: 248
    Transform* coreTransform_backing;                                                         // Offset: 256
    void* onSkillActivatedServer;                                                             // Offset: 264
    void* onSkillActivatedAuthority;                                                          // Offset: 272
    Action* onReleaseBodyTriggeredServer;                                                     // Offset: 280
    String* speedOnPickupEvents;                                                              // Offset: 288
    EffectManagerHelper* extraStatsOnLevelUpScrapEffectInstance;                              // Offset: 296
    IncreaseDamageOnMultiKillItemDisplayUpdater* increaseDamageOnMultiKillItemDisplayUpdater; // Offset: 304
    ScreenDamageCalculatorDisabledSkills* screenDamageSkillsDisabled;                         // Offset: 312
    BoostAllStatsDisplay* boostAllStatsDisplay;                                               // Offset: 320
    void* OnNetworkItemBehaviorUpdate;                                                        // Offset: 328
    HelfireController* helfireController;                                                     // Offset: 336
    DamageTrail* fireTrail;                                                                   // Offset: 344
    GameObject* lunarMissilePrefab;                                                           // Offset: 352
    GameObject* timeBubbleWardInstance;                                                       // Offset: 360
    TemporaryVisualEffect* engiShieldTempEffectInstance;                                      // Offset: 368
    TemporaryVisualEffect* bucklerShieldTempEffectInstance;                                   // Offset: 376
    TemporaryVisualEffect* slowDownTimeTempEffectInstance;                                    // Offset: 384
    TemporaryVisualEffect* crippleEffectInstance;                                             // Offset: 392
    TemporaryVisualEffect* tonicBuffEffectInstance;                                           // Offset: 400
    TemporaryVisualEffect* weakTempEffectInstance;                                            // Offset: 408
    TemporaryVisualEffect* energizedTempEffectInstance;                                       // Offset: 416
    TemporaryVisualEffect* barrierTempEffectInstance;                                         // Offset: 424
    TemporaryVisualEffect* nullifyStack1EffectInstance;                                       // Offset: 432
    TemporaryVisualEffect* nullifyStack2EffectInstance;                                       // Offset: 440
    TemporaryVisualEffect* nullifyStack3EffectInstance;                                       // Offset: 448
    TemporaryVisualEffect* regenBoostEffectInstance;                                          // Offset: 456
    TemporaryVisualEffect* elephantDefenseEffectInstance;                                     // Offset: 464
    TemporaryVisualEffect* healingDisabledEffectInstance;                                     // Offset: 472
    TemporaryVisualEffect* noCooldownEffectInstance;                                          // Offset: 480
    TemporaryVisualEffect* doppelgangerEffectInstance;                                        // Offset: 488
    TemporaryVisualEffect* deathmarkEffectInstance;                                           // Offset: 496
    TemporaryVisualEffect* crocoRegenEffectInstance;                                          // Offset: 504
    TemporaryVisualEffect* mercExposeEffectInstance;                                          // Offset: 512
    TemporaryVisualEffect* lifestealOnHitEffectInstance;                                      // Offset: 520
    TemporaryVisualEffect* teamWarCryEffectInstance;                                          // Offset: 528
    TemporaryVisualEffect* randomDamageEffectInstance;                                        // Offset: 536
    TemporaryVisualEffect* lunarGolemShieldEffectInstance;                                    // Offset: 544
    TemporaryVisualEffect* warbannerEffectInstance;                                           // Offset: 552
    TemporaryVisualEffect* teslaFieldEffectInstance;                                          // Offset: 560
    TemporaryVisualEffect* lunarSecondaryRootEffectInstance;                                  // Offset: 568
    TemporaryVisualEffect* lunarDetonatorEffectInstance;                                      // Offset: 576
    TemporaryVisualEffect* fruitingEffectInstance;                                            // Offset: 584
    TemporaryVisualEffect* overheatEffectInstance;                                            // Offset: 592
    TemporaryVisualEffect* mushroomVoidTempEffectInstance;                                    // Offset: 600
    TemporaryVisualEffect* bearVoidTempEffectInstance;                                        // Offset: 608
    TemporaryVisualEffect* outOfCombatArmorEffectInstance;                                    // Offset: 616
    TemporaryVisualEffect* voidFogMildEffectInstance;                                         // Offset: 624
    TemporaryVisualEffect* voidFogStrongEffectInstance;                                       // Offset: 632
    TemporaryVisualEffect* voidJailerSlowEffectInstance;                                      // Offset: 640
    TemporaryVisualEffect* voidRaidcrabWardWipeFogEffectInstance;                             // Offset: 648
    TemporaryVisualEffect* aurelioniteBlessingEffectInstance;                                 // Offset: 656
    TemporaryVisualEffect* growthNectarMaxGlowInstance;                                       // Offset: 664
    TemporaryVisualEffect* delayedDamageBuffUpInstance;                                       // Offset: 672
    TemporaryVisualEffect* knockbackHitEnemiesVulnerableInstance;                             // Offset: 680
    TemporaryVisualEffect* falseSonMeridianWillCoreBuffInstance;                              // Offset: 688
    TemporaryVisualEffect* nanoBugEffectInstance;                                             // Offset: 696
    TemporaryVisualEffect* shieldTankDamageEffectInstance;                                    // Offset: 704
    TemporaryVisualEffect* DroneShockDamageActivateInstance;                                  // Offset: 712
    TemporaryVisualEffect* DroneShockDamageActivateOnDroneInstance;                           // Offset: 720
    TemporaryVisualEffect* DroneShockDamageSpreadInstance;                                    // Offset: 728
    TemporaryVisualEffect* nullify2Stack1EffectInstance;                                      // Offset: 736
    TemporaryVisualEffect* nullify2Stack2EffectInstance;                                      // Offset: 744
    TemporaryVisualEffect* nullify2Stack3EffectInstance;                                      // Offset: 752
    TemporaryVisualEffect* AccelerantGooedInstance;                                           // Offset: 760
    TemporaryVisualEffect* sharedSufferingInstance;                                           // Offset: 768
    TemporaryVisualEffect* TransferDebuffInstance;                                            // Offset: 776
    TemporaryVisualEffect* SureProcInstance;                                                  // Offset: 784
    TemporaryVisualEffect* jailerDroneCagedEffectInstance;                                    // Offset: 792
    AnimationCurve* spreadBloomCurve;                                                         // Offset: 800
    GameObject* _defaultCrosshairPrefab;                                                      // Offset: 808
    GameObject* sprintCrosshairPrefabOverride;                                                // Offset: 816
    DamageInfo* secondHalfOfDamage;                                                           // Offset: 824
    void* incomingDamageList;                                                                 // Offset: 832
    DamageInfo* runicLensDamageInfo;                                                          // Offset: 840
    CharacterBody* meteorVictimBody;                                                          // Offset: 848
    OnShardSpawned* onShardSpawnedEvent;                                                      // Offset: 856
    Transform* aimOriginTransform;                                                            // Offset: 864
    Transform* scanOverrideTransform;                                                         // Offset: 872
    Texture* portraitIcon;                                                                    // Offset: 880
    void* altDeathBodyName;                                                                   // Offset: 888
    EntityStateMachine* vehicleIdleStateMachine;                                              // Offset: 896
    VehicleSeat* currentVehicle;                                                              // Offset: 904
    GameObject* preferredPodPrefab;                                                           // Offset: 912
    SerializableEntityStateType_Value preferredInitialStateType;                              // Offset: 920
    void* customKillTotalStatName;                                                            // Offset: 936
    Transform* overrideCoreTransform;                                                         // Offset: 944
    BodyIndex_Value bodyIndex;                                                                // Offset: 952
    BodyIndex_Value altDeathEventBodyIndex;                                                   // Offset: 956
    bool CharacterIsVisible;                                                                  // Offset: 960
    char padding115[3];                                                                       // Padding
    float cost;                                                                               // Offset: 964
    bool dontCull;                                                                            // Offset: 968
    char padding117[3];                                                                       // Padding
    int32_t BaseImportance;                                                                   // Offset: 972
    int32_t Importance;                                                                       // Offset: 976
    bool inLava;                                                                              // Offset: 980
    char padding120[3];                                                                       // Padding
    int32_t activeBuffsListCount;                                                             // Offset: 984
    int32_t eliteBuffCount;                                                                   // Offset: 988
    int32_t pendingTonicAfflictionCount;                                                      // Offset: 992
    int32_t previousMultiKillBuffCount;                                                       // Offset: 996
    BodyFlags_Value bodyFlags;                                                                // Offset: 1000
    NetworkInstanceId_Value masterObjectId;                                                   // Offset: 1004
    bool linkedToMaster;                                                                      // Offset: 1008
    bool isPlayerControlled_backing;                                                          // Offset: 1009
    bool disablingHurtBoxes;                                                                  // Offset: 1010
    char padding129[1];                                                                       // Padding
    EquipmentIndex_Value previousEquipmentIndex;                                              // Offset: 1012
    float executeEliteHealthFraction_backing;                                                 // Offset: 1016
    bool isRemoteOp_backing;                                                                  // Offset: 1020
    char padding132[3];                                                                       // Padding
    float lavaCooldown;                                                                       // Offset: 1024
    float lavaTimer;                                                                          // Offset: 1028
    ItemAvailability_Value itemAvailability;                                                  // Offset: 1032
    char padding135[1];                                                                       // Padding
    bool hasEffectiveAuthority_backing;                                                       // Offset: 1040
    bool _isSprinting;                                                                        // Offset: 1041
    char padding137[2];                                                                       // Padding
    float outOfCombatStopwatch;                                                               // Offset: 1044
    float outOfDangerStopwatch;                                                               // Offset: 1048
    bool outOfCombat_backing;                                                                 // Offset: 1052
    bool _outOfDanger;                                                                        // Offset: 1053
    bool shieldBrokeSinceDangerCheck;                                                         // Offset: 1054
    char padding142[1];                                                                       // Padding
    Vector3 previousPosition;                                                                 // Offset: 1056
    float notMovingStopwatch;                                                                 // Offset: 1068
    bool rootMotionInMainState;                                                               // Offset: 1072
    char padding145[3];                                                                       // Padding
    float mainRootSpeed;                                                                      // Offset: 1076
    float baseMaxHealth;                                                                      // Offset: 1080
    float baseRegen;                                                                          // Offset: 1084
    float baseMaxShield;                                                                      // Offset: 1088
    float baseMoveSpeed;                                                                      // Offset: 1092
    float baseAcceleration;                                                                   // Offset: 1096
    float baseJumpPower;                                                                      // Offset: 1100
    float baseDamage;                                                                         // Offset: 1104
    float baseAttackSpeed;                                                                    // Offset: 1108
    float baseCrit;                                                                           // Offset: 1112
    float baseArmor;                                                                          // Offset: 1116
    float baseVisionDistance;                                                                 // Offset: 1120
    int32_t baseJumpCount;                                                                    // Offset: 1124
    float sprintingSpeedMultiplier;                                                           // Offset: 1128
    bool autoCalculateLevelStats;                                                             // Offset: 1132
    char padding160[3];                                                                       // Padding
    float levelMaxHealth;                                                                     // Offset: 1136
    float levelRegen;                                                                         // Offset: 1140
    float levelMaxShield;                                                                     // Offset: 1144
    float levelMoveSpeed;                                                                     // Offset: 1148
    float levelJumpPower;                                                                     // Offset: 1152
    float levelDamage;                                                                        // Offset: 1156
    float levelAttackSpeed;                                                                   // Offset: 1160
    float levelCrit;                                                                          // Offset: 1164
    float levelArmor;                                                                         // Offset: 1168
    float experience_backing;                                                                 // Offset: 1172
    float level_backing;                                                                      // Offset: 1176
    float maxHealth_backing;                                                                  // Offset: 1180
    float maxBarrier_backing;                                                                 // Offset: 1184
    float barrierDecayRate_backing;                                                           // Offset: 1188
    float regen_backing;                                                                      // Offset: 1192
    float maxShield_backing;                                                                  // Offset: 1196
    float moveSpeed_backing;                                                                  // Offset: 1200
    float acceleration_backing;                                                               // Offset: 1204
    float m_surfaceSpeedBoost;                                                                // Offset: 1208
    float jumpPower_backing;                                                                  // Offset: 1212
    int32_t maxJumpCount_backing;                                                             // Offset: 1216
    float maxJumpHeight_backing;                                                              // Offset: 1220
    float damage_backing;                                                                     // Offset: 1224
    float attackSpeed_backing;                                                                // Offset: 1228
    float crit_backing;                                                                       // Offset: 1232
    float critMultiplier_backing;                                                             // Offset: 1236
    float bleedChance_backing;                                                                // Offset: 1240
    float armor_backing;                                                                      // Offset: 1244
    float visionDistance_backing;                                                             // Offset: 1248
    float critHeal_backing;                                                                   // Offset: 1252
    float cursePenalty_backing;                                                               // Offset: 1256
    bool hasOneShotProtection_backing;                                                        // Offset: 1260
    bool isGlass_backing;                                                                     // Offset: 1261
    char padding193[2];                                                                       // Padding
    float oneShotProtectionFraction_backing;                                                  // Offset: 1264
    bool canPerformBackstab_backing;                                                          // Offset: 1268
    bool canReceiveBackstab_backing;                                                          // Offset: 1269
    bool statsDirty;                                                                          // Offset: 1270
    char padding197[1];                                                                       // Padding
    int32_t currentHealthLevel;                                                               // Offset: 1272
    int32_t oldHealthLevel;                                                                   // Offset: 1276
    float damageFromRecalculateStats;                                                         // Offset: 1280
    int32_t numberOfKills;                                                                    // Offset: 1284
    bool canCleanInventory;                                                                   // Offset: 1288
    char padding202[3];                                                                       // Padding
    int32_t extraSecondaryFromSkill;                                                          // Offset: 1292
    int32_t extraSpecialFromSkill;                                                            // Offset: 1296
    float maxBonusHealth_backing;                                                             // Offset: 1300
    int32_t extraStatsOnLevelUpCount_CachedLastApplied;                                       // Offset: 1304
    bool lowerPricedChestsActive;                                                             // Offset: 1308
    bool canPurchaseLoweredPricedChests;                                                      // Offset: 1309
    bool allSkillsDisabled;                                                                   // Offset: 1310
    bool isTeleporting;                                                                       // Offset: 1311
    int32_t currentGrowthNectarBuffCount;                                                     // Offset: 1312
    int32_t prevGrowthNectarBuffCount;                                                        // Offset: 1316
    int32_t maxGrowthNectarBuffCount;                                                         // Offset: 1320
    float growthNectarRemovalTimer;                                                           // Offset: 1324
    bool alwaysAim;                                                                           // Offset: 1328
    char padding215[3];                                                                       // Padding
    float aimTimer;                                                                           // Offset: 1332
    int32_t originalLayer_backing;                                                            // Offset: 1336
    int32_t _fakeActorCounter;                                                                // Offset: 1340
    int32_t killCountServer_backing;                                                          // Offset: 1344
    float helfireLifetime;                                                                    // Offset: 1348
    bool wasLucky;                                                                            // Offset: 1352
    char padding221[3];                                                                       // Padding
    float poisonballTimer;                                                                    // Offset: 1356
    float lunarMissileRechargeTimer;                                                          // Offset: 1360
    float lunarMissileTimerBetweenShots;                                                      // Offset: 1364
    int32_t remainingMissilesToFire;                                                          // Offset: 1368
    float spreadBloomDecayTime;                                                               // Offset: 1372
    float spreadBloomInternal;                                                                // Offset: 1376
    bool hideCrosshair;                                                                       // Offset: 1380
    bool useSprintCrosshair;                                                                  // Offset: 1381
    char padding229[2];                                                                       // Padding
    float multiKillTimer;                                                                     // Offset: 1384
    int32_t multiKillCount_backing;                                                           // Offset: 1388
    float increasedDamageKillTimer;                                                           // Offset: 1392
    bool increasedDamageKillCountDirty;                                                       // Offset: 1396
    char padding233[3];                                                                       // Padding
    int32_t _increasedDamageKillCount;                                                        // Offset: 1400
    int32_t increasedDamageKillCountBuff_backing;                                             // Offset: 1404
    bool canAddIncrasePrimaryDamage;                                                          // Offset: 1408
    char padding236[3];                                                                       // Padding
    float delayedDamageRefreshTime;                                                           // Offset: 1412
    float delayedDamageRefreshTimer;                                                          // Offset: 1416
    float secondHalfOfDamageTime;                                                             // Offset: 1420
    int32_t oldDelayedDamageCount;                                                            // Offset: 1424
    bool halfDamageReady;                                                                     // Offset: 1428
    char padding241[3];                                                                       // Padding
    float halfDamageTimer;                                                                    // Offset: 1432
    float knockBackFinGroundedTimer;                                                          // Offset: 1436
    Vector3 runicLensPosition;                                                                // Offset: 1440
    float runicLensStartTime;                                                                 // Offset: 1452
    float runicLensImpactTime;                                                                // Offset: 1456
    bool runicLensMeteorReady;                                                                // Offset: 1460
    char padding247[3];                                                                       // Padding
    int32_t spawnedPickupCount;                                                               // Offset: 1464
    bool useTemporaryPathfindingFootposition;                                                 // Offset: 1468
    char padding249[3];                                                                       // Padding
    float radius_backing;                                                                     // Offset: 1472
    bool limitCooldown;                                                                       // Offset: 1476
    char padding251[3];                                                                       // Padding
    float minCooldownCoefficient;                                                             // Offset: 1480
    HullClassification_Value hullClassification;                                              // Offset: 1484
    Color_Value bodyColor;                                                                    // Offset: 1488
    bool doNotReassignToTeamBasedCollisionLayer;                                              // Offset: 1504
    bool isChampion;                                                                          // Offset: 1505
    bool isElite_backing;                                                                     // Offset: 1506
    char padding257[1];                                                                       // Padding
    uint32_t skinIndex;                                                                       // Offset: 1508
    FixedTimeStamp_Value localStartTime_backing;                                              // Offset: 1512
    bool requestEquipmentFire;                                                                // Offset: 1516
    char padding260[3];                                                                       // Padding
    int32_t increaseDamageOnMultiKillItemCount;                                               // Offset: 1520
};

// Generated from RoR2.NetworkUser
struct NetworkUser {
    char padding0[48];                                            // Padding
    NetworkLoadout* networkLoadout_backing;                       // Offset: 48
    NetworkUserId_Value _id;                                      // Offset: 56
    char padding2[7];                                             // Padding
    LocalUser* localUser;                                         // Offset: 80
    CameraRigController* cameraRigController;                     // Offset: 88
    void* userName;                                               // Offset: 96
    void* cachedMaster;                                           // Offset: 104
    char padding6[8];                                             // Padding
    void* cachedPlayerCharacterMasterController;                  // Offset: 120
    char padding7[8];                                             // Padding
    void* cachedPlayerStatsComponent;                             // Offset: 136
    char padding8[8];                                             // Padding
    GameObject* _masterObject;                                    // Offset: 152
    void* unlockables;                                            // Offset: 160
    void* debugUnlockablesList;                                   // Offset: 168
    uint8_t rewiredPlayerId;                                      // Offset: 176
    char padding12[3];                                            // Padding
    NetworkInstanceId_Value _masterObjectId;                      // Offset: 180
    Color32_Value userColor;                                      // Offset: 184
    uint32_t netLunarCoins;                                       // Offset: 188
    ItemIndex_Value rebirthItem;                                  // Offset: 192
    BodyIndex_Value bodyIndexPreference;                          // Offset: 196
    float secondAccumulator;                                      // Offset: 200
    bool serverIsClientLoaded_backing;                            // Offset: 204
    char padding19[3];                                            // Padding
    NetworkInstanceId_Value _serverLastStageAcknowledgedByClient; // Offset: 208
};

// Generated from RoR2.PlayerCharacterMasterController
struct PlayerCharacterMasterController {
    char padding0[48];                                 // Padding
    CharacterBody* body;                               // Offset: 48
    InputBankTest* bodyInputs;                         // Offset: 56
    CharacterMotor* bodyMotor;                         // Offset: 64
    GameObject* previousBodyObject;                    // Offset: 72
    CharacterMaster* master_backing;                   // Offset: 80
    void* finalMessageTokenServer_backing;             // Offset: 88
    PingerController* pingerController;                // Offset: 96
    GameObject* resolvedNetworkUserGameObjectInstance; // Offset: 104
    NetworkUser* resolvedNetworkUserInstance;          // Offset: 112
    GameObject* crosshair;                             // Offset: 120
    NetworkIdentity* netid;                            // Offset: 128
    Action* onLinkedToNetworkUserServer;               // Offset: 136
    NetworkInstanceId_Value networkUserInstanceId;     // Offset: 144
    bool networkUserResolved;                          // Offset: 148
    bool alreadyLinkedToNetworkUserOnce;               // Offset: 149
    char padding15[2];                                 // Padding
    float cameraMinPitch;                              // Offset: 152
    float cameraMaxPitch;                              // Offset: 156
    Vector3 crosshairPosition;                         // Offset: 160
    bool sprintInputPressReceived;                     // Offset: 172
    bool jumpWasClaimed;                               // Offset: 173
    bool claimNextJump;                                // Offset: 174
    char padding21[1];                                 // Padding
    float lunarCoinChanceMultiplier;                   // Offset: 176
};

// Generated from RoR2.CharacterMaster
struct CharacterMaster {
    char padding0[48];                                                        // Padding
    GameObject* bodyPrefab;                                                   // Offset: 48
    GameObject* originalBodyPrefab;                                           // Offset: 56
    NetworkIdentity* networkIdentity_backing;                                 // Offset: 64
    void* onBodyStart;                                                        // Offset: 72
    void* onBodyDestroyed;                                                    // Offset: 80
    void* onPlayerBodyDamaged;                                                // Offset: 88
    UnityEvent* onBodyDeath;                                                  // Offset: 96
    Inventory* inventory_backing;                                             // Offset: 104
    BaseAI* aiComponents;                                                     // Offset: 112
    PlayerCharacterMasterController* playerCharacterMasterController_backing; // Offset: 120
    PlayerStatsComponent* playerStatsComponent_backing;                       // Offset: 128
    MinionOwnership* minionOwnership_backing;                                 // Offset: 136
    Loadout* loadout;                                                         // Offset: 144
    GameObject* resolvedBodyInstance;                                         // Offset: 152
    Inventory* _persistentInventory;                                          // Offset: 160
    GameObject* resolvedCombatSquadInstance;                                  // Offset: 168
    void* OnGoldCollected;                                                    // Offset: 176
    Xoroshiro128Plus* cloverVoidRng;                                          // Offset: 184
    void* deployablesList;                                                    // Offset: 192
    GameObject* devotionInventoryPrefab;                                      // Offset: 200
    DevotionInventoryController* devotionInventoryUpdaterRef;                 // Offset: 208
    Interactor* summonerRef;                                                  // Offset: 216
    int32_t _masterIndex;                                                     // Offset: 224
    bool hideAllyCard;                                                        // Offset: 228
    bool spawnOnStart;                                                        // Offset: 229
    TeamIndex_Value _teamIndex;                                               // Offset: 230
    bool hasEffectiveAuthority_backing;                                       // Offset: 231
    bool destroyOnBodyDeath;                                                  // Offset: 232
    char padding28[3];                                                        // Padding
    NetworkInstanceId_Value _bodyInstanceId;                                  // Offset: 236
    bool bodyResolved;                                                        // Offset: 240
    char padding30[3];                                                        // Padding
    BodyIndex_Value backupBodyIndex_backing;                                  // Offset: 244
    bool enablePrintController_backing;                                       // Offset: 248
    char padding32[3];                                                        // Padding
    NetworkInstanceId_Value _combatSquadInstanceId;                           // Offset: 252
    int32_t numberOfBeadStatsGained_XPGainNerf;                               // Offset: 256
    char padding34[4];                                                        // Padding
    uint64_t beadExperience;                                                  // Offset: 264
    uint32_t oldBeadLevel;                                                    // Offset: 272
    uint32_t newBeadLevel;                                                    // Offset: 276
    uint64_t beadXPNeededForCurrentLevel;                                     // Offset: 280
    uint32_t _money;                                                          // Offset: 288
    uint32_t ExtraBossMissileMoneyRemainder;                                  // Offset: 292
    uint32_t trackedFreeUnlocks;                                              // Offset: 296
    int32_t trackedMissileCount;                                              // Offset: 300
    float moneyEarnedSoFar;                                                   // Offset: 304
    uint32_t _voidCoins;                                                      // Offset: 308
    float luck_backing;                                                       // Offset: 312
    bool isBoss;                                                              // Offset: 316
    bool preventGameOver;                                                     // Offset: 317
    char padding47[2];                                                        // Padding
    Vector3 deathFootPosition_backing;                                        // Offset: 320
    Vector3 deathAimVector;                                                   // Offset: 332
    bool killedByUnsafeArea;                                                  // Offset: 344
    char padding50[3];                                                        // Padding
    float _internalSurvivalTime;                                              // Offset: 348
    BodyIndex_Value killerBodyIndex;                                          // Offset: 352
    bool preventRespawnUntilNextStageServer;                                  // Offset: 356
    uint8_t _seekerChakraGate;                                                // Offset: 357
    bool _seekerRevivedOnce;                                                  // Offset: 358
    bool _seekerRevitalize;                                                   // Offset: 359
    bool seekerSelfRevive;                                                    // Offset: 360
    char padding57[3];                                                        // Padding
    ItemIndex_Value devotionItem;                                             // Offset: 364
    int32_t devotedEvolutionTracker;                                          // Offset: 368
    bool isDevotedMinion;                                                     // Offset: 372
    bool godMode;                                                             // Offset: 373
    char padding61[2];                                                        // Padding
    uint32_t lostBodyToDeathFlag;                                             // Offset: 376
    uint32_t _miscFlags;                                                      // Offset: 380
};

// Generated from RoR2.CameraTargetParams
struct CameraTargetParams {
    char padding0[24];                                        // Padding
    CharacterCameraParams* cameraParams;                      // Offset: 24
    Transform* cameraPivotTransform;                          // Offset: 32
    void* cameraLerpRequests;                                 // Offset: 40
    void* cameraParamsOverrides;                              // Offset: 48
    float fovOverride;                                        // Offset: 56
    Vector2 recoil;                                           // Offset: 60
    bool dontRaycastToPivot;                                  // Offset: 68
    char padding7[3];                                         // Padding
    Vector2 targetRecoil;                                     // Offset: 72
    Vector2 recoilVelocity;                                   // Offset: 80
    Vector2 targetRecoilVelocity;                             // Offset: 88
    CharacterCameraParamsData_Value _currentCameraParamsData; // Offset: 96
};

// Generated from RoR2.CameraRigController
struct CameraRigController {
    char padding0[24];                                 // Padding
    Camera* sceneCam;                                  // Offset: 24
    Camera* uiCam;                                     // Offset: 32
    Camera* skyboxCam;                                 // Offset: 40
    ParticleSystem* sprintingParticleSystem;           // Offset: 48
    ParticleSystem* disabledSkillsParticleSystem;      // Offset: 56
    ParticleSystem* secondaryElectrictyParticleSystem; // Offset: 64
    GameObject* nextTarget;                            // Offset: 72
    CameraModeBase* _cameraMode;                       // Offset: 80
    HUD* hud_backing;                                  // Offset: 88
    GameObject* firstPersonTarget_backing;             // Offset: 96
    CharacterBody* targetBody_backing;                 // Offset: 104
    GameObject* _target;                               // Offset: 112
    ICameraStateProvider* overrideCam;                 // Offset: 120
    CameraModeContext_Value cameraModeContext;         // Offset: 128
    char padding14[6];                                 // Padding
    NetworkUser* _viewer;                              // Offset: 296
    LocalUser* _localUserViewer;                       // Offset: 304
    CameraTargetParams* targetParams;                  // Offset: 312
    AimAssistInfo_Value lastAimAssist;                 // Offset: 320
    AimAssistInfo_Value aimAssist;                     // Offset: 352
    HurtBox* lastCrosshairHurtBox_backing;             // Offset: 384
    float baseFov;                                     // Offset: 392
    Rect viewport;                                     // Offset: 396
    float maxAimRaycastDistance;                       // Offset: 412
    bool isCutscene;                                   // Offset: 416
    bool enableFading;                                 // Offset: 417
    char padding25[2];                                 // Padding
    float fadeStartDistance;                           // Offset: 420
    float fadeEndDistance;                             // Offset: 424
    bool createHud;                                    // Offset: 428
    bool suppressPlayerCameras;                        // Offset: 429
    bool enableMusic;                                  // Offset: 430
    bool disableSpectating_backing;                    // Offset: 431
    TeamIndex_Value targetTeamIndex_backing;           // Offset: 432
    char padding32[3];                                 // Padding
    Vector3 rawScreenShakeDisplacement_backing;        // Offset: 436
    Vector3 crosshairWorldPosition_backing;            // Offset: 448
    CameraState_Value desiredCameraState;              // Offset: 460
    CameraState_Value currentCameraState;              // Offset: 492
    bool doNotOverrideCameraState;                     // Offset: 524
    char padding37[3];                                 // Padding
    CameraState_Value lerpCameraState;                 // Offset: 528
    float lerpCameraTime;                              // Offset: 560
    float lerpCameraTimeScale;                         // Offset: 564
    float hitmarkerAlpha;                              // Offset: 568
    float hitmarkerTimer;                              // Offset: 572
};

// Generated from RoR2.Stats.PlayerStatsComponent
struct PlayerStatsComponent {
    char padding0[48];                                                        // Padding
    CharacterMaster* characterMaster_backing;                                 // Offset: 48
    PlayerCharacterMasterController* playerCharacterMasterController_backing; // Offset: 56
    GameObject* cachedBodyObject;                                             // Offset: 64
    CharacterBody* cachedCharacterBody;                                       // Offset: 72
    CharacterMotor* cachedBodyCharacterMotor;                                 // Offset: 80
    Transform* cachedBodyTransform;                                           // Offset: 88
    StatSheet* currentStats;                                                  // Offset: 96
    StatSheet* clientDeltaStatsBuffer;                                        // Offset: 104
    StatSheet* recordedStats;                                                 // Offset: 112
    bool ClientSynced;                                                        // Offset: 120
    char padding10[3];                                                        // Padding
    float serverTransmitTimer;                                                // Offset: 124
    float serverTransmitInterval;                                             // Offset: 128
    Vector3 previousBodyPosition;                                             // Offset: 132
};

// Generated from RoR2.LocalUser
struct LocalUser {
    char padding0[16];                                               // Padding
    Player* _inputPlayer;                                            // Offset: 16
    MPEventSystem* eventSystem_backing;                              // Offset: 24
    UserProfile* _userProfile;                                       // Offset: 32
    NetworkUser* currentNetworkUser_backing;                         // Offset: 40
    PlayerCharacterMasterController* cachedMasterController_backing; // Offset: 48
    PauseScreenController* pauseScreenController;                    // Offset: 56
    CharacterMaster* cachedMaster_backing;                           // Offset: 64
    GameObject* cachedMasterObject_backing;                          // Offset: 72
    CharacterBody* cachedBody_backing;                               // Offset: 80
    GameObject* cachedBodyObject_backing;                            // Offset: 88
    PlayerStatsComponent* cachedStatsComponent_backing;              // Offset: 96
    CameraRigController* _cameraRigController;                       // Offset: 104
    Action* onBodyChanged;                                           // Offset: 112
    Action* onMasterChanged;                                         // Offset: 120
    void* onCameraDiscovered;                                        // Offset: 128
    void* onCameraLost;                                              // Offset: 136
    void* onNetworkUserFound;                                        // Offset: 144
    void* onNetworkUserLost;                                         // Offset: 152
    int32_t id;                                                      // Offset: 160
};

// Generated from RoR2.RuleBook
struct RuleBook {
    char padding0[16]; // Padding
    Byte* ruleValues;  // Offset: 16
};

// Generated from RoR2.ServerManagerBase`1
struct ServerManagerBase {
    char padding0[32];                       // Padding
    void* collectAdditionalTags;             // Offset: 32
    void* tags;                              // Offset: 40
    RuleBook* currentRuleBook;               // Offset: 48
    KeyValueSplitter_Value ruleBookKvHelper; // Offset: 56
    KeyValueSplitter_Value modListKvHelper;  // Offset: 88
    bool disposed;                           // Offset: 120
    char padding6[3];                        // Padding
    float playerUpdateTimer;                 // Offset: 124
    float playerUpdateInterval;              // Offset: 128
};

// Generated from RoR2.RangeFloat
struct RangeFloat {
    char padding0[16]; // Padding
    float min;         // Offset: 16
    float max;         // Offset: 20
};

// Generated from RoR2.CombatSquad
struct CombatSquad {
    char padding0[48];                       // Padding
    void* membersList;                       // Offset: 48
    void* memberHistory;                     // Offset: 56
    void* readOnlyMembersList_backing;       // Offset: 64
    void* onDestroyCallbacksServer;          // Offset: 72
    Action* onDefeatedServer;                // Offset: 80
    UnityEvent* onDefeatedClientLogicEvent;  // Offset: 88
    UnityEvent* onDefeatedServerLogicEvent;  // Offset: 96
    void* onMemberDeathServer;               // Offset: 104
    void* onMemberDefeatedServer;            // Offset: 112
    void* onMemberAddedServer;               // Offset: 120
    void* onMemberDiscovered;                // Offset: 128
    void* onMemberLost;                      // Offset: 136
    bool propagateMembershipToSummons;       // Offset: 144
    bool grantBonusHealthInMultiplayer;      // Offset: 145
    bool defeatedServer;                     // Offset: 146
    bool delayDefeat_backing;                // Offset: 147
    FixedTimeStamp_Value awakeTime;          // Offset: 148
    int32_t invalidDataUpdateRequestCounter; // Offset: 152
};

// Generated from RoR2.SpawnCard
struct SpawnCard {
    char padding0[24];                 // Padding
    GameObject* prefab;                // Offset: 24
    bool sendOverNetwork;              // Offset: 32
    char padding2[3];                  // Padding
    HullClassification_Value hullSize; // Offset: 36
    GraphType_Value nodeGraphType;     // Offset: 40
    NodeFlags_Value requiredFlags;     // Offset: 44
    NodeFlags_Value forbiddenFlags;    // Offset: 45
    char padding6[2];                  // Padding
    int32_t directorCreditCost;        // Offset: 48
    bool occupyPosition;               // Offset: 52
    char padding8[3];                  // Padding
    EliteRules_Value eliteRules;       // Offset: 56
};

// Generated from RoR2.RuleCategoryDef
struct RuleCategoryDef {
    char padding0[16];                       // Padding
    void* displayToken;                      // Offset: 16
    void* subtitleToken;                     // Offset: 24
    void* emptyTipToken;                     // Offset: 32
    void* editToken;                         // Offset: 40
    void* children;                          // Offset: 48
    void* hiddenTest;                        // Offset: 56
    int32_t position;                        // Offset: 64
    Color_Value color;                       // Offset: 68
    RuleCategoryType_Value ruleCategoryType; // Offset: 84
};

// Generated from RoR2.RuleDef
struct RuleDef {
    char padding0[16];          // Padding
    void* globalName;           // Offset: 16
    void* displayToken;         // Offset: 24
    void* choices;              // Offset: 32
    RuleCategoryDef* category;  // Offset: 40
    int32_t globalIndex;        // Offset: 48
    int32_t defaultChoiceIndex; // Offset: 52
    bool forceLobbyDisplay;     // Offset: 56
    bool hideLobbyDisplay;      // Offset: 57
};

// Generated from RoR2.EntitlementManagement.EntitlementDef
struct EntitlementDef {
    char padding0[24];                       // Padding
    void* nameToken;                         // Offset: 24
    void* eosItemId;                         // Offset: 32
    void* egsItemName;                       // Offset: 40
    void* GamecoreID;                        // Offset: 48
    void* SonyID;                            // Offset: 56
    EntitlementIndex_Value entitlementIndex; // Offset: 64
    uint32_t steamAppId;                     // Offset: 68
    uint64_t switchNsUid;                    // Offset: 72
    uint32_t switchDLCIndex;                 // Offset: 80
};

// Generated from RoR2.RuleChoiceDef
struct RuleChoiceDef {
    char padding0[16];                      // Padding
    RuleDef* ruleDef;                       // Offset: 16
    Sprite* sprite;                         // Offset: 24
    void* tooltipNameToken;                 // Offset: 32
    void* getTooltipName;                   // Offset: 40
    void* tooltipBodyToken;                 // Offset: 48
    void* localName;                        // Offset: 56
    void* globalName;                       // Offset: 64
    UnlockableDef* requiredUnlockable;      // Offset: 72
    RuleChoiceDef* requiredChoiceDef;       // Offset: 80
    EntitlementDef* requiredEntitlementDef; // Offset: 88
    ExpansionDef* requiredExpansionDef;     // Offset: 96
    void* extraData;                        // Offset: 104
    void* selectionUISound;                 // Offset: 112
    void* serverTag;                        // Offset: 120
    Color_Value tooltipNameColor;           // Offset: 128
    Color_Value tooltipBodyColor;           // Offset: 144
    int32_t localIndex;                     // Offset: 160
    int32_t globalIndex;                    // Offset: 164
    bool availableInSinglePlayer;           // Offset: 168
    bool availableInMultiPlayer;            // Offset: 169
    char padding20[2];                      // Padding
    DifficultyIndex_Value difficultyIndex;  // Offset: 172
    ArtifactIndex_Value artifactIndex;      // Offset: 176
    ItemIndex_Value itemIndex;              // Offset: 180
    EquipmentIndex_Value equipmentIndex;    // Offset: 184
    DroneIndex_Value droneIndex;            // Offset: 188
    bool excludeByDefault;                  // Offset: 192
    bool onlyShowInGameBrowserIfNonDefault; // Offset: 193
};

// Generated from RoR2.ExpansionManagement.ExpansionDef
struct ExpansionDef {
    char padding0[24];                   // Padding
    EntitlementDef* requiredEntitlement; // Offset: 24
    void* nameToken;                     // Offset: 32
    void* descriptionToken;              // Offset: 40
    Sprite* iconSprite;                  // Offset: 48
    Sprite* disabledIconSprite;          // Offset: 56
    RuleChoiceDef* enabledChoice;        // Offset: 64
    GameObject* runBehaviorPrefab;       // Offset: 72
    ExpansionIndex_Value expansionIndex; // Offset: 80
};

// Generated from RoR2.ExpansionManagement.ExpansionRequirementComponent
struct ExpansionRequirementComponent {
    char padding0[24];                         // Padding
    ExpansionDef* requiredExpansion;           // Offset: 24
    bool requireEntitlementIfPlayerControlled; // Offset: 32
};

// Generated from RoR2.UnlockableDef
struct UnlockableDef {
    char padding0[24];                   // Padding
    void* _cachedName;                   // Offset: 24
    void* nameToken;                     // Offset: 32
    void* displayModelPath;              // Offset: 40
    GameObject* displayModelPrefab;      // Offset: 48
    Sprite* achievementIcon;             // Offset: 56
    ExpansionDef* requiredExpansion;     // Offset: 64
    void* getHowToUnlockString_backing;  // Offset: 72
    void* getUnlockedString_backing;     // Offset: 80
    UnlockableIndex_Value index_backing; // Offset: 88
    bool hidden;                         // Offset: 92
    char padding10[3];                   // Padding
    int32_t sortScore_backing;           // Offset: 96
};

// Generated from RoR2.DirectorCard
struct DirectorCard {
    char padding0[16];                        // Padding
    void* spawnCardReference;                 // Offset: 16
    SpawnCard* spawnCard;                     // Offset: 24
    void* requiredUnlockable;                 // Offset: 32
    void* forbiddenUnlockable;                // Offset: 40
    UnlockableDef* requiredUnlockableDef;     // Offset: 48
    UnlockableDef* forbiddenUnlockableDef;    // Offset: 56
    int32_t selectionWeight;                  // Offset: 64
    MonsterSpawnDistance_Value spawnDistance; // Offset: 68
    bool preventOverhead;                     // Offset: 72
    char padding9[3];                         // Padding
    int32_t minimumStageCompletions;          // Offset: 76
};

// Generated from .Category
struct Category {
    char padding0[16];     // Padding
    void* name;            // Offset: 16
    DirectorCard* cards;   // Offset: 24
    float selectionWeight; // Offset: 32
};

// Generated from RoR2.DirectorCardCategorySelection
struct DirectorCardCategorySelection {
    char padding0[24];        // Padding
    void* expansionsInEffect; // Offset: 24
    Category* categories;     // Offset: 32
};

// Generated from RoR2.EliteDef
struct EliteDef {
    char padding0[24];                          // Padding
    void* modifierToken;                        // Offset: 24
    EquipmentDef* eliteEquipmentDef;            // Offset: 32
    EliteIndex_Value eliteIndex_backing;        // Offset: 40
    Color32_Value color;                        // Offset: 44
    int32_t shaderEliteRampIndex;               // Offset: 48
    float healthBoostCoefficient;               // Offset: 52
    float damageBoostCoefficient;               // Offset: 56
    DevotionEvolutionLevel_Value devotionLevel; // Offset: 60
};

// Generated from .EliteTierDef
struct EliteTierDef {
    char padding0[16];                      // Padding
    EliteDef* eliteTypes;                   // Offset: 16
    void* isAvailable;                      // Offset: 24
    void* availableDefs;                    // Offset: 32
    float costMultiplier;                   // Offset: 40
    bool canSelectWithoutAvailableEliteDef; // Offset: 44
};

// Generated from .DirectorMoneyWave
struct DirectorMoneyWave {
    char padding0[16];      // Padding
    float interval;         // Offset: 16
    float timer;            // Offset: 20
    float multiplier;       // Offset: 24
    float accumulatedAward; // Offset: 28
};

// Generated from RoR2.CombatDirector
struct CombatDirector {
    char padding0[24];                                        // Padding
    void* customName;                                         // Offset: 24
    RangeFloat* moneyWaveIntervals;                           // Offset: 32
    OnSpawnedServer* onSpawnedServer;                         // Offset: 40
    OnSpawnedWithDirectorServer* onSpawnedWithDirectorServer; // Offset: 48
    CombatSquad* combatSquad;                                 // Offset: 56
    GameObject* spawnEffectPrefab;                            // Offset: 64
    DirectorCardCategorySelection* _monsterCards;             // Offset: 72
    DirectorCardCategorySelection* _backupMonsterCards;       // Offset: 80
    DirectorCard* lastAttemptedMonsterCard_backing;           // Offset: 88
    Xoroshiro128Plus* rng;                                    // Offset: 96
    DirectorCard* currentMonsterCard;                         // Offset: 104
    EliteTierDef* currentActiveEliteTier;                     // Offset: 112
    EliteDef* currentActiveEliteDef;                          // Offset: 120
    EliteDef* ActiveEliteDefOverride;                         // Offset: 128
    void* monsterCardsSelection;                              // Offset: 136
    DirectorCard* _bossOverride;                              // Offset: 144
    GameObject* currentSpawnTarget;                           // Offset: 152
    Component* _customPlacementBase;                          // Offset: 160
    DirectorMoneyWave* moneyWaves;                            // Offset: 168
    float monsterCredit;                                      // Offset: 176
    float refundedMonsterCredit;                              // Offset: 180
    float expRewardCoefficient;                               // Offset: 184
    float goldRewardCoefficient;                              // Offset: 188
    float minSeriesSpawnInterval;                             // Offset: 192
    float maxSeriesSpawnInterval;                             // Offset: 196
    float minRerollSpawnInterval;                             // Offset: 200
    float maxRerollSpawnInterval;                             // Offset: 204
    TeamIndex_Value teamIndex;                                // Offset: 208
    char padding28[3];                                        // Padding
    float creditMultiplier;                                   // Offset: 212
    float spawnDistanceMultiplier;                            // Offset: 216
    float maxSpawnDistance;                                   // Offset: 220
    float minSpawnRange;                                      // Offset: 224
    bool shouldSpawnOneWave;                                  // Offset: 228
    bool targetPlayers;                                       // Offset: 229
    bool skipSpawnIfTooCheap;                                 // Offset: 230
    char padding35[1];                                        // Padding
    int32_t maxConsecutiveCheapSkips;                         // Offset: 232
    bool resetMonsterCardIfFailed;                            // Offset: 236
    char padding37[3];                                        // Padding
    int32_t maximumNumberToSpawnBeforeSkipping;               // Offset: 240
    float eliteBias;                                          // Offset: 244
    bool increaseSpawnDistanceOnFailure;                      // Offset: 248
    char padding40[3];                                        // Padding
    uint32_t maxSquadCount;                                   // Offset: 252
    bool ignoreTeamSizeLimit;                                 // Offset: 256
    bool fallBackToStageMonsterCards;                         // Offset: 257
    char padding43[2];                                        // Padding
    float monsterSpawnTimer_backing;                          // Offset: 260
    float totalCreditsSpent_backing;                          // Offset: 264
    bool hasStartedWave;                                      // Offset: 268
    char padding46[3];                                        // Padding
    int32_t currentMonsterCardCost;                           // Offset: 272
    int32_t consecutiveCheapSkips;                            // Offset: 276
    bool _bossOverrideSpawnSingleBoss;                        // Offset: 280
    bool ignoreCostOfNextMonster;                             // Offset: 281
    char padding50[2];                                        // Padding
    float playerRetargetTimer;                                // Offset: 284
    int32_t spawnCountInCurrentWave;                          // Offset: 288
    bool isHalcyonShrineSpawn;                                // Offset: 292
    char padding53[3];                                        // Padding
    int32_t shrineHalcyoniteDifficultyLevel;                  // Offset: 296
};

// Generated from EntityStates.EntityState
struct EntityState {
    char padding0[16];         // Padding
    EntityStateMachine* outer; // Offset: 16
    float age_backing;         // Offset: 24
    float fixedAge_backing;    // Offset: 28
};

// Generated from RoR2.NetworkStateMachine
struct NetworkStateMachine {
    char padding0[48];                 // Padding
    EntityStateMachine* stateMachines; // Offset: 48
    NetworkIdentity* networkIdentity;  // Offset: 56
};

// Generated from RoR2.EntityStateMachine
struct EntityStateMachine {
    char padding0[24];                                  // Padding
    EntityState* state_backing;                         // Offset: 24
    EntityState* nextState;                             // Offset: 32
    void* customName;                                   // Offset: 40
    SerializableEntityStateType_Value initialStateType; // Offset: 48
    SerializableEntityStateType_Value mainStateType;    // Offset: 64
    NetworkStateMachine* networker_backing;             // Offset: 80
    NetworkIdentity* networkIdentity_backing;           // Offset: 88
    CommonComponentCache_Value commonComponents;        // Offset: 96
    ModifyNextStateDelegate* nextStateModifier;         // Offset: 240
    int32_t networkIndex;                               // Offset: 248
    bool AllowStartWithoutNetworker;                    // Offset: 252
    bool ShouldStateTransitionOnUpdate;                 // Offset: 253
    bool debug;                                         // Offset: 254
    bool destroying_backing;                            // Offset: 255
};

// Generated from RoR2.OutsideInteractableLocker
struct OutsideInteractableLocker {
    char padding0[24];             // Padding
    GameObject* lockPrefab;        // Offset: 24
    void* lockObjectMap;           // Offset: 32
    void* eggLockInfoMap;          // Offset: 40
    void* lockInteractableMap;     // Offset: 48
    IEnumerator* currentCoroutine; // Offset: 56
    float updateInterval;          // Offset: 64
    bool lockInside;               // Offset: 68
    char padding7[3];              // Padding
    float radius_backing;          // Offset: 72
    float updateTimer;             // Offset: 76
};

// Generated from RoR2.PickupDropTable
struct PickupDropTable {
    char padding0[24];      // Padding
    bool canDropBeReplaced; // Offset: 24
};

// Generated from .BossMemory
struct BossMemory {
    char padding0[16];                        // Padding
    NetworkInstanceId_Value masterInstanceId; // Offset: 16
    float maxObservedMaxHealth;               // Offset: 20
    float lastObservedHealth;                 // Offset: 24
    char padding3[4];                         // Padding
    CharacterMaster* cachedMaster;            // Offset: 32
    CharacterBody* cachedBody;                // Offset: 40
    float priority;                           // Offset: 48
};

// Generated from RoR2.BossGroup
struct BossGroup {
    char padding0[24];                                       // Padding
    Transform* dropPosition;                                 // Offset: 24
    PickupDropTable* dropTable;                              // Offset: 32
    EncounterHealthThresholdController* thresholdController; // Offset: 40
    CombatSquad* combatSquad_backing;                        // Offset: 48
    Xoroshiro128Plus* rng;                                   // Offset: 56
    void* bossDropTables;                                    // Offset: 64
    void* bossDrops;                                         // Offset: 72
    BossMemory* bossMemories;                                // Offset: 80
    void* bestObservedName_backing;                          // Offset: 88
    void* bestObservedSubtitle_backing;                      // Offset: 96
    float bossDropChance;                                    // Offset: 104
    bool scaleRewardsByPlayerCount;                          // Offset: 108
    bool shouldDisplayHealthBarOnHud;                        // Offset: 109
    bool shouldCreateObjectiveEntry;                         // Offset: 110
    char padding14[1];                                       // Padding
    int32_t bonusRewardCount_backing;                        // Offset: 112
    bool bossDropTablesLocked;                               // Offset: 116
    char padding16[3];                                       // Padding
    FixedTimeStamp_Value enabledTime;                        // Offset: 120
    bool forceTier3Reward;                                   // Offset: 124
    char padding18[3];                                       // Padding
    int32_t bossMemoryCount;                                 // Offset: 128
    float currentPriority;                                   // Offset: 132
    int32_t bodiesOfCurrentPriority;                         // Offset: 136
    float lastDisplayedPriority;                             // Offset: 140
    float totalMaxObservedMaxHealth_backing;                 // Offset: 144
    float totalObservedHealth_backing;                       // Offset: 148
};

// Generated from .NameTransformPair
struct NameTransformPair {
    char padding0[16];    // Padding
    void* name;           // Offset: 16
    Transform* transform; // Offset: 24
};

// Generated from .ChildLocator
struct ChildLocator {
    char padding0[24];                 // Padding
    NameTransformPair* transformPairs; // Offset: 24
};

// Generated from RoR2.NetworkSoundEventDef
struct NetworkSoundEventDef {
    char padding0[24];                          // Padding
    void* eventName;                            // Offset: 24
    NetworkSoundEventIndex_Value index_backing; // Offset: 32
    uint32_t akId_backing;                      // Offset: 36
};

// Generated from RoR2.BuffDef
struct BuffDef {
    char padding0[24];                                 // Padding
    void* iconPath;                                    // Offset: 24
    Sprite* iconSprite;                                // Offset: 32
    EliteDef* eliteDef;                                // Offset: 40
    NetworkSoundEventDef* startSfx;                    // Offset: 48
    BuffIndex_Value buffIndex_backing;                 // Offset: 56
    Color_Value buffColor;                             // Offset: 60
    bool canStack;                                     // Offset: 76
    bool isDebuff;                                     // Offset: 77
    bool isDOT;                                        // Offset: 78
    bool ignoreGrowthNectar;                           // Offset: 79
    bool isCooldown;                                   // Offset: 80
    bool isHidden;                                     // Offset: 81
    char padding12[2];                                 // Padding
    Flags_Value flags;                                 // Offset: 84
    StackingDisplayMethod_Value stackingDisplayMethod; // Offset: 88
};

// Generated from RoR2.TeamFilter
struct TeamFilter {
    char padding0[48];           // Padding
    int32_t teamIndexInternal;   // Offset: 48
    TeamIndex_Value defaultTeam; // Offset: 52
};

// Generated from RoR2.BuffWard
struct BuffWard {
    char padding0[48];                      // Padding
    Transform* rangeIndicator;              // Offset: 48
    BuffDef* buffDef;                       // Offset: 56
    AnimationCurve* radiusCoefficientCurve; // Offset: 64
    void* removalSoundString;               // Offset: 72
    UnityEvent* onRemoval;                  // Offset: 80
    TeamFilter* teamFilter;                 // Offset: 88
    BuffWardShape_Value shape;              // Offset: 96
    float radius;                           // Offset: 100
    float TubeHeight;                       // Offset: 104
    Vector3 CubeSize;                       // Offset: 108
    float interval;                         // Offset: 120
    float buffDuration;                     // Offset: 124
    bool floorWard;                         // Offset: 128
    bool expires;                           // Offset: 129
    bool invertTeamFilter;                  // Offset: 130
    char padding15[1];                      // Padding
    float expireDuration;                   // Offset: 132
    bool animateRadius;                     // Offset: 136
    char padding17[3];                      // Padding
    float removalTime;                      // Offset: 140
    bool needsRemovalTime;                  // Offset: 144
    bool requireGrounded;                   // Offset: 145
    bool requireAlive;                      // Offset: 146
    bool resetStopwatchOnDisable;           // Offset: 147
    bool useTeamMemberFeetPosition;         // Offset: 148
    char padding23[3];                      // Padding
    float buffTimer;                        // Offset: 152
    float rangeIndicatorScaleVelocity;      // Offset: 156
    float stopwatch;                        // Offset: 160
    float calculatedRadius;                 // Offset: 164
};

// Generated from RoR2.HoldoutZoneController
struct HoldoutZoneController {
    char padding0[48];                                                        // Padding
    Renderer* radiusIndicator;                                                // Offset: 48
    GameObject* healingNovaItemEffect;                                        // Offset: 56
    Transform* healingNovaRoot;                                               // Offset: 64
    void* inBoundsObjectiveToken;                                             // Offset: 72
    void* outOfBoundsObjectiveToken;                                          // Offset: 80
    HoldoutZoneControllerChargedUnityEvent* onCharged;                        // Offset: 88
    CalcRadiusDelegate* calcRadius;                                           // Offset: 96
    CalcChargeRateDelegate* calcChargeRate;                                   // Offset: 104
    CalcPercentageOfPlayersInRadiusDelegate* calcPercentageOfPlayersInRadius; // Offset: 112
    CalcAccumulatedChargeDelegate* calcAccumulatedCharge;                     // Offset: 120
    CalcColorDelegate* calcColor;                                             // Offset: 128
    BuffWard* buffWard;                                                       // Offset: 136
    GameObject* healingNovaGeneratorsByTeam;                                  // Offset: 144
    HoldoutZoneShape_Value holdoutZoneShape;                                  // Offset: 152
    float baseRadius;                                                         // Offset: 156
    float minimumRadius;                                                      // Offset: 160
    float chargeRadiusDelta;                                                  // Offset: 164
    float baseChargeDuration;                                                 // Offset: 168
    float radiusSmoothTime;                                                   // Offset: 172
    bool showObjective;                                                       // Offset: 176
    bool applyFocusConvergence;                                               // Offset: 177
    bool applyHealingNova;                                                    // Offset: 178
    bool applyDevotedEvolution;                                               // Offset: 179
    bool applyDelusionResetChests;                                            // Offset: 180
    char padding24[3];                                                        // Padding
    float playerCountScaling;                                                 // Offset: 184
    float dischargeRate;                                                      // Offset: 188
    float currentRadius_backing;                                              // Offset: 192
    bool isAnyoneCharging_backing;                                            // Offset: 196
    TeamIndex_Value chargingTeam_backing;                                     // Offset: 197
    char padding29[2];                                                        // Padding
    Color_Value baseIndicatorColor;                                           // Offset: 200
    float radiusVelocity;                                                     // Offset: 216
    bool wasCharged;                                                          // Offset: 220
    char padding32[3];                                                        // Padding
    float _charge;                                                            // Offset: 224
};

// Generated from RoR2.SceneDef
struct SceneDef {
    char padding0[24];                              // Padding
    AssetReferenceScene* sceneAddress;              // Offset: 24
    void* baseSceneNameOverride;                    // Offset: 32
    ExpansionDef* requiredExpansion;                // Offset: 40
    void* nameToken;                                // Offset: 48
    void* subtitleToken;                            // Offset: 56
    Texture* previewTexture;                        // Offset: 64
    AssetReferenceTexture* previewTextureReference; // Offset: 72
    Material* portalMaterial;                       // Offset: 80
    void* portalMaterialReference;                  // Offset: 88
    void* portalSelectionMessageString;             // Offset: 96
    void* loreToken;                                // Offset: 104
    GameObject* dioramaPrefab;                      // Offset: 112
    AssetReferenceGameObject* dioramaPrefabAddress; // Offset: 120
    MusicTrackDef* mainTrack;                       // Offset: 128
    MusicTrackDef* bossTrack;                       // Offset: 136
    SceneCollection* destinationsGroup;             // Offset: 144
    SceneCollection* loopedDestinationsGroup;       // Offset: 152
    SceneDef* loopedSceneDef;                       // Offset: 160
    SceneDef* destinations;                         // Offset: 168
    GameObject* preferredPortalPrefab;              // Offset: 176
    AssetReferenceGameObject* portalPrefabAddress;  // Offset: 184
    void* _cachedName;                              // Offset: 192
    void* sceneNameOverrides;                       // Offset: 200
    SceneIndex_Value sceneDefIndex_backing;         // Offset: 208
    SceneType_Value sceneType;                      // Offset: 212
    bool isOfflineScene;                            // Offset: 216
    char padding26[3];                              // Padding
    int32_t stageOrder;                             // Offset: 220
    bool filterOutOfBazaar;                         // Offset: 224
    bool shouldIncludeInLogbook;                    // Offset: 225
    char padding29[2];                              // Padding
    Color_Value environmentColor;                   // Offset: 228
    bool hasSafeStart;                              // Offset: 244
    bool suppressPlayerEntry;                       // Offset: 245
    bool suppressNpcEntry;                          // Offset: 246
    bool blockOrbitalSkills;                        // Offset: 247
    bool validForRandomSelection;                   // Offset: 248
    bool allowItemsToSpawnObjects;                  // Offset: 249
    bool preventStageAdvanceCounter;                // Offset: 250
    bool needSkipDevotionRespawn;                   // Offset: 251
    bool shouldUpdateSceneCollectionAfterLooping;   // Offset: 252
    bool isLockedBeforeLooping;                     // Offset: 253
};

// Generated from RoR2.ConvertPlayerMoneyToExperience
struct ConvertPlayerMoneyToExperience {
    char padding0[24];   // Padding
    void* burstSizes;    // Offset: 24
    float burstTimer;    // Offset: 32
    float burstInterval; // Offset: 36
    int32_t burstCount;  // Offset: 40
};

// Generated from RoR2.SceneExitController
struct SceneExitController {
    char padding0[24];                                   // Padding
    SceneDef* destinationScene;                          // Offset: 24
    SceneDef* tier1AlternateDestinationScene;            // Offset: 32
    SceneDef* tier2AlternateDestinationScene;            // Offset: 40
    SceneDef* tier3AlternateDestinationScene;            // Offset: 48
    SceneDef* tier4AlternateDestinationScene;            // Offset: 56
    SceneDef* tier5AlternateDestinationScene;            // Offset: 64
    ConvertPlayerMoneyToExperience* experienceCollector; // Offset: 72
    bool useRunNextStageScene;                           // Offset: 80
    char padding8[3];                                    // Padding
    float teleportOutTimer;                              // Offset: 84
    bool isAlternatePath;                                // Offset: 88
    bool isColossusPortal;                               // Offset: 89
    bool portalUsedAfterTeleporterEventFinish;           // Offset: 90
    char padding12[1];                                   // Padding
    ExitState_Value exitState;                           // Offset: 92
};

// Generated from RoR2.PositionIndicator
struct PositionIndicator {
    char padding0[24];                  // Padding
    Transform* targetTransform;         // Offset: 24
    Transform* transform;               // Offset: 32
    GameObject* insideViewObject;       // Offset: 40
    GameObject* outsideViewObject;      // Offset: 48
    GameObject* alwaysVisibleObject;    // Offset: 56
    bool shouldRotateOutsideViewObject; // Offset: 64
    char padding6[3];                   // Padding
    float outsideViewRotationOffset;    // Offset: 68
    float yOffset;                      // Offset: 72
    bool generateDefaultPosition;       // Offset: 76
    char padding9[3];                   // Padding
    Vector3 defaultPosition_backing;    // Offset: 80
};

// Generated from RoR2.UI.ChargeIndicatorController
struct ChargeIndicatorController {
    char padding0[24];                            // Padding
    SpriteRenderer* iconSprites;                  // Offset: 24
    TextMeshPro* chargingText;                    // Offset: 32
    TextMeshPro* pingText;                        // Offset: 40
    HoldoutZoneController* holdoutZoneController; // Offset: 48
    ObjectScaleCurve* pingBounceAnimation;        // Offset: 56
    UserProfile* cachedUserProfile;               // Offset: 64
    Color_Value spriteBaseColor;                  // Offset: 72
    Color_Value spriteFlashColor;                 // Offset: 88
    Color_Value spriteChargingColor;              // Offset: 104
    Color_Value spriteChargedColor;               // Offset: 120
    Color_Value textBaseColor;                    // Offset: 136
    Color_Value textChargingColor;                // Offset: 152
    Color_Value playerPingColor;                  // Offset: 168
    bool disableTextWhenNotCharging;              // Offset: 184
    bool disableTextWhenCharged;                  // Offset: 185
    bool flashWhenNotCharging;                    // Offset: 186
    char padding16[1];                            // Padding
    float flashFrequency;                         // Offset: 188
    bool isCharging;                              // Offset: 192
    bool isCharged;                               // Offset: 193
    bool isDiscovered;                            // Offset: 194
    bool isActivated;                             // Offset: 195
    uint32_t chargeValue_backing;                 // Offset: 196
    float flashStopwatch;                         // Offset: 200
    bool isPinged;                                // Offset: 204
    bool showIndicatorWhenDiscovered;             // Offset: 205
};

// Generated from RoR2.InteractableSpawnCard
struct InteractableSpawnCard {
    char padding0[64];                              // Padding
    bool orientToFloor;                             // Offset: 64
    bool slightlyRandomizeOrientation;              // Offset: 65
    bool skipSpawnWhenSacrificeArtifactEnabled;     // Offset: 66
    char padding3[1];                               // Padding
    float weightScalarWhenSacrificeArtifactEnabled; // Offset: 68
    bool skipSpawnWhenDevotionArtifactEnabled;      // Offset: 72
    char padding5[3];                               // Padding
    int32_t maxSpawnsPerStage;                      // Offset: 76
    float prismaticTrialSpawnChance;                // Offset: 80
};

// Generated from RoR2.PortalSpawner
struct PortalSpawner {
    char padding0[48];                             // Padding
    InteractableSpawnCard* portalSpawnCard;        // Offset: 48
    Transform* spawnReferenceLocation;             // Offset: 56
    void* spawnPreviewMessageToken;                // Offset: 64
    void* spawnMessageToken;                       // Offset: 72
    ChildLocator* modelChildLocator;               // Offset: 80
    void* previewChildName;                        // Offset: 88
    ExpansionDef* requiredExpansion;               // Offset: 96
    void* bannedEventFlag;                         // Offset: 104
    Transform* spawnReferenceLocationOverride;     // Offset: 112
    void* requiredEventFlag;                       // Offset: 120
    String* validStages;                           // Offset: 128
    String* invalidStages;                         // Offset: 136
    Int32* validStageTiers;                        // Offset: 144
    Xoroshiro128Plus* rng;                         // Offset: 152
    GameObject* previewChild;                      // Offset: 160
    float spawnChance;                             // Offset: 168
    float minSpawnDistance;                        // Offset: 172
    float maxSpawnDistance;                        // Offset: 176
    int32_t minStagesCleared;                      // Offset: 180
    bool registerSelfToPortalRemotelyOnActivation; // Offset: 184
    bool hasRegisteredToTeleporter;                // Offset: 185
    bool willSpawn;                                // Offset: 186
};

// Generated from .Particle_SetMinSize
struct Particle_SetMinSize {
    char padding0[24];                // Padding
    float minimumPixelCoverage;       // Offset: 24
    float defaultMinimumParticleSize; // Offset: 28
    bool _isEnabled;                  // Offset: 32
};

// Generated from RoR2.TeleporterInteraction
struct TeleporterInteraction {
    char padding0[48];                                              // Padding
    CombatDirector* bonusDirector;                                  // Offset: 48
    CombatDirector* bossDirector;                                   // Offset: 56
    CombatDirector* companionBoss;                                  // Offset: 64
    EntityStateMachine* mainStateMachine;                           // Offset: 72
    OutsideInteractableLocker* outsideInteractableLocker;           // Offset: 80
    void* beginContextString;                                       // Offset: 88
    void* exitContextString;                                        // Offset: 96
    void* interactableObjectMap;                                    // Offset: 104
    BossGroup* bossGroup;                                           // Offset: 112
    ChildLocator* modelChildLocator;                                // Offset: 120
    HoldoutZoneController* holdoutZoneController_backing;           // Offset: 128
    SceneExitController* sceneExitController_backing;               // Offset: 136
    PositionIndicator* teleporterPositionIndicator;                 // Offset: 144
    ChargeIndicatorController* teleporterChargeIndicatorController; // Offset: 152
    BossShrineCounter* _bossShrineCounter;                          // Offset: 160
    Xoroshiro128Plus* rng;                                          // Offset: 168
    GameObject* chargeActivatorServer;                              // Offset: 176
    SpawnCard* shopPortalSpawnCard;                                 // Offset: 184
    SpawnCard* goldshoresPortalSpawnCard;                           // Offset: 192
    SpawnCard* msPortalSpawnCard;                                   // Offset: 200
    SpawnCard* hiddenRealmPortalSpawnCard;                          // Offset: 208
    PortalSpawner* portalSpawners;                                  // Offset: 216
    void* currentPings;                                             // Offset: 224
    Particle_SetMinSize* cachedParticleMinSizeScript;               // Offset: 232
    LocalUser* cachedLocalUser;                                     // Offset: 240
    float triggerWavePillarInterval;                                // Offset: 248
    float sceneStartAmbientLevel_backing;                           // Offset: 252
    bool _locked;                                                   // Offset: 256
    char padding28[3];                                              // Padding
    int32_t _shrineBonusStacks;                                     // Offset: 260
    bool isAccessingCodes_backing;                                  // Offset: 264
    bool isDiscovered;                                              // Offset: 265
    bool isActivated;                                               // Offset: 266
    char padding32[1];                                              // Padding
    float discoveryRadius;                                          // Offset: 268
    Color_Value originalTeleporterColor;                            // Offset: 272
    bool _shouldAttemptToSpawnShopPortal;                           // Offset: 288
    bool _shouldAttemptToSpawnGoldshoresPortal;                     // Offset: 289
    bool _shouldAttemptToSpawnMSPortal;                             // Offset: 290
    bool _shouldAttemptToSpawnHiddenRealmPortal;                    // Offset: 291
    bool monstersCleared;                                           // Offset: 292
    bool showExtraBossesIndicator;                                  // Offset: 293
    bool showAccessCodesIndicator;                                  // Offset: 294
    char padding41[1];                                              // Padding
    float baseShopSpawnChance;                                      // Offset: 296
    bool skipCombat;                                                // Offset: 300
    char padding43[3];                                              // Padding
    float particleScaleUpdateTimer;                                 // Offset: 304
    bool shouldScaleParticles;                                      // Offset: 308
};

// Generated from .VectorPID
struct VectorPID {
    char padding0[24];    // Padding
    void* customName;     // Offset: 24
    Vector3 PID;          // Offset: 32
    Vector3 inputVector;  // Offset: 44
    Vector3 targetVector; // Offset: 56
    Vector3 outputVector; // Offset: 68
    bool isAngle;         // Offset: 80
    char padding6[3];     // Padding
    float gain;           // Offset: 84
    Vector3 errorSum;     // Offset: 88
    Vector3 deltaError;   // Offset: 100
    Vector3 lastError;    // Offset: 112
    float lastTimer;      // Offset: 124
    float timer;          // Offset: 128
};

// Generated from RoR2.BodyAnimatorSmoothingParameters
struct BodyAnimatorSmoothingParameters {
    char padding0[24];                             // Padding
    SmoothingParameters_Value smoothingParameters; // Offset: 24
};

// Generated from RoR2.RigidbodyMotor
struct RigidbodyMotor {
    char padding0[48];                                                // Padding
    Rigidbody* rigid;                                                 // Offset: 48
    VectorPID* forcePID;                                              // Offset: 56
    void* animatorForward;                                            // Offset: 64
    void* animatorRight;                                              // Offset: 72
    void* animatorUp;                                                 // Offset: 80
    NetworkIdentity* networkIdentity;                                 // Offset: 88
    CharacterBody* characterBody;                                     // Offset: 96
    InputBankTest* inputBank;                                         // Offset: 104
    ModelLocator* modelLocator;                                       // Offset: 112
    Animator* animator;                                               // Offset: 120
    BodyAnimatorSmoothingParameters* bodyAnimatorSmoothingParameters; // Offset: 128
    HealthComponent* healthComponent;                                 // Offset: 136
    MovementHitDelegate* onMovementHit;                               // Offset: 144
    Vector3 moveVector;                                               // Offset: 152
    Vector3 centerOfMassOffset;                                       // Offset: 164
    bool enableOverrideMoveVectorInLocalSpace;                        // Offset: 176
    bool canTakeImpactDamage;                                         // Offset: 177
    char padding17[2];                                                // Padding
    Vector3 overrideMoveVectorInLocalSpace;                           // Offset: 180
    Vector3 rootMotion;                                               // Offset: 192
};

// Generated from .QuaternionPID
struct QuaternionPID {
    char padding0[24];     // Padding
    void* customName;      // Offset: 24
    Vector3 PID;           // Offset: 32
    Quaternion inputQuat;  // Offset: 44
    Quaternion targetQuat; // Offset: 60
    Vector3 outputVector;  // Offset: 76
    float gain;            // Offset: 88
    Vector3 errorSum;      // Offset: 92
    Vector3 deltaError;    // Offset: 104
    Vector3 lastError;     // Offset: 116
    float lastUpdateTime;  // Offset: 128
};

// Generated from .HurtBoxInfo
struct HurtBoxInfo {
    char padding0[16];     // Padding
    Transform* transform;  // Offset: 16
    float estimatedRadius; // Offset: 24
};

// Generated from RoR2.HurtBoxGroup
struct HurtBoxGroup {
    char padding0[24];                    // Padding
    HurtBox* hurtBoxes;                   // Offset: 24 - MonoArray of HurtBox pointers
    HurtBox* mainHurtBox;                 // Offset: 32
    int32_t bullseyeCount;                // Offset: 40
    int32_t _hurtBoxesDeactivatorCounter; // Offset: 44
};

// Generated from RoR2.CharacterModel
struct CharacterModel {
    char padding0[24];                                        // Padding
    CharacterBody* body;                                      // Offset: 24
    ItemDisplayRuleSet* itemDisplayRuleSet;                   // Offset: 32
    void* gameObjectActivationTransforms;                     // Offset: 40
    void* customGameObjectActivationTransforms;               // Offset: 48
    RendererInfo* baseRendererInfos;                          // Offset: 56
    LightInfo* baseLightInfos;                                // Offset: 64
    ChildLocator* childLocator;                               // Offset: 72
    GameObject* goldAffixEffect;                              // Offset: 80
    HurtBoxInfo* hurtBoxInfos;                                // Offset: 88
    Transform* coreTransform;                                 // Offset: 96
    void* temporaryOverlays;                                  // Offset: 104
    MaterialPropertyBlock* propertyStorage;                   // Offset: 112
    SkinnedMeshRenderer* mainSkinnedMeshRenderer;             // Offset: 120
    RendererVisiblity* visibilityChecker;                     // Offset: 128
    Material* particleMaterialOverride;                       // Offset: 136
    GameObject* poisonAffixEffect;                            // Offset: 144
    GameObject* hauntedAffixEffect;                           // Offset: 152
    GameObject* voidAffixEffect;                              // Offset: 160
    ItemMask* enabledItemDisplays;                            // Offset: 168
    void* parentedPrefabDisplays;                             // Offset: 176
    void* limbMaskDisplays;                                   // Offset: 184
    LimbFlagSet* limbFlagSet;                                 // Offset: 192
    Material* currentOverlays;                                // Offset: 200
    RtpcSetter_Value rtpcEliteEnemy;                          // Offset: 208
    char padding24[7];                                        // Padding
    bool autoPopulateLightInfos;                              // Offset: 248
    bool fullBodyPing;                                        // Offset: 249
    char padding26[2];                                        // Padding
    VisibilityLevel_Value _visibility;                        // Offset: 252
    bool _isGhost;                                            // Offset: 256
    bool _isDoppelganger;                                     // Offset: 257
    bool _isEcho;                                             // Offset: 258
    char padding30[1];                                        // Padding
    int32_t _invisibilityCount;                               // Offset: 260
    bool materialsDirty;                                      // Offset: 264
    char padding32[3];                                        // Padding
    EquipmentIndex_Value inventoryEquipmentIndex;             // Offset: 268
    EliteIndex_Value myEliteIndex;                            // Offset: 272
    bool constraintItemDisplaysScale;                         // Offset: 276
    char padding35[3];                                        // Padding
    float fade;                                               // Offset: 280
    float firstPersonFade;                                    // Offset: 284
    bool ignoreJitterBones;                                   // Offset: 288
    char padding38[3];                                        // Padding
    float corpseFade;                                         // Offset: 292
    float speedFade;                                          // Offset: 296
    bool CharacterOnScreen;                                   // Offset: 300
    char padding41[3];                                        // Padding
    void* lightColorOverride;                                 // Offset: 304
    char padding42[12];                                       // Padding
    bool useHullForFadeCalc;                                  // Offset: 324
    char padding43[3];                                        // Padding
    float affixHauntedCloakLockoutDuration;                   // Offset: 328
    EquipmentIndex_Value currentEquipmentDisplayIndex;        // Offset: 332
    int32_t activeOverlayCount;                               // Offset: 336
    bool wasPreviouslyClayGooed;                              // Offset: 340
    bool wasPreviouslyHaunted;                                // Offset: 341
    bool wasPreviouslyOiled;                                  // Offset: 342
    bool wasPreviouslyAccelerantGooded;                       // Offset: 343
    bool wasPreviouslyTransferDebuffed;                       // Offset: 344
    bool wasPreviouslyOverheated;                             // Offset: 345
    bool wasPreviouslyParrying;                               // Offset: 346
    char padding53[1];                                        // Padding
    DroneIndexEmissionColorPair_Value shaderDroneUpgradePair; // Offset: 348
    int32_t shaderEliteRampIndex;                             // Offset: 368
    bool eliteChanged;                                        // Offset: 372
    char padding56[3];                                        // Padding
    int32_t activeOverlays;                                   // Offset: 376
    int32_t oldOverlays;                                      // Offset: 380
    float hitFlashValue;                                      // Offset: 384
    float healFlashValue;                                     // Offset: 388
    float oldHit;                                             // Offset: 392
    float oldHeal;                                            // Offset: 396
    float oldFade;                                            // Offset: 400
    EliteIndex_Value oldEliteIndex;                           // Offset: 404
    bool forceUpdate;                                         // Offset: 408
};

// Generated from .RendererVisiblity
struct RendererVisiblity {
    char padding0[24];                         // Padding
    CharacterModel* parentModel;               // Offset: 24
    Animator* animatorToUpdateOnceWhenVisible; // Offset: 32
    DistanceClass_Value approxDistance;        // Offset: 40
    bool isVisible;                            // Offset: 44
    char padding4[3];                          // Padding
    float visualUpdateTimer;                   // Offset: 48
    bool shouldUpdateVisuals;                  // Offset: 52
};

// Generated from RoR2.RigidbodyDirection
struct RigidbodyDirection {
    char padding0[24];                 // Padding
    Rigidbody* rigid;                  // Offset: 24
    QuaternionPID* angularVelocityPID; // Offset: 32
    VectorPID* torquePID;              // Offset: 40
    ModelLocator* modelLocator;        // Offset: 48
    Animator* animator;                // Offset: 56
    void* animatorXCycle;              // Offset: 64
    void* animatorYCycle;              // Offset: 72
    void* animatorZCycle;              // Offset: 80
    InputBankTest* inputBank;          // Offset: 88
    RendererVisiblity* visibility;     // Offset: 96
    Vector3 aimDirection;              // Offset: 104
    bool freezeXRotation;              // Offset: 116
    bool freezeYRotation;              // Offset: 117
    bool freezeZRotation;              // Offset: 118
    char padding14[1];                 // Padding
    int32_t animatorXCycleIndex;       // Offset: 120
    int32_t animatorYCycleIndex;       // Offset: 124
    int32_t animatorZCycleIndex;       // Offset: 128
    float animatorTorqueScale;         // Offset: 132
    Vector3 targetTorque;              // Offset: 136
};

// Generated from RoR2.RailMotor
struct RailMotor {
    char padding0[24];                      // Padding
    Animator* modelAnimator;                // Offset: 24
    InputBankTest* inputBank;               // Offset: 32
    NodeGraph* railGraph;                   // Offset: 40
    CharacterBody* characterBody;           // Offset: 48
    CharacterDirection* characterDirection; // Offset: 56
    Vector3 inputMoveVector;                // Offset: 64
    Vector3 rootMotion;                     // Offset: 76
    NodeIndex_Value nodeA;                  // Offset: 88
    NodeIndex_Value nodeB;                  // Offset: 92
    LinkIndex_Value currentLink;            // Offset: 96
    float linkLerp;                         // Offset: 100
    Vector3 projectedMoveVector;            // Offset: 104
    Vector3 nodeAPosition;                  // Offset: 116
    Vector3 nodeBPosition;                  // Offset: 128
    Vector3 linkVector;                     // Offset: 140
    float linkLength;                       // Offset: 152
    float currentMoveSpeed;                 // Offset: 156
    bool useRootMotion;                     // Offset: 160
};

// Generated from .EmoteDef
struct EmoteDef {
    char padding0[16];                       // Padding
    void* name;                              // Offset: 16
    void* displayName;                       // Offset: 24
    EntityStateMachine* targetStateMachine;  // Offset: 32
    SerializableEntityStateType_Value state; // Offset: 40
};

// Generated from RoR2.CharacterEmoteDefinitions
struct CharacterEmoteDefinitions {
    char padding0[24];          // Padding
    EmoteDef* emoteDefinitions; // Offset: 24
};

// Generated from RoR2.Audio.LoopSoundDef
struct LoopSoundDef {
    char padding0[24];    // Padding
    void* startSoundName; // Offset: 24
    void* stopSoundName;  // Offset: 32
};

// Generated from RoR2.Projectile.ProjectileGhostController
struct ProjectileGhostController {
    char padding0[24];                             // Padding
    Transform* transform;                          // Offset: 24
    EffectManagerHelper* emh;                      // Offset: 32
    Transform* authorityTransform_backing;         // Offset: 40
    Transform* predictionTransform_backing;        // Offset: 48
    UnityEvent* ParentProjectileDestroyedEvent;    // Offset: 56
    float migration;                               // Offset: 64
    bool inheritScaleFromProjectile;               // Offset: 68
    bool alreadyRunParentProjectileDestroyedEvent; // Offset: 69
};

// Generated from RoR2.Projectile.ProjectileController
struct ProjectileController {
    char padding0[48];                                // Padding
    GameObject* ghostPrefab;                          // Offset: 48
    void* ghostPrefabReference;                       // Offset: 56
    Transform* ghostTransformAnchor;                  // Offset: 64
    void* startSound;                                 // Offset: 72
    LoopSoundDef* flightSoundLoop;                    // Offset: 80
    Rigidbody* rigidbody;                             // Offset: 88
    TeamFilter* teamFilter_backing;                   // Offset: 96
    ProjectileGhostController* ghost_backing;         // Offset: 104
    GameObject* owner;                                // Offset: 112
    void* onInitialized;                              // Offset: 120
    NetworkConnection* clientAuthorityOwner_backing;  // Offset: 128
    Collider* myColliders;                            // Offset: 136
    EffectManagerHelper* _efhGhost;                   // Offset: 144
    int32_t catalogIndex;                             // Offset: 152
    bool cannotBeDeleted;                             // Offset: 156
    bool authorityHandlesCollisionEvents;             // Offset: 157
    bool isPrediction_backing;                        // Offset: 158
    bool canImpactOnTrigger;                          // Offset: 159
    bool shouldPlaySounds_backing;                    // Offset: 160
    bool allowPrediction;                             // Offset: 161
    uint16_t predictionId;                            // Offset: 162
    bool localCollisionHappened;                      // Offset: 164
    char padding22[3];                                // Padding
    ProcChainMask_Value procChainMask_backing;        // Offset: 168
    float procCoefficient;                            // Offset: 172
    uint8_t combo;                                    // Offset: 176
    bool CheckChildrenForCollidersAndIncludeDisabled; // Offset: 177
    char padding26[2];                                // Padding
    NetworkInstanceId_Value ___ownerNetId;            // Offset: 180
};

// Generated from RoR2.ItemTierDef
struct ItemTierDef {
    char padding0[24];                // Padding
    Texture* bgIconTexture;           // Offset: 24
    GameObject* highlightPrefab;      // Offset: 32
    GameObject* dropletDisplayPrefab; // Offset: 40
    ItemTier_Value _tier;             // Offset: 48
    ColorIndex_Value colorIndex;      // Offset: 52
    ColorIndex_Value darkColorIndex;  // Offset: 56
    bool isDroppable;                 // Offset: 60
    bool canScrap;                    // Offset: 61
    bool canRestack;                  // Offset: 62
    bool canRebirth;                  // Offset: 63
    PickupRules_Value pickupRules;    // Offset: 64
};

// Generated from RoR2.ItemTag
struct ItemTag {
    char padding0[16]; // Padding
    int32_t value__;   // Offset: 16
};

// Generated from RoR2.ItemDef
struct ItemDef {
    char padding0[24];               // Padding
    ItemTierDef* _itemTierDef;       // Offset: 24
    void* nameToken;                 // Offset: 32
    void* pickupToken;               // Offset: 40
    void* descriptionToken;          // Offset: 48
    void* loreToken;                 // Offset: 56
    UnlockableDef* unlockableDef;    // Offset: 64
    GameObject* pickupModelPrefab;   // Offset: 72
    void* pickupModelReference;      // Offset: 80
    Sprite* pickupIconSprite;        // Offset: 88
    ItemTag* tags;                   // Offset: 96
    ExpansionDef* requiredExpansion; // Offset: 104
    ItemIndex_Value _itemIndex;      // Offset: 112
    ItemTier_Value deprecatedTier;   // Offset: 116
    bool isConsumed;                 // Offset: 120
    bool hidden;                     // Offset: 121
    bool canRemove;                  // Offset: 122
};

// Generated from RoR2.SkillLocator
struct SkillLocator {
    char padding0[48];                              // Padding
    GenericSkill* primary;                          // Offset: 48
    GenericSkill* secondary;                        // Offset: 56
    GenericSkill* utility;                          // Offset: 64
    GenericSkill* special;                          // Offset: 72
    PassiveSkill_Value passiveSkill;                // Offset: 80
    GenericSkill* primaryBonusStockOverrideSkill;   // Offset: 120
    GenericSkill* secondaryBonusStockOverrideSkill; // Offset: 128
    GenericSkill* utilityBonusStockOverrideSkill;   // Offset: 136
    GenericSkill* specialBonusStockOverrideSkill;   // Offset: 144
    NetworkIdentity* networkIdentity;               // Offset: 152
    GenericSkill* allSkills;                        // Offset: 160
    bool hasEffectiveAuthority;                     // Offset: 168
    char padding12[3];                              // Padding
    uint32_t skillDefDirtyFlags;                    // Offset: 172
    bool inDeserialize;                             // Offset: 176
};

// Generated from RoR2.GenericSkill
struct GenericSkill {
    char padding0[24];                                 // Padding
    SkillDef* skillDef_backing;                        // Offset: 24
    SkillFamily* _skillFamily;                         // Offset: 32
    SkillDef* baseSkill_backing;                       // Offset: 40
    void* skillName;                                   // Offset: 48
    void* loadoutTitleToken;                           // Offset: 56
    EntityStateMachine* stateMachine_backing;          // Offset: 64
    BaseSkillInstanceData* skillInstanceData_backing;  // Offset: 72
    CharacterBody* characterBody_backing;              // Offset: 80
    SkillDef* defaultSkillDef_backing;                 // Offset: 88
    void* onSkillChanged;                              // Offset: 96
    Event* skillAkEvent;                               // Offset: 104
    SkillOverride* skillOverrides;                     // Offset: 112
    StateMachineResolver* _customStateMachineResolver; // Offset: 120
    void* overriddenRechargeInterval_backing;          // Offset: 128
    bool hideInCharacterSelect;                        // Offset: 136
    bool hideInLoadoutSelect;                          // Offset: 137
    bool isCooldownBlocked_backing;                    // Offset: 138
    char padding17[1];                                 // Padding
    int32_t currentSkillOverride;                      // Offset: 140
    int32_t bonusStockFromBody;                        // Offset: 144
    int32_t maxStock_backing;                          // Offset: 148
    int32_t baseStock;                                 // Offset: 152
    float finalRechargeInterval;                       // Offset: 156
    float _cooldownScale;                              // Offset: 160
    float _flatCooldownReduction;                      // Offset: 164
    float _temporaryCooldownPenalty;                   // Offset: 168
    float _cooldownOverride;                           // Offset: 172
    float baseRechargeStopwatch;                       // Offset: 176
    bool hasExecutedSuccessfully;                      // Offset: 180
};

// Generated from RoR2.Skills.SkillDef
struct SkillDef {
    char padding0[24];                                 // Padding
    void* skillName;                                   // Offset: 24
    void* skillNameToken;                              // Offset: 32
    void* skillDescriptionToken;                       // Offset: 40
    String* keywordTokens;                             // Offset: 48
    Sprite* icon;                                      // Offset: 56
    void* activationStateMachineName;                  // Offset: 64
    SerializableEntityStateType_Value activationState; // Offset: 72
    int32_t skillIndex_backing;                        // Offset: 88
    InterruptPriority_Value interruptPriority;         // Offset: 92
    float baseRechargeInterval;                        // Offset: 96
    int32_t baseMaxStock;                              // Offset: 100
    int32_t rechargeStock;                             // Offset: 104
    int32_t requiredStock;                             // Offset: 108
    int32_t stockToConsume;                            // Offset: 112
    bool attackSpeedBuffsRestockSpeed;                 // Offset: 116
    char padding15[3];                                 // Padding
    float attackSpeedBuffsRestockSpeed_Multiplier;     // Offset: 120
    bool resetCooldownTimerOnUse;                      // Offset: 124
    bool fullRestockOnAssign;                          // Offset: 125
    bool dontAllowPastMaxStocks;                       // Offset: 126
    bool beginSkillCooldownOnSkillEnd;                 // Offset: 127
    bool isCooldownBlockedUntilManuallyReset;          // Offset: 128
    bool cancelSprintingOnActivation;                  // Offset: 129
    bool forceSprintDuringState;                       // Offset: 130
    bool canceledFromSprinting;                        // Offset: 131
    bool isCombatSkill;                                // Offset: 132
    bool mustKeyPress;                                 // Offset: 133
    bool triggeredByPressRelease;                      // Offset: 134
    bool autoHandleLuminousShot;                       // Offset: 135
    bool suppressSkillActivation;                      // Offset: 136
    bool hideStockCount;                               // Offset: 137
    bool hideCooldown;                                 // Offset: 138
};

// Generated from RoR2.BullseyeSearch
struct BullseyeSearch {
    char padding0[16];                                     // Padding
    CharacterBody* viewer;                                 // Offset: 16
    void* candidatesEnumerable;                            // Offset: 24
    void* DistinctEntityHash;                              // Offset: 32
    Vector3 searchOrigin;                                  // Offset: 40
    Vector3 searchDirection;                               // Offset: 52
    float minThetaDot;                                     // Offset: 64
    float maxThetaDot;                                     // Offset: 68
    float minDistanceFilter;                               // Offset: 72
    float maxDistanceFilter;                               // Offset: 76
    TeamMask_Value teamMaskFilter;                         // Offset: 80
    bool filterByLoS;                                      // Offset: 81
    bool filterByDistinctEntity;                           // Offset: 82
    char padding12[1];                                     // Padding
    QueryTriggerInteraction_Value queryTriggerInteraction; // Offset: 84
    SortMode_Value sortMode;                               // Offset: 88
};

// Generated from RoR2.HuntressTracker
struct HuntressTracker {
    char padding0[24];            // Padding
    GameObject* trackingPrefab;   // Offset: 24
    HurtBox* trackingTarget;      // Offset: 32
    CharacterBody* characterBody; // Offset: 40
    TeamComponent* teamComponent; // Offset: 48
    InputBankTest* inputBank;     // Offset: 56
    Indicator* indicator;         // Offset: 64
    BullseyeSearch* search;       // Offset: 72
    float maxTrackingDistance;    // Offset: 80
    float maxTrackingAngle;       // Offset: 84
    float trackerUpdateFrequency; // Offset: 88
    float trackerUpdateStopwatch; // Offset: 92
};

// Generated from RoR2.Skills.SkillFamily
struct SkillFamily {
    char padding0[24];            // Padding
    Variant* variants;            // Offset: 24
    int32_t catalogIndex_backing; // Offset: 32
    uint32_t defaultVariantIndex; // Offset: 36
};

// Generated from RoR2.PickupPickerController
struct PickupPickerController {
    char padding0[48];                                    // Padding
    GameObject* panelPrefab;                              // Offset: 48
    PickupIndexUnityEvent* onPickupSelected;              // Offset: 56
    UniquePickupUnityEvent* onUniquePickupSelected;       // Offset: 64
    InteractorUnityEvent* onServerInteractionBegin;       // Offset: 72
    void* contextString;                                  // Offset: 80
    ExplicitPickupDropTable* excludedItemsDropTable;      // Offset: 88
    ExplicitPickupDropTable* includedItemsDropTable;      // Offset: 96
    Inventory* delusionInventory;                         // Offset: 104
    NetworkUIPromptController* networkUIPromptController; // Offset: 112
    GameObject* panelInstance;                            // Offset: 120
    PickupPickerPanel* panelInstanceController;           // Offset: 128
    ChestBehavior* chestGeneratedFrom;                    // Offset: 136
    Option* options;                                      // Offset: 144
    EventFunctions* eventFunctions;                       // Offset: 152
    float cutoffDistance;                                 // Offset: 160
    bool canUseTemporaryItems;                            // Offset: 164
    bool available_backing;                               // Offset: 165
    bool shouldProximityHighlight;                        // Offset: 166
    bool synchronizeItemSelectionAcrossNetwork;           // Offset: 167
    bool isDelusionChoice;                                // Offset: 168
    bool isRebirthChoice;                                 // Offset: 169
    bool allowRemoteOpInteraction;                        // Offset: 170
    bool delusionOptionsSet;                              // Offset: 171
    int32_t delusionChoicePickupIndex;                    // Offset: 172
    int32_t numberOfPickUpsSelected;                      // Offset: 176
    bool multiOptionsAvailable;                           // Offset: 180
};

// Generated from RoR2.PickupDropletController
struct PickupDropletController {
    char padding0[48];                       // Padding
    CreatePickupInfo_Value createPickupInfo; // Offset: 48
    char padding1[6];                        // Padding
    UniquePickup_Value pickupState;          // Offset: 144
    bool alive;                              // Offset: 156
};

// Generated from RoR2.ChestBehavior
struct ChestBehavior {
    char padding0[48];                              // Padding
    PickupDropTable* dropTable;                     // Offset: 48
    Transform* dropTransform;                       // Offset: 56
    SerializableEntityStateType_Value openState;    // Offset: 64
    SerializableEntityStateType_Value closingState; // Offset: 80
    UnityEvent* dropRoller;                         // Offset: 96
    Xoroshiro128Plus* rng;                          // Offset: 104
    PickupPickerController* ppc;                    // Offset: 112
    UniquePickup_Value currentPickup_backing;       // Offset: 120
    float dropUpVelocityStrength;                   // Offset: 132
    float dropForwardVelocityStrength;              // Offset: 136
    int32_t minDropCount;                           // Offset: 140
    int32_t maxDropCount;                           // Offset: 144
    float tier1Chance;                              // Offset: 148
    float tier2Chance;                              // Offset: 152
    float tier3Chance;                              // Offset: 156
    float lunarChance;                              // Offset: 160
    ItemTag_Value requiredItemTag;                  // Offset: 164
    float lunarCoinChance;                          // Offset: 168
    int32_t dropCount;                              // Offset: 172
    bool isCommandChest;                            // Offset: 176
    bool isChestOpened;                             // Offset: 177
    bool isChestReset;                              // Offset: 178
};

// Generated from RoR2.PurchaseInteraction
struct PurchaseInteraction {
    char padding0[48];                                      // Padding
    void* displayNameToken;                                 // Offset: 48
    void* contextToken;                                     // Offset: 56
    void* requiredUnlockable;                               // Offset: 64
    String* purchaseStatNames;                              // Offset: 72
    ExpansionDef* requiredExpansion;                        // Offset: 80
    IInspectInfoProvider* cachedInspectInfoProviders;       // Offset: 88
    Interactor* lastActivator;                              // Offset: 96
    GameObject* lockGameObject;                             // Offset: 104
    Xoroshiro128Plus* rng;                                  // Offset: 112
    GameObjectUnlockableFilter* gameObjectUnlockableFilter; // Offset: 120
    PurchaseEvent* onPurchase;                              // Offset: 128
    DetailedPurchaseEvent* onDetailedPurchaseServer;        // Offset: 136
    Transform* viewerTransform;                             // Offset: 144
    CharacterBody* viewerBody;                              // Offset: 152
    CostTypeIndex_Value costType;                           // Offset: 160
    bool available;                                         // Offset: 164
    char padding16[3];                                      // Padding
    int32_t cost;                                           // Offset: 168
    int32_t solitudeCost;                                   // Offset: 172
    bool automaticallyScaleCostWithDifficulty;              // Offset: 176
    bool ignoreSpherecastForInteractability;                // Offset: 177
    bool setUnavailableOnTeleporterActivated;               // Offset: 178
    bool isShrine;                                          // Offset: 179
    bool isGoldShrine;                                      // Offset: 180
    bool isDrone;                                           // Offset: 181
    bool shouldProximityHighlight;                          // Offset: 182
    bool saleStarCompatible;                                // Offset: 183
    bool disableSpawnOnInteraction;                         // Offset: 184
    bool allowRemoteOpInteraction;                          // Offset: 185
    char padding28[2];                                      // Padding
    int32_t currentInspectIndex;                            // Offset: 188
    NetworkInstanceId_Value ___lockGameObjectNetId;         // Offset: 192
};

// Generated from RoR2.ShopTerminalBehavior
struct ShopTerminalBehavior {
    char padding0[48];                                // Padding
    PickupDisplay* pickupDisplay;                     // Offset: 48
    Transform* dropTransform;                         // Offset: 56
    PickupDropTable* dropTable;                       // Offset: 64
    Animator* animator;                               // Offset: 72
    InspectDef* MysteryItemInspectDef;                // Offset: 80
    void* inspectHintOverrideToken;                   // Offset: 88
    InspectDef* shopInspectDef;                       // Offset: 96
    IInspectInfoProvider* cachedInspectInfoProviders; // Offset: 104
    Xoroshiro128Plus* rng;                            // Offset: 112
    MultiShopController* serverMultiShopController;   // Offset: 120
    UniquePickup_Value pickup;                        // Offset: 128
    bool hidden;                                      // Offset: 140
    bool hasBeenPurchased;                            // Offset: 141
    char padding13[2];                                // Padding
    Vector3 dropVelocity;                             // Offset: 144
    RangeFloat_Value tempItemDropRangeX;              // Offset: 156
    RangeFloat_Value tempItemDropRangeY;              // Offset: 164
    RangeFloat_Value tempItemDropRangeZ;              // Offset: 172
    int32_t dropAmount;                               // Offset: 180
    int32_t debt;                                     // Offset: 184
    bool hintWantsItemFormatting;                     // Offset: 188
    bool disablesInspectionOnPurchase;                // Offset: 189
    bool inspectShop;                                 // Offset: 190
    bool ignorePingFormatting;                        // Offset: 191
    ItemTier_Value itemTier;                          // Offset: 192
    ItemTag_Value bannedItemTag;                      // Offset: 196
    bool selfGeneratePickup;                          // Offset: 200
    bool hasStarted;                                  // Offset: 201
};

// Generated from RoR2.RouletteChestController
struct RouletteChestController {
    char padding0[48];                        // Padding
    AnimationCurve* bonusTimeDecay;           // Offset: 48
    PickupDropTable* dropTable;               // Offset: 56
    Transform* ejectionTransform;             // Offset: 64
    Animator* modelAnimator;                  // Offset: 72
    PickupDisplay* pickupDisplay;             // Offset: 80
    EntityStateMachine* stateMachine;         // Offset: 88
    PurchaseInteraction* purchaseInteraction; // Offset: 96
    NetworkIdentity* networkIdentity;         // Offset: 104
    Entry* entries;                           // Offset: 112
    Xoroshiro128Plus* rng;                    // Offset: 120
    UnityEvent* onCycleBeginServer;           // Offset: 128
    UnityEvent* onCycleCompletedServer;       // Offset: 136
    UnityEvent* onChangedEntryClient;         // Offset: 144
    int32_t maxEntries;                       // Offset: 152
    float bonusTime;                          // Offset: 156
    Vector3 localEjectionVelocity;            // Offset: 160
    FixedTimeStamp_Value activationTime;      // Offset: 172
    int32_t dropCount;                        // Offset: 176
    int32_t previousEntryIndexClient;         // Offset: 180
};

// Generated from RoR2.MultiShopController
struct MultiShopController {
    char padding0[48];                         // Padding
    GameObject* terminalPrefab;                // Offset: 48
    Transform* terminalPositions;              // Offset: 56
    Animator* animator;                        // Offset: 64
    Transform* DroneVendorDeploy;              // Offset: 72
    GameObject* vendorBasePurchaseVFX;         // Offset: 80
    Interactor* _interactor;                   // Offset: 88
    PurchaseInteraction* _purchaseInteraction; // Offset: 96
    SpecialObjectAttributes* multishopSOA;     // Offset: 104
    GameObject* terminalGameObjects;           // Offset: 112
    NetworkInstanceId* terminalNetIds;         // Offset: 120
    Boolean* doCloseOnTerminalPurchase;        // Offset: 128
    Xoroshiro128Plus* rng;                     // Offset: 136
    int32_t revealCount;                       // Offset: 144
    float hiddenChance;                        // Offset: 148
    bool isTripleDroneVendor;                  // Offset: 152
    char padding15[3];                         // Padding
    float PurchaseWaitDuration;                // Offset: 156
    bool _wantsPlayPurchaseAnim;               // Offset: 160
    bool _hasPurchaseAnimPlayed;               // Offset: 161
    char padding18[2];                         // Padding
    float _currentTriggerTime;                 // Offset: 164
    bool hasResolvedTerminals;                 // Offset: 168
    char padding20[3];                         // Padding
    ItemTier_Value itemTier;                   // Offset: 172
    bool doEquipmentInstead;                   // Offset: 176
    bool hideDisplayContent;                   // Offset: 177
    bool available;                            // Offset: 178
    char padding24[1];                         // Padding
    int32_t baseCost;                          // Offset: 180
    CostTypeIndex_Value costType;              // Offset: 184
    int32_t cost;                              // Offset: 188
};

// Generated from RoR2.DelusionChestController
struct DelusionChestController {
    char padding0[24];                                 // Padding
    ChestBehavior* delusionChest;                      // Offset: 24
    void* _delusionPickupIndexesTable;                 // Offset: 32
    NetworkUIPromptController* _netUIPromptController; // Offset: 40
    PickupPickerController* _pickupPickerController;   // Offset: 48
    Interactor* interactor;                            // Offset: 56
    Inventory* interactorInventory;                    // Offset: 64
    Interactor* previousInteractor;                    // Offset: 72
    PickupIndex_Value _delusionPickupIndex;            // Offset: 80
    PickupIndex_Value _selectedPickupIndex;            // Offset: 84
    bool hasBeenReset;                                 // Offset: 88
};

// Generated from RoR2.ScrapperController
struct ScrapperController {
    char padding0[48];                              // Padding
    PickupPickerController* pickupPickerController; // Offset: 48
    EntityStateMachine* esm;                        // Offset: 56
    void* pickupPrintQueue_backing;                 // Offset: 64
    Interactor* interactor;                         // Offset: 72
    int32_t maxItemsToScrapAtATime;                 // Offset: 80
};

// Generated from RoR2.BarrelInteraction
struct BarrelInteraction {
    char padding0[48];             // Padding
    void* displayNameToken;        // Offset: 48
    void* contextToken;            // Offset: 56
    GameObject* salvageOrb;        // Offset: 64
    int32_t goldReward;            // Offset: 72
    uint32_t expReward;            // Offset: 76
    bool shouldProximityHighlight; // Offset: 80
    bool opened;                   // Offset: 81
};

// Generated from RoR2.GenericPickupController
struct GenericPickupController {
    char padding0[48];                              // Padding
    PickupDisplay* pickupDisplay;                   // Offset: 48
    ChestBehavior* chestGeneratedFrom;              // Offset: 56
    SerializablePickupIndex_Value idealPickupIndex; // Offset: 64
    UniquePickup_Value _pickupState;                // Offset: 72
    bool Recycled;                                  // Offset: 84
    bool Duplicated;                                // Offset: 85
    bool selfDestructIfPickupIndexIsNotIdeal;       // Offset: 86
    char padding7[1];                               // Padding
    float waitDuration;                             // Offset: 88
    FixedTimeStamp_Value waitStartTime;             // Offset: 92
    bool consumed;                                  // Offset: 96
};

// Generated from RoR2.TimedChestController
struct TimedChestController {
    char padding0[48];                   // Padding
    TextMeshPro* displayTimer;           // Offset: 48
    ObjectScaleCurve* displayScaleCurve; // Offset: 56
    void* contextString;                 // Offset: 64
    float lockTime;                      // Offset: 72
    Color_Value displayIsAvailableColor; // Offset: 76
    Color_Value displayIsLockedColor;    // Offset: 92
    bool purchased;                      // Offset: 108
    bool shouldProximityHighlight;       // Offset: 109
};

// Generated from RoR2.ShrineCleanseBehavior
struct ShrineCleanseBehavior {
    char padding0[24];                  // Padding
    void* contextToken;                 // Offset: 24
    GameObject* activationEffectPrefab; // Offset: 32
};

// Generated from RoR2.GenericInteraction
struct GenericInteraction {
    char padding0[48];                             // Padding
    void* contextToken;                            // Offset: 48
    InteractorUnityEvent* onActivation;            // Offset: 56
    Interactability_Value interactability;         // Offset: 64
    bool shouldIgnoreSpherecastForInteractibility; // Offset: 68
    bool shouldProximityHighlight;                 // Offset: 69
    bool allowRemoteOpInteraction;                 // Offset: 70
    bool shouldShowOnScanner;                      // Offset: 71
};

// Generated from RoR2.PressurePlateController
struct PressurePlateController {
    char padding0[24];                                // Padding
    void* switchDownSoundString;                      // Offset: 24
    void* switchUpSoundString;                        // Offset: 32
    UnityEvent* OnSwitchDown;                         // Offset: 40
    UnityEvent* OnSwitchUp;                           // Offset: 48
    Collider* pingCollider;                           // Offset: 56
    AnimationCurve* switchVisualPositionFromUpToDown; // Offset: 64
    AnimationCurve* switchVisualPositionFromDownToUp; // Offset: 72
    Transform* switchVisualTransform;                 // Offset: 80
    bool enableOverlapSphere;                         // Offset: 88
    char padding9[3];                                 // Padding
    float overlapSphereRadius;                        // Offset: 92
    float overlapSphereFrequency;                     // Offset: 96
    float overlapSphereStopwatch;                     // Offset: 100
    float animationStopwatch;                         // Offset: 104
    bool switchDown;                                  // Offset: 108
};

// Generated from RoR2.Highlight
struct Highlight {
    char padding0[24];                         // Padding
    IDisplayNameProvider* displayNameProvider; // Offset: 24
    Renderer* targetRenderer;                  // Offset: 32
    void* targetRendererList;                  // Offset: 40
    UniquePickup_Value pickupState;            // Offset: 48
    float strength;                            // Offset: 60
    HighlightColor_Value highlightColor;       // Offset: 64
    Color_Value CustomColor;                   // Offset: 68
    bool isOn;                                 // Offset: 84
};

// Generated from RoR2.PickupDisplay
struct PickupDisplay {
    char padding0[24];                      // Padding
    GameObject* tier1ParticleEffect;        // Offset: 24
    GameObject* tier2ParticleEffect;        // Offset: 32
    GameObject* tier3ParticleEffect;        // Offset: 40
    GameObject* equipmentParticleEffect;    // Offset: 48
    GameObject* lunarParticleEffect;        // Offset: 56
    GameObject* bossParticleEffect;         // Offset: 64
    GameObject* voidParticleEffect;         // Offset: 72
    GameObject* foodParticleEffect;         // Offset: 80
    ParticleSystem* coloredParticleSystems; // Offset: 88
    GameObject* temporaryItemIndicator;     // Offset: 96
    MaterialPropertyBlock* propertyStorage; // Offset: 104
    Highlight* highlight;                   // Offset: 112
    GameObject* modelObject;                // Offset: 120
    Renderer* modelRenderer_backing;        // Offset: 128
    void* modelRenderers_backing;           // Offset: 136
    GameObject* modelPrefab;                // Offset: 144
    Wave_Value verticalWave;                // Offset: 152
    bool dontInstantiatePickupModel;        // Offset: 164
    char padding18[3];                      // Padding
    float spinSpeed;                        // Offset: 168
    UniquePickup_Value pickupState;         // Offset: 172
    bool hidden;                            // Offset: 184
    bool renderersDisabled;                 // Offset: 185
    char padding22[2];                      // Padding
    float modelScale;                       // Offset: 188
    float localTime;                        // Offset: 192
    bool shouldUpdate;                      // Offset: 196
};

// Generated from RoR2.PickupDef
struct PickupDef {
    char padding0[16];                     // Padding
    void* internalName;                    // Offset: 16
    GameObject* displayPrefab;             // Offset: 24
    void* displayPrefabReference;          // Offset: 32
    GameObject* dropletDisplayPrefab;      // Offset: 40
    void* nameToken;                       // Offset: 48
    UnlockableDef* unlockableDef;          // Offset: 56
    void* interactContextToken;            // Offset: 64
    Texture* iconTexture;                  // Offset: 72
    Sprite* iconSprite;                    // Offset: 80
    AttemptGrantDelegate* attemptGrant;    // Offset: 88
    PickupIndex_Value pickupIndex_backing; // Offset: 96
    Color_Value baseColor;                 // Offset: 100
    Color_Value darkColor;                 // Offset: 116
    ItemIndex_Value itemIndex;             // Offset: 132
    EquipmentIndex_Value equipmentIndex;   // Offset: 136
    ArtifactIndex_Value artifactIndex;     // Offset: 140
    MiscPickupIndex_Value miscPickupIndex; // Offset: 144
    DroneIndex_Value droneIndex;           // Offset: 148
    ItemTier_Value itemTier;               // Offset: 152
    uint32_t coinValue;                    // Offset: 156
    bool isLunar;                          // Offset: 160
    bool isBoss;                           // Offset: 161
};

// Generated from RoR2.Run
struct Run {
    char padding0[48];                                    // Padding
    NetworkRuleBook* networkRuleBookComponent;            // Offset: 48
    void* nameToken;                                      // Offset: 56
    PickupDropTable* rebirthDropTable;                    // Offset: 64
    SceneDef* startingScenes;                             // Offset: 72
    SceneCollection* startingSceneGroup;                  // Offset: 80
    String* EventFlagsToResetOnLoop;                      // Offset: 88
    ItemMask* availableItems;                             // Offset: 96
    ItemMask* expansionLockedItems;                       // Offset: 104
    EquipmentMask* availableEquipment;                    // Offset: 112
    EquipmentMask* expansionLockedEquipment;              // Offset: 120
    DroneMask* availableDrones;                           // Offset: 128
    DroneMask* expansionLockedDrones;                     // Offset: 136
    SceneDef* nextStageScene;                             // Offset: 144
    SceneDef* blacklistedScenesForFirstScene;             // Offset: 152
    GameObject* gameOverPrefab;                           // Offset: 160
    GameObject* lobbyBackgroundPrefab;                    // Offset: 168
    GameObject* uiPrefab;                                 // Offset: 176
    void* uiInstances_backing;                            // Offset: 184
    Xoroshiro128Plus* runRNG;                             // Offset: 192
    Xoroshiro128Plus* loopRngGenerator;                   // Offset: 200
    Xoroshiro128Plus* nextStageRng;                       // Offset: 208
    Xoroshiro128Plus* stageRngGenerator;                  // Offset: 216
    Xoroshiro128Plus* stageRng;                           // Offset: 224
    Xoroshiro128Plus* bossRewardRng;                      // Offset: 232
    Xoroshiro128Plus* treasureRng;                        // Offset: 240
    Xoroshiro128Plus* spawnRng;                           // Offset: 248
    Xoroshiro128Plus* randomSurvivorOnRespawnRng;         // Offset: 256
    void* unlockablesUnlockedByAnyUser;                   // Offset: 264
    void* unlockablesUnlockedByAllUsers;                  // Offset: 272
    void* unlockablesAlreadyFullyObtained;                // Offset: 280
    void* userMasters;                                    // Offset: 288
    void* availableTier1DropList;                         // Offset: 296
    void* availableTier2DropList;                         // Offset: 304
    void* availableTier3DropList;                         // Offset: 312
    void* availableEquipmentDropList;                     // Offset: 320
    void* availableLunarEquipmentDropList;                // Offset: 328
    void* availableLunarItemDropList;                     // Offset: 336
    void* availableLunarCombinedDropList;                 // Offset: 344
    void* availableBossDropList;                          // Offset: 352
    void* availableVoidTier1DropList;                     // Offset: 360
    void* availableVoidTier2DropList;                     // Offset: 368
    void* availableVoidTier3DropList;                     // Offset: 376
    void* availableVoidBossDropList;                      // Offset: 384
    void* availableFoodTierDropList;                      // Offset: 392
    void* availablePowerShapeItemsDropList;               // Offset: 400
    void* smallChestDropTierSelector;                     // Offset: 408
    void* mediumChestDropTierSelector;                    // Offset: 416
    void* largeChestDropTierSelector;                     // Offset: 424
    void* availableTier1DroneList;                        // Offset: 432
    void* availableTier2DroneList;                        // Offset: 440
    void* availableTier3DroneList;                        // Offset: 448
    void* availableRemoteOpDroneList;                     // Offset: 456
    void* eventFlags;                                     // Offset: 464
    GameModeIndex_Value gameModeIndex;                    // Offset: 472
    bool userPickable;                                    // Offset: 476
    bool inSceneTransition;                               // Offset: 477
    char padding56[2];                                    // Padding
    NetworkGuid_Value _uniqueId;                          // Offset: 480
    bool isRemoteOperationAllowed;                        // Offset: 496
    char padding58[7];                                    // Padding
    NetworkDateTime_Value startTimeUtc;                   // Offset: 504
    bool isRunWeekly;                                     // Offset: 512
    char padding60[3];                                    // Padding
    float fixedTime;                                      // Offset: 516
    float time;                                           // Offset: 520
    bool isRunning;                                       // Offset: 524
    char padding63[3];                                    // Padding
    RunStopwatch_Value runStopwatch;                      // Offset: 528
    char padding64[3];                                    // Padding
    bool isRunStopwatchForcePaused;                       // Offset: 536
    char padding65[3];                                    // Padding
    int32_t stageClearCount;                              // Offset: 540
    int32_t prestiegeArtifactMountainValue;               // Offset: 544
    int32_t _loopClearCount;                              // Offset: 548
    int32_t stageClearCountAtLoopStart;                   // Offset: 552
    char padding69[4];                                    // Padding
    uint64_t _seed;                                       // Offset: 560
    float difficultyCoefficient;                          // Offset: 568
    float compensatedDifficultyCoefficient;               // Offset: 572
    float oneOverCompensatedDifficultyCoefficientSquared; // Offset: 576
    int32_t selectedDifficultyInternal;                   // Offset: 580
    float ambientLevel_backing;                           // Offset: 584
    int32_t ambientLevelFloor_backing;                    // Offset: 588
    int32_t shopPortalCount;                              // Offset: 592
    bool lowerPricedChestsTimerOn;                        // Offset: 596
    char padding78[3];                                    // Padding
    float lowerPricedChestsTimer;                         // Offset: 600
    float lowerPricedChestsDuration;                      // Offset: 604
    bool shutdown;                                        // Offset: 608
    bool allowNewParticipants;                            // Offset: 609
    bool preventGameOver_backing;                         // Offset: 610
    bool isGameOverServer_backing;                        // Offset: 611
};

// Generated from RoR2.CharacterSpawnCard
struct CharacterSpawnCard {
    char padding0[64];              // Padding
    SerializableLoadout* _loadout;  // Offset: 64
    Inventory* inventoryToCopy;     // Offset: 72
    void* inventoryItemCopyFilter;  // Offset: 80
    EquipmentDef* equipmentToGrant; // Offset: 88
    ItemCountPair* itemsToGrant;    // Offset: 96
    Loadout* runtimeLoadout;        // Offset: 104
    bool noElites;                  // Offset: 112
    bool forbiddenAsBoss;           // Offset: 113
};

// Generated from RoR2.TeamManager
struct TeamManager {
    char padding0[48];                  // Padding
    UInt64* teamExperience;             // Offset: 48
    UInt32* teamLevels;                 // Offset: 56
    UInt64* teamCurrentLevelExperience; // Offset: 64
    UInt64* teamNextLevelExperience;    // Offset: 72
};

#pragma pack(pop)
