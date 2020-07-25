#include <kernel/thread.h>
#include <rwfm/rwfmmodel.c>

//#define prints 0

int currsub=0, ns=10;
SUBJECT s[10];
int currobj=0, no=10;
OBJECT o[10];
epMapping epMap[10];
int currep=0;
thrMapping thrMap[10];
int currthr=0;

int strCmp(char* , char* );
void handleRWFMSubReg(void);
void handleRWFMIntReg(void);
void handleRWFMEpReg(void);
void handleRWFMThreadReg(void);
void registerEpptrPerEp(endpoint_t* epptr, cptr_t ep);
void uGetString(char [], char *, int *, char );
int uGetNumber(char *, int *, char );
int checkRWFMWrite(char *, endpoint_t *);
int checkRWFMRead(char *, endpoint_t *);
void rwfmUpdateLabels(char *, endpoint_t *);
int getIntNo(char *, endpoint_t *);
int isRWFMRequired(tcb_t *, tcb_t *);

int strCmp(char* s1, char* s2)
{
    while(*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void uGetString(char dest[], char *src, int *left, char delimiter)
{
    //Parse the next string before the delimiter.
    int right = *left;
    int i = *left;
    while(src[right] != '\0' && src[right] != delimiter) right++;
    for(; i<right; i++) dest[i-(*left)] = src[i];
    dest[i-(*left)] = '\0';
    *left = right+1;
}

int uGetNumber(char *src, int *left, char delimiter)
{
    //Parse the next number before the delimiter.
    int right = *left;
    int sum = 0;
    while(src[right] != '\0' && src[right] != delimiter) right++;
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

    uGetString(s[currsub].compName, mssg, &left, '~');
    s[currsub].sub_id_index = uGetNumber(mssg, &left, '~');
    s[currsub].owner        = uGetNumber(mssg, &left, '~');
    s[currsub].readers	    = uGetNumber(mssg, &left, '~');
    s[currsub].writers	    = uGetNumber(mssg, &left, '~');
    ++currsub;
    
    #ifdef prints
    int i = currsub-1;
    kprintf("Thread: %s sub id: %d owner: %d readers: %ld writers: %ld\n",
		   s[i].compName, s[i].sub_id_index, s[i].owner, s[i].readers, s[i].writers);
    #endif
    mssg = '\0';
}

void handleRWFMIntReg(void)
{
    //Register the camkes interfaces as RWFM objects.
    char *mssg = (char*)(lookupIPCBuffer(true, NODE_STATE(ksCurThread)) + 1);
    int left = 0;

    uGetString(o[currobj].objName, mssg, &left, '~');
    o[currobj].obj_id_index = uGetNumber(mssg, &left, '~');
    o[currobj].owner        = uGetNumber(mssg, &left, '~');
    o[currobj].readers      = uGetNumber(mssg, &left, '~');
    o[currobj].writers      = uGetNumber(mssg, &left, '~');
    ++currobj;

    #ifdef prints
    int i = currobj-1;
    kprintf("Interface: %s, obj id: %d owner: %d readers: %ld writers: %ld\n",
		    o[i].objName, o[i].obj_id_index, o[i].owner, o[i].readers, o[i].writers);
    #endif
    mssg = '\0';
}

void handleRWFMEpReg(void)
{
    //Register the endpoints created corresponding to each interface for a camkes component.
    char *mssg = (char*)(lookupIPCBuffer(true, NODE_STATE(ksCurThread)) + 1);
    int left = 0;

    epMap[currep].epNo    = uGetNumber(mssg, &left, '~');
    epMap[currep].compNo  = uGetNumber(mssg, &left, '~');
    uGetString(epMap[currep].compName, mssg, &left, '~');
    epMap[currep].intNo   = uGetNumber(mssg, &left, '~');
    uGetString(epMap[currep].intName, mssg, &left, '~');
    epMap[currep].epptr = NULL;
    ++currep;

    #ifdef prints
    int i = currep-1;
    kprintf("Ep id: %d, CompNo: %d, CompName: %s, IntNo: %d, IntName: %s\n",
		    epMap[i].epNo, epMap[i].compNo, epMap[i].compName, epMap[i].intNo, epMap[i].intName);
    #endif
    mssg = '\0';
}

void handleRWFMThreadReg(void)
{
    //Register the thread id for each camkes component.
    char *mssg = (char*)(lookupIPCBuffer(true, NODE_STATE(ksCurThread)) + 1);
    int left = 0;

    thrMap[currthr].thrNo  = uGetNumber(mssg, &left, '~');
    uGetString(thrMap[currthr].thrName, mssg, &left, '~');
    thrMap[currthr].compNo = uGetNumber(mssg, &left, '~');
    uGetString(thrMap[currthr].compName, mssg, &left, '~');
    ++currthr;

    #ifdef prints
    int i = currthr-1;
    kprintf("ThrNo: %d, ThrName: %s, CompNo: %d, CompName: %s\n",
		    thrMap[i].thrNo, thrMap[currthr].thrName, thrMap[i].compNo, thrMap[currthr].compName);
    #endif
    mssg = '\0';
}

void registerEpptrPerEp(endpoint_t* epptr, cptr_t ep)
{
  char thrName[20];
  int left = 0;
  uGetString(thrName, ksCurThread->tcbName, &left, ':');
  for(int i=0;i<currep;i++)
  {
    if ((!strCmp(thrName, epMap[i].compName)) && ep == epMap[i].epNo && epMap[i].epptr==NULL)
    {
      epMap[i].epptr = epptr;
      //kprintf("The EP for %s is registered.\n", thrName);
      break;
    }
  }
}

int checkRWFMWrite(char *sender, endpoint_t* intEpptr)
{
  char subName[20];
  int subIdNo, left = 0;
  uGetString(subName, sender, &left, ':');
  USER_SET objIntNo, subReaders, subWriters, objReaders, objWriters;

  subIdNo = objIntNo = subReaders = subWriters = objReaders = objWriters = -1;

  for(int i=0;i<currep;i++)
  {
    if ((!strCmp(subName, epMap[i].compName)) && intEpptr == epMap[i].epptr)
    {
      //kprintf("Send interface number %d.", epMap[i].intNo);
      objIntNo = epMap[i].intNo;
      subIdNo = epMap[i].compNo;
      break;
    }
  }

 if (objIntNo == -1)
  {
    //kprintf("The endpoint for thread %s is not registered with RWFM.\n", ksCurThread->tcbName);
    return SUCCESS;
  }
  for(int i=0;i<currsub;i++)
  {
    if (subIdNo == s[i].sub_id_index)
    {
      subReaders = s[i].readers;
      subWriters = s[i].writers;
      break;
    }
  }
  if (subIdNo == -1)
  {
    kprintf("The thread %s is not registered with RWFM.\n", ksCurThread->tcbName);
    return SUCCESS;
  }

  for(int i=0;i<currobj;i++)
  {
    if (objIntNo == o[i].obj_id_index)
    {
      objReaders = o[i].readers;
      objWriters = o[i].writers;
      break;
    }
  }
  kprintf("Checking for write rule from subject: %d to object: %ld\n", subIdNo, objIntNo);
  //kprintf("%d %ld %ld %ld %ld\n", subIdNo, subReaders, subWriters, objReaders, objWriters);
  return do_write(subIdNo, &subReaders, &subWriters, &objReaders, &objWriters);
}

int checkRWFMRead(char *receiver, endpoint_t* destEpptr)
{
  char subName[20];
  int subIdNo, left = 0;
  uGetString(subName, receiver, &left, ':');
  USER_SET sNo, objIntNo, objReaders, objWriters;

  subIdNo = objIntNo = objReaders = objWriters = -1;

  for(int i=0;i<currep;i++)
  {
    if ((!strCmp(subName, epMap[i].compName)) && destEpptr == epMap[i].epptr)
    {
      //kprintf("Send interface number %d.", epMap[i].intNo);
      objIntNo = epMap[i].intNo;
      subIdNo = epMap[i].compNo;
      break;
    }
  }

  if (objIntNo == -1)
  {
    //kprintf("The endpoint for thread %s is not registered with RWFM.\n", ksCurThread->tcbName);
    return SUCCESS;
  }

  for(sNo=0;sNo<currsub;sNo++)
  {
    if (s[sNo].sub_id_index == subIdNo) break;
  }
  if (subIdNo == -1)
  {
    //kprintf("The thread %s is not registered with RWFM.\n", ksCurThread->tcbName);
    return SUCCESS;
  }

  for(int i=0;i<currobj;i++)
  {
    if (o[i].obj_id_index == objIntNo)
    {
      objReaders = o[i].readers;
      objWriters = o[i].writers;
      break;
    }
  }
  kprintf("Checking from read rule from object: %ld to subject: %d\n", objIntNo, subIdNo);
  //kprintf("%d %ld %ld %ld %ld\n", subIdNo, s[sNo].readers, s[sNo].writers, objReaders, objWriters);
  return do_read(subIdNo, &s[sNo].readers, &s[sNo].writers, &objReaders, &objWriters);
}

void rwfmUpdateLabels(char* receiver, endpoint_t* epptr)
{
  char subName[20];
  int subIdNo, left = 0;
  uGetString(subName, receiver, &left, ':');
  USER_SET sNo, objIntNo, objReaders, objWriters;

  subIdNo = objIntNo = objReaders = objWriters = -1;

  for(int i=0;i<currep;i++)
  {
    if ((!strCmp(subName, epMap[i].compName)) && epptr == epMap[i].epptr)
    {
      //kprintf("Send interface number %d.", epMap[i].intNo);
      objIntNo = epMap[i].intNo;
      subIdNo = epMap[i].compNo;
      break;
    }
  }

  for(sNo=0;sNo<currsub;sNo++)
  {
    if (s[sNo].sub_id_index == subIdNo) break;
  }
  if (subIdNo == -1)
  {
    //kprintf("The thread %s is not registered with RWFM.\n", ksCurThread->tcbName);
    return ;
  }

  for(int i=0;i<currobj;i++)
  {
    if (o[i].obj_id_index == objIntNo)
    {
      objReaders = o[i].readers;
      objWriters = o[i].writers;
      break;
    }
  }
  kprintf("For object: %ld to subject %d\n", objIntNo, subIdNo);
  update_labels(subIdNo, &s[sNo].readers, &s[sNo].writers, &objReaders, &objWriters);
  //kprintf("Soon I will be able to update the labels.\n");
}

int getIntNo(char *name, endpoint_t *epptr)
{
  char subName[20];
  int left = 0;
  uGetString(subName, name, &left, ':');
  for(int i=0;i<currep;i++)
  {
    if ((!strCmp(subName, epMap[i].compName)) && epptr == epMap[i].epptr)
    {
      //kprintf("Send interface number %d.", epMap[i].intNo);
      return epMap[i].intNo;
    }
  }
  return -1;
}

int isRWFMRequired(tcb_t* sender, tcb_t* receiver)
{
  char senderName[20], receiverName[20];
  int left = 0;
  seL4_MessageInfo_t tag;

  uGetString(senderName, sender->tcbName, &left, ':');
  left = 0;
  uGetString(receiverName, receiver->tcbName, &left, ':');

  tag = messageInfoFromWord(getRegister(sender, msgInfoRegister));
  if (seL4_MessageInfo_get_length(tag) == 0) return FAILURE;

  if (!strCmp(senderName, receiverName)) return FAILURE;
  //kprintf("Mssg got: %ld %ld\n", getRegister(sender, msgRegisters[0]), getRegister(receiver, msgRegisters[0]));
  if (getRegister(sender, msgRegisters[0]) == 0) //If the message is just 0.
    return FAILURE;
  return SUCCESS;
}
