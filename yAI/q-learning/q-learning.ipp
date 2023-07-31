# pragma once
# include <set>
# include <map>
# include <random>
# include <iomanip>
# include <optional>

namespace SimpleAI
{
	namespace Q_Learning
	{
		using namespace std;

		typedef long double Number;

		template<typename T> concept Stateable = requires(T a, T b) {
			a == b;
			a++;
			a--;
			a > b;
			a < b;
		};

		template<typename T> concept Actionable = Stateable<T>;

		template<Stateable T>
		struct StepData
		{
			StepData(T nextState, Number reward, bool done)
				:nextState{ nextState }, reward{ reward }, done{ done }
			{
			}

			T nextState;
			Number reward;
			bool done;
		};
		
		/// <summary>
		/// Q-Learning環境
		/// </summary>
		/// <typeparam name="stateType">Stateの型</typeparam>
		/// <typeparam name="actionType">Actionの型</typeparam>
		template<Stateable stateType, Actionable actionType>
		class EnvBase
		{
		public:
			EnvBase()
			{
			}

			/// <summary>
			/// Actionから次のStateや報酬、終了したかどうかを取得します。
			/// </summary>
			/// <param name="action">Action</param>
			/// <returns>報酬などのデータ</returns>
			virtual StepData<stateType> step(actionType action) = 0;
		};

		/// <summary>
		/// エージェント(Q-Learning環境を操作するためのもの)
		/// </summary>
		/// <typeparam name="stateType"></typeparam>
		/// <typeparam name="actionType"></typeparam>
		template<Stateable stateType, Actionable actionType>
		class AgentBase
		{
		public:
			typedef map<stateType, map<actionType, Number>> tableType;

			/// <summary>
			/// エージェントを作成します。
			/// </summary>
			/// <param name="numStates">stateの数</param>
			/// <param name="numActions">actionの数</param>
			/// <param name="epsilon">探求する確率</param>
			/// <param name="learningRate">学習率(この値が高いと学習結果が固定化し、柔軟な対応ができなくなります。)</param>
			/// <param name="discountFactor">割引率(報酬の重要度)</param>
			AgentBase(unsigned int numStates, unsigned int numActions, Number epsilon = 0.25, Number learningRate = 0.1, Number discountFactor = 0.99)
				:numStates{ numStates },
				numActions{ numActions },
				epsilon{ epsilon },
				learningRate{ learningRate },
				discountFactor{ discountFactor },
				rndGen{ random_device() },
				rndCount{ 0 }
			{
				rnd = mt19937(rndGen());

				stateType state{};
				for (unsigned int i = 0; i < numStates; i++)
				{
					actionType action{};
					for (unsigned int j = 0; j < numActions; j++)
					{
						qTable[state][action] = 0;
						action++;
					}
					state++;
				}
			}

			/// <summary>
			/// 現在学習されたデータからactionを選択します。
			/// </summary>
			/// <param name="state">現在の状態</param>
			/// <returns>次のaction</returns>
			 optional<actionType> chooseAction(stateType state)
			 {
				if (rndCount > 624)
				{
					rnd = mt19937(rndGen());
					rndCount = 0;
				}

				rndCount++;

				if (rnd() < mt19937::_Max * epsilon)
				{
					const unsigned int count = rnd() % numStates;
					actionType action{};
					for (unsigned int i = 0; i < count; i++)
					{
						action++;
					}
					return action;
				}

				Number Max = -10e9;
				vector<actionType> indexes;

				for (const auto& value : qTable[state])
				{
					if (!isActable(state, value.first)) continue;
					if (Max < value.second)
					{
						indexes = vector<actionType>();
						indexes.push_back(value.first);
						Max = value.second;
					}
					else if (Max == value.second)
					{
						indexes.push_back(value.first);
					}
				}

				rndCount++;

				if (rndCount > 624)
				{
					rnd = mt19937(rndGen());
					rndCount = 0;
				}

				if (indexes.size() == 0)
				{
					return optional<actionType>();
				}

				return indexes[rnd() % indexes.size()];
			}

			/// <summary>
			/// stateからそのactionが利用可能かを判定します。
			/// </summary>
			/// <param name="state">現在の状態</param>
			/// <param name="action">判定対象のaction</param>
			/// <returns>actionが利用可能かどうか</returns>
			virtual bool isActable(stateType state, actionType action)
			{
				return true;
			}

			/// <summary>
			/// 生成したアクションをもとにAIに学習させます。
			/// </summary>
			void learn(stateType state, actionType action, stateType nextState, Number reward)
			{
				Number maxNext = 0;
				for (unsigned int i = 0; i < numActions; i++) {
					if (isActable(nextState, i) && qTable[nextState][i] > maxNext) {
						maxNext = qTable[nextState][i];
					}
				}
				qTable[state][action] = qTable[state][action] + learningRate * (reward + discountFactor * maxNext - qTable[state][action]);

				return;
			}

			Number epsilon;

		private:
			tableType qTable;
			Number learningRate;
			Number discountFactor;
			unsigned int numActions;
			unsigned int numStates;
			unsigned int rndCount;
			mt19937 rnd;
			random_device rndGen;
		};
	}
}