#pragma once
#include <string>
class Object
{
public:
	Object();
	~Object();
	void Initialize(std::string p_objName);
	void Update();
	void Shutdown();

private:
	void LoadObj();
	std::string m_objectName;
};

