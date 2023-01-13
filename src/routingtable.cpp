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
 