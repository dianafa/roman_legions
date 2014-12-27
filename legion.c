#include <stdlib.h>
#include "def.h"
#include "queue.h"
#include "time.h"

int mytid;          //swoj id
int myid;
int legion_count;
int route_count;

int insertElem(struct elem legion,struct elem* head)
{
  struct elem *tmp = (struct elem*) calloc(1,sizeof( struct elem));
  tmp-> id = legion.id;
  tmp-> clock = legion.clock;
  
  if((head = NULL) || (legion.clock < head->clock) || ((legion.clock == head->clock)&&(legion.id<head->id)) ) //first element
  {
    tmp-> next = NULL;
    head = tmp;
  }
  else
  {
    struct elem *i = head;
    while(i-> next!=NULL && i->next->clock < legion.clock)
    {
      i = i-> next;
    }
    while((i->next->clock==legion.clock) && (i->next->id<legion.id));
    tmp-> next = i-> next;
    i -> next  = tmp;
  }
}

int removeElem(int id,struct elem* head)
{
  struct elem *i = head;
  if(head->id == id)
  {
    head = i->next; 
    free(i);
    return 1;
  }
  
  while(i-> next!=NULL && i->next->id != id)
  {
    i = i-> next;
  }
  
  if(i-> next == NULL) return -1; //if id not found
  else 
  {
    struct elem *t = i->next;
    i -> next = i->next-> next;
    free(t);
    return 1;
  }
}

int sum_before(int id,int legions[],struct elem* head){
  struct elem *i = head;
  int sum=0;
  if(head->id == id)
    return 0;
  
  while(i != NULL && i-> id != id)
  {
    sum=sum + legions[i->id];
    i = i-> next;
  }
  
  return sum; //if id not found function returns size of whole queue
}

int max(int x1,int x2){
  if(x1>=x2) return x1;
  return x2;
}

int receiveOrder(int route_count){
  return rand()%route_count;
}

int interpretMSG(message* m, int onRoute[],int legions[],struct elem** queues){
  Lamport_clock = max(Lamport_clock,m->clock) + 1;  
  int ret=0;
  if(m->TYPE==MSG_REQ){
    struct elem newElem;
    newElem.id=m->senderID;
    newElem.clock=m->clock;
    
    insertElem(newElem,queues[m->routeID]);
    
    message ans;
    ans.TYPE=MSG_ACK;
    ans.clock=Lamport_clock;
    ans.senderID=myid;
    ans.routeID=m->routeID;
    ans.senderTID=mytid;
    
    pvm_initsend(PvmDataDefault);
    pvm_pkbyte((char*) &ans,1,1);
    pvm_send(m->senderTID,MSG_ACK);
    
  }else if(m->TYPE==MSG_ACK){
    ret=1;// decrease the counter of acks
  }else if(m->TYPE==MSG_ENTER){
    
    
    onRoute[m->routeID]+=legions[m->senderID];
    
    removeElem(m->senderID,queues[m->routeID]);
  }else if(m->TYPE==MSG_LEAVE){
    onRoute[m->routeID]-=legions[m->senderID];
    
  }
  
  return ret;
}

int marchOrRest(int onRoute[],int legions[],struct elem** queues){
  
  float rand_period = rand()%6+1;
  float secs = 0.0;
  clock_t start = clock();
  clock_t end;
  struct timeval *delay;
  message* m = calloc(1, sizeof(message));
  
  while(rand_period>secs){
    //recalculate timeout;
    rand_period=(rand_period-secs);
    delay->tv_sec=(long)rand_period;
    delay->tv_usec=(long)(rand_period-(delay->tv_sec))*1000000;
    pvm_trecv(-1,-1,delay);
    pvm_upkbyte((char*)m,1,1);
    interpretMSG(m,onRoute,legions,queues);
    end = clock();
    secs=((float)(end-start))/CLOCKS_PER_SEC;
  }
  
}



