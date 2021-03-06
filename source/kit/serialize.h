#pragma once

#include <istream>
#include <ostream>
#include <exception>
#include <string>
#include <vector>

inline void serialize(std::ostream & out, void const * v, int numBytes)
{
	out.write((char const *)v, numBytes);
	if(!out.good())
	{
		throw std::exception();
	}
}

inline void deserialize(std::istream & in, void * v, int numBytes)
{
	in.read((char *)v, numBytes);
	if(!in.good())
	{
		throw std::exception();
	}
}

inline void serialize(std::ostream & out, bool const & v)
{
	serialize(out, (void const *)&v, 1);
}

inline void serialize(std::ostream & out, char const & v)
{
	serialize(out, (void const *)&v, 1);
}

inline void serialize(std::ostream & out, unsigned char const & v)
{
	serialize(out, (void const *)&v, 1);
}

inline void serialize(std::ostream & out, short const & v)
{
	serialize(out, (void const *)&v, 2);
}

inline void serialize(std::ostream & out, unsigned short const & v)
{
	serialize(out, (void const *)&v, 2);
}

inline void serialize(std::ostream & out, int const & v)
{
	serialize(out, (void const *)&v, 4);
}

inline void serialize(std::ostream & out, unsigned int const & v)
{
	serialize(out, (void const *)&v, 4);
}

inline void serialize(std::ostream & out, long const & v)
{
	serialize(out, (void const *)&v, 4);
}

inline void serialize(std::ostream & out, unsigned long const & v)
{
	serialize(out, (void const *)&v, 4);
}

inline void serialize(std::ostream & out, long long const & v)
{
	serialize(out, (void const *)&v, 8);
}

inline void serialize(std::ostream & out, unsigned long long const & v)
{
	serialize(out, (void const *)&v, 8);
}

inline void serialize(std::ostream & out, float const & v)
{
	serialize(out, (void const *)&v, 4);
}

inline void serialize(std::ostream & out, double const & v)
{
	serialize(out, (void const *)&v, 8);
}

inline void deserialize(std::istream & in, bool & v)
{
	v = 0;
	deserialize(in, (void *)&v, 1);
}

inline void deserialize(std::istream & in, char & v)
{
	v = 0;
	deserialize(in, (void *)&v, 1);
}

inline void deserialize(std::istream & in, unsigned char & v)
{
	v = 0;
	deserialize(in, (void *)&v, 1);
}

inline void deserialize(std::istream & in, short & v)
{
	v = 0;
	deserialize(in, (void *)&v, 2);
}

inline void deserialize(std::istream & in, unsigned short & v)
{
	v = 0;
	deserialize(in, (void *)&v, 2);
}

inline void deserialize(std::istream & in, int & v)
{
	v = 0;
	deserialize(in, (void *)&v, 4);
}

inline void deserialize(std::istream & in, unsigned int & v)
{
	v = 0;
	deserialize(in, (void *)&v, 4);
}

inline void deserialize(std::istream & in, long & v)
{
	v = 0;
	deserialize(in, (void *)&v, 4);
}

inline void deserialize(std::istream & in, unsigned long & v)
{
	v = 0;
	deserialize(in, (void *)&v, 4);
}

inline void deserialize(std::istream & in, long long & v)
{
	v = 0;
	deserialize(in, (void *)&v, 8);
}

inline void deserialize(std::istream & in, unsigned long long & v)
{
	v = 0;
	deserialize(in, (void *)&v, 8);
}

inline void deserialize(std::istream & in, float & v)
{
	v = 0;
	deserialize(in, (void *)&v, 4);
}

inline void deserialize(std::istream & in, double & v)
{
	v = 0;
	deserialize(in, (void *)&v, 8);
}

inline void serialize(std::ostream & out, std::string const & v)
{
	serialize(out, (unsigned int)v.size());
	serialize(out, (void const *)&v[0], v.size());
}

inline void deserialize(std::istream & in, std::string & v)
{
	unsigned int size;
	deserialize(in, size);
	v.clear();
	v.resize(size);
	deserialize(in, (void *)&v[0], v.size());
}

template <class T>
void serialize(std::ostream & out, std::vector<T> const & v, void(*serializeItem)(std::ostream &, T const &))
{
	serialize(out, (unsigned int)v.size());
	for(T const & item : v)
	{
		serializeItem(out, item);
	}
}

template <class T>
void deserialize(std::istream & in, std::vector<T> & v, void(*deserializeItem)(std::istream &, T &))
{
	unsigned int size;
	deserialize(in, size);
	v.clear();
	v.resize(size);
	for(T & item : v)
	{
		deserializeItem(in, item);
	}
}

