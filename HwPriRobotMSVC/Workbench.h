#pragma once
class Workbench
{
public:
	int type;
	double x;
	double y;
	int remain_t = -1;
	int raw_status = 0; // �Ѵ�ż���ԭ���� λ��ʾ
	int	pdt_status = 0;

	int id;

public:
	Workbench(int id, int type, double x, double y);
	void scanWorkbench();
	void printWorkbench();
};

