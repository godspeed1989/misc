#ifndef __FILEREADER_STATIC_HPP__
#define __FILEHEADER_STATIC_HPP__

#define T_BIT          3
#define T_BYTE         4
#define T_BIT_OTHER    7
#define T_BYTE_OTHER   16

static int readin(bitfile &reader, PARA_entity* e, vector<data> &container)
{
	data d;
	switch(e->attr.type)
	{
		case T_BIT:
			d.lenb = e->attr.len.l;      break;
		case T_BYTE:
			d.lenb = e->attr.len.l << 3; break;
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
	d.ref = e;
	d.p = malloc((d.lenb >> 3) + 1);
	reader.readb(d.p, d.lenb);
	if(reader.eof())
		return -1;
	container.push_back(d);
	return 0;
}

static long get_value_by_name(vector<data> &data_set, const xmlChar* name)
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
	if(data_set[i].lenb > sizeof(long)*8)
		printf("warning: data length of %s exceed return type.\n", name);
	return (long)((long*)(data_set[i].p));
}

static bool range_equal(range &rng, long value)
{
	if(rng.type == T_ANY)
		return true;
	if(rng.type == T_VALUE)
		return (rng.low == value);
	if(rng.type == T_RANGE)
		return (value >= rng.low && value <= rng.high);
	return false;
}

#endif

