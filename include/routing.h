#if	!defined(_routing_h)
#define	_routing_h

///////////////////////////////////////////////////////////
// Try to decide the best transport to use for a message //
///////////////////////////////////////////////////////////

// This tends to prefer IP links where possible, due to their higher bandwidth (lower cost)
extern Transport *least_cost_route(Client *src, Client *dest, int max_cost, int priority);

// This tries to send directly or via max_hops hops at most
extern Transport *shortest_path_route(Client *src, Client *dest, int max_hops, int priority);

// tries to reliably deliver the message, even if slower
extern Transport *most_reliable_route(Client *src, Client *dest, int max_cost, int priority);

#endif	// !defined(_routing_h)
