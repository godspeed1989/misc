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
	u32 i;
	printf("\tdebug: %s %p %u = ", d.ref->name, d.p, d.lenb);
	for(i = 0; i < (d.lenb>>3) && i < 10; ++i)
		printf("%02x ", *((u8*)d.p+i));
	if(i < (d.lenb>>3))
		printf("...");
	printf("\n");
}

static const data& get_data_by_name(const vector<data> &data_set, const xmlChar* name)
{
	size_t i;
	static data d;
	for(i = 0; i < data_set.size(); ++i)
	{
		if(xmlStrncasecmp(data_set[i].ref->name, name, MLEN) == 0)
			break;
	}
	if(i == data_set.size())
	{
		printf("warning: can't find data of [%s]\n", name);
		return d;
	}
	return data_set[i];
}

// get a value by its name from a data set
static void* get_valuep_by_name(const vector<data> &data_set, const xmlChar* name)
{
	return get_data_by_name(data_set, name).p;
}

// get data length by its name from a data set
static int get_lenB_by_name(const vector<data> &data_set, const xmlChar* name)
{
	u32 lenb = get_data_by_name(data_set, name).lenb;
	return  (lenb >> 3) + ((lenb & 7) != 0);
}

// get a value by its reference pointer
static int get_value_by_ref(const vector<data> &data_set, const PARA_entity* ref)
{
	int value;
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
	switch(get_lenB_by_name(data_set, ref->name))
	{
		case 1: value = *((char*)data_set[i].p);	break;
		case 2: value = *((short*)data_set[i].p);	break;
		case 4: value = *((int*)data_set[i].p);		break;
		default:
			printf("%s: not suppported value length\n", ref->name);
			throw;
	}
	return value;
}

// read in one entity to the container
static int readin_entity(bitfile &reader, PARA_entity* e, vector<data> &container)
{
	data d;
	d.ref = e;
	if(reader.eof())
	{
		printf("read in para entity [%s] reached EOF\n", e->name);
		return -1;
	}
	// calculate the length by type
	switch(e->attr.type)
	{
		T_BIT_CASE: T_BYTE_CASE:
			//len is a direct value
			d.lenb = e->attr.len.lb;
			break;
		T_BIT_REF_CASE:
			//len is based on other in BIT
			d.lenb = get_value_by_ref(container, e->attr.len.le);
			break;
		T_BYTE_REF_CASE:
			//len is based on other in BYTE
			d.lenb = get_value_by_ref(container, e->attr.len.le) << 3;
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
	// read the data from file
	d.p = malloc((d.lenb >> 3) + 1);
	reader.readb(d.p, d.lenb);
#if 0
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
			long val = get_value_by_ref(container, es[i]->depend);

			// match the choice range, continue to read
			if(range_equal(es[i]->attr.rng, val))
			{
				continue;
			}
			else
			{
				// skip the adjacent entities with the higher depth
				size_t cur = i;
				++i;
				while(i < es.size() && es[i]->depth > es[cur]->depth)
					++i;
			}
		}
		else
			throw;
	}
	return 0;
}

#endif

