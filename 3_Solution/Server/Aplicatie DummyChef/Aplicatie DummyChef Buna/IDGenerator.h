#pragma once
#include<iostream>
#include<vector>
#include<string>
#include<algorithm>

class IDGenerator
{
private:
	std::vector<int>id_uri;
public:
	IDGenerator() { srand(time(NULL)); }
	int getID()
	{
		int id;
		bool ok = true;
		while (ok)
		{
			id = rand() % 100000 + 1;
			ok = false;
			std::for_each(id_uri.begin(), id_uri.end(), [&](int i) { if (i == id) { ok = true; }});
		}
		id_uri.push_back(id);
		return id;
	}
	void eraseID(int id)
	{
		id_uri.erase(std::remove(id_uri.begin(), id_uri.end(), id));
	}
	~IDGenerator() {};
};