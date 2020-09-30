#pragma once
#include "Enums.h"
class Prioritizer
{
private:
	conflict_action share_priority = none;
	conflict_action compliance_priority = none;
public:
	Prioritizer(conflict_action sp, conflict_action cp);
	~Prioritizer();

	std::pair<conflict_action, conflict_action> get_priority()
	{
		return std::make_pair(share_priority, compliance_priority);
	}
	void set_priority(conflict_action sp, conflict_action cp)
	{
		share_priority = sp;
		compliance_priority = cp;
	}
};

Prioritizer::Prioritizer(conflict_action sp, conflict_action cp)
{
	set_priority(sp, cp);
}

Prioritizer::~Prioritizer()
{
}