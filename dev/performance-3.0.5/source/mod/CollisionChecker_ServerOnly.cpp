#include <vector>

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"

#include "common/const.h"
#include "common/com_model.h"
#include "pm_defs.h"
#include "common/vector_util.h"
#include "mod/AvHHulls.h"
#include "types.h"
#include "util/MathUtil.h"

#include "CollisionChecker.h"

const int CollisionChecker::IGNORE_NONE = 0;
const int CollisionChecker::IGNORE_INVISIBLE = 1;
const int CollisionChecker::IGNORE_INTANGIBLE = 2;

void CollisionChecker::UpdateNumEntities() const
{
	switch(ignore_entity_class_)
	{
	case IGNORE_NONE:
		memset(&local_ent,0,sizeof(local_ent));
		entity_count_ = gpGlobals->maxEntities;
		break;
	case IGNORE_INVISIBLE:
		entity_count_ = pmove_ ? pmove_->numvisent : 0;
		break;
	case IGNORE_INTANGIBLE:
		entity_count_ = pmove_ ? pmove_->numphysent : 0;
		break;
	default:
		ASSERT(0 && "Unknown ignore entity class");
	};
}

const physent_t* CollisionChecker::GetPhysentForIndex(int entity_index) const
{
	switch(ignore_entity_class_)
	{
	case IGNORE_NONE:
	{
		edict_t* entity = INDEXENT(entity_index);
		if(entity)
		{
			local_ent.info = entity_index;
			local_ent.player = entity_index && entity_index <= gpGlobals->maxClients ? 1 : 0;
			local_ent.origin = entity->v.origin;
			local_ent.angles = entity->v.angles;
			local_ent.mins = entity->v.mins;
			local_ent.maxs = entity->v.maxs;
			local_ent.solid = entity->v.solid;
			if(local_ent.solid != SOLID_BBOX)
			{
				for(int counter = 0; counter < pmove_->numvisent; ++counter)
				{
					if(local_ent.info == pmove_->visents[counter].info)
					{
						local_ent.model = pmove_->visents[counter].model;
						break;
					}
				}
			}
			return &local_ent;
		}
		return NULL;
	}
	case IGNORE_INVISIBLE:
		return pmove_ ? &pmove_->visents[entity_index] : NULL;
	case IGNORE_INTANGIBLE:
		return pmove_ ? &pmove_->physents[entity_index] : NULL;
	}
	ASSERT(0 && "Unknown ignore entity class");
	return NULL;
}
