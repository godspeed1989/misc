#ifndef __XML_READER_STATIC_H__
#define __XML_READER_STATIC_H__

// resolve a range value
static void resolve_range(range& rng, const xmlChar* str)
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
			rng.type = T_VALUE; // a fixed value
			rng.low = rng.high = atol(p);
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
static PARA_entity* dup_PARA_entity(const PARA_entity* entity)
{
	PARA_entity *pentity = (PARA_entity*)malloc(sizeof(PARA_entity));
	memset(pentity, 0, sizeof(PARA_entity));
	memcpy(pentity, entity, sizeof(PARA_entity));
	return pentity;
}

// free a PARA entity
static void free_PARA_entity(PARA_entity* entity)
{
	free(entity);
}

// get a entity reference by its name
static PARA_entity* get_ref_by_name(vector<PARA_entity*>* es, const xmlChar* name)
{
	if(name[0] == '$') // skip $ at the begin of name
		++name;
	vector<PARA_entity*>::reverse_iterator rit;
	for(rit = es->rbegin(); rit != es->rend(); ++rit)
	{
		if(xmlStrncasecmp((*rit)->name, name, MLEN))
			break;
	}
	if(rit == es->rend())
	{
		printf("error: can't find entity ref by name %s\n", name);
		throw;
	}
	return *rit;
}

// output functions
static void show_range(const range& rng)
{
	if(rng.type == T_ANY)
		printf("ANY");
	else if(rng.type == T_VALUE)
		printf("%ld", rng.low);
	else if(rng.type == T_RANGE)
		printf("%ld~%ld", rng.low, rng.high);
}

static void show_length(const PARA_entity *entity)
{
	printf("length=");
	if(entity->attr.len.lb != -1)
		printf("%db", entity->attr.len.lb);
	else if(entity->attr.len.le)
		printf("'$%s'", entity->attr.len.le->name);
	else
	{
		printf("??unknown??");
		throw;
	}
}

static void show_PARA_entity(const PARA_entity *entity)
{
	for(int i = 2; i < entity->depth; ++i)
		printf("+---");
	if(entity->type == T_PARA)
	{
		printf("name=%s type=%d ", entity->name, entity->attr.type);
		show_length(entity);
		if(entity->depend)
			printf(" depend='%s'", entity->depend->name);
	}
	else if(entity->type == T_PARACHOICE)
	{
		printf("CHOICE on %s value=", entity->depend->name);
		show_range(entity->attr.rng);
	}
	printf("\n");
}

static void show_one_log_fmt(const log_format *log)
{
	printf("<LOG type=");
	show_range(log->rng);
	printf(">(%zu)\n", log->entities.size());
	for(size_t i = 0; i < log->entities.size(); ++i)
	{
		show_PARA_entity(log->entities[i]);
	}
}

#endif

