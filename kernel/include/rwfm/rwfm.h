#include <kernel/thread.h>
#include <rwfm/rwfmmodel.c>

//#define prints
#define ns 10
#define no 10

int currsub = 0, currobj = 0, currep = 0, currthr = 0;
SUBJECT s[10];
OBJECT o[10];
epMapping epMap[10];
thrMapping thrMap[10];
int dataFlowStatus[ns][no];

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
void handleCheckDataFlowStatus(void);
bool_t alreadyRegisteredEp(int epNo, int compNo, int intNo);

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
    kprintf("{%s}: Thread: %s sub id: %d owner: %d readers: %ld writers: %ld\n",
		   __func__, s[i].compName, s[i].sub_id_index, s[i].owner, s[i].readers, s[i].writers);
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
    kprintf("{%s}: Interface: %s, obj id: %d owner: %d readers: %ld writers: %ld\n",
		    __func__, o[i].objName, o[i].obj_id_index, o[i].owner, o[i].readers, o[i].writers);
    #endif
    mssg = '\0';
}

bool_t alreadyRegisteredEp(int epNo, int compNo, int intNo) {
	for (int i = 0; i < currep; i++) {
		if (epMap[i].epNo == epNo && epMap[i].compNo == compNo && epMap[i].intNo == intNo)
			return true;
	}
	return false;
}

void handleRWFMEpReg(void)
{
    //Register the endpoints created corresponding to each interface for a camkes component.
    char *mssg = (char*)(lookupIPCBuffer(true, NODE_STATE(ksCurThread)) + 1);
		//kprintf("{%s}: Ep string: %s %d\n", __func__, mssg, currep);
    int left = 0;
		char compName[20];
		int epNo = uGetNumber(mssg, &left, '~');
		int compNo = uGetNumber(mssg, &left, '~');

		uGetString(compName, mssg, &left, '~');
		int intNo = uGetNumber(mssg, &left, '~');

		if (alreadyRegisteredEp(epNo, compNo, intNo)) {
			#ifdef prints
      kprintf("{%s} Already registered Endpoint number\n", __func__);
      #endif
			mssg = '\0';
			return ;
		}
		epMap[currep].epNo = epNo;
		epMap[currep].compNo = compNo;
		strlcpy(epMap[currep].compName, compName, 20);
		epMap[currep].intNo = intNo;
    uGetString(epMap[currep].intName, mssg, &left, '~');
    epMap[currep].epptr = NULL;
    ++currep;

    #ifdef prints
    int i = currep-1;
    kprintf("{%s}: Ep id: %d, CompNo: %d, CompName: %s, IntNo: %d, IntName: %s, Epptr: %p\n",
										__func__, epMap[i].epNo, epMap[i].compNo, epMap[i].compName, epMap[i].intNo,
										epMap[i].intName, (void *)epMap[i].epptr);
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
    kprintf("{%s}: ThrNo: %d, ThrName: %s, CompNo: %d, CompName: %s\n",
		    __func__, thrMap[i].thrNo, thrMap[currthr].thrName, thrMap[i].compNo, thrMap[currthr].compName);
    #endif
    mssg = '\0';
}

void registerEpptrPerEp(endpoint_t* epptr, cptr_t ep)
{
  char thrName[20];
  int left = 0;
  uGetString(thrName, ksCurThread->tcbName, &left, ':');
	//kprintf("{%s}: Thr: %s, Ind: %ld %p\n", __func__, thrName, ep, epptr);
  for(int i=0;i<currep;i++)
  {
    if ((!strCmp(thrName, epMap[i].compName)) && ep == epMap[i].epNo && epMap[i].epptr==NULL)
    {
      epMap[i].epptr = epptr;
      #ifdef prints
			kprintf("{%s}: Ep id: %d, CompNo: %d, CompName: %s, IntNo: %d, IntName: %s, Epptr: %p\n",
											__func__, epMap[i].epNo, epMap[i].compNo, epMap[i].compName, epMap[i].intNo,
											epMap[i].intName, (void *)epMap[i].epptr);
      kprintf("{%s}: The EP for %s is registered.\n", __func__, thrName);
      #endif
      break;
    }
  }
}

