#pragma once
#include <cstdint>

struct LocalUser;
struct PlayerStatsComponent;
struct CharacterMaster;
struct CharacterBody;
struct CameraRigController;
struct CameraTargetParams;

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
typedef void* NetworkUser;
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
typedef void* TeamComponent;
typedef void* EquipmentSlot;
typedef void* SkillLocator;
typedef void* SfxLocator;
typedef void* ModelLocator;
typedef void* HurtBoxGroup;
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

template <typename T>
struct MonoArray_ {
    int length;  // The array length stored at the beginning
    T data[1];   // Flexible array member for the elements
    
    // Helper methods
    int Length() const {
        return length;
    }
    
    // Access elements with bounds checking
    T& operator[](int index) {
        if (index < 0 || index >= length) {
            static T defaultValue{};
            return defaultValue;
        }
        return data[index];
    }
    
    const T& operator[](int index) const {
        if (index < 0 || index >= length) {
            static T defaultValue{};
            return defaultValue;
        }
        return data[index];
    }
    
    // Allow iteration
    T* begin() { return data; }
    T* end() { return data + length; }
    const T* begin() const { return data; }
    const T* end() const { return data + length; }
};

#pragma pack(push, 1)

enum class TeamIndex : int8_t {
    None = -1,
    Neutral = 0,
    Player = 1,
    Monster = 2,
    Lunar = 3,
    Void = 4,
    Count = 5
};

