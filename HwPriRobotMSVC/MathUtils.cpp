#include "MathUtils.h"
#include <algorithm>
#include <iostream>
using std::cerr;
using std::endl;

MinimumPowerMatch::MinimumPowerMatch():
	times(msize, vector<int>(msize, 0)), robotLabel(msize, 0),
	matchId(msize, -1), matchRobot(msize, false), matchTask(msize, false),
	taskLabel(msize, 0), slack(msize, 0), robotMatchTaskId(msize, -1)
{
}

int MinimumPowerMatch::minimumTaskId(const vector<vector<int>>& ntimes, int rid)
{
	int m = ntimes.size();
	int n = ntimes[0].size();

#ifdef _DEBUG
	//for (int i = 0; i < m; i++) {
	//	for (int j = 0; j < n; j++) {
	//		cerr << ntimes[i][j] << " ";
	//	}
	//	cerr << endl;
	//}
#endif // _DEBUG

	tNum = std::max(m, n);
	for (int i = 0; i < tNum; i++) {
		for (int j = 0; j < tNum; j++) {
			if (i < m && j < n) {
				times[i][j] = maxV - ntimes[i][j];
			}
			else {
				times[i][j] = 0;
			}
		}
	}
	std::fill(matchId.begin(), matchId.end(), -1);
	std::fill(taskLabel.begin(), taskLabel.end(), 0);

	for (int i = 0; i < tNum; i++) {
		std::fill_n(slack.begin(), tNum, 10*maxV);
		while (true) {
			std::fill(matchRobot.begin(), matchRobot.end(), false);
			std::fill(matchTask.begin(), matchTask.end(), false);
			if (dfs(i)) {
				break;
			}
			int decrease = 10*maxV;
			for (int j = 0; j < tNum; j++) {
				if (!matchTask[j]) {
					decrease = std::min(decrease, slack[j]);
				}
			}
			for (int j = 0; j < tNum; j++) {
				if (matchRobot[j]) {
					robotLabel[j] -= decrease;
				}
				if (matchTask[j]) {
					taskLabel[j] += decrease;
				}
				else {
					slack[j] -= decrease;
				}
			}
		}
	}
	for (int ti = 0; ti < tNum; ti++) {
		robotMatchTaskId[matchId[ti]] = ti;
	}
#ifdef _DEBUG
	//for (int i = 0; i < m; i++) {
	//	cerr << i << " match " << matchId[i] <<endl;
	//}
#endif // _DEBUG
	return robotMatchTaskId[rid];
}

bool MinimumPowerMatch::dfs(int now)
{
	matchRobot[now] = true;
	for (int i = 0; i < tNum; i++) {
		if (matchTask[i]) {
			continue;
		}
		int temp = robotLabel[now] + taskLabel[i] - times[now][i];
		if (temp == 0) {
			matchTask[i] = true;
			if (matchId[i] == -1 || dfs(matchId[i])) {
				matchId[i] = now;
				return true;
			}
		}
		else {
			slack[i] = std::min(slack[i], temp);
		}
	}
	return false;
}
