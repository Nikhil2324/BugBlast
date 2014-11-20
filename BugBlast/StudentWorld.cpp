#include "StudentWorld.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include "GameWorld.h"
#include <algorithm>
#include <queue>

using namespace std;

GameWorld* createStudentWorld()
{
	return new StudentWorld();
}

StudentWorld::StudentWorld()
{
	user = NULL;
	m_zumis = 0;
	makeSound = true;
	m_finished = false;
}

void StudentWorld::insertDeadBricks(int x, int y)
{
	deadBricks.push_back(Coordinates(x, y)); //Inserts Coordinates of dead bricks into list
}

void StudentWorld::insert(Object* actor)
{
	actors.push_back(actor); //Inserts new actors into list
}

void StudentWorld::setDone()
{
	m_finished = true; //Sets level to finished
}

StudentWorld::~StudentWorld()
{
	int size = actors.size();
	list<Object*>::iterator iter;
	for (iter = actors.begin(); iter != actors.end(); iter++)
		delete *iter; //Deallocates all dyamically allocated objects in list

	actors.clear(); //Clears list
	delete user; //Deallocates player pointers

	if (deadBricks.empty() == false)
		deadBricks.clear(); //Clears list if has elements
	while (search.empty() == false)
		search.pop(); //Clears queue if it has elements

	if (searched.empty() == false)
		searched.clear(); //Clears list if it has elements
}

Level StudentWorld::getLevelObject() const
{
	return l;
}

Player* StudentWorld::getUser() const
{
	return user;
}

int StudentWorld::Zumis() const
{
	return m_zumis;
}

void StudentWorld::cleanUp() //Exact same as destructor
{
	int size = actors.size();
	list<Object*>::iterator iter;
	for (iter = actors.begin(); iter != actors.end(); iter++)
		delete *iter;

	actors.clear();
	delete user;

	if (deadBricks.empty() == false)
		deadBricks.clear();

	while (search.empty() == false)
		search.pop();

	if (searched.empty() == false)
		searched.clear();
}

bool StudentWorld::hasBrick(int x, int y) //Checks level file to see if the map started with a brick in this coordinate
{
	if (l.getContentsOf(x, y) == 5 || l.getContentsOf(x, y) == 6)
		return true;
	return false;
}

bool StudentWorld::hasDestructableBrick(int x, int y) //checks level file to see if map started with destructable brick in this coordinate
{
	if (l.getContentsOf(x, y) == 6)
		return true;
	return false;
}

bool StudentWorld::hasPermaBrick(int x, int y) //checks level file to see if map started with permanent brick in this coordinate
{
	if (l.getContentsOf(x, y) == 5)
		return true;
	return false;
}

bool StudentWorld::searchMaze(Zumi* bug, int x, int y, int direction)
{
	int count = 0;

	while (search.empty() == false)
		search.pop(); //Empties queue from previous search (if there was one)

	if (searched.empty() == false) 
		searched.clear(); //Empties list from previous search (if there was one)
	
	search.push(Coordinates(x, y)); //Pushes starting coordinates onto queue
	while (search.empty() == false)
	{
		int xc = search.front().X();
		int yc = search.front().Y();
		int dir = search.front().direction();

		if (xc == user->getX() && yc == user->getY())
			return true; //If queues front coordinates equal players, return true

		search.pop(); 

		if (bug->canZumiMove(xc, yc + 1) && contains(xc, yc + 1) == false) //If the zumi can move north and it hasn't been searched yet
		{
			if (count == 0)
				search.push(Coordinates(xc, yc + 1, KEY_PRESS_UP)); //If this is the first move in this direction, push its direction and coordinates
			else
				search.push(Coordinates(xc, yc + 1, dir)); //Otherwise, push only coordinates, keep directions as before
			searched.push_back(Coordinates(xc, yc + 1)); //Mark coordinate as visited
		}

		if (bug->canZumiMove(xc, yc - 1) && contains(xc, yc - 1) == false) //Same as above except for south
		{
			if (count == 0)
				search.push(Coordinates(xc, yc - 1, KEY_PRESS_DOWN));
			else
				search.push(Coordinates(xc, yc - 1, dir));
			searched.push_back(Coordinates(xc, yc - 1));
		}

		if (bug->canZumiMove(xc - 1, yc) && contains(xc - 1, yc) == false) //Same as above except for west
		{
			if (count == 0)
				search.push(Coordinates(xc - 1, yc, KEY_PRESS_LEFT));
			else
				search.push(Coordinates(xc - 1, yc, dir));			
			searched.push_back(Coordinates(xc - 1, yc));
		}

		if (bug->canZumiMove(xc+1, yc) && contains(xc + 1, yc) == false) //Same as above except for east
		{
			if (count == 0)
				search.push(Coordinates(xc + 1, yc, KEY_PRESS_RIGHT));
			else
				search.push(Coordinates(xc + 1, yc, dir));
			searched.push_back(Coordinates(xc + 1, yc));
		}
		count++;
	}
	return false; //If queue empty, means can't reach player
}