main()
{
  
  //initialization
  srand(time(NULL));
  int i;
  pvm_recv( -1, MSG_MSTR);
  pvm_upkint(&myid, 1, 1);
  pvm_upkint(&legion_count,1,1);
  int legions[legion_count]; //licznosci kazdego z legionow  
  pvm_upkint(legions,legion_count,1);
  pvm_upkint(&route_count,1,1);
  int routes[route_count];        //pojemnosci kazdego z traktow     
  pvm_upkint(routes,route_count,1);
  
  mytid=pvm_mytid();
  pvm_joingroup(EMPIRE); //for the glory of Cezar!
  
  
  //sync before start of the actuall algoritm
  pvm_barrier(EMPIRE,legion_count+1);
  int onRoute[route_count];//liczba legionistów z wszystkich obcych legionów o których wiemy że znajdują się w danym momencie na danym trakcie.
  struct elem **routeQueues=(struct elem**)calloc(route_count,sizeof(struct elem*));
  for(i=0; i<route_count; i++){
      onRoute[i]=0;
      routeQueues[i]=(struct elem*)calloc(1,sizeof(struct elem));
  }
  
  //queue routeQueues[route_count];   //Dla każdego traktu: lokalna kolejkę żądań wejścia
  //message myLastMSG;      //Ostatnie wysłane przeze mnie żądanie (wraz z ID traktu i znacznikiem czasowym)
  //message[legion_count];      //Najnowsze znaczniki czasowe potwierdzeń otrzymanych od poszczególnych legionów
  int whichRoute;
  int responseCount;
  int myLegionSize = legions[mytid];
  
  message* m = calloc(1, sizeof(message));
  //start of legion route
  while(1){
    
    marchOrRest(onRoute,legions,routeQueues); // simulates resting after march
    
    
    whichRoute = receiveOrder(route_count);
    //TODO:: Add own request to the queue
    struct elem newElem;
    newElem.id=myid;
    newElem.clock=Lamport_clock+1;
    
    insertElem(newElem,routeQueues[whichRoute]);
    
    message msgr;
    msgr.TYPE = MSG_REQ;
    msgr.clock = ++Lamport_clock;
    msgr.senderID = myid;
    msgr.routeID = whichRoute;
    msgr.senderTID = mytid;
    
    pvm_initsend(PvmDataDefault);
    pvm_pkbyte((char*) &msgr,1,1);
    pvm_bcast(EMPIRE,MSG_REQ);
    
    //go to recieving mode
    responseCount=legion_count;
    while(responseCount>0){
    
        pvm_recv( -1, -1 );
        pvm_upkbyte((char*) m, 1, 1 );
        
        responseCount-=interpretMSG(m,onRoute,legions,routeQueues); //counter to be made global    
    }
    while(sum_before(mytid,legions,routeQueues[whichRoute]) + myLegionSize > routes[whichRoute]){ //wait till there is space on the selected route
        
        pvm_recv( -1, -1 );
        pvm_upkbyte((char*) m, 1, 1 );
        
        interpretMSG(m,onRoute,legions,routeQueues);
    }
    //enter the route
    onRoute[whichRoute]+=myLegionSize;
    //TODO:: delete own request from queue
    removeElem(mytid,routeQueues[whichRoute]);
    msgr.TYPE = MSG_ENTER;
    msgr.clock = ++Lamport_clock;
    msgr.senderID = myid;
    msgr.routeID = whichRoute;
    msgr.senderTID=mytid;
    
    pvm_initsend(PvmDataDefault);
    pvm_pkbyte((char*) &msgr,1,1);
    pvm_bcast(EMPIRE,MSG_ENTER);
    
    marchOrRest(onRoute,legions,routeQueues); //simulates marching (while actively responding to the messages)
    
    //leave the route
    onRoute[whichRoute]-=myLegionSize;
    
    msgr.TYPE = MSG_LEAVE;
    msgr.clock = ++Lamport_clock;
    msgr.senderID = myid;
    msgr.routeID = whichRoute;
    msgr.senderTID=mytid;
    
    pvm_initsend(PvmDataDefault);
    pvm_pkbyte((char*) &msgr,1,1);
    pvm_bcast(EMPIRE,MSG_ENTER);
}
pvm_exit();
}

