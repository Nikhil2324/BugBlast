#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include <string>
class StudentWorld;

using namespace std;

class Object: public GraphObject
{
public:
	Object(int id, int x, int y, StudentWorld* s); 
	virtual ~Object();
	virtual void doSomething() = 0;
	StudentWorld* getWorld();
	virtual void kill();
	bool isDead() const;
private:
	StudentWorld* m_s;
	bool m_dead;
};

class Brick : public Object
{
public:
	Brick(int id, int x, int y, StudentWorld* s);
	virtual void doSomething();
};

class destroyableBrick: public Brick
{
public:
	destroyableBrick(int x, int y, StudentWorld* s);
	virtual void kill();
};

class Exit : public Object
{
public:
	Exit(int x, int y, StudentWorld* s);
	virtual void doSomething();
private:
	bool m_visible;
};

class BugSprayer : public Object
{
public:
	BugSprayer(int x, int y, StudentWorld* s);
	virtual void doSomething();
	virtual void kill();
private:
	int m_lifetime;
};

class Zumi : public Object
{
public:
	Zumi(int id, int x, int y, StudentWorld* s, int ticksPerMove);
	virtual void doSomething();
	int randInt(int lowest, int highest);
	virtual void kill();
	bool canZumiMove(int x, int y);
	bool turnToMove();
	void simpleMove();
	void setDirection(int direction);
private:
	int m_ticksPerMove;
	int m_direction;
	int m_ticks;
};

class ComplexZumi: public Zumi
{
public:
	ComplexZumi(int id, int x, int y, StudentWorld* sw, int ticksPerMove);
	virtual void doSomething();
	virtual void kill();
};

class Player : public Object
{
public:
	Player(int x, int y, StudentWorld* sw);
	virtual void doSomething();
	virtual void kill();
	void setJesus();
	void increaseSprayers();
	int getMaxSprayers() const;
	void decrementSprayers();
	bool canPlayerMove(int x, int y);
	int getSprayers() const;
private:
	bool m_ethereal;
	int max_sprayers;
	int m_sprayers;
	int m_SprayerLife;
	int m_WallLife;
};

class BugSpray : public Object
{
public:
	BugSpray(int x, int y, StudentWorld* sw);
	virtual void doSomething();
private:
	int m_ticks;
};

class Goodie : public Object
{
public:
	Goodie(int id, int x, int y, StudentWorld* sw);
	virtual void doSomething() = 0;
	int getLife() const;
	void decrementLife();
private:
	int m_lifetime;
};

class ExtraLife : public Goodie
{
public:
	ExtraLife(int x, int y, StudentWorld* sw);
	virtual void doSomething();
};

class WalkWalls : public Goodie
{
public:
	WalkWalls(int x, int y, StudentWorld* sw);
	virtual void doSomething();
};

class MoreSprayers : public Goodie
{
public:
	MoreSprayers(int x, int y, StudentWorld* sw);
	virtual void doSomething();
};

class Coordinates //Coordinates of interesting locations
{
public: 
	Coordinates(int x, int y) //This constructor only takes x and y coordinates, not direction, because they are not always important
	{
		m_x = x;
		m_y = y;
		m_direction = -1; //Sets direction to random value
	}

	Coordinates(int x, int y, int direction) //If direction important, it is taken
	{
		m_x = x;
		m_y = y;
		m_direction = direction;
	}

	int X() const
	{
		return m_x;
	}

	int Y() const
	{
		return m_y;
	}

	int direction() const
	{
		return m_direction;
	}
private:
	int m_x;
	int m_y;
	int m_direction;
};

#endif // ACTOR_H_