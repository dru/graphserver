Graph*
#ifndef RETRO
gShortestPathTree( Graph* this, char *from, char *to, State* init_state ) {
#else
gShortestPathTreeRetro( Graph* this, char *from, char *to, State* init_state ) {
#endif
/*
 *  VARIABLE SETUP
 */
  //Iteration Variables
  Vertex *u, *v;
  Vertex *spt_u, *spt_v;
  State *du, *dv;

  //Goal Variables
#ifndef RETRO
  char* origin = from;
  char* target = to;
#else
  char* origin = to;
  char* target = from;
#endif

  //Return Tree
  Graph* spt = gNew();
  gAddVertex( spt, origin )->payload = init_state;
  //Priority Queue
  dirfibheap_t q = dirfibheap_new( gSize( this ) );
  dirfibheap_insert_or_dec_key( q, gGetVertex( this, origin ), 0 );

/*
 *  CENTRAL ITERATION
 *
 */ 
  
  while( !dirfibheap_empty( q ) ) {                //Until the priority queue is empty:
    u = dirfibheap_extract_min( q );                 //get the lowest-weight Vertex 'u',

    if( !strcmp( u->label, target ) )                //(end search if reached destination vertex)
      break;

    spt_u = gGetVertex( spt, u->label );             //get corresponding SPT Vertex,
    du = (State*)spt_u->payload;                     //and get State of u 'du'.

#ifndef RETRO
    ListNode* edges = vGetOutgoingEdgeList( u );
#else
    ListNode* edges = vGetIncomingEdgeList( u );
#endif
    while( edges ) {                                 //For each Edge 'edge' outgoing from u
      Edge* edge = edges->data;
#ifndef RETRO
      v = edge->to;                                  //to Vertex v:
#else
      v = edge->from;
#endif

      spt_v = gGetVertex( spt, v->label );             //get the SPT Vertex corresponding to 'v' 
                                                       //(which may not exist yet)
      dv = (spt_v ? (State*)spt_v->payload : NULL);    //and its State 'dv'.
      
#ifndef RETRO
      State *new_dv = eWalk( edge, du );               //Get the State of v via edge 'new_dv'.
#else
      State *new_dv = eWalkBack( edge, du );
#endif

      // When an edge leads nowhere (as indicated by returning NULL), the iteration is over.
      if(!new_dv) {
        edges = edges->next;
        continue;
      }

      // States cannot have weights lower than their parent State.
      if(new_dv->weight < du->weight) {
        fprintf(stderr, "Negative weight (%s -> %s)(%ld) = %ld\n",edge->from->label, edge->to->label, du->weight, new_dv->weight);
        edges = edges->next;
        continue;
      }

      long old_w = ( dv ? dv->weight : INFINITY);
      long new_w = new_dv->weight;
      // If the weight to 'v' via 'edge' 'new_dv' is smaller than the previously known weight to 'v',
      if( new_w < old_w ) {
        dirfibheap_insert_or_dec_key( q, v, new_w );    // rekey v in the priority queue

        // If this is the first time v has been reached
        if( !spt_v )
          spt_v = gAddVertex( spt, v->label );        //Copy v over to the SPT

        spt_v->payload = new_dv;                      //Set the State of v in the SPT to the current winner

        vSetParent( spt_v, spt_u, edge->payloadtype, edge->payload );      //Make u the parent of v in the SPT
      } else {
        free(new_dv); //new_dv will never be used; merge it with the infinite.
      }
      edges = edges->next;
    }
  }

  dirfibheap_delete( q );

  return spt;
}