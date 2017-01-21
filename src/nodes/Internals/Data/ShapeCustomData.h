#pragma once

#include "Box2D.h"

public class ShapeCustomData
{
public:
	ShapeCustomData(void);
	~ShapeCustomData(void);
	int Id;
	char* Custom;
	bool MarkedForDeletion;
	bool MarkedForUpdate;
	b2ShapeDef* NewShape;
	double LifeTime;

};