int checkRWFMWrite(char *sender, endpoint_t* intEpptr)
{
  char subName[20];
  int subIdNo, objIntNo, left = 0;
  uGetString(subName, sender, &left, ':');
  int sNo, oNo, epNo;

  subIdNo = objIntNo = -1;

  for(int i=0;i<currep;i++)
  {
    if ((!strCmp(subName, epMap[i].compName)) && intEpptr == epMap[i].epptr)
    {
      //kprintf("{%s}: Send interface number %d.", __func__, epMap[i].intNo);
      objIntNo = epMap[i].intNo;
      subIdNo = epMap[i].compNo;
			epNo = epMap[i].epNo;
      break;
    }
  }

	if (objIntNo == -1)
  {
    //kprintf("{%s} The endpoint for thread %s is not registered with RWFM.\n", __func__, ksCurThread->tcbName);
    return SUCCESS;
  }
  if (subIdNo == -1)
  {
    //kprintf("{%s}: The thread %s is not registered with RWFM.\n", __func__, ksCurThread->tcbName);
    return SUCCESS;
  }

	for(sNo=0; sNo<currsub; sNo++)
  {
    if (subIdNo == s[sNo].sub_id_index)	break;
  }
  for(oNo=0; oNo<currobj; oNo++)
  {
    if (objIntNo == o[oNo].obj_id_index)	break;
  }
  #ifdef prints
  kprintf("{%s}: Checking for write rule from subject: %d to object: %d\n", __func__, subIdNo, objIntNo);
  #endif
  //kprintf("{%s}: %d %ld %ld %ld %ld\n", __func__, subIdNo, s[sNo].readers, s[sNo].writers, o[oNo].readers, o[oNo].writers);
	//dataFlowStatus is SUCCESS for a client on an endpoint if the data flow is allowed otherwise FAILURE.
  dataFlowStatus[subIdNo][epNo] = do_write(subIdNo, &s[sNo].readers, &s[sNo].writers, &o[oNo].readers, &o[oNo].writers);
	return dataFlowStatus[subIdNo][epNo];
}

int checkRWFMRead(char *receiver, endpoint_t* destEpptr)
{
  char subName[20];
  int subIdNo, objIntNo, left = 0;
  uGetString(subName, receiver, &left, ':');
  int sNo, oNo;

  subIdNo = objIntNo = -1;

  for(int i=0;i<currep;i++)
  {
    if ((!strCmp(subName, epMap[i].compName)) && destEpptr == epMap[i].epptr)
    {
      //kprintf("{%s}: Receive interface number %d.", __func__, epMap[i].intNo);
      objIntNo = epMap[i].intNo;
      subIdNo = epMap[i].compNo;
      break;
    }
  }

  if (objIntNo == -1)
  {
    //kprintf("{%s}: The endpoint for thread %s is not registered with RWFM.\n", __func__, ksCurThread->tcbName);
    return SUCCESS;
  }
  if (subIdNo == -1)
  {
    //kprintf("{%s}: The thread %s is not registered with RWFM.\n", __func__, ksCurThread->tcbName);
    return SUCCESS;
  }

	for(sNo=0;sNo<currsub;sNo++)
  {
    if (s[sNo].sub_id_index == subIdNo)	break;
  }

  for(oNo = 0; oNo < currobj; oNo++)
  {
    if (o[oNo].obj_id_index == objIntNo)	break;
  }
  #ifdef prints
  kprintf("{%s}: Checking from read rule from object: %d to subject: %d\n", __func__, objIntNo, subIdNo);
  #endif
  //kprintf("{%s}: %d %ld %ld %ld %ld\n", __func__, subIdNo, s[sNo].readers, s[sNo].writers, &o[oNo].readers, &o[oNo].writers);
  return do_read(subIdNo, &s[sNo].readers, &s[sNo].writers, &o[oNo].readers, &o[oNo].writers);
}

void rwfmUpdateLabels(char* receiver, endpoint_t* epptr)
{
  char subName[20];
  int subIdNo, objIntNo, left = 0;
  uGetString(subName, receiver, &left, ':');
  int sNo, oNo;

  subIdNo = objIntNo = -1;

  for(int i=0;i<currep;i++)
  {
    if ((!strCmp(subName, epMap[i].compName)) && epptr == epMap[i].epptr)
    {
      //kprintf("{%s}: Send interface number %d.", __func__, epMap[i].intNo);
      objIntNo = epMap[i].intNo;
      subIdNo = epMap[i].compNo;
      break;
    }
  }
  if (objIntNo == -1)
  {
    //kprintf("{%s}: The endpoint for thread %s is not registered with RWFM.\n", __func__, ksCurThread->tcbName);
    return ;
  }
	if (subIdNo == -1)
  {
    //kprintf("{%s}: The thread %s is not registered with RWFM.\n", __func__, ksCurThread->tcbName);
    return ;
  }
  for(sNo=0;sNo<currsub;sNo++)
  {
    if (s[sNo].sub_id_index == subIdNo) break;
  }

  for(oNo=0; oNo<currobj; oNo++)
  {
    if (o[oNo].obj_id_index == objIntNo)	break;
  }
  #ifdef prints
  kprintf("{%s}: For object: %d to subject %d\n", __func__, objIntNo, subIdNo);
  #endif
  update_labels(subIdNo, &s[sNo].readers, &s[sNo].writers, &o[oNo].readers, &o[oNo].writers);
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
      //kprintf("{%s}: Interface number %d.", __func__, epMap[i].intNo);
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
  //kprintf("{%s}: Mssg got: %ld %ld\n", __func__, getRegister(sender, msgRegisters[0]), getRegister(receiver, msgRegisters[0]));
  if (getRegister(sender, msgRegisters[0]) == 0) //If the message is just 0.
    return FAILURE;
  return SUCCESS;
}

void handleCheckDataFlowStatus(void)
{
	int compNo = getRegister(ksCurThread, msgRegisters[0]);
	int epNo = getRegister(ksCurThread, msgRegisters[1]);

	setRegister(ksCurThread, msgRegisters[0], dataFlowStatus[compNo][epNo]);
	dataFlowStatus[compNo][epNo] = 0;
}
