#ifndef __XML_READER_STATIC_H__
#define __XML_READER_STATIC_H__

// resolve a ranged value
static void resolveRng(range& rng, const xmlChar* str)
{
	const char * p = (const char *)str;
	if(xmlStrlen(str) == 0)
	{
		rng.type = T_ANY; // any value
	}
	else
	{
		int i;
		char buf[MLEN];
		for(i = 0; i < xmlStrlen(str); ++i)
			if(str[i] == '~') break;
		if(i == xmlStrlen(str))
		{
			rng.type = T_VALUE;
			rng.low = rng.high = atol(p); // fixed value
		}
		else
		{
			rng.type = T_RANGE; // a value range
			strncpy(buf, p, i);
			buf[i] = '\0';
			rng.low = atol(buf);
			++i;
			strncpy(buf, p+i, xmlStrlen(str)-i);
			buf[xmlStrlen(str)-i] = '\0';
			rng.high = atol(buf);
		}
	}
}

// duplicate a PARA entity
static PARA_entity* dup_PARA_entity(PARA_entity* entity)
{
	PARA_entity *pentity = (PARA_entity*)malloc(sizeof(PARA_entity));
	memset(pentity, 0, sizeof(PARA_entity));
	memcpy(pentity, entity, sizeof(PARA_entity));
	return pentity;
}

// free a PARA entity
static void free_PARA_entity(PARA_entity* entity)
{
	//if(entity->name) xmlFree(entity->name);
	free(entity);
}

// output functions

static void show_range(range& rng)
{
	if(rng.type == T_ANY)
		printf("ANY");
	else if(rng.type == T_VALUE)
		printf("%ld", rng.low);
	else if(rng.type == T_RANGE)
		printf("%ld~%ld", rng.low, rng.high);
}

static void show_PARA_entity(PARA_entity *entity)
{
	for(int i = 2; i < entity->depth; ++i)
		printf("    ");
	if(entity->type == T_PARA)
		printf("name=%s type=%d length=%d\n", entity->name, entity->attr.type, entity->attr.len.l);
	else if(entity->type == T_PARACHOICE)
	{
		printf("CHOICE on %s value=", entity->attr.depend->name);
		show_range(entity->attr.rng);
		printf("\n");
	}
}

static void show_one_log_fmt(log_format *log)
{
	printf("<LOG type=");
	show_range(log->rng);
	printf(">(%d)\n", log->entities.size());
	for(size_t i = 0; i < log->entities.size(); ++i)
	{
		show_PARA_entity(log->entities[i]);
	}
}

#endif

