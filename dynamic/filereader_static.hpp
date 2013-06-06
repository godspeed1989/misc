#ifndef __FILEREADER_STATIC_HPP__
#define __FILEHEADER_STATIC_HPP__

#define T_BIT          3
#define T_BYTE         4
#define T_BIT_OTHER    7
#define T_BYTE_OTHER   16

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

// read in on entity to the container
static int readin_entity(bitfile &reader, PARA_entity* e, vector<data> &container)
{
	data d;
	d.ref = e;
	// calculate the length by type
	switch(e->attr.type)
	{
		case T_BIT:
			d.lenb = e->attr.len.l;
			break;
		case T_BYTE:
			d.lenb = e->attr.len.l << 3;
			break;
		case T_BIT_OTHER:
			//TODO len is based other in BIT
			break;
		case T_BYTE_OTHER:
			//TODO len is based other in BYTE
			break;
		default:
			d.lenb = e->attr.len.l << 3;
			break;
	}
	if(d.lenb == 0)
	{
		d.p = NULL;
		return 0;
	}
	d.p = malloc((d.lenb >> 3) + 1);
	// read the data from file
	reader.readb(d.p, d.lenb);
	if(reader.eof())
		return -1;
	container.push_back(d);
	return 0;
}

static long get_value(const data& d)
{
	if(d.lenb > sizeof(long)*8)
		printf("W: data length of %s exceed return type.\n", d.ref->name);
	return (long)((long*)(d.p));
}

// get a value by its reference pointer
static long get_value_by_ref(const vector<data> &data_set, const PARA_entity* ref)
{
	size_t i;
	for(i = 0; i < data_set.size(); ++i)
	{
		if(data_set[i].ref == ref)
			break;
	}
	if(i == data_set.size())
	{
		printf("error: can't find data refer [%s]\n", ref->name);
		throw;
	}
	return get_value(data_set[i]);
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
			long val = get_value_by_ref(container, es[i]->attr.depend);
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
				while(i < es.size() && es[i].depth > es[j].depth)
					++i;
			}
		}
		else
			throw;
	}
	return container.size();
}

// get a value by its name from a data set
static long get_value_by_name(const vector<data> &data_set, const xmlChar* name)
{
	size_t i;
	for(i = 0; i < data_set.size(); ++i)
	{
		if(xmlStrncasecmp(data_set[i].ref->name, name, MLEN) == 0)
			break;
	}
	if(i == data_set.size())
	{
		printf("error: can't find value of [%s]\n", name);
		throw;
	}
	return get_value(data_set[i]);
}

#endif