enum class CharacterBody_BodyFlags : uint32_t {
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

enum class EquipmentIndex : int32_t {
    None = -1, // 0xFFFFFFFF
};

enum class HullClassification : int32_t {
    Human = 0,
    Golem = 1,
    BeetleQueen = 2,
    Count = 3,
};

/* No Header Structs */

struct Vector2 {
    float x;
    float y;
}; // Size: 8

struct Vector3 {
    float x;
    float y;
    float z;
}; // Size: 12

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

struct CameraState {
    Vector3 position;
    Quaternion rotation;
    float fov;
}; // Size: 32

struct AimAssistInfo
{
    void* aimAssistHurtbox;
    Vector3 localPositionOnHurtbox;
    Vector3 worldPosition;
}; // Size: 32

struct BlendableFloat {
    float value;
    float alpha;
};

struct BlendableVector3 {
    Vector3 value;
    float alpha;
};

struct BlendableBool {
    bool value;
    char padding0[3]; // Padding
    float alpha;
};

struct CameraInfo {
    CameraRigController* cameraRigController;
    Camera* sceneCam;
    ICameraStateProvider* overrideCam;
    CameraState previousCameraState;
    float baseFov;
    char padding4[4];
}; // Size: 64

struct TargetInfo
{
    GameObject* target;
    CharacterBody* body;
    InputBankTest* inputBank;
    CameraTargetParams* targetParams;
    TeamIndex teamIndex;
    bool isSprinting;
    bool skillsAreDisabled;
    bool showSecondaryElectricity;
    bool isViewerControlled;
    char padding8[3];
    CharacterMaster* master;
    NetworkUser* networkUser;
    NetworkedViewAngles* networkedViewAngles;
}; // Size: 64

struct ViewerInfo
{
    LocalUser* localUser;
    UserProfile* userProfile;
    Player* inputPlayer;
    MPEventSystem* eventSystem;
    bool hasCursor;
    bool isUIFocused;
    char padding5[6];
}; // Size: 40

struct CameraModeContext {
    CameraInfo cameraInfo;
    TargetInfo targetInfo;
    ViewerInfo viewerInfo;
}; // Size: 168

struct CharacterCameraParamsData {
    BlendableFloat minPitch;
    BlendableFloat maxPitch;
    BlendableFloat wallCushion;
    BlendableFloat pivotVerticalOffset;
    BlendableVector3 idealLocalCameraPos;
    BlendableFloat fov;
    BlendableBool isFirstPerson;
    float overrideFirstPersonFadeDuration;
};

struct NetworkInstanceId {
    uint32_t m_Value;
}; // Size: 4

struct BodyIndex {
    uint32_t value__;
}; // Size: 4

struct CharacterMaster_SEEKER_REVIVE_STATUS {
    uint32_t value__;
}; // Size: 4

struct ItemIndex {
    uint32_t value__;
}; // Size: 4

struct Type {
    int32_t value__;
}; // Size: 4

struct SerializableEntityStateType {
    Type* _stateType;
    void* _typeName;
}; // Size: 16

struct ItemAvailability {
    int32_t helfireCount;
    bool hasFireTrail;
    bool hasAffixLunar;
    bool hasAffixPoison;
    char padding3[1]; // Padding
}; // Size: 8

struct Color {
    float r;
    float g;
    float b;
    float a;
}; // Size: 8

struct FixedTimeStamp {
    float t;
}; // Size: 4

struct HealthComponent_ItemCounts {
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
    FixedTimeStamp lastHitTime_backing; // Offset: 148
    FixedTimeStamp lastHealTime_backing; // Offset: 152
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
    HealthComponent_ItemCounts itemCounts; // Offset: 200
    EquipmentIndex currentEquipmentIndex; // Offset: 288
};

// Generated from RoR2.EquipmentState
struct EquipmentState {
    char padding0[16]; // Padding
    EquipmentIndex equipmentIndex; // Offset: 16
    FixedTimeStamp chargeFinishTime; // Offset: 20
    uint8_t charges; // Offset: 24
    bool isDisabled; // Offset: 25
    bool dirty; // Offset: 26
    char padding5[5]; // Padding
    EquipmentDef* equipmentDef; // Offset: 32
};

// Generated from RoR2.Inventory
struct Inventory {
    char padding0[48]; // Padding
    int* itemStacks; // Offset: 48
    void* itemAcquisitionOrder; // Offset: 56
    Action* onInventoryChanged; // Offset: 64
    Action* onEquipmentExternalRestockServer; // Offset: 72
    MonoArray_<EquipmentState>* equipmentStateSlots; // Offset: 80
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

// Generated from RoR2.CharacterBody
struct CharacterBody {
    char padding0[48]; // Padding
    void* baseNameToken; // Offset: 48
    void* subtitleNameToken; // Offset: 56
    BuffIndex* activeBuffsList; // Offset: 64
    MonoArray_<int>* buffs; // Offset: 72
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
    AnimationCurve* spreadBloomCurve; // Offset: 656
    GameObject* _defaultCrosshairPrefab; // Offset: 664
    DamageInfo* secondHalfOfDamage; // Offset: 672
    void* incomingDamageList; // Offset: 680
    DamageInfo* runicLensDamageInfo; // Offset: 688
    CharacterBody* meteorVictimBody; // Offset: 696
    OnShardSpawned* onShardSpawnedEvent; // Offset: 704
    Transform* aimOriginTransform; // Offset: 712
    Texture* portraitIcon; // Offset: 720
    VehicleSeat* currentVehicle; // Offset: 728
    GameObject* preferredPodPrefab; // Offset: 736
    SerializableEntityStateType preferredInitialStateType; // Offset: 744
    void* customKillTotalStatName; // Offset: 760
    Transform* overrideCoreTransform; // Offset: 768
    BodyIndex bodyIndex; // Offset: 776
    bool CharacterIsVisible; // Offset: 780
    char padding92[3]; // Padding
    float cost; // Offset: 784
    bool dontCull; // Offset: 788
    char padding94[3]; // Padding
    int32_t BaseImportance; // Offset: 792
    int32_t Importance; // Offset: 796
    bool inLava; // Offset: 800
    char padding97[3]; // Padding
    int32_t activeBuffsListCount; // Offset: 804
    int32_t eliteBuffCount; // Offset: 808
    int32_t pendingTonicAfflictionCount; // Offset: 812
    int32_t previousMultiKillBuffCount; // Offset: 816
    CharacterBody_BodyFlags bodyFlags; // Offset: 820
    NetworkInstanceId masterObjectId; // Offset: 824
    bool linkedToMaster; // Offset: 828
    bool isPlayerControlled_backing; // Offset: 829
    bool disablingHurtBoxes; // Offset: 830
    char padding106[1]; // Padding
    EquipmentIndex previousEquipmentIndex; // Offset: 832
    float executeEliteHealthFraction_backing; // Offset: 836
    float lavaCooldown; // Offset: 840
    float lavaTimer; // Offset: 844
    ItemAvailability itemAvailability; // Offset: 848
    bool hasEffectiveAuthority_backing; // Offset: 856
    bool _isSprinting; // Offset: 857
    char padding113[2]; // Padding
    float outOfCombatStopwatch; // Offset: 860
    float outOfDangerStopwatch; // Offset: 864
    bool outOfCombat_backing; // Offset: 868
    bool _outOfDanger; // Offset: 869
    char padding117[2]; // Padding
    Vector3 previousPosition; // Offset: 872
    float notMovingStopwatch; // Offset: 884
    bool rootMotionInMainState; // Offset: 888
    char padding120[3]; // Padding
    float mainRootSpeed; // Offset: 892
    float baseMaxHealth; // Offset: 896
    float baseRegen; // Offset: 900
    float baseMaxShield; // Offset: 904
    float baseMoveSpeed; // Offset: 908
    float baseAcceleration; // Offset: 912
    float baseJumpPower; // Offset: 916
    float baseDamage; // Offset: 920
    float baseAttackSpeed; // Offset: 924
    float baseCrit; // Offset: 928
    float baseArmor; // Offset: 932
    float baseVisionDistance; // Offset: 936
    int32_t baseJumpCount; // Offset: 940
    float sprintingSpeedMultiplier; // Offset: 944
    bool autoCalculateLevelStats; // Offset: 948
    char padding135[3]; // Padding
    float levelMaxHealth; // Offset: 952
    float levelRegen; // Offset: 956
    float levelMaxShield; // Offset: 960
    float levelMoveSpeed; // Offset: 964
    float levelJumpPower; // Offset: 968
    float levelDamage; // Offset: 972
    float levelAttackSpeed; // Offset: 976
    float levelCrit; // Offset: 980
    float levelArmor; // Offset: 984
    float experience_backing; // Offset: 988
    float level_backing; // Offset: 992
    float maxHealth_backing; // Offset: 996
    float maxBarrier_backing; // Offset: 1000
    float barrierDecayRate_backing; // Offset: 1004
    float regen_backing; // Offset: 1008
    float maxShield_backing; // Offset: 1012
    float moveSpeed_backing; // Offset: 1016
    float acceleration_backing; // Offset: 1020
    float m_surfaceSpeedBoost; // Offset: 1024
    float jumpPower_backing; // Offset: 1028
    int32_t maxJumpCount_backing; // Offset: 1032
    float maxJumpHeight_backing; // Offset: 1036
    float damage_backing; // Offset: 1040
    float attackSpeed_backing; // Offset: 1044
    float crit_backing; // Offset: 1048
    float critMultiplier_backing; // Offset: 1052
    float bleedChance_backing; // Offset: 1056
    float armor_backing; // Offset: 1060
    float visionDistance_backing; // Offset: 1064
    float critHeal_backing; // Offset: 1068
    float cursePenalty_backing; // Offset: 1072
    bool hasOneShotProtection_backing; // Offset: 1076
    bool isGlass_backing; // Offset: 1077
    char padding168[2]; // Padding
    float oneShotProtectionFraction_backing; // Offset: 1080
    bool canPerformBackstab_backing; // Offset: 1084
    bool canReceiveBackstab_backing; // Offset: 1085
    char padding171[2]; // Padding
    float amputatedMaxHealth; // Offset: 1088
    float amputateMaxHealthMinimum; // Offset: 1092
    bool statsDirty; // Offset: 1096
    char padding174[3]; // Padding
    int32_t currentHealthLevel; // Offset: 1100
    int32_t oldHealthLevel; // Offset: 1104
    float damageFromRecalculateStats; // Offset: 1108
    int32_t numberOfKills; // Offset: 1112
    bool canCleanInventory; // Offset: 1116
    char padding179[3]; // Padding
    int32_t extraSecondaryFromSkill; // Offset: 1120
    int32_t extraSpecialFromSkill; // Offset: 1124
    float maxBonusHealth_backing; // Offset: 1128
    int32_t extraStatsOnLevelUpCount_CachedLastApplied; // Offset: 1132
    bool lowerPricedChestsActive; // Offset: 1136
    bool canPurchaseLoweredPricedChests; // Offset: 1137
    bool allSkillsDisabled; // Offset: 1138
    bool isTeleporting; // Offset: 1139
    int32_t currentGrowthNectarBuffCount; // Offset: 1140
    int32_t prevGrowthNectarBuffCount; // Offset: 1144
    int32_t maxGrowthNectarBuffCount; // Offset: 1148
    float growthNectarRemovalTimer; // Offset: 1152
    float aimTimer; // Offset: 1156
    int32_t killCountServer_backing; // Offset: 1160
    float helfireLifetime; // Offset: 1164
    bool wasLucky; // Offset: 1168
    char padding195[3]; // Padding
    float poisonballTimer; // Offset: 1172
    float lunarMissileRechargeTimer; // Offset: 1176
    float lunarMissileTimerBetweenShots; // Offset: 1180
    int32_t remainingMissilesToFire; // Offset: 1184
    float spreadBloomDecayTime; // Offset: 1188
    float spreadBloomInternal; // Offset: 1192
    bool hideCrosshair; // Offset: 1196
    char padding202[3]; // Padding
    float multiKillTimer; // Offset: 1200
    int32_t multiKillCount_backing; // Offset: 1204
    float increasedDamageKillTimer; // Offset: 1208
    bool increasedDamageKillCountDirty; // Offset: 1212
    char padding206[3]; // Padding
    int32_t _increasedDamageKillCount; // Offset: 1216
    int32_t increasedDamageKillCountBuff_backing; // Offset: 1220
    bool canAddIncrasePrimaryDamage; // Offset: 1224
    char padding209[3]; // Padding
    float delayedDamageRefreshTime; // Offset: 1228
    float delayedDamageRefreshTimer; // Offset: 1232
    float secondHalfOfDamageTime; // Offset: 1236
    int32_t oldDelayedDamageCount; // Offset: 1240
    bool halfDamageReady; // Offset: 1244
    char padding214[3]; // Padding
    float halfDamageTimer; // Offset: 1248
    float knockBackFinGroundedTimer; // Offset: 1252
    Vector3 runicLensPosition; // Offset: 1256
    float runicLensStartTime; // Offset: 1268
    float runicLensImpactTime; // Offset: 1272
    bool runicLensMeteorReady; // Offset: 1276
    char padding220[3]; // Padding
    int32_t spawnedPickupCount; // Offset: 1280
    bool tamperedHeartActive; // Offset: 1284
    char padding222[3]; // Padding
    float tamperedHeartRegenBonus; // Offset: 1288
    float tamperedHeartMSpeedBonus; // Offset: 1292
    float tamperedHeartDamageBonus; // Offset: 1296
    float tamperedHeartAttackSpeedBonus; // Offset: 1300
    float tamperedHeartArmorBonus; // Offset: 1304
    float radius_backing; // Offset: 1308
    HullClassification hullClassification; // Offset: 1312
    Color bodyColor; // Offset: 1316
    bool doNotReassignToTeamBasedCollisionLayer; // Offset: 1332
    bool isChampion; // Offset: 1333
    bool isElite_backing; // Offset: 1334
    char padding233[1]; // Padding
    uint32_t skinIndex; // Offset: 1336
    FixedTimeStamp localStartTime_backing; // Offset: 1340
    bool requestEquipmentFire; // Offset: 1344
    char padding236[3]; // Padding
    int32_t increaseDamageOnMultiKillItemCount; // Offset: 1348
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
    NetworkInstanceId networkUserInstanceId; // Offset: 144
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
    TeamIndex _teamIndex; // Offset: 213
    bool hasEffectiveAuthority_backing; // Offset: 214
    bool destroyOnBodyDeath; // Offset: 215
    NetworkInstanceId _bodyInstanceId; // Offset: 216
    bool bodyResolved; // Offset: 220
    char padding27[3]; // Padding
    BodyIndex backupBodyIndex_backing; // Offset: 224
    NetworkInstanceId _combatSquadInstanceId; // Offset: 228
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
    BodyIndex killerBodyIndex; // Offset: 328
    bool preventRespawnUntilNextStageServer; // Offset: 332
    char padding49[3]; // Padding
    CharacterMaster_SEEKER_REVIVE_STATUS seekerUsedRevive; // Offset: 336
    ItemIndex devotionItem; // Offset: 340
    int32_t devotedEvolutionTracker; // Offset: 344
    bool isDevotedMinion; // Offset: 348
    bool godMode; // Offset: 349
    char padding54[2]; // Padding
    uint32_t lostBodyToDeathFlag; // Offset: 352
    uint32_t _miscFlags; // Offset: 356
};

// Generated from RoR2.CameraTargetParams
struct CameraTargetParams {
    char padding0[24]; // Padding
    CharacterCameraParams* cameraParams; // Offset: 24
    Transform* cameraPivotTransform; // Offset: 32
    void* aimRequestStack; // Offset: 40
    void* cameraParamsOverrides; // Offset: 48
    float fovOverride; // Offset: 56
    Vector2 recoil; // Offset: 60
    bool dontRaycastToPivot; // Offset: 68
    char padding7[3]; // Padding
    Vector2 targetRecoil; // Offset: 72
    Vector2 recoilVelocity; // Offset: 80
    Vector2 targetRecoilVelocity; // Offset: 88
    CharacterCameraParamsData _currentCameraParamsData; // Offset: 96
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
    CameraModeContext cameraModeContext; // Offset: 128
    NetworkUser* _viewer; // Offset: 296
    LocalUser* _localUserViewer; // Offset: 304
    CameraTargetParams* targetParams; // Offset: 312
    AimAssistInfo lastAimAssist; // Offset: 320
    AimAssistInfo aimAssist; // Offset: 352
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
    char padding30[1]; // Padding
    uint64_t musicListenerId; // Offset: 432
    bool musicEnabled; // Offset: 440
    bool disableSpectating_backing; // Offset: 441
    TeamIndex targetTeamIndex_backing; // Offset: 442
    char padding34[1]; // Padding
    Vector3 rawScreenShakeDisplacement_backing; // Offset: 444
    Vector3 crosshairWorldPosition_backing; // Offset: 456
    CameraState desiredCameraState; // Offset: 468
    CameraState currentCameraState; // Offset: 500
    bool doNotOverrideCameraState; // Offset: 532
    char padding39[3]; // Padding
    CameraState lerpCameraState; // Offset: 536
    float lerpCameraTime; // Offset: 568
    float lerpCameraTimeScale; // Offset: 572
    float hitmarkerAlpha; // Offset: 576
    float hitmarkerTimer; // Offset: 580
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

#pragma pack(pop)