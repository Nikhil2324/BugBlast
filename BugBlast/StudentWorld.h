#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "Level.h"
#include <list>
#include <queue>

class StudentWorld : public GameWorld
{
public:
	StudentWorld();
	~StudentWorld();
	virtual int init();
	virtual int move();
	virtual void cleanUp();
	Player* getUser() const;
	bool hasBrick(int x, int y);
	bool hasPermaBrick(int x, int y);
	bool hasDestructableBrick(int x, int y);
	void setDisplayText();
	Level getLevelObject() const;
	int Zumis() const;
	void decreaseZumis();
	void insert(Object* actor);
	bool isBrickDestroyed(int x, int y);
	bool hasBugSprayer(int x, int y);
	bool searchAndDestroyZumi(int x, int y);
	bool searchAndDestroyBrick(int x, int y);
	void insertDeadBricks(int x, int y);
	void exterminate(int x, int y);
	void setDone();
	bool searchMaze(Zumi*, int x, int y, int direction);
	void executeSearch(Zumi* bug);
	bool contains(int x, int y);
private:
	Player* user;
	Level l;
	int m_zumis;
	bool makeSound;
	int m_livesLeft;
	int m_bonus;
	int m_level;
	list<Object*>actors;
	list<Coordinates>deadBricks;
	queue<Coordinates>search;
	list<Coordinates> searched;
	int maze[15][15];
	bool m_finished;
};

#endif // STUDENTWORLD_H_