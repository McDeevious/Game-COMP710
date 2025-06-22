#pragma once
#ifndef ENEMYTYPE_H
#define ENEMYTYPE_H

enum EnemyType { 
    ENEMY_NONE,

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
    ATTACK_NONE,
    ATTACK_1,
    ATTACK_2,
};

// Behavior states for AI
enum EnemyBehavior {
    IDLE,
    PATROL,
    AGGRESSIVE,
};

#endif // ENEMYTYPE_H