bool StudentWorld::contains(int x, int y)
{
	list<Coordinates>::iterator p;
	for (p = searched.begin(); p != searched.end(); p++) //Use iterator to search through list of searched items, 
	{
		if ((*p).X() == x && (*p).Y() == y) //if coordinates the same, it has been searched
			return true;
	}
	return false;
}

void StudentWorld::executeSearch(Zumi* bug)
{
	if (search.empty() == true) 
		return;
	if (search.front().direction() == KEY_PRESS_DOWN) //If direction in front of queue is down, move bug down
		bug->moveTo(bug->getX(), bug->getY() - 1);
	else if (search.front().direction() == KEY_PRESS_UP) //Same as above, but up
		bug->moveTo(bug->getX(), bug->getY() + 1);
	else if (search.front().direction() == KEY_PRESS_LEFT) //Same as above, but left
		bug->moveTo(bug->getX() - 1, bug->getY());
	else if (search.front().direction() == KEY_PRESS_RIGHT) //Same as above, but right
		bug->moveTo(bug->getX() + 1, bug->getY());
	bug->setDirection(search.front().direction()); //Set direction to new direction moving, so if player moves out of scope, current direction stays same
	search.pop(); //Pop this from queue
}

void StudentWorld::setDisplayText()
{	
	ostringstream line;
	line.fill('0');
	line << "Score: " << setw(8) << getScore() << "  " << "Level: " << setw(2) << getLevel() << "  " << "Lives: " << setw(3) << getLives();
	line.fill(' ');
	line << "  " << "Bonus: " << setw(6) << m_bonus;
	string s = line.str();
	setGameStatText(s);
	m_bonus--;
}

