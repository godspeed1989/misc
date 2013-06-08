#ifndef __FILEREADER_STATIC_HPP__
#define __FILEHEADER_STATIC_HPP__

// check wether a value fix into the range
static bool range_equal(const range &rng, const long value)
{
	if(rng.type == T_ANY)
		return true;
	if(rng.type == T_VALUE)
		return (rng.low == value);
	if(rng.type == T_RANGE)
		return (value >= rng.low && value <= rng.high);
	return false;
}

static void dump_data(const data& d)
{
	printf("\tdebug: %s %p = ", d.ref->name, d.p);
	for(int i = 0; i < (d.lenb>>3) && i < 10; ++i)
		printf("%x ", *((u8*)d.p+i));
	printf("\n");
}

// get a value by its reference pointer
static void* get_value_by_ref(const vector<data> &data_set, const PARA_entity* ref)
{
	size_t i;
	for(i = 0; i < data_set.size(); ++i)
	{
		if(data_set[i].ref == ref)
			break;
	}
	if(i == data_set.size())
	{
		printf("error: can't find value refer to [%s]\n", ref->name);
		throw;
	}
	return data_set[i].p;
}

static const data& get_data_by_name(const vector<data> &data_set, const xmlChar* name)
{
	size_t i;
	for(i = 0; i < data_set.size(); ++i)
	{
		if(xmlStrncasecmp(data_set[i].ref->name, name, MLEN) == 0)
			break;
	}
	if(i == data_set.size())
	{
		printf("error: can't find data of [%s]\n", name);
		throw;
	}
#if 0
	dump_data(data_set[i]);
#endif
	return data_set[i];
}

// get a value by its name from a data set
static void* get_value_by_name(const vector<data> &data_set, const xmlChar* name)
{
	return get_data_by_name(data_set, name).p;
}

// get data length by its name from a data set
static int get_lenB_by_name(const vector<data> &data_set, const xmlChar* name)
{
	return get_data_by_name(data_set, name).lenb >> 3;
}

// read in on entity to the container
static int readin_entity(bitfile &reader, PARA_entity* e, vector<data> &container)
{
	data d;
	d.ref = e;
	// calculate the length by type
	switch(e->attr.type)
	{
		T_BIT_CASE: T_BYTE_CASE:
			d.lenb = e->attr.len.lb;
			break;
		T_BIT_REF_CASE:
			//TODO len is based other in BIT
			d.lenb = *((u32*)get_value_by_ref(container, e->attr.len.le));
			break;
		T_BYTE_REF_CASE:
			//TODO len is based other in BYTE
			d.lenb = *((u32*)get_value_by_ref(container, e->attr.len.le)) << 3;
			break;
		default:
			printf("unkonw attr type %d of %s\n", e->attr.type, e->name);
			return -1;
	}
	if(d.lenb == 0)
	{
		printf("warning: the length of %s is zero\n", e->name);
		d.p = NULL;
		return 0;
	}
	if(reader.eof())
	{
		printf("read in para entity [%s] reached EOF\n", e->name);
		return -1;
	}
	// read the data from file
	d.p = malloc((d.lenb >> 3) + 1);
	reader.readb(d.p, d.lenb);
#if 1
	dump_data(d);
#endif
	container.push_back(d);
	return 0;
}

// read in set of entites to a container
static int readin_entities(bitfile &reader, vector<PARA_entity*> es, vector<data> &container)
{
	size_t i;
	for(i = 0; i < es.size(); ++i)
	{
		if(es[i]->type == T_PARA)
		{
			if(readin_entity(reader, es[i], container))
			{
				printf("read in para entity [%s] error\n", es[i]->name);
				return -1;
			}
		}
		else if(es[i]->type == T_PARACHOICE)
		{
			long val = *((long*)get_value_by_ref(container, es[i]->attr.depend));
			// match the choice range, continue to read
			if(range_equal(es[i]->attr.rng, val))
			{
				continue;
			}
			else
			{
				// skip the entities with the higher depth
				size_t j = i;
				++i;
				while(i < es.size() && es[i]->depth > es[j]->depth)
					++i;
			}
		}
		else
			throw;
	}
	return container.size();
}

#endif

