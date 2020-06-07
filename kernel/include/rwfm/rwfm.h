#include <kernel/thread.h>
#include <rwfm/rwfmmodel.c>

int currsub=0, ns=5;
SUBJECT s[5];
int currobj=0, no=10;
OBJECT o[10];
epMapping epMap[10];
int currep=0;
thrMapping thrMap[5];
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
    
    if (currsub == 2){
      for(int i=0;i<currsub;i++) 
	kprintf("sub id: %d owner: %d readers: %ld writers: %ld\n", s[i].sub_id_index, s[i].owner, s[i].readers, s[i].writers);
    }
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

    if (currobj == 2){
      for(int i=0;i<currobj;i++)
        kprintf("obj id: %d owner: %d readers: %ld writers: %ld\n", o[i].obj_id_index, o[i].owner, o[i].readers, o[i].writers);
    }
}

void handleRWFMEpReg(void)
{
    tcb_t *thread;
    
    thread = NODE_STATE(ksCurThread);

    epMap[currep].epNo	  = getRegister(thread, msgRegisters[0]);
    epMap[currep].compNo  = getRegister(thread, msgRegisters[1]);
    epMap[currep].intNo   = getRegister(thread, msgRegisters[2]);
    ++currep;
}

void handleRWFMThreadReg(void)
{
    tcb_t *thread;
    
    thread = NODE_STATE(ksCurThread);

    thrMap[currthr].thrNo  = getRegister(thread, msgRegisters[0]);
    thrMap[currthr].compNo = getRegister(thread, msgRegisters[1]);
    ++currthr;
}
