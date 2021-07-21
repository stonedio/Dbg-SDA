#pragma once
#include "ExprTreeOperationalNode.h"

namespace CE::Decompiler::ExprTree
{
	// todo: inherit from OperationalNode to make its processing single through one interface
	class AbstractCondition : public Node, public PCode::IRelatedToInstruction
	{
	public:
		PCode::Instruction* m_instr;

		AbstractCondition(PCode::Instruction* instr = nullptr);

		virtual void inverse() = 0;

		int getSize() override;

		std::list<PCode::Instruction*> getInstructionsRelatedTo() override;
	};

	class BooleanValue : public AbstractCondition
	{
	public:
		bool m_value;

		BooleanValue(bool value, PCode::Instruction* instr = nullptr);

		void inverse() override;

		INode* clone(NodeCloneContext* ctx) override;

		HS getHash() override;
	};

	class Condition : public AbstractCondition, public INodeAgregator
	{
	public:
		enum ConditionType
		{
			None,
			Eq,
			Ne,
			Lt,
			Le,
			Gt,
			Ge
		};

		INode* m_leftNode;
		INode* m_rightNode;
		ConditionType m_cond;

		Condition(INode* leftNode, INode* rightNode, ConditionType cond, PCode::Instruction* instr = nullptr);

		~Condition();

		void replaceNode(INode* node, INode* newNode) override;

		std::list<INode*> getNodesList() override;

		INode* clone(NodeCloneContext* ctx) override;

		HS getHash() override;

		void inverse() override;
	};

	class CompositeCondition : public AbstractCondition, public INodeAgregator
	{
	public:
		enum CompositeConditionType
		{
			None,
			Not,
			And,
			Or
		};

		static std::string ShowConditionType(CompositeConditionType condType);

		AbstractCondition* m_leftCond;
		AbstractCondition* m_rightCond;
		CompositeConditionType m_cond;

		CompositeCondition(AbstractCondition* leftCond, AbstractCondition* rightCond = nullptr, CompositeConditionType cond = None, PCode::Instruction* instr = nullptr);

		~CompositeCondition() override;

		void replaceNode(INode* node, INode* newNode) override;

		std::list<INode*> getNodesList() override;

		INode* clone(NodeCloneContext* ctx) override;

		HS getHash() override;

		void inverse() override;
	};
};