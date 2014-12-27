#include "def.h"
#include <unistd.h>

printMSG(struct message *m){
  switch(m->TYPE){
    case MSG_REQ: 
      printf("Clock: %d\tLegion %d wants to enter on route %d", m->clock, m->senderID, m-> routeID); 
      break;
    case MSG_ENTER: 
      printf("Clock: %d\tLegion %d enters on route %d", m->clock, m->senderID, m-> routeID); 
      break;
    case MSG_LEAVE: 
      printf("Clock: %d\tLegion %d leaves route %d", m->clock, m->senderID, m-> routeID); 
      break;
  }
}


main()
{

	int nproc, i, who;
    
    //load from file:
    int legion_count = 0;
    int route_count  = 0;
    printf("initialization\n");
    FILE *input_file=NULL;
    input_file=fopen("input.txt","r");
    if (input_file==NULL) {
        perror("Error opening input file ");
        return 1;
    }
    
    fscanf(input_file,"%d\n",&legion_count);
    int legions[legion_count];
    printf("NUMBER OF LEGIONS: %d\n",legion_count);
    
    for( i=0;i<legion_count;i++){
        fscanf(input_file,"%d",&legions[i]);
    }
    
    fscanf(input_file,"\n%d\n",&route_count);
    int routes[route_count];
    printf("NUMBER OF ROUTES: %d\n",route_count);
    
    for( i=0;i<route_count;i++){
        fscanf(input_file,"%d",&routes[i]);
    }
    
    fclose(input_file);
	
    int legion_ids[legion_count];
	nproc=pvm_spawn(SLAVENAME, NULL, PvmTaskDefault, "", legion_count, legion_ids);
    //inicjalizacja legionów
    
    if(nproc != legion_count){
        printf("Couldn't spawn all legions -- %d spawned\n",nproc);
        return 1;
    }
    
    printf("%d legions spawned!\n",nproc);
    
	for( i=0 ; i<nproc ; i++ )
	{
		pvm_initsend(PvmDataDefault);
		pvm_pkint(&i, 1, 1);
        pvm_pkint(&legion_count,1,1);
        pvm_pkint(legions,legion_count,1);
        pvm_pkint(&route_count,1,1);
        pvm_pkint(routes,route_count,1);
		pvm_send(legion_ids[i], MSG_MSTR);
	}
	
  printf("Sent the initialization info to the legions\n");	
  pvm_joingroup(EMPIRE); //Cezar? I don't care, I'm the master.
  
  printf("joined empire group\n");
  //sync before start of the actuall algoritm
  pvm_barrier(EMPIRE,legion_count+1);
  fflush(stdout);
   printf("got througs the barrier\n");
	//TODO:: recieve diagnostic messages
    struct message* m =(struct message*)calloc(1,sizeof(struct message));
	while(1){
	  pvm_recv( -1, -1 );
	  pvm_upkbyte((char*) m, 1, 1 );
	  printMSG(m);
	}

	pvm_exit();
}

