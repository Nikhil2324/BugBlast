#include "Actor.h"
#include <iostream>
#include "StudentWorld.h"
#include <vector>
#include <algorithm>

using namespace std;

Object::Object(int id, int x, int y, StudentWorld* s)
	:GraphObject(id, x, y)
{
	setVisible(true); //All objects start visible
	m_s = s; //Student world pointer validated
	m_dead = false; //all objects start alive
}

bool Object::isDead() const
{
	return m_dead;
}

void Object::kill()
{
	m_dead = true;
}

Object::~Object()
{}

StudentWorld* Object::getWorld()
{
	return m_s;
}

Brick::Brick(int id, int x, int y, StudentWorld* s)
	:Object(id, x, y, s)
{}

void Brick::doSomething()
{}

destroyableBrick::destroyableBrick(int x, int y, StudentWorld* s)
	:Brick(IID_DESTROYABLE_BRICK, x, y, s)
{}

void destroyableBrick::kill()
{
	getWorld()->insertDeadBricks(getX(), getY()); //Insert the now brick's coordinates nto the list in StudentWorld
	Object::kill();
}

Exit::Exit(int x, int y, StudentWorld* s)
	:Object(IID_EXIT, x, y, s)
{
	setVisible(false); //Starts out invisible
	m_visible = false;
}

void Exit::doSomething() 
{
	if (getWorld()->Zumis() <= 0 && m_visible == false) //If there are no zumis and its invisible
	{
		setVisible(true);
		getWorld()->playSound(SOUND_REVEAL_EXIT);
		m_visible = true;
	}

	if (getX() == getWorld()->getUser()->getX() && getY() == getWorld()->getUser()->getY() && m_visible == true)
	{
		getWorld()->playSound(SOUND_FINISHED_LEVEL);
		getWorld()->setDone(); //Tell studentWorld that level is finished
	}
	return;
}

BugSprayer::BugSprayer(int x, int y, StudentWorld* s)
	:Object(IID_BUGSPRAYER, x, y, s)
{
	m_lifetime = 40; //Bugsprayers start out with life of 40
}

void BugSprayer::doSomething()
{
	if (isDead())
		return;
	m_lifetime--;
	if (m_lifetime <= 0)
		kill();
	return;
}

void BugSprayer::kill()
{
		getWorld()->insert(new BugSpray(getX(), getY(), getWorld())); //CURRENT LOCATION
		for (int k = 1; k <= 2; k++)//WEST
		{
			if (getWorld()->hasPermaBrick(getX() - k, getY())) //If there is a permanent brick here, can't spread
				break;
			else if (getWorld()->hasDestructableBrick(getX() - k, getY()) && getWorld()->isBrickDestroyed(getX() - k, getY()) == false) //If there is a destructable brick here, put bugspray here, doesn't spread further
			{
				getWorld()->insert(new BugSpray(getX() - k, getY(), getWorld()));
				break;
			}
			else
				getWorld()->insert(new BugSpray(getX() - k, getY(), getWorld())); //Spread the bugspray
		}

		for (int k = 1; k <= 2; k++) 
		{
			if (getWorld()->hasPermaBrick(getX() + k, getY())) //EAST
				break;
			else if (getWorld()->hasDestructableBrick(getX() + k, getY()) && getWorld()->isBrickDestroyed(getX() + k, getY()) == false)
			{
				getWorld()->insert(new BugSpray(getX() + k, getY(), getWorld()));
				break;
			}
			else
				getWorld()->insert(new BugSpray(getX() + k, getY(), getWorld()));
		}

		for (int k = 1; k <= 2; k++)
		{
			if (getWorld()->hasPermaBrick(getX(), getY() - k)) //SOUTH
				break;
			else if (getWorld()->hasDestructableBrick(getX(), getY() - k) && getWorld()->isBrickDestroyed(getX(), getY() - k) == false)
			{
				getWorld()->insert(new BugSpray(getX(), getY() - k, getWorld()));
				break;
			}
			else
				getWorld()->insert(new BugSpray(getX(), getY() - k, getWorld()));
		}

		for (int k = 1; k <= 2; k++)
		{
			if (getWorld()->hasPermaBrick(getX(), getY() + k)) //NORTH
				break;
			else if (getWorld()->hasDestructableBrick(getX(), getY() + k) && getWorld()->isBrickDestroyed(getX(), getY() + k) == false)
			{
				getWorld()->insert(new BugSpray(getX(), getY() + k, getWorld()));
				break;
			}
			else
				getWorld()->insert(new BugSpray(getX(), getY() + k, getWorld()));
		}

	getWorld()->playSound(SOUND_SPRAY);
	m_lifetime = 0;
	Object::kill();
	getWorld()->getUser()->decrementSprayers();
}

