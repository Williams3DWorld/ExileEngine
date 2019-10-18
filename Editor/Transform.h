#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "Headers.h"


class Transform
{
public:
	struct Data
	{
		glm::vec3 p;
		glm::vec3 r;
		glm::vec3 s;

		glm::mat4 m;

		Data() : p(glm::vec3(0.0f)), r(glm::vec3(0.0f)), s(glm::vec3(1.0f)), m(glm::mat4(1.0f)) {}
	};

	static void MakeModel(Data& x)
	{
		x.m = glm::scale(x.m, glm::vec3(x.s));
		x.m = glm::rotate(x.m, x.r.x, glm::vec3(1.0f, 0.0f, 0.0f));
		x.m = glm::rotate(x.m, x.r.y, glm::vec3(0.0f, 1.0f, 0.0f));
		x.m = glm::rotate(x.m, x.r.z, glm::vec3(0.0f, 0.0f, 1.0f));
		x.m = glm::translate(x.m, glm::vec3(x.p));
	}

	static void Translate(Data& d, glm::vec3 x)
	{
		d.p = x;
		MakeModel(d);
	}

	static void RotateLocalX(Data& d, float x)
	{
		d.r.x = x;
		MakeModel(d);
	}

	static void RotateLocalY(Data& d, float x)
	{
		d.r.y = x;
		MakeModel(d);
	}

	static void RotateLocalZ(Data& d, float x)
	{
		d.r.z = x;
		MakeModel(d);
	}

	static void RotateWorldX(Data& d, float x)
	{

	}

	static void RotateWorldY(Data& d, float x)
	{

	}

	static void RotateWorldZ(Data& d, float x)
	{

	}

	static void Scale(Data& d, glm::vec3 x)
	{
		d.s = x;
		MakeModel(d);
	}

	static void ScaleX(Data& d, float x)
	{
		d.s.x = x;
		MakeModel(d);
	}

	static void ScaleY(Data& d, float x)
	{
		d.s.y = x;
		MakeModel(d);
	}

	static void ScaleZ(Data& d, float x)
	{
		d.s.z = x;
		MakeModel(d);
	}
};



#endif