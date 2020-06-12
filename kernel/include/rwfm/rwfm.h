#include <kernel/thread.h>
#include <rwfm/rwfmmodel.c>

int currsub=0, ns=10;
SUBJECT s[10];
int currobj=0, no=10;
OBJECT o[10];
epMapping epMap[10];
int currep=0;
thrMapping thrMap[10];
int currthr=0;

void handleRWFMSubReg(void);
void handleRWFMIntReg(void);
void handleRWFMEpReg(void);
void handleRWFMThreadReg(void);

void handleRWFMSubReg(void)
{
    tcb_t *thread;

    thread = NODE_STATE(ksCurThread);

    s[currsub].sub_id_index = getRegister(thread, msgRegisters[0]);
    s[currsub].owner        = getRegister(thread, msgRegisters[1]);
    s[currsub].readers	    = getRegister(thread, msgRegisters[2]);
    s[currsub].writers	    = getRegister(thread, msgRegisters[3]);
    ++currsub;
    
    int i = currsub-1;
    kprintf("Thread: %s sub id: %d owner: %d readers: %ld writers: %ld\n", thread->tcbName, s[i].sub_id_index, s[i].owner, s[i].readers, s[i].writers);
}

void handleRWFMIntReg(void)
{
    tcb_t *thread;

    thread = NODE_STATE(ksCurThread);

    o[currobj].obj_id_index = getRegister(thread, msgRegisters[0]);
    o[currobj].owner        = getRegister(thread, msgRegisters[1]);
    o[currobj].readers      = getRegister(thread, msgRegisters[2]);
    o[currobj].writers      = getRegister(thread, msgRegisters[3]);
    ++currobj;

    int i = currobj-1;
    kprintf("Thread: %s, obj id: %d owner: %d readers: %ld writers: %ld\n", thread->tcbName, o[i].obj_id_index, o[i].owner, o[i].readers, o[i].writers);
}

void handleRWFMEpReg(void)
{
    tcb_t *thread;
    
    thread = NODE_STATE(ksCurThread);

    epMap[currep].epNo	  = getRegister(thread, msgRegisters[0]);
    epMap[currep].compNo  = getRegister(thread, msgRegisters[1]);
    epMap[currep].intNo   = getRegister(thread, msgRegisters[2]);
    ++currep;

    int i = currep-1;
    kprintf("Thread: %s, Ep id: %d CompNo: %d IntNo: %d\n", thread->tcbName, epMap[i].epNo, epMap[i].compNo, epMap[i].intNo);
}

void handleRWFMThreadReg(void)
{
    tcb_t *thread;
    
    thread = NODE_STATE(ksCurThread);

    thrMap[currthr].thrNo  = getRegister(thread, msgRegisters[0]);
    thrMap[currthr].compNo = getRegister(thread, msgRegisters[1]);
    ++currthr;

    int i = currthr-1;
    kprintf("Thread: %s, thr id: %d compNo: %d\n", thread->tcbName, thrMap[i].thrNo, thrMap[i].compNo);

}
