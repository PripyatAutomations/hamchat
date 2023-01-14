#include "hamchat.h"

///////////////////////////////////////////////////////
// Here we keep track of indirectly reachable nodes. //
// We build a tree of reachable nodes here.          //
//                                                   //
// Reachable is determined by either hearing them or //
// forwarded message for them.                       //
///////////////////////////////////////////////////////

/*
 * Tree layout:
 *
 *	[Local]
 *       |........Reachable neighbor KA1ABC (1)
 *       |             |....... Indirect neighbor K8ABC (2)
 *       |             |          |..... Indirect neighbor K9LMN (3)
 *       |             `....... Indirect neighbor K9ABC (2)
 *       |........Reachable neighbor KB2ABC (1)
 *       |             |....... Indirect neighbor W8ABC (2)
 *       |                        |..... Indirect neighbor W9LMN (3)
 *       |             |....... Indirect neighbor W9ABC (2)
 *       |........Reachable neighbor K9ABC (1) *
 */

// This tends to prefer IP links where possible, due to their higher bandwidth (lower cost)
Transport *least_cost_route(Client *src, Client *dest, int max_cost, int priority) {
   return NULL;
}

// This tries to send directly or via max_hops hops at most
Transport *shortest_path_route(Client *src, Client *dest, int max_hops, int priority) {
   return NULL;
}

// tries to reliably deliver the message, even if slower
Transport *most_reliable_route(Client *src, Client *dest, int max_cost, int priority) {
   return NULL;
}
