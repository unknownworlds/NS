#ifndef CFURNITURE_H
#define CFURNITURE_H

//=========================================================
// Furniture - this is the cool comment I cut-and-pasted
//=========================================================
class CFurniture : public CBaseMonster
{
public:
	void Spawn ( void );
	void Die( void );
	int	 Classify ( void );
	virtual int	ObjectCaps( void ) { return (CBaseMonster :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION); }
};

#endif