Zumi::Zumi(int id, int x, int y, StudentWorld* s, int ticksvalue)
	:Object(id, x, y, s)
{
	m_ticksPerMove = ticksvalue; //ticks per zumi moved determined by parameter
	m_direction = randInt(KEY_PRESS_LEFT, KEY_PRESS_DOWN); //CURRENT DIRECTION IS RANDOMLY LEFT, RIGHT, DOWN, OR UP
	m_ticks = 0;
}

bool Zumi::canZumiMove(int x, int y)
{
	if (((getWorld()->hasBrick(x, y) == false) || (getWorld()->hasDestructableBrick(x, y) && getWorld()->isBrickDestroyed(x, y))) && getWorld()->hasBugSprayer(x, y) == false)
		return true; //If (the x, y coordinate did not have a brick initially OR the x, y coordinate had a destructible brick that was destroyed) AND the x, y coordinate doesn’t have a bugsprayer

	return false;
}

bool Zumi::turnToMove()
{
	if (m_ticks != 0 && m_ticks % m_ticksPerMove == 0) //If the number of ticks passed is divisble by the ticks per move, it can move
	{
		m_ticks++; 
		return true;
	}
	m_ticks++; //regardless, increase the number of ticks
	return false;
}

void Zumi::simpleMove()
{
		switch(m_direction) //Same idea for all directions
		{
		case (KEY_PRESS_LEFT):
			if (canZumiMove(getX()-1, getY())) //If the zumi can move in the current direction, do so
				moveTo(getX() - 1, getY());
			else
			{
				m_ticks = 0; //Otherwise, set ticks passed to 0
				m_direction = randInt(KEY_PRESS_LEFT, KEY_PRESS_DOWN); //Pick new random direction
			}
			break;

		case (KEY_PRESS_RIGHT):
			if (canZumiMove(getX() + 1, getY()))
				moveTo(getX() + 1, getY());
			else
			{
				m_ticks = 0;
				m_direction = randInt(KEY_PRESS_LEFT, KEY_PRESS_DOWN);
			}
			break;

		case (KEY_PRESS_DOWN):
			if (canZumiMove(getX(), getY() - 1))
				moveTo(getX(), getY() - 1);
			else
			{
				m_ticks = 0;
				m_direction = randInt(KEY_PRESS_LEFT, KEY_PRESS_DOWN);
			}
			break;

		case (KEY_PRESS_UP):
			if (canZumiMove(getX(), getY() + 1))
				moveTo(getX(), getY() + 1);
			else
			{
				m_ticks = 0;
				m_direction = randInt(KEY_PRESS_LEFT, KEY_PRESS_DOWN);
			}
			break;
		}
}

void Zumi::doSomething()
{
	if (isDead())
		return;
	if (getX() == getWorld()->getUser()->getX() && getY() == getWorld()->getUser()->getY()) //If Zumi's coordinate same as players
		getWorld()->getUser()->kill(); //kill the user
	if (turnToMove()) //If its the time to move, make it move simply
		simpleMove();
	
	return;
}

void Zumi::kill()
{
	getWorld()->decreaseZumis();
	getWorld()->playSound(SOUND_ENEMY_DIE);
	getWorld()->increaseScore(100);

	int prob = getWorld()->getLevelObject().getOptionValue(optionProbOfGoodieOverall); //Get probabilty of dropping a goodie
	int actual = randInt(0, 99); //Get random number between 0 and 99
	
	if (actual < prob) //If probability greater than random number
	{
		int rand = randInt(0, 99); //Get new random number
		int probWalkThroughWalls = getWorld()->getLevelObject().getOptionValue(optionProbOfWalkThruGoodie);  //Probability of walking through walls
		int probExtraLife = getWorld()->getLevelObject().getOptionValue(optionProbOfExtraLifeGoodie); //Probability of extra life
		int probMoreSprayers = getWorld()->getLevelObject().getOptionValue(optionProbOfMoreSprayersGoodie); //Probability of more sprayers

		if (rand < probWalkThroughWalls) //If only between 0 and walkthroughwalls
			getWorld()->insert(new WalkWalls(getX(), getY(), getWorld())); //create walkthroughwalls goodie
		else if (rand < probWalkThroughWalls + probExtraLife) //If between walkthroughwalls and extralife
			getWorld()->insert(new ExtraLife(getX(), getY(), getWorld())); //create extralife goodie
		else
			getWorld()->insert(new MoreSprayers(getX(), getY(), getWorld())); //Otherwise, create moreSprayers goodie
	}
	
	Object::kill();
}

