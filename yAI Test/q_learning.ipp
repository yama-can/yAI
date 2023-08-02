# pragma once
# include <iostream>
# include <yAI/yAI.hpp>

class Env : public Q_Learning::EnvBase<int, int>
{
public:
	Q_Learning::StepData<int> step(int state)
	{
		// 13‚Ì‚Æ‚«‚Ì‚İ•ñV‚ğ—^‚¦Adone‚·‚é
		if (state == 13)
		{
			return Q_Learning::StepData<int>(0, 1000, true);
		}

		return Q_Learning::StepData<int>(0, -1000, false);
	}
};

class Agent : public Q_Learning::AgentBase<int, int>
{
public:
	Agent()
		:Q_Learning::AgentBase<int, int>::AgentBase(14, 14, 0.3)
	{
	}

	bool isActable(int state, int action)
	{
		return true;
	}
};

inline void q_learning()
{
	Env env;
	Agent agent;

	int totalReward = 0;

	for (int i = 0; i < 100; i++)
	{
		int state = 0;
		int reward = 0;

		for (int j = 0; j < 100; i++)
		{
			const auto action = agent.chooseAction(state);
			if (action.has_value())
			{
				const auto data = env.step(*action);

				agent.learn(state, *action, data.nextState, data.reward);

				reward += data.reward;

				if (data.done) break;
				state = data.nextState;
			}
			else
			{
				break;
			}
		}

		// ’T‹—¦‚ğŒ¸‚ç‚·
		agent.epsilon *= 0.95;

		totalReward += reward;
	}
	std::cout << "Q_Learning TotalReward: " << totalReward << std::endl;
}
