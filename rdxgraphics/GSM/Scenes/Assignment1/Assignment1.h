#pragma once
#include "GSM/BaseScene.h"

class SphereXSphereScene : public BaseScene
{
	RX_SCENE_DEFAULT(SphereXSphereScene);
public:
	void StartImpl() override;
};

class AABBXSphereScene : public BaseScene
{
	RX_SCENE_DEFAULT(AABBXSphereScene);
public:
	void StartImpl() override;
};

class AABBXAABBScene : public BaseScene
{
	RX_SCENE_DEFAULT(AABBXAABBScene);
public:
	void StartImpl() override;
};

class PointXSphereScene : public BaseScene
{
	RX_SCENE_DEFAULT(PointXSphereScene);
public:
	void StartImpl() override;
};

class PointXAABBScene : public BaseScene
{
	RX_SCENE_DEFAULT(PointXAABBScene);
public:
	void StartImpl() override;
};

class PointXTriangleScene : public BaseScene
{
	RX_SCENE_DEFAULT(PointXTriangleScene);
public:
	void StartImpl() override;
};

class PointXPlaneScene : public BaseScene
{
	RX_SCENE_DEFAULT(PointXPlaneScene);
public:
	void StartImpl() override;
};

class RayXSphereScene : public BaseScene
{
	RX_SCENE_DEFAULT(RayXSphereScene);
public:
	void StartImpl() override;
};

class RayXAABBScene : public BaseScene
{
	RX_SCENE_DEFAULT(RayXAABBScene);
public:
	void StartImpl() override;
};

class RayXTriangleScene : public BaseScene
{
	RX_SCENE_DEFAULT(RayXTriangleScene);
public:
	void StartImpl() override;
};

class RayXPlaneScene : public BaseScene
{
	RX_SCENE_DEFAULT(RayXPlaneScene);
public:
	void StartImpl() override;
};

class PlaneXAABBScene : public BaseScene
{
	RX_SCENE_DEFAULT(PlaneXAABBScene);
public:
	void StartImpl() override;
};

class PlaneXSphereScene : public BaseScene
{
	RX_SCENE_DEFAULT(PlaneXSphereScene);
public:
	void StartImpl() override;
};