int Zumi::randInt(int lowest, int highest)
{
    if (highest < lowest)
        swap(highest, lowest);
    return lowest + (rand() % (highest - lowest + 1)); //generic random number formula
}

void Zumi::setDirection(int direction)
{
	if (direction != KEY_PRESS_DOWN && direction != KEY_PRESS_LEFT && direction != KEY_PRESS_RIGHT && direction != KEY_PRESS_UP) //If direction is invalid
		return;
	m_direction = direction; //Set direction
}

ComplexZumi::ComplexZumi(int id, int x, int y, StudentWorld* s, int ticksPerMove)
	:Zumi(id, x, y, s, ticksPerMove)
{}

void ComplexZumi::doSomething()
{
	if (isDead())
		return;

	if (getX() == getWorld()->getUser()->getX() && getY() == getWorld()->getUser()->getY())
		getWorld()->getUser()->kill();
	
	if (turnToMove())
	{
		int horizontal = abs(getX() - getWorld()->getUser()->getX()); //Horizontal distance between player and zumi
		int vertical = abs(getY() - getWorld()->getUser()->getY());//Vertical distance between player and zumi
		int smellDistance = getWorld()->getLevelObject().getOptionValue(optionComplexZumiSearchDistance); //Distance a zumi can search
		if (smellDistance >= vertical && smellDistance >= horizontal) //If within smelling distance
		{
			if (getWorld()->searchMaze(this, getX(), getY(), -1)) //Look through maze to see if Zumi can reach player
				getWorld()->executeSearch(this); //If so, step in that direction to shortest path
			else
				Zumi::simpleMove(); //If not, move like a simple zumi
		}
		else
			Zumi::simpleMove(); //If not, move like a simple zumi
	}
}

void ComplexZumi::kill()
{
	getWorld()->increaseScore(400); //Increase score by 400 (in addition to 100 by killing regular zumi)
	Zumi::kill();
}

Player::Player(int x, int y, StudentWorld* s)
	:Object(IID_PLAYER, x, y, s)
{
	m_sprayers = 0;
	m_ethereal = false;
	m_SprayerLife = 0;
	m_WallLife = 0;
	max_sprayers = 2;
}

bool Player::canPlayerMove(int x, int y)
{
	if ((getWorld()->hasBrick(x, y) == false) ||
		(getWorld()->hasDestructableBrick(x, y) && getWorld()->isBrickDestroyed(x, y)) ||
		(m_ethereal == true && getWorld()->hasPermaBrick(x, y) == false))
		//(If the coordinate did not have a brick there OR the coordinate had a destroyable brick that has been destroyed) 
		//OR (the player can walk through destructible bricks
		//AND this coordinate has a destructible brick)
		return true;
	return false;
}

void Player::doSomething()
{
	int input;
	if (isDead())
		return;

	if(getWorld()->searchAndDestroyZumi(getX(), getY())) //If coordinates are same as a zumi's
		kill();

		if (m_ethereal == true) //If can walk through walls, decrease time it can do so
			m_WallLife--;

		if (m_WallLife == 0)
			m_ethereal = false;

		if (m_SprayerLife > 0) //If can drop more than 2 sprayers, decrease time it can do so
			m_SprayerLife--;
		if (m_SprayerLife <= 0)
			max_sprayers = 2;
		
	if (m_ethereal == false) //If can't walk through walls
	{
		if(getWorld()->searchAndDestroyBrick(getX(), getY())) //If it is in one right now
			kill(); //Kill the player
	}	

	if (getWorld()->getKey(input)) //Get input from player
	{
		switch (input)
		{
		case KEY_PRESS_UP: //move up, down, left, or right, if it can
			if (canPlayerMove(getX(), getY() + 1))
				GraphObject::moveTo(getX(), getY() + 1);
			break;
		case KEY_PRESS_DOWN:
			if (canPlayerMove(getX(), getY() - 1))
				GraphObject::moveTo(getX(), getY() - 1);
			break;
		case KEY_PRESS_LEFT:
			if (canPlayerMove(getX() - 1, getY()))
				GraphObject::moveTo(getX() - 1, getY());
			break;
		case KEY_PRESS_RIGHT:
			if (canPlayerMove(getX() + 1, getY()))
				GraphObject::moveTo(getX() + 1, getY());
			break;
		case KEY_PRESS_SPACE: //If hits space, and sprayers on board less than max, and doesn't have a bugsprayer on the space currently, and it either didn't have a brick or it had a destructable brick that has been destroyed
			if (m_sprayers < max_sprayers && getWorld()->hasBugSprayer(getX(),getY()) == false && (getWorld()->hasBrick(getX(), getY()) == false || getWorld()->hasDestructableBrick(getX(), getY()) && getWorld()->isBrickDestroyed(getX(), getY())))
			{
				getWorld()->insert(new BugSprayer(getX(), getY(), getWorld())); //Place a bugsprayer on this space
				m_sprayers++;
			}
			break;
		}
	}
	return;
}

