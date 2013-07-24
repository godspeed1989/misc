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
		if(xmlStrncasecmp((*rit)->name, name, MLEN) == 0)
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
static void show_range(const range& rng, FILE* fout)
{
	if(rng.type == T_ANY)
		fprintf(fout, "ANY");
	else if(rng.type == T_VALUE)
		fprintf(fout, "%ld", rng.low);
	else if(rng.type == T_RANGE)
		fprintf(fout, "%ld~%ld", rng.low, rng.high);
}

static void show_length(const PARA_entity *entity, FILE *fout)
{
	fprintf(fout, "len=");
	switch(entity->attr.type)
	{
		T_BIT_CASE: T_BYTE_CASE:
			fprintf(fout, "%db", entity->attr.len.lb);
			break;
		T_BIT_REF_CASE: T_BYTE_REF_CASE:
			fprintf(fout, "'$%s'", entity->attr.len.le->name);
			break;
		T_NULL_CASE:
			fprintf(fout, "''");
			break;
		default:
			fprintf(fout, "??unknown??");
			throw;
	}
}

static void show_PARA_entity(const PARA_entity *entity, FILE *fout)
{
	for(int i = 2; i < entity->depth; ++i)
		fprintf(fout, "+---");
	if(entity->type == T_PARA)
	{
		fprintf(fout, "%s %d ", entity->name, entity->attr.type);
		show_length(entity, fout);
		if(entity->depend)
			fprintf(fout, " depend='%s'", entity->depend->name);
	}
	else if(entity->type == T_PARACHOICE)
	{
		fprintf(fout, "CHOICE on %s value=", entity->depend->name);
		show_range(entity->attr.rng, fout);
	}
	fprintf(fout, "\n");
}

static void show_one_log_fmt(const log_format *log, FILE* fout)
{
	fprintf(fout, "<LOG type=");
	show_range(log->rng, fout);
	fprintf(fout, ">(%zu)\n", log->entities.size());
	for(size_t i = 0; i < log->entities.size(); ++i)
	{
		show_PARA_entity(log->entities[i], fout);
	}
}

#endif