int StudentWorld::init()
{
	m_finished = false; //Level not finished
	m_zumis = 0; //No zumis
	makeSound = true; //Make sound if exit revealed
	ostringstream level;
	level.fill('0');
	level << "level" << setw(2) << GameWorld::getLevel() << ".dat";
	string currentLevel = level.str();
	//Level::LoadResult result = l.loadLevel(currentLevel); //Load current level
	Level::LoadResult result = l.loadLevel("level02.dat");// DEBUGGING PURPOSES
	m_bonus = l.getOptionValue(optionLevelBonus);

	if (getLevel() == 0 && result == Level::load_fail_file_not_found) //If level 0 and can't find level
		return GWSTATUS_NO_FIRST_LEVEL;

	if (getLevel() != 0 && result == Level::load_fail_file_not_found) //If level not 0 and can't find level
		return GWSTATUS_PLAYER_WON;

	if (result == Level::load_fail_bad_format) //If level file of bad format
		return GWSTATUS_LEVEL_ERROR;
	
	for (int k = 0; k < 15; k++)
	{
		for (int j = 0; j < 15; j++)
		{
			Level::MazeEntry item = l.getContentsOf(k, j); //Get contents of file, and create new object depending on what is read, insert into list of actors
			if (item == Level::player)
				user = new Player(k, j, this);

			if (item == Level::destroyable_brick)
				actors.push_back(new destroyableBrick(k, j, this));

			if (item == Level::perma_brick)
				actors.push_back(new Brick(IID_PERMA_BRICK, k, j, this));
			
			if (item == Level::exit)
				actors.push_back(new Exit(k, j, this));
			
			if (item == Level::simple_zumi)
			{
				int value = l.getOptionValue(optionTicksPerSimpleZumiMove);
				actors.push_back(new Zumi(IID_SIMPLE_ZUMI, k, j, this, value));
				m_zumis++;
			}

			if (item == Level::complex_zumi)
			{
				int value = l.getOptionValue(optionTicksPerComplexZumiMove);
				actors.push_back(new ComplexZumi(IID_COMPLEX_ZUMI, k, j, this, value));
				m_zumis++;
			}
		}
	}
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::exterminate(int x, int y)
{
	list<Object*>::iterator p;
	for (p = actors.begin(); p != actors.end(); ++p) //Use iterator to look through entire actor array
	{	
		destroyableBrick* dp = NULL;
		Zumi* zp = NULL;
		BugSprayer* bp = NULL;

		dp = dynamic_cast<destroyableBrick*>(*p); 
		if (dp != NULL) //If actor is a destructable brick
		{
			if (dp->getX() == x && dp->getY() == y) //If coordinates are the same
			{
				if (isBrickDestroyed(x, y) == false) //If this brick is alive
					dp->kill(); //Kill it
			}
		}
			
		zp = dynamic_cast<Zumi*>(*p); 
		if (zp != NULL) //If the actor is a zumi
		{
			if (zp->getX() == x && zp->getY() == y && zp->isDead() == false) //If the coordinates are the same and it is alive
				zp->kill(); //Kill it
		}
		
		bp = dynamic_cast<BugSprayer*>(*p); 
		if (bp != NULL) //If the actor is a BugSprayer
		{
			if (bp->getX() == x && bp->getY() == y) //If its coordinates are the same
			{
				if (bp->isDead() == false) //If its alive
					bp->kill(); //Kill it
			}
		}
			
		bp = NULL;
		dp = NULL;
		zp = NULL;
	}
}

bool StudentWorld::searchAndDestroyZumi(int x, int y)
{
	list<Object*>::iterator p;
	Zumi* zp = NULL;

	for (p = actors.begin(); p != actors.end(); ++p) 
	{	
		zp = dynamic_cast<Zumi*>(*p);
		if (zp != NULL) //If actor is a zumi
			if (zp->getX() == user->getX() && zp->getY() == user->getY()) //If coordinates are same
					return true;
		zp = NULL;
	}
	return false;
}

bool StudentWorld::searchAndDestroyBrick(int x, int y)
{
	list<Object*>::iterator p;
	destroyableBrick* dp = NULL;

		for (p = actors.begin(); p != actors.end(); ++p)
		{
			dp = dynamic_cast<destroyableBrick*>(*p); 
			if (dp != NULL && dp->isDead() == false) //If actor is a brick still on the map
				if (user->getX() == (*p)->getX() && user->getY() == (*p)->getY()) //If coordinates are the same
						return true;
			dp = NULL;
		}
		return false;
}

void StudentWorld::decreaseZumis()
{
	m_zumis--;
}

bool StudentWorld::isBrickDestroyed(int x, int y)
{
	if (deadBricks.size() == 0)
		return false;
	list<Coordinates>::iterator iter;
	for (iter = deadBricks.begin(); iter != deadBricks.end(); ++iter)
		if (x == (*iter).X() && y == (*iter).Y()) //If coordinates of destroyed brick are same as current coordinates
			return true;
	return false;
}

bool StudentWorld::hasBugSprayer(int x, int y)
{
	list<Object*>::iterator p;
	BugSprayer* bp = NULL;
	for (p = actors.begin(); p != actors.end(); ++p)
	{	
			bp = dynamic_cast<BugSprayer*>(*p);
			if (bp != NULL) //If actor is a bugsprayer
				if (bp->getX() == x && bp->getY() == y) //If coordinates are equal
					return true;
			bp = NULL;
	}
	return false;
}

int StudentWorld::move()
{
	setDisplayText(); //Set display text on screen
	user->doSomething(); //Player does something
	
	if (m_finished == true) //If player beat level
	{
		increaseScore(m_bonus); //Increase score by the bonus
		return GWSTATUS_FINISHED_LEVEL;
	}
		
	list<Object*>::iterator p;
	
	for (p = actors.begin(); p != actors.end(); ++p)
	{	
		if (!(*p)->isDead()) //If the actor is not dead, it does something
			(*p)->doSomething();
	}

	if (user->isDead()) //If player is dead, decrement lives
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	list<Object*>::iterator iter;
	for (iter = actors.begin() ; iter != actors.end();) 
	{
		if ((*iter)->isDead()) //If actor is dead
		{
			delete *iter; //delete its data
			iter = actors.erase(iter); //Validate iterator
		}
		else
			++iter;
	}

	return GWSTATUS_CONTINUE_GAME;
}