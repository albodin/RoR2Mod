#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "utils/Math.h"

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
typedef void* CharacterMotor;
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
typedef void* BullseyeSearch;
typedef void* Variant;
typedef int32_t Int32;
typedef unsigned char byte;


struct MonoObject_Internal {
    void* klass;         // pointer to the array’s element class
    void* gc_header;  // GC‐bookkeeping (locks, color bits, etc)
};

struct MonoArrayBounds {
    uint32_t length;
    int32_t  lower_bound;
};

struct MonoArray_Internal {
    MonoObject_Internal object;
    MonoArrayBounds* bounds;
    uintptr_t max_length;
    uintptr_t vector_arr[0];
};

template<typename T>
static T* mono_array_addr(MonoArray_Internal* arr) {
    return reinterpret_cast<T*>(
        reinterpret_cast<uint8_t*>(arr) +
        offsetof(MonoArray_Internal, vector_arr)
    );
}
/*
    uint32_t len = static_cast<uint32_t>(((MonoArray_Internal*)itemDefsArray)->max_length);
    ItemDef** data = mono_array_addr<ItemDef*>((MonoArray_Internal*)itemDefsArray);
    for (uint32_t i = 0; i < len; ++i) {
        ItemDef* obj = data[i];
        // ... inspect obj, e.g. via mono_object_get_class(obj) etc.
    }
*/

#pragma pack(push, 1)

enum class TeamIndex_Value : int8_t {
    None = -1,
    Neutral = 0,
    Player = 1,
    Monster = 2,
    Lunar = 3,
    Void = 4,
    Count = 5
};

