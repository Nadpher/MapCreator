#pragma once

namespace nadpher
{

// struct used to index tiles in a map
struct Coord
{
	int x, y;

	bool operator< (const Coord& other) const
	{
		if (x == other.x)
		{
			return y < other.y;
		}

		return x < other.x;
	}
};

}

