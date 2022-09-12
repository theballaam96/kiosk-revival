#include "../../include/common.h"

typedef struct army_paad {
    /* 0x000 */ char unk_00[0xE];
    /* 0x00E */ short unk_0E;
    /* 0x010 */ short unk_10;
    /* 0x012 */ char unk_12[0x2C-0x12];
    /* 0x02C */ short unk_2C;
} army_paad;

void ArmyAnimCode(army_paad* paad) {
    int anim_start = *(int*)(0x807FF700);
    if (paad->unk_2C) {
        paad->unk_2C -= 1;
        if (paad->unk_2C == 0) {
            if (CurrentActorPointer_0->render->animation == anim_start) {
                playActorAnimation(CurrentActorPointer_0,anim_start+1);
            } else {
                playActorAnimation(CurrentActorPointer_0,anim_start+2);
            }
            paad->unk_2C = 0x78;
        }
    }
}

void ArmyCode(void) {
    /*
        Animations:
            0x24F (0x2B1) = Walking
            0x250 (0x2B2) = Jumping into ball
            0x251 (0x2B3) = Land?
            0x252 (0x2B4) = Death?
    */
    int anim_start = *(int*)(0x807FF700);
    initCharSpawnerActor();
    army_paad* paad = CurrentActorPointer_0->paad;
    int ignore = 0;
    if ((CurrentActorPointer_0->obj_props_bitfield & 0x10) == 0) {
        paad->unk_2C = 0x78;
        playActorAnimation(CurrentActorPointer_0,anim_start);
    }
    if (CurrentActorPointer_0->render) {
        if (CurrentActorPointer_0->render->animation == anim_start) {
            CurrentActorPointer_0->subdata = 1;
        } else {
            CurrentActorPointer_0->subdata = 2;
        }
    } else {
        CurrentActorPointer_0->subdata = 2;
    }
    if (((collisionType == 9) || (collisionActive)) && (CurrentActorPointer_0->control_state != 0x37)) {
        CurrentActorPointer_0->unk_C8 = 300.0f;
        animationSomething((void*)*(int*)(0x807FBD6C), anim_start+3); 
        int rng = getRNGLower31();
        paad->unk_2C = ((rng >> 0xF) % 200) + 200;
    }
    int control_state = CurrentActorPointer_0->control_state;
    if (control_state == 1) {
        ArmyAnimCode(paad);
        generalActorHandle(control_state, PlayerPointer_0->xPos, PlayerPointer_0->zPos, 0, 0);
    } else if ((control_state == 2) || (control_state == 3)) {
        ArmyAnimCode(paad);
    } else if (control_state == 0x1F) {
        generalActorHandle(control_state, PlayerPointer_0->xPos, PlayerPointer_0->zPos, 0, 0);
    } else if (control_state != 0x35) {
        if (control_state == 0x37) {
            CurrentActorPointer_0->unk_EA += paad->unk_2C;
            if (paad->unk_2C > 0) {
                paad->unk_2C -= 10;
            }
            int csp = CurrentActorPointer_0->control_state_progress;
            if (csp == 0) {
                generalActorHandle(0x37,0,0,0x22,0);
            } else {
                if (csp == 1) {
                    if ((currentCharSpawner_0->unk_46 & 1) == 0) {
                        spawnEnemyDrops(CurrentActorPointer_0); // TODO: Make army drop something
                    }
                   CurrentActorPointer_0->control_state_progress += 1;
                } else if (csp != 2) {
                    if (csp == 3) {
                        // DAT_807B73E0 -= 1; // Don't think this is needed. Only used in Army/Jack in box code
                        CurrentActorPointer_0->control_state = 0x40;
                    }
                    ignore = 1;
                }
                if (!ignore) {
                    reduceShadowIntensity(5);
                }
            }
        } else {
            unkArmyFunction(anim_start);
        }
        ignore = 1;
    }
    if ((control_state != 0x1F) && (!ignore) && ((control_state == 0) || (control_state > 3))) {
        generalActorHandle(control_state, paad->unk_10, paad->unk_0E, 0, 0);
    }
    control_state = CurrentActorPointer_0->control_state;
    if ((control_state == 2) || (control_state == 3)) {
        unkArmyFunction_0(control_state, 1);
    } else {
        unkArmyFunction_1(10000,anim_start+2,anim_start+1);
    }
    renderActor(CurrentActorPointer_0,0);
}


void initKioskEnemies(void) {
    // Army (Actor 151)
    *(int*)(0x8074C2FC) = (int)&ArmyCode;
    *(short*)(0x8074DDD0) = 20; // Change paad size to 0x30
    *(short*)(0x8075EB80) = 151; // Replace beaver actor in Char Spawner mapping
    *(short*)(0x8075EB82) = 0xED; // Replace beaver model in Char Spawner mapping
}