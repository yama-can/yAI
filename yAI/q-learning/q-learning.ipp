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
		/// Q-Learning��
		/// </summary>
		/// <typeparam name="stateType">State�̌^</typeparam>
		/// <typeparam name="actionType">Action�̌^</typeparam>
		template<Stateable stateType, Actionable actionType>
		class EnvBase
		{
		public:
			EnvBase()
			{
			}

			/// <summary>
			/// Action���玟��State���V�A�I���������ǂ������擾���܂��B
			/// </summary>
			/// <param name="action">Action</param>
			/// <returns>��V�Ȃǂ̃f�[�^</returns>
			virtual StepData<stateType> step(actionType action) = 0;
		};

		/// <summary>
		/// �G�[�W�F���g(Q-Learning���𑀍삷�邽�߂̂���)
		/// </summary>
		/// <typeparam name="stateType"></typeparam>
		/// <typeparam name="actionType"></typeparam>
		template<Stateable stateType, Actionable actionType>
		class AgentBase
		{
		public:
			typedef map<stateType, map<actionType, Number>> tableType;

			/// <summary>
			/// �G�[�W�F���g���쐬���܂��B
			/// </summary>
			/// <param name="numStates">state�̐�</param>
			/// <param name="numActions">action�̐�</param>
			/// <param name="epsilon">�T������m��</param>
			/// <param name="learningRate">�w�K��(���̒l�������Ɗw�K���ʂ��Œ艻���A�_��ȑΉ����ł��Ȃ��Ȃ�܂��B)</param>
			/// <param name="discountFactor">������(��V�̏d�v�x)</param>
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
			/// ���݊w�K���ꂽ�f�[�^����action��I�����܂��B
			/// </summary>
			/// <param name="state">���݂̏��</param>
			/// <returns>����action</returns>
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
			/// state���炻��action�����p�\���𔻒肵�܂��B
			/// </summary>
			/// <param name="state">���݂̏��</param>
			/// <param name="action">����Ώۂ�action</param>
			/// <returns>action�����p�\���ǂ���</returns>
			virtual bool isActable(stateType state, actionType action)
			{
				return true;
			}

			/// <summary>
			/// ���������A�N�V���������Ƃ�AI�Ɋw�K�����܂��B
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