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
void uGetString(char [], char *, int *);
int uGetNumber(char *, int *);

void uGetString(char dest[], char *src, int *left)
{
    //Parse the next string before the delimiter.
    int right = *left;
    int i = *left;
    while(src[right]!='\0' && src[right]!='~') right++;
    for(; i<right; i++) dest[i-(*left)] = src[i];
    dest[i-(*left)] = '\0';
    *left = right+1;
}

int uGetNumber(char *src, int *left)
{
    //Parse the next number before the delimiter.
    int right = *left;
    int sum = 0;
    while(src[right]!='\0' && src[right]!='~') right++;
    while((*left)<right)
    {
      sum = sum*10 + src[(*left)]-'0';
      (*left)++;
    }
    (*left)++;
    return sum;
}

void handleRWFMSubReg(void)
{
    //Register a camkes component as subject for RWFM.
    char *mssg = (char*)(lookupIPCBuffer(true, NODE_STATE(ksCurThread)) + 1);
    int left = 0;

    uGetString(s[currsub].compName, mssg, &left);
    s[currsub].sub_id_index = uGetNumber(mssg, &left);
    s[currsub].owner        = uGetNumber(mssg, &left);
    s[currsub].readers	    = uGetNumber(mssg, &left);
    s[currsub].writers	    = uGetNumber(mssg, &left);
    ++currsub;
    
    int i = currsub-1;
    kprintf("Thread: %s sub id: %d owner: %d readers: %ld writers: %ld\n",
		   s[i].compName, s[i].sub_id_index, s[i].owner, s[i].readers, s[i].writers);
    mssg = '\0';
}

void handleRWFMIntReg(void)
{
    //Register the camkes interfaces as RWFM objects.
    char *mssg = (char*)(lookupIPCBuffer(true, NODE_STATE(ksCurThread)) + 1);
    int left = 0;

    uGetString(o[currobj].objName, mssg, &left);
    o[currobj].obj_id_index = uGetNumber(mssg, &left);
    o[currobj].owner        = uGetNumber(mssg, &left);
    o[currobj].readers      = uGetNumber(mssg, &left);
    o[currobj].writers      = uGetNumber(mssg, &left);
    ++currobj;

    int i = currobj-1;
    kprintf("Interface: %s, obj id: %d owner: %d readers: %ld writers: %ld\n",
		    o[i].objName, o[i].obj_id_index, o[i].owner, o[i].readers, o[i].writers);
    mssg = '\0';
}

void handleRWFMEpReg(void)
{
    //Register the endpoints created corresponding to each interface for a camkes component.
    char *mssg = (char*)(lookupIPCBuffer(true, NODE_STATE(ksCurThread)) + 1);
    int left = 0;

    epMap[currep].epNo    = uGetNumber(mssg, &left);
    epMap[currep].compNo  = uGetNumber(mssg, &left);
    uGetString(epMap[currep].compName, mssg, &left);
    epMap[currep].intNo   = uGetNumber(mssg, &left);
    uGetString(epMap[currep].intName, mssg, &left);
    ++currep;

    int i = currep-1;
    kprintf("Ep id: %d, CompNo: %d, CompName: %s, IntNo: %d, IntName: %s\n",
		    epMap[i].epNo, epMap[i].compNo, epMap[i].compName, epMap[i].intNo, epMap[i].intName);
    mssg = '\0';
}

void handleRWFMThreadReg(void)
{
    //Register the thread id for each camkes component.
    char *mssg = (char*)(lookupIPCBuffer(true, NODE_STATE(ksCurThread)) + 1);
    int left = 0;

    thrMap[currthr].thrNo  = uGetNumber(mssg, &left);
    uGetString(thrMap[currthr].thrName, mssg, &left);
    thrMap[currthr].compNo = uGetNumber(mssg, &left);
    uGetString(thrMap[currthr].compName, mssg, &left);
    ++currthr;

    int i = currthr-1;
    kprintf("ThrNo: %d, ThrName: %s, CompNo: %d, CompName: %s\n",
		    thrMap[i].thrNo, thrMap[currthr].thrName, thrMap[i].compNo, thrMap[currthr].compName);
    mssg = '\0';
}
