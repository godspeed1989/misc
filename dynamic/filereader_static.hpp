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

#if 0
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
#endif

static const data& get_data_by_name(const vector<data> &data_set, const xmlChar* name)
{
	size_t i;
	static const data d;
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
static const void* get_valuep_by_name(const vector<data> &data_set, const xmlChar* name)
{
	return get_data_by_name(data_set, name).p;
}

// get data length by its name from a data set
static int get_lenB_by_name(const vector<data> &data_set, const xmlChar* name)
{
	u32 lenb = get_data_by_name(data_set, name).lenb;
	return  (lenb >> 3) + ((lenb & 7) != 0);
}

// get a data ref by its reference pointer
static const data& get_data_by_ref(const vector<data> &data_set, const PARA_entity* ref)
{
	size_t i;
	for(i = 0; i < data_set.size(); ++i)
	{
		if(data_set[i].ref == ref)
			break;
	}
	if(i == data_set.size())
	{
		printf("error: can't find data [%s]\n", ref->name);
		throw;
	}
	return data_set[i];
}

// get a value by its reference pointer
static int get_value_by_ref(const vector<data> &data_set, const PARA_entity* ref)
{
	int lenB, value;
	const data &dat = get_data_by_ref(data_set, ref);
	lenB = (dat.lenb >> 3) + ((dat.lenb & 7) != 0);
	switch(lenB)
	{
		case 1:  value = *((char*)dat.p);	break;
		case 2:  value = *((short*)dat.p);	break;
		case 4:  value = *((int*)dat.p);	break;
		default:
			printf("%s: not suppported value length\n", ref->name);
			throw;
	}
	return value;
}

// read in one entity to the container
static int readin_entity(bitfile &reader, const PARA_entity* e, vector<data> &container)
{
	data d;
	d.ref = e;
	// check the dependent parameter
	if(e->depend && (get_value_by_ref(container, e->depend) == 0))
	{
		return 0;
	}
	// calculate the length by type
	switch(e->attr.type)
	{
		T_BIT_CASE: T_BYTE_CASE:
			//len is a direct value
			d.lenb = e->attr.len.lb;
			break;
		T_BIT_REF_CASE:
			//len is based on other's value in BIT
			d.lenb = get_value_by_ref(container, e->attr.len.le);
			break;
		T_BYTE_REF_CASE:
			//len is based on other's value in BYTE
			d.lenb = get_value_by_ref(container, e->attr.len.le) << 3;
			break;
		T_NULL_CASE:
			return 0;
		default:
			printf("unkonw attr type %d of %s\n", e->attr.type, e->name);
			return -1;
	}
	if(d.lenb == 0)
	{
		printf("warning: the length of %s is zero\n", e->name);
		return 0;
	}
	// read the data from file
	if(reader.eof())
	{
		printf("read in para entity [%s] reached EOF\n", e->name);
		return -1;
	}
	d.p = malloc((d.lenb >> 3) + 1);
	d.lenb = reader.readb(d.p, d.lenb);
#if 0
	dump_data(d);
#endif
	container.push_back(d);
	return 0;
}

// read in set of entites to a container
static int readin_entities(bitfile &reader, const vector<PARA_entity*> es, vector<data> &container)
{
	for(size_t i = 0; i < es.size(); ++i)
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
			if(es[i]->attr.rng.type == T_ANY)
			{
				continue;
			}
			long val = get_value_by_ref(container, es[i]->depend);
			// match the choice range, continue to read
			if(range_equal(es[i]->attr.rng, val))
			{
				continue;
			}
			else
			{
				// skip the following entities with the higher depth
				size_t a = 1;
				while(i+a < es.size() && es[i+a]->depth > es[i]->depth)
				{
					++a;
				}
				i += (a-1);
			}
		}
		else
		{
			throw;
		}
	}
	return 0;
}

#endif

