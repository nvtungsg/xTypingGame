#pragma once

struct Laser {
	int x, y, huong;
	int time;
	int fireTime;
	bool hasFired = false;
};

void Laser_Run();