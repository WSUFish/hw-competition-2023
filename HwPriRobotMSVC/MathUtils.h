#pragma once
#include <vector>
using std::vector;
const int msize = 32;
const int maxV = 100000;

class MinimumPowerMatch {
public:
	MinimumPowerMatch();
	int minimumTaskId(const vector<vector<int>> &ntimes, int rid);
	bool dfs(int now);

	int tNum = msize;
	vector<vector<int>> times;
	vector<int> robotLabel;
	vector<int> taskLabel;
	vector<bool> matchRobot;
	vector<bool> matchTask;
	vector<int> matchId;
	vector<int> slack;

	vector<int> robotMatchTaskId;
};