void Player::setJesus() //Can walk through walls
{
	m_ethereal = true;
	m_WallLife = getWorld()->getLevelObject().getOptionValue(optionWalkThruLifetimeTicks);
}

void Player::decrementSprayers()
{
	if (m_sprayers > 0)
		m_sprayers--;
}

void Player::kill()
{
	Object::kill();
	getWorld()->playSound(SOUND_PLAYER_DIE);
}

void Player::increaseSprayers()
{
	max_sprayers = getWorld()->getLevelObject().getOptionValue(optionMaxBoostedSprayers);
	m_SprayerLife = getWorld()->getLevelObject().getOptionValue(optionBoostedSprayerLifetimeTicks);
}

int Player::getMaxSprayers() const
{
	return max_sprayers;
}

int Player::getSprayers() const
{
	return m_sprayers;
}

BugSpray::BugSpray(int x, int y, StudentWorld* sw)
	:Object(IID_BUGSPRAY, x, y, sw)
{
	m_ticks = 3;
}

void BugSpray::doSomething()
{
	if (isDead())
		return;

	m_ticks--;

	if (m_ticks <= 0)
		kill();

	else
	{
		if (getX() == getWorld()->getUser()->getX() && getY() == getWorld()->getUser()->getY()) //If coordinates are same as players, kill playe
			getWorld()->getUser()->kill();

		getWorld()->exterminate(getX(), getY()); //If coordinates are same as zumi, BugSprayer, or destructable brick, kill whichever it is
	}
	return;
}

Goodie::Goodie(int id, int x, int y, StudentWorld* sw)
	:Object(id, x, y, sw)
{
	m_lifetime = getWorld()->getLevelObject().getOptionValue(optionGoodieLifetimeInTicks); //How long it stays on the board for
}

int Goodie::getLife() const
{
	return m_lifetime;
}

void Goodie::decrementLife()
{
	m_lifetime--;
}

ExtraLife::ExtraLife(int x, int y, StudentWorld* sw)
	:Goodie(IID_EXTRA_LIFE_GOODIE, x, y, sw)
{}

WalkWalls::WalkWalls(int x, int y, StudentWorld* sw)
	:Goodie(IID_WALK_THRU_GOODIE, x, y, sw)
{}

MoreSprayers::MoreSprayers(int x, int y, StudentWorld* sw)
	:Goodie(IID_INCREASE_SIMULTANEOUS_SPRAYER_GOODIE, x, y, sw)
{}

void ExtraLife::doSomething()
{
	if (isDead())
		return;
	decrementLife();
	if (getLife() <= 0)
		kill();
	else
	{
		if (getX() == getWorld()->getUser()->getX() && getY() == getWorld()->getUser()->getY()) //If coordinates same as user
		{
			getWorld()->incLives(); //increase lives by 1
			getWorld()->increaseScore(1000);
			kill();
			getWorld()->playSound(SOUND_GOT_GOODIE);
		}
	}
	return;
}

void WalkWalls::doSomething()
{
	if (isDead())
		return;
	decrementLife();
	if (getLife() <= 0)
		kill();
	else
	{
		if (getX() == getWorld()->getUser()->getX() && getY() == getWorld()->getUser()->getY()) //If coordinates same as user
		{
			getWorld()->getUser()->setJesus(); //Player can now walk through destructible bricks
			getWorld()->increaseScore(1000);
			kill();
			getWorld()->playSound(SOUND_GOT_GOODIE);
		}
	}
}

void MoreSprayers::doSomething()
{
	if (isDead())
		return;
	decrementLife();
	if (getLife() <= 0)
		kill();
	else
	{
		if (getX() == getWorld()->getUser()->getX() && getY() == getWorld()->getUser()->getY()) //If coordinates same as user
		{
			getWorld()->getUser()->increaseSprayers(); //Player can now drop more sprayers
			getWorld()->increaseScore(1000);
			kill();
			getWorld()->playSound(SOUND_GOT_GOODIE);
		}
	}
}