enum class BodyFlags_Value : uint32_t {
    None = 0,
    IgnoreFallDamage = 1,
    Mechanical = 2,
    Masterless = 4,
    ImmuneToGoo = 8,
    ImmuneToExecutes = 16, // 0x00000010
    SprintAnyDirection = 32, // 0x00000020
    ResistantToAOE = 64, // 0x00000040
    HasBackstabPassive = 128, // 0x00000080
    HasBackstabImmunity = 256, // 0x00000100
    OverheatImmune = 512, // 0x00000200
    Void = 1024, // 0x00000400
    ImmuneToVoidDeath = 2048, // 0x00000800
    IgnoreItemUpdates = 4096, // 0x00001000
    Devotion = 8192, // 0x00002000
    IgnoreKnockback = 16384, // 0x00004000
    ImmuneToLava = 32768, // 0x00008000
    UsesAmbientLevel = 65536, // 0x00010000
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

enum class ItemTier_Value
{
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

enum class ItemTag_Value
{
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

enum class QueryTriggerInteraction_Value : int32_t {
    UseGlobal = 0,
    Ignore = 1,
    Collide = 2
};

enum class InterruptPriority_Value : int32_t {
    Any = 0,
    Skill = 1,
    PrioritySkill = 2,
    Pain = 3,
    Stun = 4,
    Frozen = 5,
    Vehicle = 6,
    Death = 7
};

/* No Header Structs */

struct RoR2Item {
    int index;
    std::string displayName;
    std::string name;
    std::string nameToken;
    std::string pickupToken;
    std::string descriptionToken;
    std::string loreToken;
    ItemTier_Value tier;
    std::string tierName;
    bool isDroppable;
    bool canScrap;
    bool canRestack;
    bool canRemove;
    bool isConsumed;
    bool hidden;
    std::vector<int> tags;
};

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

struct CameraState_Value {
    Vector3 position;
    Quaternion rotation;
    float fov;
}; // Size: 32

struct AimAssistInfo_Value
{
    void* aimAssistHurtbox;
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

struct CameraInfo_Value {
    CameraRigController* cameraRigController;
    Camera* sceneCam;
    ICameraStateProvider* overrideCam;
    CameraState_Value previousCameraState;
    float baseFov;
    char padding4[4];
}; // Size: 64

struct TargetInfo_Value
{
    GameObject* target;
    CharacterBody* body;
    InputBankTest* inputBank;
    CameraTargetParams* targetParams;
    TeamIndex_Value teamIndex;
    bool isSprinting;
    bool skillsAreDisabled;
    bool showSecondaryElectricity;
    bool isViewerControlled;
    char padding8[3];
    CharacterMaster* master;
    NetworkUser* networkUser;
    NetworkedViewAngles* networkedViewAngles;
}; // Size: 64

struct ViewerInfo_Value
{
    LocalUser* localUser;
    UserProfile* userProfile;
    Player* inputPlayer;
    MPEventSystem* eventSystem;
    bool hasCursor;
    bool isUIFocused;
}; // Size: 40

struct CameraModeContext_Value {
    CameraInfo_Value cameraInfo;
    TargetInfo_Value targetInfo;
    ViewerInfo_Value viewerInfo;
}; // Size: 168

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
    int32_t rgba;
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct DevotionEvolutionLevel_Value {
    int32_t value__;
};

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

struct SceneType_Value {
    int32_t value__;
};

struct ExitState_Value {
    int32_t value__;
};

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

struct Ray_Value {
    Vector3 m_Origin;
    Vector3 m_Direction;
};

struct RaycastHit_Value {
    Vector3 m_Point;
    Vector3 m_Normal;
    uint32_t m_FaceID;
    float m_Distance;
    Vector2 m_UV;
    int32_t m_Collider;
};

struct NetworkUserId_Value {
    uint64_t value;
    void* strValue;
    uint8_t subId;
};

struct PassiveSkill_Value {
    bool enabled;
    char padding1[7];
    void* skillNameToken;
    void* skillDescriptionToken;
    void* keywordToken;
    Sprite* icon;
};

/* Normal Class Objects with headers */

// Generated from RoR2.HealthComponent
struct HealthComponent {
    char padding0[48]; // Padding
    CharacterBody* body; // Offset: 48
    ModelLocator* modelLocator; // Offset: 56
    IPainAnimationHandler* painAnimationHandler; // Offset: 64
    IOnIncomingDamageServerReceiver* onIncomingDamageReceivers; // Offset: 72
    IOnTakeDamageServerReceiver* onTakeDamageReceivers; // Offset: 80
    ScreenDamageCalculator* screenDamageCalculator; // Offset: 88
    GameObject* lastHitAttacker_backing; // Offset: 96
    RepeatHealComponent* repeatHealComponent; // Offset: 104
    float health; // Offset: 112
    float shield; // Offset: 116
    float echoDamage; // Offset: 120
    float barrier; // Offset: 124
    float magnetiCharge; // Offset: 128
    bool dontShowHealthbar; // Offset: 132
    bool isDefaultGodMode; // Offset: 133
    char padding15[2]; // Padding
    float recentlyTookDamageCoyoteTimer; // Offset: 136
    float globalDeathEventChanceCoefficient; // Offset: 140
    uint32_t _killingDamageType; // Offset: 144
    FixedTimeStamp_Value lastHitTime_backing; // Offset: 148
    FixedTimeStamp_Value lastHealTime_backing; // Offset: 152
    bool godMode_backing; // Offset: 156
    char padding21[3]; // Padding
    float potionReserve_backing; // Offset: 160
    bool isInFrozenState_backing; // Offset: 164
    char padding23[3]; // Padding
    float devilOrbHealPool; // Offset: 168
    float devilOrbTimer; // Offset: 172
    float regenAccumulator; // Offset: 176
    bool wasAlive; // Offset: 180
    char padding27[3]; // Padding
    float adaptiveArmorValue; // Offset: 184
    bool isShieldRegenForced; // Offset: 188
    bool forceCulled; // Offset: 189
    char padding30[2]; // Padding
    float ospTimer; // Offset: 192
    float serverDamageTakenThisUpdate; // Offset: 196
    ItemCounts_Value itemCounts; // Offset: 200
    EquipmentIndex_Value currentEquipmentIndex; // Offset: 288
};

// Generated from RoR2.EquipmentState
struct EquipmentState {
    char padding0[16]; // Padding
    EquipmentIndex_Value equipmentIndex; // Offset: 16
    FixedTimeStamp_Value chargeFinishTime; // Offset: 20
    uint8_t charges; // Offset: 24
    bool isDisabled; // Offset: 25
    bool dirty; // Offset: 26
    char padding5[5]; // Padding
    EquipmentDef* equipmentDef; // Offset: 32
};

// Generated from RoR2.Inventory
struct Inventory {
    char padding0[48]; // Padding
    Int32* itemStacks; // Offset: 48
    void* itemAcquisitionOrder; // Offset: 56
    Action* onInventoryChanged; // Offset: 64
    Action* onEquipmentExternalRestockServer; // Offset: 72
    EquipmentState* equipmentStateSlots; // Offset: 80
    void* onItemAddedClient; // Offset: 88
    void* onEquipmentChangedClient; // Offset: 96
    uint8_t activeEquipmentSlot_backing; // Offset: 104
    bool equipmentDisabled; // Offset: 105
    char padding9[2]; // Padding
    float beadAppliedHealth; // Offset: 108
    float beadAppliedShield; // Offset: 112
    float beadAppliedRegen; // Offset: 116
    float beadAppliedDamage; // Offset: 120
    uint32_t infusionBonus_backing; // Offset: 124
};

// Generated from RoR2.TeamComponent
struct TeamComponent {
    char padding0[48]; // Padding
    CharacterBody* body_backing; // Offset: 48
    GameObject* defaultIndicatorPrefab; // Offset: 56
    GameObject* indicator; // Offset: 64
    bool hideAllyCardDisplay; // Offset: 72
    TeamIndex_Value _teamIndex; // Offset: 73
    TeamIndex_Value oldTeamIndex; // Offset: 74
};

// Generated from .DestructionNotifier
struct DestructionNotifier {
    char padding0[24]; // Padding
    ModelLocator* subscriber_backing; // Offset: 24
};

// Generated from RoR2.ModelLocator
struct ModelLocator {
    char padding0[24]; // Padding
    Transform* _modelTransform; // Offset: 24
    RendererVisiblity* modelVisibility; // Offset: 32
    DestructionNotifier* modelDestructionNotifier; // Offset: 40
    Transform* modelBaseTransform; // Offset: 48
    void* onModelChanged; // Offset: 56
    Transform* modelParentTransform; // Offset: 64
    CharacterMotor* characterMotor; // Offset: 72
    float modelScaleCompensation; // Offset: 80
    bool autoUpdateModelTransform; // Offset: 84
    bool dontDetatchFromParent; // Offset: 85
    bool noCorpse; // Offset: 86
    bool dontReleaseModelOnDeath; // Offset: 87
    bool preserveModel; // Offset: 88
    bool forceCulled; // Offset: 89
    bool normalizeToFloor; // Offset: 90
    char padding15[1]; // Padding
    float normalSmoothdampTime; // Offset: 92
    float normalMaxAngleDelta; // Offset: 96
    Vector3 normalSmoothdampVelocity; // Offset: 100
    Vector3 targetNormal; // Offset: 112
    Vector3 currentNormal; // Offset: 124
};

// Generated from RoR2.CharacterBody
struct CharacterBody {
    char padding0[48]; // Padding
    void* baseNameToken; // Offset: 48
    void* subtitleNameToken; // Offset: 56
    BuffIndex* activeBuffsList; // Offset: 64
    Int32* buffs; // Offset: 72
    void* timedBuffs; // Offset: 80
    JumpDelegate* onJump; // Offset: 88
    GameObject* warCryEffectInstance; // Offset: 96
    GameObject* _masterObject; // Offset: 104
    CharacterMaster* _master; // Offset: 112
    Inventory* inventory_backing; // Offset: 120
    Action* onInventoryChanged; // Offset: 128
    Transform* transform; // Offset: 136
    Rigidbody* rigidbody_backing; // Offset: 144
    NetworkIdentity* networkIdentity_backing; // Offset: 152
    CharacterMotor* characterMotor_backing; // Offset: 160
    CharacterDirection* characterDirection_backing; // Offset: 168
    TeamComponent* teamComponent_backing; // Offset: 176
    HealthComponent* healthComponent_backing; // Offset: 184
    EquipmentSlot* equipmentSlot_backing; // Offset: 192
    InputBankTest* inputBank_backing; // Offset: 200
    SkillLocator* skillLocator_backing; // Offset: 208
    SfxLocator* sfxLocator; // Offset: 216
    ModelLocator* modelLocator_backing; // Offset: 224
    HurtBoxGroup* hurtBoxGroup_backing; // Offset: 232
    HurtBox* mainHurtBox_backing; // Offset: 240
    Transform* coreTransform_backing; // Offset: 248
    void* onSkillActivatedServer; // Offset: 256
    void* onSkillActivatedAuthority; // Offset: 264
    EffectManagerHelper* extraStatsOnLevelUpScrapEffectInstance; // Offset: 272
    IncreaseDamageOnMultiKillItemDisplayUpdater* increaseDamageOnMultiKillItemDisplayUpdater; // Offset: 280
    ScreenDamageCalculatorDisabledSkills* screenDamageSkillsDisabled; // Offset: 288
    BoostAllStatsDisplay* boostAllStatsDisplay; // Offset: 296
    void* OnNetworkItemBehaviorUpdate; // Offset: 304
    HelfireController* helfireController; // Offset: 312
    DamageTrail* fireTrail; // Offset: 320
    GameObject* lunarMissilePrefab; // Offset: 328
    GameObject* timeBubbleWardInstance; // Offset: 336
    TemporaryVisualEffect* engiShieldTempEffectInstance; // Offset: 344
    TemporaryVisualEffect* bucklerShieldTempEffectInstance; // Offset: 352
    TemporaryVisualEffect* slowDownTimeTempEffectInstance; // Offset: 360
    TemporaryVisualEffect* crippleEffectInstance; // Offset: 368
    TemporaryVisualEffect* tonicBuffEffectInstance; // Offset: 376
    TemporaryVisualEffect* weakTempEffectInstance; // Offset: 384
    TemporaryVisualEffect* energizedTempEffectInstance; // Offset: 392
    TemporaryVisualEffect* barrierTempEffectInstance; // Offset: 400
    TemporaryVisualEffect* nullifyStack1EffectInstance; // Offset: 408
    TemporaryVisualEffect* nullifyStack2EffectInstance; // Offset: 416
    TemporaryVisualEffect* nullifyStack3EffectInstance; // Offset: 424
    TemporaryVisualEffect* regenBoostEffectInstance; // Offset: 432
    TemporaryVisualEffect* elephantDefenseEffectInstance; // Offset: 440
    TemporaryVisualEffect* healingDisabledEffectInstance; // Offset: 448
    TemporaryVisualEffect* noCooldownEffectInstance; // Offset: 456
    TemporaryVisualEffect* doppelgangerEffectInstance; // Offset: 464
    TemporaryVisualEffect* deathmarkEffectInstance; // Offset: 472
    TemporaryVisualEffect* crocoRegenEffectInstance; // Offset: 480
    TemporaryVisualEffect* mercExposeEffectInstance; // Offset: 488
    TemporaryVisualEffect* lifestealOnHitEffectInstance; // Offset: 496
    TemporaryVisualEffect* teamWarCryEffectInstance; // Offset: 504
    TemporaryVisualEffect* randomDamageEffectInstance; // Offset: 512
    TemporaryVisualEffect* lunarGolemShieldEffectInstance; // Offset: 520
    TemporaryVisualEffect* warbannerEffectInstance; // Offset: 528
    TemporaryVisualEffect* teslaFieldEffectInstance; // Offset: 536
    TemporaryVisualEffect* lunarSecondaryRootEffectInstance; // Offset: 544
    TemporaryVisualEffect* lunarDetonatorEffectInstance; // Offset: 552
    TemporaryVisualEffect* fruitingEffectInstance; // Offset: 560
    TemporaryVisualEffect* mushroomVoidTempEffectInstance; // Offset: 568
    TemporaryVisualEffect* bearVoidTempEffectInstance; // Offset: 576
    TemporaryVisualEffect* outOfCombatArmorEffectInstance; // Offset: 584
    TemporaryVisualEffect* voidFogMildEffectInstance; // Offset: 592
    TemporaryVisualEffect* voidFogStrongEffectInstance; // Offset: 600
    TemporaryVisualEffect* voidJailerSlowEffectInstance; // Offset: 608
    TemporaryVisualEffect* voidRaidcrabWardWipeFogEffectInstance; // Offset: 616
    TemporaryVisualEffect* aurelioniteBlessingEffectInstance; // Offset: 624
    TemporaryVisualEffect* growthNectarMaxGlowInstance; // Offset: 632
    TemporaryVisualEffect* delayedDamageBuffUpInstance; // Offset: 640
    TemporaryVisualEffect* knockbackHitEnemiesVulnerableInstance; // Offset: 648
    TemporaryVisualEffect* falseSonMeridianWillCoreBuffInstance; // Offset: 656
    AnimationCurve* spreadBloomCurve; // Offset: 664
    GameObject* _defaultCrosshairPrefab; // Offset: 672
    DamageInfo* secondHalfOfDamage; // Offset: 680
    void* incomingDamageList; // Offset: 688
    DamageInfo* runicLensDamageInfo; // Offset: 696
    CharacterBody* meteorVictimBody; // Offset: 704
    OnShardSpawned* onShardSpawnedEvent; // Offset: 712
    Transform* aimOriginTransform; // Offset: 720
    Texture* portraitIcon; // Offset: 728
    VehicleSeat* currentVehicle; // Offset: 736
    GameObject* preferredPodPrefab; // Offset: 744
    SerializableEntityStateType_Value preferredInitialStateType; // Offset: 752
    void* customKillTotalStatName; // Offset: 768
    Transform* overrideCoreTransform; // Offset: 776
    BodyIndex_Value bodyIndex; // Offset: 784
    bool CharacterIsVisible; // Offset: 788
    char padding93[3]; // Padding
    float cost; // Offset: 792
    bool dontCull; // Offset: 796
    char padding95[3]; // Padding
    int32_t BaseImportance; // Offset: 800
    int32_t Importance; // Offset: 804
    bool inLava; // Offset: 808
    char padding98[3]; // Padding
    int32_t activeBuffsListCount; // Offset: 812
    int32_t eliteBuffCount; // Offset: 816
    int32_t pendingTonicAfflictionCount; // Offset: 820
    int32_t previousMultiKillBuffCount; // Offset: 824
    BodyFlags_Value bodyFlags; // Offset: 828
    NetworkInstanceId_Value masterObjectId; // Offset: 832
    bool linkedToMaster; // Offset: 836
    bool isPlayerControlled_backing; // Offset: 837
    bool disablingHurtBoxes; // Offset: 838
    char padding107[1]; // Padding
    EquipmentIndex_Value previousEquipmentIndex; // Offset: 840
    float executeEliteHealthFraction_backing; // Offset: 844
    float lavaCooldown; // Offset: 848
    float lavaTimer; // Offset: 852
    ItemAvailability_Value itemAvailability; // Offset: 856
    char padding112[1]; // Padding
    bool hasEffectiveAuthority_backing; // Offset: 864
    bool _isSprinting; // Offset: 865
    char padding114[2]; // Padding
    float outOfCombatStopwatch; // Offset: 868
    float outOfDangerStopwatch; // Offset: 872
    bool outOfCombat_backing; // Offset: 876
    bool _outOfDanger; // Offset: 877
    char padding118[2]; // Padding
    Vector3 previousPosition; // Offset: 880
    float notMovingStopwatch; // Offset: 892
    bool rootMotionInMainState; // Offset: 896
    char padding121[3]; // Padding
    float mainRootSpeed; // Offset: 900
    float baseMaxHealth; // Offset: 904
    float baseRegen; // Offset: 908
    float baseMaxShield; // Offset: 912
    float baseMoveSpeed; // Offset: 916
    float baseAcceleration; // Offset: 920
    float baseJumpPower; // Offset: 924
    float baseDamage; // Offset: 928
    float baseAttackSpeed; // Offset: 932
    float baseCrit; // Offset: 936
    float baseArmor; // Offset: 940
    float baseVisionDistance; // Offset: 944
    int32_t baseJumpCount; // Offset: 948
    float sprintingSpeedMultiplier; // Offset: 952
    bool autoCalculateLevelStats; // Offset: 956
    char padding136[3]; // Padding
    float levelMaxHealth; // Offset: 960
    float levelRegen; // Offset: 964
    float levelMaxShield; // Offset: 968
    float levelMoveSpeed; // Offset: 972
    float levelJumpPower; // Offset: 976
    float levelDamage; // Offset: 980
    float levelAttackSpeed; // Offset: 984
    float levelCrit; // Offset: 988
    float levelArmor; // Offset: 992
    float experience_backing; // Offset: 996
    float level_backing; // Offset: 1000
    float maxHealth_backing; // Offset: 1004
    float maxBarrier_backing; // Offset: 1008
    float barrierDecayRate_backing; // Offset: 1012
    float regen_backing; // Offset: 1016
    float maxShield_backing; // Offset: 1020
    float moveSpeed_backing; // Offset: 1024
    float acceleration_backing; // Offset: 1028
    float m_surfaceSpeedBoost; // Offset: 1032
    float jumpPower_backing; // Offset: 1036
    int32_t maxJumpCount_backing; // Offset: 1040
    float maxJumpHeight_backing; // Offset: 1044
    float damage_backing; // Offset: 1048
    float attackSpeed_backing; // Offset: 1052
    float crit_backing; // Offset: 1056
    float critMultiplier_backing; // Offset: 1060
    float bleedChance_backing; // Offset: 1064
    float armor_backing; // Offset: 1068
    float visionDistance_backing; // Offset: 1072
    float critHeal_backing; // Offset: 1076
    float cursePenalty_backing; // Offset: 1080
    bool hasOneShotProtection_backing; // Offset: 1084
    bool isGlass_backing; // Offset: 1085
    char padding169[2]; // Padding
    float oneShotProtectionFraction_backing; // Offset: 1088
    bool canPerformBackstab_backing; // Offset: 1092
    bool canReceiveBackstab_backing; // Offset: 1093
    char padding172[2]; // Padding
    float amputatedMaxHealth; // Offset: 1096
    float amputateMaxHealthMinimum; // Offset: 1100
    bool statsDirty; // Offset: 1104
    char padding175[3]; // Padding
    int32_t extraSecondaryFromSkill; // Offset: 1108
    int32_t extraSpecialFromSkill; // Offset: 1112
    float maxBonusHealth_backing; // Offset: 1116
    int32_t extraStatsOnLevelUpCount_CachedLastApplied; // Offset: 1120
    bool lowerPricedChestsActive; // Offset: 1124
    bool canPurchaseLoweredPricedChests; // Offset: 1125
    bool allSkillsDisabled; // Offset: 1126
    bool isTeleporting; // Offset: 1127
    int32_t currentGrowthNectarBuffCount; // Offset: 1128
    int32_t prevGrowthNectarBuffCount; // Offset: 1132
    int32_t maxGrowthNectarBuffCount; // Offset: 1136
    float growthNectarRemovalTimer; // Offset: 1140
    float aimTimer; // Offset: 1144
    int32_t killCountServer_backing; // Offset: 1148
    float helfireLifetime; // Offset: 1152
    bool wasLucky; // Offset: 1156
    char padding191[3]; // Padding
    float poisonballTimer; // Offset: 1160
    float lunarMissileRechargeTimer; // Offset: 1164
    float lunarMissileTimerBetweenShots; // Offset: 1168
    int32_t remainingMissilesToFire; // Offset: 1172
    float spreadBloomDecayTime; // Offset: 1176
    float spreadBloomInternal; // Offset: 1180
    bool hideCrosshair; // Offset: 1184
    char padding198[3]; // Padding
    float multiKillTimer; // Offset: 1188
    int32_t multiKillCount_backing; // Offset: 1192
    float increasedDamageKillTimer; // Offset: 1196
    bool increasedDamageKillCountDirty; // Offset: 1200
    char padding202[3]; // Padding
    int32_t _increasedDamageKillCount; // Offset: 1204
    int32_t increasedDamageKillCountBuff_backing; // Offset: 1208
    bool canAddIncrasePrimaryDamage; // Offset: 1212
    char padding205[3]; // Padding
    float delayedDamageRefreshTime; // Offset: 1216
    float delayedDamageRefreshTimer; // Offset: 1220
    float secondHalfOfDamageTime; // Offset: 1224
    int32_t oldDelayedDamageCount; // Offset: 1228
    bool halfDamageReady; // Offset: 1232
    char padding210[3]; // Padding
    float halfDamageTimer; // Offset: 1236
    float knockBackFinGroundedTimer; // Offset: 1240
    Vector3 runicLensPosition; // Offset: 1244
    float runicLensStartTime; // Offset: 1256
    float runicLensImpactTime; // Offset: 1260
    bool runicLensMeteorReady; // Offset: 1264
    char padding216[3]; // Padding
    int32_t spawnedPickupCount; // Offset: 1268
    float radius_backing; // Offset: 1272
    HullClassification_Value hullClassification; // Offset: 1276
    Color_Value bodyColor; // Offset: 1280
    bool doNotReassignToTeamBasedCollisionLayer; // Offset: 1296
    bool isChampion; // Offset: 1297
    bool isElite_backing; // Offset: 1298
    char padding223[1]; // Padding
    uint32_t skinIndex; // Offset: 1300
    FixedTimeStamp_Value localStartTime_backing; // Offset: 1304
    bool requestEquipmentFire; // Offset: 1308
    char padding226[3]; // Padding
    int32_t increaseDamageOnMultiKillItemCount; // Offset: 1312
};

// Generated from RoR2.NetworkUser
struct NetworkUser {
    char padding0[48]; // Padding
    NetworkLoadout* networkLoadout_backing; // Offset: 48
    NetworkUserId_Value _id; // Offset: 56
    char padding2[7]; // Padding
    LocalUser* localUser; // Offset: 80
    CameraRigController* cameraRigController; // Offset: 88
    void* userName; // Offset: 96
    void* cachedMaster; // Offset: 104
    char padding6[8]; // Padding
    void* cachedPlayerCharacterMasterController; // Offset: 120
    char padding7[8]; // Padding
    void* cachedPlayerStatsComponent; // Offset: 136
    char padding8[8]; // Padding
    GameObject* _masterObject; // Offset: 152
    void* unlockables; // Offset: 160
    void* debugUnlockablesList; // Offset: 168
    uint8_t rewiredPlayerId; // Offset: 176
    char padding12[3]; // Padding
    NetworkInstanceId_Value _masterObjectId; // Offset: 180
    Color32_Value userColor; // Offset: 184
    uint32_t netLunarCoins; // Offset: 188
    ItemIndex_Value rebirthItem; // Offset: 192
    BodyIndex_Value bodyIndexPreference; // Offset: 196
    float secondAccumulator; // Offset: 200
    bool serverIsClientLoaded_backing; // Offset: 204
    char padding19[3]; // Padding
    NetworkInstanceId_Value _serverLastStageAcknowledgedByClient; // Offset: 208
};

// Generated from RoR2.PlayerCharacterMasterController
struct PlayerCharacterMasterController {
    char padding0[48]; // Padding
    CharacterBody* body; // Offset: 48
    InputBankTest* bodyInputs; // Offset: 56
    CharacterMotor* bodyMotor; // Offset: 64
    GameObject* previousBodyObject; // Offset: 72
    CharacterMaster* master_backing; // Offset: 80
    void* finalMessageTokenServer_backing; // Offset: 88
    PingerController* pingerController; // Offset: 96
    GameObject* resolvedNetworkUserGameObjectInstance; // Offset: 104
    NetworkUser* resolvedNetworkUserInstance; // Offset: 112
    GameObject* crosshair; // Offset: 120
    NetworkIdentity* netid; // Offset: 128
    Action* onLinkedToNetworkUserServer; // Offset: 136
    NetworkInstanceId_Value networkUserInstanceId; // Offset: 144
    bool networkUserResolved; // Offset: 148
    bool alreadyLinkedToNetworkUserOnce; // Offset: 149
    char padding15[2]; // Padding
    float cameraMinPitch; // Offset: 152
    float cameraMaxPitch; // Offset: 156
    Vector3 crosshairPosition; // Offset: 160
    bool sprintInputPressReceived; // Offset: 172
    bool jumpWasClaimed; // Offset: 173
    bool claimNextJump; // Offset: 174
    char padding21[1]; // Padding
    float lunarCoinChanceMultiplier; // Offset: 176
};

// Generated from RoR2.CharacterMaster
struct CharacterMaster {
    char padding0[48]; // Padding
    GameObject* bodyPrefab; // Offset: 48
    NetworkIdentity* networkIdentity_backing; // Offset: 56
    void* onBodyStart; // Offset: 64
    void* onBodyDestroyed; // Offset: 72
    void* onPlayerBodyDamaged; // Offset: 80
    UnityEvent* onBodyDeath; // Offset: 88
    Inventory* inventory_backing; // Offset: 96
    BaseAI* aiComponents; // Offset: 104
    PlayerCharacterMasterController* playerCharacterMasterController_backing; // Offset: 112
    PlayerStatsComponent* playerStatsComponent_backing; // Offset: 120
    MinionOwnership* minionOwnership_backing; // Offset: 128
    Loadout* loadout; // Offset: 136
    GameObject* resolvedBodyInstance; // Offset: 144
    GameObject* resolvedCombatSquadInstance; // Offset: 152
    void* OnGoldCollected; // Offset: 160
    Xoroshiro128Plus* cloverVoidRng; // Offset: 168
    void* deployablesList; // Offset: 176
    GameObject* devotionInventoryPrefab; // Offset: 184
    DevotionInventoryController* devotionInventoryUpdaterRef; // Offset: 192
    Interactor* summonerRef; // Offset: 200
    int32_t _masterIndex; // Offset: 208
    bool spawnOnStart; // Offset: 212
    TeamIndex_Value _teamIndex; // Offset: 213
    bool hasEffectiveAuthority_backing; // Offset: 214
    bool destroyOnBodyDeath; // Offset: 215
    NetworkInstanceId_Value _bodyInstanceId; // Offset: 216
    bool bodyResolved; // Offset: 220
    char padding27[3]; // Padding
    BodyIndex_Value backupBodyIndex_backing; // Offset: 224
    NetworkInstanceId_Value _combatSquadInstanceId; // Offset: 228
    int32_t numberOfBeadStatsGained_XPGainNerf; // Offset: 232
    char padding30[4]; // Padding
    uint64_t beadExperience; // Offset: 240
    uint32_t oldBeadLevel; // Offset: 248
    uint32_t newBeadLevel; // Offset: 252
    uint64_t beadXPNeededForCurrentLevel; // Offset: 256
    uint32_t _money; // Offset: 264
    uint32_t ExtraBossMissileMoneyRemainder; // Offset: 268
    uint32_t trackedFreeUnlocks; // Offset: 272
    int32_t trackedMissileCount; // Offset: 276
    float moneyEarnedSoFar; // Offset: 280
    uint32_t _voidCoins; // Offset: 284
    float luck_backing; // Offset: 288
    bool isBoss; // Offset: 292
    bool preventGameOver; // Offset: 293
    char padding43[2]; // Padding
    Vector3 deathFootPosition_backing; // Offset: 296
    Vector3 deathAimVector; // Offset: 308
    bool killedByUnsafeArea; // Offset: 320
    char padding46[3]; // Padding
    float _internalSurvivalTime; // Offset: 324
    BodyIndex_Value killerBodyIndex; // Offset: 328
    bool preventRespawnUntilNextStageServer; // Offset: 332
    uint8_t _seekerChakraGate; // Offset: 333
    bool _seekerRevivedOnce; // Offset: 334
    bool _seekerRevitalize; // Offset: 335
    bool seekerSelfRevive; // Offset: 336
    char padding53[3]; // Padding
    ItemIndex_Value devotionItem; // Offset: 340
    int32_t devotedEvolutionTracker; // Offset: 344
    bool isDevotedMinion; // Offset: 348
    bool godMode; // Offset: 349
    char padding57[2]; // Padding
    uint32_t lostBodyToDeathFlag; // Offset: 352
    uint32_t _miscFlags; // Offset: 356
};

// Generated from RoR2.CameraTargetParams
struct CameraTargetParams {
    char padding0[24]; // Padding
    CharacterCameraParams* cameraParams; // Offset: 24
    Transform* cameraPivotTransform; // Offset: 32
    void* cameraLerpRequests; // Offset: 40
    void* cameraParamsOverrides; // Offset: 48
    float fovOverride; // Offset: 56
    Vector2 recoil; // Offset: 60
    bool dontRaycastToPivot; // Offset: 68
    char padding7[3]; // Padding
    Vector2 targetRecoil; // Offset: 72
    Vector2 recoilVelocity; // Offset: 80
    Vector2 targetRecoilVelocity; // Offset: 88
    CharacterCameraParamsData_Value _currentCameraParamsData; // Offset: 96
};

// Generated from RoR2.CameraRigController
struct CameraRigController {
    char padding0[24]; // Padding
    Camera* sceneCam; // Offset: 24
    Camera* uiCam; // Offset: 32
    Camera* skyboxCam; // Offset: 40
    ParticleSystem* sprintingParticleSystem; // Offset: 48
    ParticleSystem* disabledSkillsParticleSystem; // Offset: 56
    ParticleSystem* secondaryElectrictyParticleSystem; // Offset: 64
    GameObject* nextTarget; // Offset: 72
    CameraModeBase* _cameraMode; // Offset: 80
    HUD* hud_backing; // Offset: 88
    GameObject* firstPersonTarget_backing; // Offset: 96
    CharacterBody* targetBody_backing; // Offset: 104
    GameObject* _target; // Offset: 112
    ICameraStateProvider* overrideCam; // Offset: 120
    CameraModeContext_Value cameraModeContext; // Offset: 128
    char padding14[6]; // Padding
    NetworkUser* _viewer; // Offset: 296
    LocalUser* _localUserViewer; // Offset: 304
    CameraTargetParams* targetParams; // Offset: 312
    AimAssistInfo_Value lastAimAssist; // Offset: 320
    AimAssistInfo_Value aimAssist; // Offset: 352
    HurtBox* lastCrosshairHurtBox_backing; // Offset: 384
    float baseFov; // Offset: 392
    Rect viewport; // Offset: 396
    float maxAimRaycastDistance; // Offset: 412
    bool isCutscene; // Offset: 416
    bool enableFading; // Offset: 417
    char padding25[2]; // Padding
    float fadeStartDistance; // Offset: 420
    float fadeEndDistance; // Offset: 424
    bool createHud; // Offset: 428
    bool suppressPlayerCameras; // Offset: 429
    bool enableMusic; // Offset: 430
    bool disableSpectating_backing; // Offset: 431
    TeamIndex_Value targetTeamIndex_backing; // Offset: 432
    char padding32[3]; // Padding
    Vector3 rawScreenShakeDisplacement_backing; // Offset: 436
    Vector3 crosshairWorldPosition_backing; // Offset: 448
    CameraState_Value desiredCameraState; // Offset: 460
    CameraState_Value currentCameraState; // Offset: 492
    bool doNotOverrideCameraState; // Offset: 524
    char padding37[3]; // Padding
    CameraState_Value lerpCameraState; // Offset: 528
    float lerpCameraTime; // Offset: 560
    float lerpCameraTimeScale; // Offset: 564
    float hitmarkerAlpha; // Offset: 568
    float hitmarkerTimer; // Offset: 572
};

// Generated from RoR2.Stats.PlayerStatsComponent
struct PlayerStatsComponent {
    char padding0[48]; // Padding
    CharacterMaster* characterMaster_backing; // Offset: 48
    PlayerCharacterMasterController* playerCharacterMasterController_backing; // Offset: 56
    GameObject* cachedBodyObject; // Offset: 64
    CharacterBody* cachedCharacterBody; // Offset: 72
    CharacterMotor* cachedBodyCharacterMotor; // Offset: 80
    Transform* cachedBodyTransform; // Offset: 88
    StatSheet* currentStats; // Offset: 96
    StatSheet* clientDeltaStatsBuffer; // Offset: 104
    StatSheet* recordedStats; // Offset: 112
    bool ClientSynced; // Offset: 120
    char padding10[3]; // Padding
    float serverTransmitTimer; // Offset: 124
    float serverTransmitInterval; // Offset: 128
    Vector3 previousBodyPosition; // Offset: 132
};

// Generated from RoR2.LocalUser
struct LocalUser {
    char padding0[16]; // Padding
    Player* _inputPlayer; // Offset: 16
    MPEventSystem* eventSystem_backing; // Offset: 24
    UserProfile* _userProfile; // Offset: 32
    NetworkUser* currentNetworkUser_backing; // Offset: 40
    PlayerCharacterMasterController* cachedMasterController_backing; // Offset: 48
    PauseScreenController* pauseScreenController; // Offset: 56
    CharacterMaster* cachedMaster_backing; // Offset: 64
    GameObject* cachedMasterObject_backing; // Offset: 72
    CharacterBody* cachedBody_backing; // Offset: 80
    GameObject* cachedBodyObject_backing; // Offset: 88
    PlayerStatsComponent* cachedStatsComponent_backing; // Offset: 96
    CameraRigController* _cameraRigController; // Offset: 104
    Action* onBodyChanged; // Offset: 112
    Action* onMasterChanged; // Offset: 120
    void* onCameraDiscovered; // Offset: 128
    void* onCameraLost; // Offset: 136
    void* onNetworkUserFound; // Offset: 144
    void* onNetworkUserLost; // Offset: 152
    int32_t id; // Offset: 160
};

// Generated from RoR2.RuleBook
struct RuleBook {
    char padding0[16]; // Padding
    byte* ruleValues; // Offset: 16
};

// Generated from RoR2.ServerManagerBase`1
struct ServerManagerBase {
    char padding0[32]; // Padding
    void* collectAdditionalTags; // Offset: 32
    void* tags; // Offset: 40
    RuleBook* currentRuleBook; // Offset: 48
    KeyValueSplitter_Value ruleBookKvHelper; // Offset: 56
    KeyValueSplitter_Value modListKvHelper; // Offset: 88
    bool disposed; // Offset: 120
    char padding6[3]; // Padding
    float playerUpdateTimer; // Offset: 124
    float playerUpdateInterval; // Offset: 128
};

// Generated from RoR2.RangeFloat
struct RangeFloat {
    char padding0[16]; // Padding
    float min; // Offset: 16
    float max; // Offset: 20
};

// Generated from RoR2.CombatSquad
struct CombatSquad {
    char padding0[48]; // Padding
    void* membersList; // Offset: 48
    void* memberHistory; // Offset: 56
    void* readOnlyMembersList_backing; // Offset: 64
    void* onDestroyCallbacksServer; // Offset: 72
    Action* onDefeatedServer; // Offset: 80
    UnityEvent* onDefeatedClientLogicEvent; // Offset: 88
    UnityEvent* onDefeatedServerLogicEvent; // Offset: 96
    void* onMemberDeathServer; // Offset: 104
    void* onMemberDefeatedServer; // Offset: 112
    void* onMemberAddedServer; // Offset: 120
    void* onMemberDiscovered; // Offset: 128
    void* onMemberLost; // Offset: 136
    bool propagateMembershipToSummons; // Offset: 144
    bool grantBonusHealthInMultiplayer; // Offset: 145
    bool defeatedServer; // Offset: 146
    char padding15[1]; // Padding
    FixedTimeStamp_Value awakeTime; // Offset: 148
};

// Generated from RoR2.SpawnCard
struct SpawnCard {
    char padding0[24]; // Padding
    GameObject* prefab; // Offset: 24
    bool sendOverNetwork; // Offset: 32
    char padding2[3]; // Padding
    HullClassification_Value hullSize; // Offset: 36
    GraphType_Value nodeGraphType; // Offset: 40
    NodeFlags_Value requiredFlags; // Offset: 44
    NodeFlags_Value forbiddenFlags; // Offset: 45
    char padding6[2]; // Padding
    int32_t directorCreditCost; // Offset: 48
    bool occupyPosition; // Offset: 52
    char padding8[3]; // Padding
    EliteRules_Value eliteRules; // Offset: 56
};

// Generated from RoR2.RuleCategoryDef
struct RuleCategoryDef {
    char padding0[16]; // Padding
    void* displayToken; // Offset: 16
    void* subtitleToken; // Offset: 24
    void* emptyTipToken; // Offset: 32
    void* editToken; // Offset: 40
    void* children; // Offset: 48
    void* hiddenTest; // Offset: 56
    int32_t position; // Offset: 64
    Color_Value color; // Offset: 68
    RuleCategoryType_Value ruleCategoryType; // Offset: 84
};

// Generated from RoR2.RuleDef
struct RuleDef {
    char padding0[16]; // Padding
    void* globalName; // Offset: 16
    void* displayToken; // Offset: 24
    void* choices; // Offset: 32
    RuleCategoryDef* category; // Offset: 40
    int32_t globalIndex; // Offset: 48
    int32_t defaultChoiceIndex; // Offset: 52
    bool forceLobbyDisplay; // Offset: 56
};

// Generated from RoR2.EntitlementManagement.EntitlementDef
struct EntitlementDef {
    char padding0[24]; // Padding
    void* nameToken; // Offset: 24
    void* eosItemId; // Offset: 32
    void* egsItemName; // Offset: 40
    void* GamecoreID; // Offset: 48
    void* SonyID; // Offset: 56
    EntitlementIndex_Value entitlementIndex; // Offset: 64
    uint32_t steamAppId; // Offset: 68
    uint64_t switchNsUid; // Offset: 72
    uint32_t switchDLCIndex; // Offset: 80
};

// Generated from RoR2.RuleChoiceDef
struct RuleChoiceDef {
    char padding0[16]; // Padding
    RuleDef* ruleDef; // Offset: 16
    Sprite* sprite; // Offset: 24
    void* tooltipNameToken; // Offset: 32
    void* getTooltipName; // Offset: 40
    void* tooltipBodyToken; // Offset: 48
    void* localName; // Offset: 56
    void* globalName; // Offset: 64
    UnlockableDef* requiredUnlockable; // Offset: 72
    RuleChoiceDef* requiredChoiceDef; // Offset: 80
    EntitlementDef* requiredEntitlementDef; // Offset: 88
    ExpansionDef* requiredExpansionDef; // Offset: 96
    void* extraData; // Offset: 104
    void* selectionUISound; // Offset: 112
    void* serverTag; // Offset: 120
    Color_Value tooltipNameColor; // Offset: 128
    Color_Value tooltipBodyColor; // Offset: 144
    int32_t localIndex; // Offset: 160
    int32_t globalIndex; // Offset: 164
    bool availableInSinglePlayer; // Offset: 168
    bool availableInMultiPlayer; // Offset: 169
    char padding20[2]; // Padding
    DifficultyIndex_Value difficultyIndex; // Offset: 172
    ArtifactIndex_Value artifactIndex; // Offset: 176
    ItemIndex_Value itemIndex; // Offset: 180
    EquipmentIndex_Value equipmentIndex; // Offset: 184
    bool excludeByDefault; // Offset: 188
    bool onlyShowInGameBrowserIfNonDefault; // Offset: 189
};

// Generated from RoR2.ExpansionManagement.ExpansionDef
struct ExpansionDef {
    char padding0[24]; // Padding
    EntitlementDef* requiredEntitlement; // Offset: 24
    void* nameToken; // Offset: 32
    void* descriptionToken; // Offset: 40
    Sprite* iconSprite; // Offset: 48
    Sprite* disabledIconSprite; // Offset: 56
    RuleChoiceDef* enabledChoice; // Offset: 64
    GameObject* runBehaviorPrefab; // Offset: 72
    ExpansionIndex_Value expansionIndex; // Offset: 80
};

// Generated from RoR2.UnlockableDef
struct UnlockableDef {
    char padding0[24]; // Padding
    void* _cachedName; // Offset: 24
    void* nameToken; // Offset: 32
    void* displayModelPath; // Offset: 40
    GameObject* displayModelPrefab; // Offset: 48
    Sprite* achievementIcon; // Offset: 56
    ExpansionDef* requiredExpansion; // Offset: 64
    void* getHowToUnlockString_backing; // Offset: 72
    void* getUnlockedString_backing; // Offset: 80
    UnlockableIndex_Value index_backing; // Offset: 88
    bool hidden; // Offset: 92
    char padding10[3]; // Padding
    int32_t sortScore_backing; // Offset: 96
};


// Generated from RoR2.DirectorCard
struct DirectorCard {
    char padding0[16]; // Padding
    SpawnCard* spawnCard; // Offset: 16
    void* requiredUnlockable; // Offset: 24
    void* forbiddenUnlockable; // Offset: 32
    UnlockableDef* requiredUnlockableDef; // Offset: 40
    UnlockableDef* forbiddenUnlockableDef; // Offset: 48
    int32_t selectionWeight; // Offset: 56
    MonsterSpawnDistance_Value spawnDistance; // Offset: 60
    bool preventOverhead; // Offset: 64
    char padding8[3]; // Padding
    int32_t minimumStageCompletions; // Offset: 68
};

// Generated from .Category
struct Category {
    char padding0[16]; // Padding
    void* name; // Offset: 16
    DirectorCard* cards; // Offset: 24
    float selectionWeight; // Offset: 32
};

// Generated from RoR2.DirectorCardCategorySelection
struct DirectorCardCategorySelection {
    char padding0[24]; // Padding
    void* expansionsInEffect; // Offset: 24
    Category* categories; // Offset: 32
};

// Generated from RoR2.EliteDef
struct EliteDef {
    char padding0[24]; // Padding
    void* modifierToken; // Offset: 24
    EquipmentDef* eliteEquipmentDef; // Offset: 32
    EliteIndex_Value eliteIndex_backing; // Offset: 40
    Color32_Value color; // Offset: 44
    int32_t shaderEliteRampIndex; // Offset: 48
    float healthBoostCoefficient; // Offset: 52
    float damageBoostCoefficient; // Offset: 56
    DevotionEvolutionLevel_Value devotionLevel; // Offset: 60
};

// Generated from .EliteTierDef
struct EliteTierDef {
    char padding0[16]; // Padding
    EliteDef* eliteTypes; // Offset: 16
    void* isAvailable; // Offset: 24
    void* availableDefs; // Offset: 32
    float costMultiplier; // Offset: 40
    bool canSelectWithoutAvailableEliteDef; // Offset: 44
};

// Generated from .DirectorMoneyWave
struct DirectorMoneyWave {
    char padding0[16]; // Padding
    float interval; // Offset: 16
    float timer; // Offset: 20
    float multiplier; // Offset: 24
    float accumulatedAward; // Offset: 28
};

// Generated from RoR2.CombatDirector
struct CombatDirector {
    char padding0[24]; // Padding
    void* customName; // Offset: 24
    RangeFloat* moneyWaveIntervals; // Offset: 32
    OnSpawnedServer* onSpawnedServer; // Offset: 40
    CombatSquad* combatSquad; // Offset: 48
    GameObject* spawnEffectPrefab; // Offset: 56
    DirectorCardCategorySelection* _monsterCards; // Offset: 64
    DirectorCard* lastAttemptedMonsterCard_backing; // Offset: 72
    Xoroshiro128Plus* rng; // Offset: 80
    DirectorCard* currentMonsterCard; // Offset: 88
    EliteTierDef* currentActiveEliteTier; // Offset: 96
    EliteDef* currentActiveEliteDef; // Offset: 104
    void* monsterCardsSelection; // Offset: 112
    GameObject* currentSpawnTarget; // Offset: 120
    DirectorMoneyWave* moneyWaves; // Offset: 128
    float monsterCredit; // Offset: 136
    float refundedMonsterCredit; // Offset: 140
    float expRewardCoefficient; // Offset: 144
    float goldRewardCoefficient; // Offset: 148
    float minSeriesSpawnInterval; // Offset: 152
    float maxSeriesSpawnInterval; // Offset: 156
    float minRerollSpawnInterval; // Offset: 160
    float maxRerollSpawnInterval; // Offset: 164
    TeamIndex_Value teamIndex; // Offset: 168
    char padding23[3]; // Padding
    float creditMultiplier; // Offset: 172
    float spawnDistanceMultiplier; // Offset: 176
    float maxSpawnDistance; // Offset: 180
    float minSpawnRange; // Offset: 184
    bool shouldSpawnOneWave; // Offset: 188
    bool targetPlayers; // Offset: 189
    bool skipSpawnIfTooCheap; // Offset: 190
    char padding30[1]; // Padding
    int32_t maxConsecutiveCheapSkips; // Offset: 192
    bool resetMonsterCardIfFailed; // Offset: 196
    char padding32[3]; // Padding
    int32_t maximumNumberToSpawnBeforeSkipping; // Offset: 200
    float eliteBias; // Offset: 204
    uint32_t maxSquadCount; // Offset: 208
    bool ignoreTeamSizeLimit; // Offset: 212
    bool fallBackToStageMonsterCards; // Offset: 213
    char padding37[2]; // Padding
    float monsterSpawnTimer_backing; // Offset: 216
    float totalCreditsSpent_backing; // Offset: 220
    bool hasStartedWave; // Offset: 224
    char padding40[3]; // Padding
    int32_t currentMonsterCardCost; // Offset: 228
    int32_t consecutiveCheapSkips; // Offset: 232
    float playerRetargetTimer; // Offset: 236
    int32_t spawnCountInCurrentWave; // Offset: 240
    bool isHalcyonShrineSpawn; // Offset: 244
    char padding45[3]; // Padding
    int32_t shrineHalcyoniteDifficultyLevel; // Offset: 248
};

// Generated from EntityStates.EntityState
struct EntityState {
    char padding0[16]; // Padding
    EntityStateMachine* outer; // Offset: 16
    float age_backing; // Offset: 24
    float fixedAge_backing; // Offset: 28
};

// Generated from RoR2.NetworkStateMachine
struct NetworkStateMachine {
    char padding0[48]; // Padding
    EntityStateMachine* stateMachines; // Offset: 48
    NetworkIdentity* networkIdentity; // Offset: 56
};

// Generated from RoR2.EntityStateMachine
struct EntityStateMachine {
    char padding0[24]; // Padding
    EntityState* state_backing; // Offset: 24
    EntityState* nextState; // Offset: 32
    void* customName; // Offset: 40
    SerializableEntityStateType_Value initialStateType; // Offset: 48
    SerializableEntityStateType_Value mainStateType; // Offset: 64
    NetworkStateMachine* networker_backing; // Offset: 80
    NetworkIdentity* networkIdentity_backing; // Offset: 88
    CommonComponentCache_Value commonComponents; // Offset: 96
    ModifyNextStateDelegate* nextStateModifier; // Offset: 240
    int32_t networkIndex; // Offset: 248
    bool AllowStartWithoutNetworker; // Offset: 252
    bool ShouldStateTransitionOnUpdate; // Offset: 253
    bool debug; // Offset: 254
    bool destroying_backing; // Offset: 255
};

// Generated from RoR2.OutsideInteractableLocker
struct OutsideInteractableLocker {
    char padding0[24]; // Padding
    GameObject* lockPrefab; // Offset: 24
    void* lockObjectMap; // Offset: 32
    void* eggLockInfoMap; // Offset: 40
    IEnumerator* currentCoroutine; // Offset: 48
    float updateInterval; // Offset: 56
    bool lockInside; // Offset: 60
    char padding6[3]; // Padding
    float radius_backing; // Offset: 64
    float updateTimer; // Offset: 68
};

// Generated from RoR2.PickupDropTable
struct PickupDropTable {
    char padding0[24]; // Padding
    bool canDropBeReplaced; // Offset: 24
};

// Generated from .BossMemory
struct BossMemory {
    char padding0[16]; // Padding
    NetworkInstanceId_Value masterInstanceId; // Offset: 16
    float maxObservedMaxHealth; // Offset: 20
    float lastObservedHealth; // Offset: 24
    char padding3[4]; // Padding
    CharacterMaster* cachedMaster; // Offset: 32
    CharacterBody* cachedBody; // Offset: 40
};

// Generated from RoR2.BossGroup
struct BossGroup {
    char padding0[24]; // Padding
    Transform* dropPosition; // Offset: 24
    PickupDropTable* dropTable; // Offset: 32
    CombatSquad* combatSquad_backing; // Offset: 40
    Xoroshiro128Plus* rng; // Offset: 48
    void* bossDropTables; // Offset: 56
    void* bossDrops; // Offset: 64
    BossMemory* bossMemories; // Offset: 72
    void* bestObservedName_backing; // Offset: 80
    void* bestObservedSubtitle_backing; // Offset: 88
    float bossDropChance; // Offset: 96
    bool scaleRewardsByPlayerCount; // Offset: 100
    bool shouldDisplayHealthBarOnHud; // Offset: 101
    char padding12[2]; // Padding
    int32_t bonusRewardCount_backing; // Offset: 104
    FixedTimeStamp_Value enabledTime; // Offset: 108
    bool forceTier3Reward; // Offset: 112
    char padding15[3]; // Padding
    int32_t bossMemoryCount; // Offset: 116
    float totalMaxObservedMaxHealth_backing; // Offset: 120
    float totalObservedHealth_backing; // Offset: 124
};

// Generated from .NameTransformPair
struct NameTransformPair {
    char padding0[16]; // Padding
    void* name; // Offset: 16
    Transform* transform; // Offset: 24
};

// Generated from .ChildLocator
struct ChildLocator {
    char padding0[24]; // Padding
    NameTransformPair* transformPairs; // Offset: 24
};

// Generated from RoR2.NetworkSoundEventDef
struct NetworkSoundEventDef {
    char padding0[24]; // Padding
    void* eventName; // Offset: 24
    NetworkSoundEventIndex_Value index_backing; // Offset: 32
    uint32_t akId_backing; // Offset: 36
};

// Generated from RoR2.BuffDef
struct BuffDef {
    char padding0[24]; // Padding
    void* iconPath; // Offset: 24
    Sprite* iconSprite; // Offset: 32
    EliteDef* eliteDef; // Offset: 40
    NetworkSoundEventDef* startSfx; // Offset: 48
    BuffIndex_Value buffIndex_backing; // Offset: 56
    Color_Value buffColor; // Offset: 60
    bool canStack; // Offset: 76
    bool isDebuff; // Offset: 77
    bool isDOT; // Offset: 78
    bool ignoreGrowthNectar; // Offset: 79
    bool isCooldown; // Offset: 80
    bool isHidden; // Offset: 81
    char padding12[2]; // Padding
    Flags_Value flags; // Offset: 84
};

// Generated from RoR2.TeamFilter
struct TeamFilter {
    char padding0[48]; // Padding
    int32_t teamIndexInternal; // Offset: 48
    TeamIndex_Value defaultTeam; // Offset: 52
};

// Generated from RoR2.BuffWard
struct BuffWard {
    char padding0[48]; // Padding
    Transform* rangeIndicator; // Offset: 48
    BuffDef* buffDef; // Offset: 56
    AnimationCurve* radiusCoefficientCurve; // Offset: 64
    void* removalSoundString; // Offset: 72
    UnityEvent* onRemoval; // Offset: 80
    TeamFilter* teamFilter; // Offset: 88
    BuffWardShape_Value shape; // Offset: 96
    float radius; // Offset: 100
    float interval; // Offset: 104
    float buffDuration; // Offset: 108
    bool floorWard; // Offset: 112
    bool expires; // Offset: 113
    bool invertTeamFilter; // Offset: 114
    char padding13[1]; // Padding
    float expireDuration; // Offset: 116
    bool animateRadius; // Offset: 120
    char padding15[3]; // Padding
    float removalTime; // Offset: 124
    bool needsRemovalTime; // Offset: 128
    bool requireGrounded; // Offset: 129
    char padding18[2]; // Padding
    float buffTimer; // Offset: 132
    float rangeIndicatorScaleVelocity; // Offset: 136
    float stopwatch; // Offset: 140
    float calculatedRadius; // Offset: 144
};

// Generated from RoR2.HoldoutZoneController
struct HoldoutZoneController {
    char padding0[48]; // Padding
    Renderer* radiusIndicator; // Offset: 48
    GameObject* healingNovaItemEffect; // Offset: 56
    Transform* healingNovaRoot; // Offset: 64
    void* inBoundsObjectiveToken; // Offset: 72
    void* outOfBoundsObjectiveToken; // Offset: 80
    HoldoutZoneControllerChargedUnityEvent* onCharged; // Offset: 88
    CalcRadiusDelegate* calcRadius; // Offset: 96
    CalcChargeRateDelegate* calcChargeRate; // Offset: 104
    CalcPercentageOfPlayersInRadiusDelegate* calcPercentageOfPlayersInRadius; // Offset: 112
    CalcAccumulatedChargeDelegate* calcAccumulatedCharge; // Offset: 120
    CalcColorDelegate* calcColor; // Offset: 128
    BuffWard* buffWard; // Offset: 136
    GameObject* healingNovaGeneratorsByTeam; // Offset: 144
    HoldoutZoneShape_Value holdoutZoneShape; // Offset: 152
    float baseRadius; // Offset: 156
    float minimumRadius; // Offset: 160
    float chargeRadiusDelta; // Offset: 164
    float baseChargeDuration; // Offset: 168
    float radiusSmoothTime; // Offset: 172
    bool showObjective; // Offset: 176
    bool applyFocusConvergence; // Offset: 177
    bool applyHealingNova; // Offset: 178
    bool applyDevotedEvolution; // Offset: 179
    bool applyDelusionResetChests; // Offset: 180
    char padding24[3]; // Padding
    float playerCountScaling; // Offset: 184
    float dischargeRate; // Offset: 188
    float currentRadius_backing; // Offset: 192
    bool isAnyoneCharging_backing; // Offset: 196
    TeamIndex_Value chargingTeam_backing; // Offset: 197
    char padding29[2]; // Padding
    Color_Value baseIndicatorColor; // Offset: 200
    float radiusVelocity; // Offset: 216
    bool wasCharged; // Offset: 220
    char padding32[3]; // Padding
    float _charge; // Offset: 224
};

// Generated from RoR2.SceneDef
struct SceneDef {
    char padding0[24]; // Padding
    AssetReferenceScene* sceneAddress; // Offset: 24
    void* baseSceneNameOverride; // Offset: 32
    ExpansionDef* requiredExpansion; // Offset: 40
    void* nameToken; // Offset: 48
    void* subtitleToken; // Offset: 56
    Texture* previewTexture; // Offset: 64
    Material* portalMaterial; // Offset: 72
    void* portalSelectionMessageString; // Offset: 80
    void* loreToken; // Offset: 88
    GameObject* dioramaPrefab; // Offset: 96
    MusicTrackDef* mainTrack; // Offset: 104
    MusicTrackDef* bossTrack; // Offset: 112
    SceneCollection* destinationsGroup; // Offset: 120
    SceneCollection* loopedDestinationsGroup; // Offset: 128
    SceneDef* loopedSceneDef; // Offset: 136
    SceneDef* destinations; // Offset: 144
    GameObject* preferredPortalPrefab; // Offset: 152
    void* _cachedName; // Offset: 160
    void* sceneNameOverrides; // Offset: 168
    SceneIndex_Value sceneDefIndex_backing; // Offset: 176
    SceneType_Value sceneType; // Offset: 180
    bool isOfflineScene; // Offset: 184
    char padding22[3]; // Padding
    int32_t stageOrder; // Offset: 188
    bool shouldIncludeInLogbook; // Offset: 192
    char padding24[3]; // Padding
    Color_Value environmentColor; // Offset: 196
    bool hasSafeStart; // Offset: 212
    bool suppressPlayerEntry; // Offset: 213
    bool suppressNpcEntry; // Offset: 214
    bool blockOrbitalSkills; // Offset: 215
    bool validForRandomSelection; // Offset: 216
    bool allowItemsToSpawnObjects; // Offset: 217
    bool preventStageAdvanceCounter; // Offset: 218
    bool needSkipDevotionRespawn; // Offset: 219
    bool shouldUpdateSceneCollectionAfterLooping; // Offset: 220
    bool isLockedBeforeLooping; // Offset: 221
};

// Generated from RoR2.ConvertPlayerMoneyToExperience
struct ConvertPlayerMoneyToExperience {
    char padding0[24]; // Padding
    void* burstSizes; // Offset: 24
    float burstTimer; // Offset: 32
    float burstInterval; // Offset: 36
    int32_t burstCount; // Offset: 40
};

// Generated from RoR2.SceneExitController
struct SceneExitController {
    char padding0[24]; // Padding
    SceneDef* destinationScene; // Offset: 24
    SceneDef* tier1AlternateDestinationScene; // Offset: 32
    SceneDef* tier2AlternateDestinationScene; // Offset: 40
    SceneDef* tier3AlternateDestinationScene; // Offset: 48
    SceneDef* tier4AlternateDestinationScene; // Offset: 56
    SceneDef* tier5AlternateDestinationScene; // Offset: 64
    ConvertPlayerMoneyToExperience* experienceCollector; // Offset: 72
    bool useRunNextStageScene; // Offset: 80
    char padding8[3]; // Padding
    float teleportOutTimer; // Offset: 84
    bool isAlternatePath; // Offset: 88
    bool isColossusPortal; // Offset: 89
    bool portalUsedAfterTeleporterEventFinish; // Offset: 90
    char padding12[1]; // Padding
    ExitState_Value exitState; // Offset: 92
};

// Generated from RoR2.PositionIndicator
struct PositionIndicator {
    char padding0[24]; // Padding
    Transform* targetTransform; // Offset: 24
    Transform* transform; // Offset: 32
    GameObject* insideViewObject; // Offset: 40
    GameObject* outsideViewObject; // Offset: 48
    GameObject* alwaysVisibleObject; // Offset: 56
    bool shouldRotateOutsideViewObject; // Offset: 64
    char padding6[3]; // Padding
    float outsideViewRotationOffset; // Offset: 68
    float yOffset; // Offset: 72
    bool generateDefaultPosition; // Offset: 76
    char padding9[3]; // Padding
    Vector3 defaultPosition_backing; // Offset: 80
};

// Generated from RoR2.UI.ChargeIndicatorController
struct ChargeIndicatorController {
    char padding0[24]; // Padding
    SpriteRenderer* iconSprites; // Offset: 24
    TextMeshPro* chargingText; // Offset: 32
    TextMeshPro* pingText; // Offset: 40
    HoldoutZoneController* holdoutZoneController; // Offset: 48
    ObjectScaleCurve* pingBounceAnimation; // Offset: 56
    UserProfile* cachedUserProfile; // Offset: 64
    Color_Value spriteBaseColor; // Offset: 72
    Color_Value spriteFlashColor; // Offset: 88
    Color_Value spriteChargingColor; // Offset: 104
    Color_Value spriteChargedColor; // Offset: 120
    Color_Value textBaseColor; // Offset: 136
    Color_Value textChargingColor; // Offset: 152
    Color_Value playerPingColor; // Offset: 168
    bool disableTextWhenNotCharging; // Offset: 184
    bool disableTextWhenCharged; // Offset: 185
    bool flashWhenNotCharging; // Offset: 186
    char padding16[1]; // Padding
    float flashFrequency; // Offset: 188
    bool isCharging; // Offset: 192
    bool isCharged; // Offset: 193
    bool isDiscovered; // Offset: 194
    bool isActivated; // Offset: 195
    uint32_t chargeValue_backing; // Offset: 196
    float flashStopwatch; // Offset: 200
    bool isPinged; // Offset: 204
    bool showIndicatorWhenDiscovered; // Offset: 205
};

// Generated from RoR2.InteractableSpawnCard
struct InteractableSpawnCard {
    char padding0[64]; // Padding
    bool orientToFloor; // Offset: 64
    bool slightlyRandomizeOrientation; // Offset: 65
    bool skipSpawnWhenSacrificeArtifactEnabled; // Offset: 66
    char padding3[1]; // Padding
    float weightScalarWhenSacrificeArtifactEnabled; // Offset: 68
    bool skipSpawnWhenDevotionArtifactEnabled; // Offset: 72
    char padding5[3]; // Padding
    int32_t maxSpawnsPerStage; // Offset: 76
    float prismaticTrialSpawnChance; // Offset: 80
};

// Generated from RoR2.PortalSpawner
struct PortalSpawner {
    char padding0[48]; // Padding
    InteractableSpawnCard* portalSpawnCard; // Offset: 48
    Transform* spawnReferenceLocation; // Offset: 56
    void* spawnPreviewMessageToken; // Offset: 64
    void* spawnMessageToken; // Offset: 72
    ChildLocator* modelChildLocator; // Offset: 80
    void* previewChildName; // Offset: 88
    ExpansionDef* requiredExpansion; // Offset: 96
    void* bannedEventFlag; // Offset: 104
    void* requiredEventFlag; // Offset: 112
    String* validStages; // Offset: 120
    String* invalidStages; // Offset: 128
    Int32* validStageTiers; // Offset: 136
    Xoroshiro128Plus* rng; // Offset: 144
    GameObject* previewChild; // Offset: 152
    float spawnChance; // Offset: 160
    float minSpawnDistance; // Offset: 164
    float maxSpawnDistance; // Offset: 168
    int32_t minStagesCleared; // Offset: 172
    bool willSpawn; // Offset: 176
};

// Generated from .Particle_SetMinSize
struct Particle_SetMinSize {
    char padding0[24]; // Padding
    float minimumPixelCoverage; // Offset: 24
    float defaultMinimumParticleSize; // Offset: 28
    bool _isEnabled; // Offset: 32
};

// Generated from RoR2.TeleporterInteraction
struct TeleporterInteraction {
    char padding0[48]; // Padding
    CombatDirector* bonusDirector; // Offset: 48
    CombatDirector* bossDirector; // Offset: 56
    EntityStateMachine* mainStateMachine; // Offset: 64
    OutsideInteractableLocker* outsideInteractableLocker; // Offset: 72
    void* beginContextString; // Offset: 80
    void* exitContextString; // Offset: 88
    void* interactableObjectMap; // Offset: 96
    BossGroup* bossGroup; // Offset: 104
    ChildLocator* modelChildLocator; // Offset: 112
    HoldoutZoneController* holdoutZoneController_backing; // Offset: 120
    SceneExitController* sceneExitController_backing; // Offset: 128
    PositionIndicator* teleporterPositionIndicator; // Offset: 136
    ChargeIndicatorController* teleporterChargeIndicatorController; // Offset: 144
    GameObject* bossShrineIndicator; // Offset: 152
    Xoroshiro128Plus* rng; // Offset: 160
    GameObject* chargeActivatorServer; // Offset: 168
    SpawnCard* shopPortalSpawnCard; // Offset: 176
    SpawnCard* goldshoresPortalSpawnCard; // Offset: 184
    SpawnCard* msPortalSpawnCard; // Offset: 192
    PortalSpawner* portalSpawners; // Offset: 200
    void* currentPings; // Offset: 208
    Particle_SetMinSize* cachedParticleMinSizeScript; // Offset: 216
    LocalUser* cachedLocalUser; // Offset: 224
    float triggerWavePillarInterval; // Offset: 232
    float sceneStartAmbientLevel_backing; // Offset: 236
    bool _locked; // Offset: 240
    char padding26[3]; // Padding
    int32_t shrineBonusStacks_backing; // Offset: 244
    bool isDiscovered; // Offset: 248
    bool isActivated; // Offset: 249
    char padding29[2]; // Padding
    float discoveryRadius; // Offset: 252
    Color_Value originalTeleporterColor; // Offset: 256
    bool _shouldAttemptToSpawnShopPortal; // Offset: 272
    bool _shouldAttemptToSpawnGoldshoresPortal; // Offset: 273
    bool _shouldAttemptToSpawnMSPortal; // Offset: 274
    bool monstersCleared; // Offset: 275
    bool showExtraBossesIndicator; // Offset: 276
    char padding36[3]; // Padding
    float baseShopSpawnChance; // Offset: 280
    float particleScaleUpdateTimer; // Offset: 284
    bool shouldScaleParticles; // Offset: 288
};

// Generated from .VectorPID
struct VectorPID {
    char padding0[24]; // Padding
    void* customName; // Offset: 24
    Vector3 PID; // Offset: 32
    Vector3 inputVector; // Offset: 44
    Vector3 targetVector; // Offset: 56
    Vector3 outputVector; // Offset: 68
    bool isAngle; // Offset: 80
    char padding6[3]; // Padding
    float gain; // Offset: 84
    Vector3 errorSum; // Offset: 88
    Vector3 deltaError; // Offset: 100
    Vector3 lastError; // Offset: 112
    float lastTimer; // Offset: 124
    float timer; // Offset: 128
};

// Generated from RoR2.BodyAnimatorSmoothingParameters
struct BodyAnimatorSmoothingParameters {
    char padding0[24]; // Padding
    SmoothingParameters_Value smoothingParameters; // Offset: 24
};

// Generated from RoR2.RigidbodyMotor
struct RigidbodyMotor {
    char padding0[48]; // Padding
    Rigidbody* rigid; // Offset: 48
    VectorPID* forcePID; // Offset: 56
    void* animatorForward; // Offset: 64
    void* animatorRight; // Offset: 72
    void* animatorUp; // Offset: 80
    NetworkIdentity* networkIdentity; // Offset: 88
    CharacterBody* characterBody; // Offset: 96
    InputBankTest* inputBank; // Offset: 104
    ModelLocator* modelLocator; // Offset: 112
    Animator* animator; // Offset: 120
    BodyAnimatorSmoothingParameters* bodyAnimatorSmoothingParameters; // Offset: 128
    HealthComponent* healthComponent; // Offset: 136
    Vector3 moveVector; // Offset: 144
    Vector3 centerOfMassOffset; // Offset: 156
    bool enableOverrideMoveVectorInLocalSpace; // Offset: 168
    bool canTakeImpactDamage; // Offset: 169
    char padding16[2]; // Padding
    Vector3 overrideMoveVectorInLocalSpace; // Offset: 172
    Vector3 rootMotion; // Offset: 184
};

// Generated from .QuaternionPID
struct QuaternionPID {
    char padding0[24]; // Padding
    void* customName; // Offset: 24
    Vector3 PID; // Offset: 32
    Quaternion inputQuat; // Offset: 44
    Quaternion targetQuat; // Offset: 60
    Vector3 outputVector; // Offset: 76
    float gain; // Offset: 88
    Vector3 errorSum; // Offset: 92
    Vector3 deltaError; // Offset: 104
    Vector3 lastError; // Offset: 116
    float lastUpdateTime; // Offset: 128
};

// Generated from .HurtBoxInfo
struct HurtBoxInfo {
    char padding0[16]; // Padding
    Transform* transform; // Offset: 16
    float estimatedRadius; // Offset: 24
};

// Generated from RoR2.HurtBoxGroup
struct HurtBoxGroup {
    char padding0[24]; // Padding
    HurtBox* hurtBoxes; // Offset: 24 - MonoArray of HurtBox pointers
    HurtBox* mainHurtBox; // Offset: 32
    int32_t bullseyeCount; // Offset: 40
    int32_t _hurtBoxesDeactivatorCounter; // Offset: 44
};

// Generated from RoR2.CharacterModel
struct CharacterModel {
    char padding0[24]; // Padding
    CharacterBody* body; // Offset: 24
    ItemDisplayRuleSet* itemDisplayRuleSet; // Offset: 32
    void* gameObjectActivationTransforms; // Offset: 40
    RendererInfo* baseRendererInfos; // Offset: 48
    LightInfo* baseLightInfos; // Offset: 56
    ChildLocator* childLocator; // Offset: 64
    GameObject* goldAffixEffect; // Offset: 72
    HurtBoxInfo* hurtBoxInfos; // Offset: 80
    Transform* coreTransform; // Offset: 88
    void* temporaryOverlays; // Offset: 96
    MaterialPropertyBlock* propertyStorage; // Offset: 104
    SkinnedMeshRenderer* mainSkinnedMeshRenderer; // Offset: 112
    RendererVisiblity* visibilityChecker; // Offset: 120
    Material* particleMaterialOverride; // Offset: 128
    GameObject* poisonAffixEffect; // Offset: 136
    GameObject* hauntedAffixEffect; // Offset: 144
    GameObject* voidAffixEffect; // Offset: 152
    ItemMask* enabledItemDisplays; // Offset: 160
    void* parentedPrefabDisplays; // Offset: 168
    void* limbMaskDisplays; // Offset: 176
    LimbFlagSet* limbFlagSet; // Offset: 184
    Material* currentOverlays; // Offset: 192
    RtpcSetter_Value rtpcEliteEnemy; // Offset: 200
    char padding23[7]; // Padding
    bool autoPopulateLightInfos; // Offset: 240
    char padding24[3]; // Padding
    VisibilityLevel_Value _visibility; // Offset: 244
    bool _isGhost; // Offset: 248
    bool _isDoppelganger; // Offset: 249
    bool _isEcho; // Offset: 250
    char padding28[1]; // Padding
    int32_t _invisibilityCount; // Offset: 252
    bool materialsDirty; // Offset: 256
    char padding30[3]; // Padding
    EquipmentIndex_Value inventoryEquipmentIndex; // Offset: 260
    EliteIndex_Value myEliteIndex; // Offset: 264
    float fade; // Offset: 268
    float firstPersonFade; // Offset: 272
    float corpseFade; // Offset: 276
    bool CharacterOnScreen; // Offset: 280
    char padding36[3]; // Padding
    void* lightColorOverride; // Offset: 284
    char padding37[12]; // Padding
    float affixHauntedCloakLockoutDuration; // Offset: 304
    EquipmentIndex_Value currentEquipmentDisplayIndex; // Offset: 308
    int32_t activeOverlayCount; // Offset: 312
    bool wasPreviouslyClayGooed; // Offset: 316
    bool wasPreviouslyHaunted; // Offset: 317
    bool wasPreviouslyOiled; // Offset: 318
    char padding43[1]; // Padding
    int32_t shaderEliteRampIndex; // Offset: 320
    bool eliteChanged; // Offset: 324
    char padding45[3]; // Padding
    int32_t activeOverlays; // Offset: 328
    int32_t oldOverlays; // Offset: 332
    float hitFlashValue; // Offset: 336
    float healFlashValue; // Offset: 340
    float oldHit; // Offset: 344
    float oldHeal; // Offset: 348
    float oldFade; // Offset: 352
    EliteIndex_Value oldEliteIndex; // Offset: 356
    bool forceUpdate; // Offset: 360
};

// Generated from .RendererVisiblity
struct RendererVisiblity {
    char padding0[24]; // Padding
    CharacterModel* parentModel; // Offset: 24
    Animator* animatorToUpdateOnceWhenVisible; // Offset: 32
    DistanceClass_Value approxDistance; // Offset: 40
    bool isVisible; // Offset: 44
    char padding4[3]; // Padding
    float visualUpdateTimer; // Offset: 48
    bool shouldUpdateVisuals; // Offset: 52
};

// Generated from RoR2.RigidbodyDirection
struct RigidbodyDirection {
    char padding0[24]; // Padding
    Rigidbody* rigid; // Offset: 24
    QuaternionPID* angularVelocityPID; // Offset: 32
    VectorPID* torquePID; // Offset: 40
    ModelLocator* modelLocator; // Offset: 48
    Animator* animator; // Offset: 56
    void* animatorXCycle; // Offset: 64
    void* animatorYCycle; // Offset: 72
    void* animatorZCycle; // Offset: 80
    InputBankTest* inputBank; // Offset: 88
    RendererVisiblity* visibility; // Offset: 96
    Vector3 aimDirection; // Offset: 104
    bool freezeXRotation; // Offset: 116
    bool freezeYRotation; // Offset: 117
    bool freezeZRotation; // Offset: 118
    char padding14[1]; // Padding
    int32_t animatorXCycleIndex; // Offset: 120
    int32_t animatorYCycleIndex; // Offset: 124
    int32_t animatorZCycleIndex; // Offset: 128
    float animatorTorqueScale; // Offset: 132
    Vector3 targetTorque; // Offset: 136
};

// Generated from RoR2.RailMotor
struct RailMotor {
    char padding0[24]; // Padding
    Animator* modelAnimator; // Offset: 24
    InputBankTest* inputBank; // Offset: 32
    NodeGraph* railGraph; // Offset: 40
    CharacterBody* characterBody; // Offset: 48
    CharacterDirection* characterDirection; // Offset: 56
    Vector3 inputMoveVector; // Offset: 64
    Vector3 rootMotion; // Offset: 76
    NodeIndex_Value nodeA; // Offset: 88
    NodeIndex_Value nodeB; // Offset: 92
    LinkIndex_Value currentLink; // Offset: 96
    float linkLerp; // Offset: 100
    Vector3 projectedMoveVector; // Offset: 104
    Vector3 nodeAPosition; // Offset: 116
    Vector3 nodeBPosition; // Offset: 128
    Vector3 linkVector; // Offset: 140
    float linkLength; // Offset: 152
    float currentMoveSpeed; // Offset: 156
    bool useRootMotion; // Offset: 160
};

// Generated from .EmoteDef
struct EmoteDef {
    char padding0[16]; // Padding
    void* name; // Offset: 16
    void* displayName; // Offset: 24
    EntityStateMachine* targetStateMachine; // Offset: 32
    SerializableEntityStateType_Value state; // Offset: 40
};

// Generated from RoR2.CharacterEmoteDefinitions
struct CharacterEmoteDefinitions {
    char padding0[24]; // Padding
    EmoteDef* emoteDefinitions; // Offset: 24
};

// Generated from RoR2.Audio.LoopSoundDef
struct LoopSoundDef {
    char padding0[24]; // Padding
    void* startSoundName; // Offset: 24
    void* stopSoundName; // Offset: 32
};

// Generated from RoR2.Projectile.ProjectileGhostController
struct ProjectileGhostController {
    char padding0[24]; // Padding
    Transform* transform; // Offset: 24
    EffectManagerHelper* emh; // Offset: 32
    Transform* authorityTransform_backing; // Offset: 40
    Transform* predictionTransform_backing; // Offset: 48
    UnityEvent* ParentProjectileDestroyedEvent; // Offset: 56
    float migration; // Offset: 64
    bool inheritScaleFromProjectile; // Offset: 68
    bool alreadyRunParentProjectileDestroyedEvent; // Offset: 69
};

// Generated from RoR2.Projectile.ProjectileController
struct ProjectileController {
    char padding0[48]; // Padding
    GameObject* ghostPrefab; // Offset: 48
    Transform* ghostTransformAnchor; // Offset: 56
    void* startSound; // Offset: 64
    LoopSoundDef* flightSoundLoop; // Offset: 72
    Rigidbody* rigidbody; // Offset: 80
    TeamFilter* teamFilter_backing; // Offset: 88
    ProjectileGhostController* ghost_backing; // Offset: 96
    GameObject* owner; // Offset: 104
    void* onInitialized; // Offset: 112
    NetworkConnection* clientAuthorityOwner_backing; // Offset: 120
    Collider* myColliders; // Offset: 128
    EffectManagerHelper* _efhGhost; // Offset: 136
    int32_t catalogIndex; // Offset: 144
    bool cannotBeDeleted; // Offset: 148
    bool authorityHandlesCollisionEvents; // Offset: 149
    bool isPrediction_backing; // Offset: 150
    bool canImpactOnTrigger; // Offset: 151
    bool shouldPlaySounds_backing; // Offset: 152
    bool allowPrediction; // Offset: 153
    uint16_t predictionId; // Offset: 154
    bool localCollisionHappened; // Offset: 156
    char padding21[3]; // Padding
    ProcChainMask_Value procChainMask_backing; // Offset: 160
    float procCoefficient; // Offset: 164
    uint8_t combo; // Offset: 168
    bool CheckChildrenForCollidersAndIncludeDisabled; // Offset: 169
    char padding25[2]; // Padding
    NetworkInstanceId_Value ___ownerNetId; // Offset: 172
};

// Generated from RoR2.ItemTierDef
struct ItemTierDef {
    char padding0[24]; // Padding
    Texture* bgIconTexture; // Offset: 24
    GameObject* highlightPrefab; // Offset: 32
    GameObject* dropletDisplayPrefab; // Offset: 40
    ItemTier_Value _tier; // Offset: 48
    int colorIndex; // Offset: 52
    int darkColorIndex; // Offset: 56
    bool isDroppable; // Offset: 60
    bool canScrap; // Offset: 61
    bool canRestack; // Offset: 62
    char padding9[1]; // Padding
    int pickupRules; // Offset: 64
};

// Generated from RoR2.ItemTag
struct ItemTag {
    char padding0[16]; // Padding
    int32_t value__; // Offset: 16
};

// Generated from RoR2.ItemDef
struct ItemDef {
    char padding0[24]; // Padding
    ItemTierDef* _itemTierDef; // Offset: 24
    void* nameToken; // Offset: 32
    void* pickupToken; // Offset: 40
    void* descriptionToken; // Offset: 48
    void* loreToken; // Offset: 56
    UnlockableDef* unlockableDef; // Offset: 64
    GameObject* pickupModelPrefab; // Offset: 72
    Sprite* pickupIconSprite; // Offset: 80
    ItemTag* tags; // Offset: 88
    ExpansionDef* requiredExpansion; // Offset: 96
    ItemIndex_Value _itemIndex; // Offset: 104
    ItemTier_Value deprecatedTier; // Offset: 108
    bool isConsumed; // Offset: 112
    bool hidden; // Offset: 113
    bool canRemove; // Offset: 114
};

// Generated from RoR2.SkillLocator
struct SkillLocator {
    char padding0[48]; // Padding
    GenericSkill* primary; // Offset: 48
    GenericSkill* secondary; // Offset: 56
    GenericSkill* utility; // Offset: 64
    GenericSkill* special; // Offset: 72
    PassiveSkill_Value passiveSkill; // Offset: 80
    GenericSkill* primaryBonusStockOverrideSkill; // Offset: 120
    GenericSkill* secondaryBonusStockOverrideSkill; // Offset: 128
    GenericSkill* utilityBonusStockOverrideSkill; // Offset: 136
    GenericSkill* specialBonusStockOverrideSkill; // Offset: 144
    NetworkIdentity* networkIdentity; // Offset: 152
    GenericSkill* allSkills; // Offset: 160
    bool hasEffectiveAuthority; // Offset: 168
    char padding12[3]; // Padding
    uint32_t skillDefDirtyFlags; // Offset: 172
    bool inDeserialize; // Offset: 176
};

// Generated from RoR2.GenericSkill
struct GenericSkill {
    char padding0[24]; // Padding
    SkillDef* skillDef_backing; // Offset: 24
    SkillFamily* _skillFamily; // Offset: 32
    SkillDef* baseSkill_backing; // Offset: 40
    void* skillName; // Offset: 48
    EntityStateMachine* stateMachine_backing; // Offset: 56
    BaseSkillInstanceData* skillInstanceData_backing; // Offset: 64
    CharacterBody* characterBody_backing; // Offset: 72
    SkillDef* defaultSkillDef_backing; // Offset: 80
    void* onSkillChanged; // Offset: 88
    SkillOverride* skillOverrides; // Offset: 96
    StateMachineResolver* _customStateMachineResolver; // Offset: 104
    bool hideInCharacterSelect; // Offset: 112
    bool isCooldownBlocked_backing; // Offset: 113
    char padding13[2]; // Padding
    int32_t currentSkillOverride; // Offset: 116
    int32_t bonusStockFromBody; // Offset: 120
    int32_t maxStock_backing; // Offset: 124
    int32_t baseStock; // Offset: 128
    float finalRechargeInterval; // Offset: 132
    float _cooldownScale; // Offset: 136
    float _flatCooldownReduction; // Offset: 140
    float _temporaryCooldownPenalty; // Offset: 144
    float _cooldownOverride; // Offset: 148
    float baseRechargeStopwatch; // Offset: 152
    bool hasExecutedSuccessfully; // Offset: 156
};

// Generated from RoR2.Skills.SkillDef
struct SkillDef {
    char padding0[24]; // Padding
    void* skillName; // Offset: 24
    void* skillNameToken; // Offset: 32
    void* skillDescriptionToken; // Offset: 40
    String* keywordTokens; // Offset: 48
    Sprite* icon; // Offset: 56
    void* activationStateMachineName; // Offset: 64
    SerializableEntityStateType_Value activationState; // Offset: 72
    int32_t skillIndex_backing; // Offset: 88
    InterruptPriority_Value interruptPriority; // Offset: 92
    float baseRechargeInterval; // Offset: 96
    int32_t baseMaxStock; // Offset: 100
    int32_t rechargeStock; // Offset: 104
    int32_t requiredStock; // Offset: 108
    int32_t stockToConsume; // Offset: 112
    bool attackSpeedBuffsRestockSpeed; // Offset: 116
    char padding15[3]; // Padding
    float attackSpeedBuffsRestockSpeed_Multiplier; // Offset: 120
    bool resetCooldownTimerOnUse; // Offset: 124
    bool fullRestockOnAssign; // Offset: 125
    bool dontAllowPastMaxStocks; // Offset: 126
    bool beginSkillCooldownOnSkillEnd; // Offset: 127
    bool isCooldownBlockedUntilManuallyReset; // Offset: 128
    bool cancelSprintingOnActivation; // Offset: 129
    bool forceSprintDuringState; // Offset: 130
    bool canceledFromSprinting; // Offset: 131
    bool isCombatSkill; // Offset: 132
    bool mustKeyPress; // Offset: 133
    bool triggeredByPressRelease; // Offset: 134
    bool autoHandleLuminousShot; // Offset: 135
    bool hideStockCount; // Offset: 136
};

// Generated from RoR2.HuntressTracker
struct HuntressTracker {
    char padding0[24]; // Padding
    GameObject* trackingPrefab; // Offset: 24
    HurtBox* trackingTarget; // Offset: 32
    CharacterBody* characterBody; // Offset: 40
    TeamComponent* teamComponent; // Offset: 48
    InputBankTest* inputBank; // Offset: 56
    Indicator* indicator; // Offset: 64
    BullseyeSearch* search; // Offset: 72
    float maxTrackingDistance; // Offset: 80
    float maxTrackingAngle; // Offset: 84
    float trackerUpdateFrequency; // Offset: 88
    float trackerUpdateStopwatch; // Offset: 92
};

// Generated from RoR2.Skills.SkillFamily
struct SkillFamily {
    char padding0[24]; // Padding
    Variant* variants; // Offset: 24
    int32_t catalogIndex_backing; // Offset: 32
    uint32_t defaultVariantIndex; // Offset: 36
};


#pragma pack(pop)