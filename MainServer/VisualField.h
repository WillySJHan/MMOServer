#pragma once
class VisualField : public std::enable_shared_from_this<VisualField>
{
public:
	VisualField(std::shared_ptr<class Player> player);
	~VisualField();

	std::unordered_set<std::shared_ptr<class BaseObject>> SurroundingObjects();

	void Update();

public:
	std::weak_ptr<class Player> _owner;
	std::unordered_set<std::shared_ptr<class BaseObject>> _previousObjects;
	std::shared_ptr<class Task> _task;
};

