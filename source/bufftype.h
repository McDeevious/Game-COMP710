#pragma once

#ifndef BUFFTYPE_H
#define BUFFTYPE_H

enum BuffType {
    BUFF_DEFUP,
    BUFF_DMGUP,
    BUFF_SPEED,
    BUFF_JUMP,
    BUFF_HEALTH,
    BUFF_REGEN,
    BUFF_COUNT // helper
};

enum BuffChoiceState {
    BUFF_WAITING,
    BUFF_SELECTED_1,
    BUFF_SELECTED_2,
    BUFF_DONE
};

enum BuffFocus {
    BUFF_FOCUS_NONE,
    BUFF_FOCUS_1,
    BUFF_FOCUS_2
};

#endif // BUFFTYPE_H

