#pragma once
#ifndef ENEMYTYPE_H
#define ENEMYTYPE_H

enum EnemyType { 
    ORC,
    ORC_ARMORED,
    ORC_ELITE,
    ORC_RIDER,

    SKELETON,
    SKELETON_ARMORED,
    SKELETON_GREAT,

    WEREWOLF,
    WEREBEAR
};

// Type of attack the enemy is performing
enum EnemyAttackType {
    ORC_ATTACK_NONE,
    ORC_ATTACK_1,
    ORC_ATTACK_2,

    SKELETON_ATTACK_NONE,
    SKELETON_ATTACK_1,
    SKELETON_ATTACK_2,

    LYCAN_ATTACK_NONE,
    LYCAN_ATTACK_1,
    LYCAN_ATTACK_2,

};

// Behavior states for AI
enum EnemyBehavior {
    IDLE,
    PATROL,
    AGGRESSIVE,
};

#endif // ENEMYTYPE_H

