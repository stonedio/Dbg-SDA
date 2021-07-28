#pragma once
#include "AbstractManagerController.h"
#include "managers/TypeManager.h"

namespace GUI
{
	static std::string GetGroupName(CE::DataType::IType* dataType) {
		std::string groupName = "Unknown";
		switch (dataType->getGroup()) {
		case CE::DataType::IType::Simple:
			groupName = "Simple";
			break;
		case CE::DataType::IType::Typedef:
			groupName = "Typedef";
			break;
		case CE::DataType::IType::Enum:
			groupName = "Enum";
			break;
		case CE::DataType::IType::Structure:
			groupName = "Structure";
			break;
		case CE::DataType::IType::Class:
			groupName = "Class";
			break;
		case CE::DataType::IType::FunctionSignature:
			groupName = "Function signature";
			break;
		}
		return groupName;
	}

	class EnumFieldListModel : public IListModel<int>
	{
		CE::DataType::Enum::FieldMapType* m_fields;
	public:
		class FieldIterator : public Iterator
		{
			CE::DataType::Enum::FieldMapType::iterator m_it;
			CE::DataType::Enum::FieldMapType* m_fields;
		public:
			FieldIterator(CE::DataType::Enum::FieldMapType* fields)
				: m_fields(fields), m_it(fields->begin())
			{}

			void getNextItem(std::string* text, int* data) override {
				const auto& [offset, name] = *m_it;
				*text = name + "," + std::to_string(offset);
				*data = m_it->first;
				++m_it;
			}

			bool hasNextItem() override {
				return m_it != m_fields->end();
			}
		};

		EnumFieldListModel(CE::DataType::Enum::FieldMapType* fields)
			: m_fields(fields)
		{}

		void newIterator(const IteratorCallback& callback) override
		{
			FieldIterator iterator(m_fields);
			callback(&iterator);
		}
	};

	class StructureFieldListModel : public IListModel<CE::DataType::IStructure::Field*>
	{
		CE::DataType::IStructure::FieldMapType* m_fields;
	public:
		class FieldIterator : public Iterator
		{
			CE::DataType::IStructure::FieldMapType::iterator m_it;
			CE::DataType::IStructure::FieldMapType* m_fields;
		public:
			FieldIterator(CE::DataType::IStructure::FieldMapType* fields)
				: m_fields(fields), m_it(fields->begin())
			{}

			void getNextItem(std::string* text, CE::DataType::IStructure::Field** data) override {
				const auto& [offset, field] = *m_it;
				*text = std::to_string(field->getOffset()) + "," + std::to_string(field->getBitOffset()) + "," + std::to_string(field->getSize()) + "," + field->getDataType()->getDisplayName() + "," + field->getName();
				*data = field;
				++m_it;
			}

			bool hasNextItem() override {
				return m_it != m_fields->end();
			}
		};

		StructureFieldListModel(CE::DataType::IStructure::FieldMapType* fields)
			: m_fields(fields)
		{}

		void newIterator(const IteratorCallback& callback) override
		{
			FieldIterator iterator(m_fields);
			callback(&iterator);
		}
	};

	// todo: move all such controllers into core (implement undo, redo operations)
	class UserDataTypeController
	{
		CE::DataType::IUserDefinedType* m_dataType;
	protected:
		bool m_changed = false;
	public:
		UserDataTypeController(CE::DataType::IUserDefinedType* dataType)
			: m_dataType(dataType)
		{}

		~UserDataTypeController() {
			if (m_changed) {
				m_dataType->getTypeManager()->getProject()->getTransaction()->markAsDirty(m_dataType);
			}
		}

		void rename(const std::string& name) {
			m_dataType->setName(name);
			m_changed = true;
		}
	};

	class StructureController : public UserDataTypeController
	{
		CE::DataType::IStructure* m_dataType;
		using Field = CE::DataType::IStructure::Field;
	public:
		StructureController(CE::DataType::IStructure* dataType)
			: UserDataTypeController(dataType), m_dataType(dataType)
		{}

		void addField(Field* field) {
			m_dataType->addField(field);
			m_changed = true;
		}

		void removeField(Field* field) {
			m_dataType->removeField(field);
			m_changed = true;
		}

		void removeAllFields() {
			m_dataType->removeAllFields();
			m_changed = true;
		}
	};

	class FunctionSingautreController : public UserDataTypeController
	{
		CE::DataType::IFunctionSignature* m_dataType;
	public:
		FunctionSingautreController(CE::DataType::IFunctionSignature* dataType)
			: UserDataTypeController(dataType), m_dataType(dataType)
		{}

		void addParameter(CE::Symbol::FuncParameterSymbol* symbol) {
			m_dataType->addParameter(symbol);
			m_changed = true;
		}

		void removeLastParameter() {
			m_dataType->removeLastParameter();
			m_changed = true;
		}

		void removeAllParameters() {
			m_dataType->deleteAllParameters();
			m_changed = true;
		}
	};
	
	class DataTypeManagerController : public AbstractManagerController<CE::DataType::IType, CE::TypeManager>
	{
	public:
		struct DataTypeFilter
		{
			std::string m_name;
		};

		class DataTypeListModel : public ListModel
		{
		public:
			bool m_isTable;
			DataTypeListModel(DataTypeManagerController* controller, bool isTable)
				: ListModel(controller), m_isTable(isTable)
			{}

		private:
			class DataTypeIterator : public Iterator
			{
			public:
				using Iterator::Iterator;

			private:
				std::string getText(CE::DataType::IType* item) override
				{
					if (dynamic_cast<DataTypeListModel*>(m_listModel)->m_isTable)
						return item->getName() + "," + GetGroupName(item);
					return item->getName();
				}
			};

			void newIterator(const IteratorCallback& callback) override
			{
				DataTypeIterator iterator(this);
				callback(&iterator);
			}
		};

		DataTypeFilter m_filter;
		DataTypeListModel m_listModel;
		DataTypeListModel m_tableListModel;

		DataTypeManagerController(CE::TypeManager* manager)
			: AbstractManagerController<CE::DataType::IType, CE::TypeManager>(manager), m_listModel(this, false), m_tableListModel(this, true)
		{}

		CE::DataTypePtr parseDataType(const std::string& text) const {
			std::string typeName;
			std::string typePtrLevels;
			bool isTypeName = true;
			for (auto c : text) {
				if (c == '*' || c == '[')
					isTypeName = false;
				if (isTypeName) {
					typeName.push_back(c);
				}
				else {
					typePtrLevels.push_back(c);
				}
			}
			if (const auto dataType = m_manager->findTypeByName(typeName))
				return GetUnit(dataType, typePtrLevels);
			return nullptr;
		}

	private:
		bool filter(CE::DataType::IType* item) override
		{
			using namespace Helper::String;
			if (!m_filter.m_name.empty() && ToLower(item->getName()).find(ToLower(m_filter.m_name)) == std::string::npos)
				return false;
			return true;
		}
	};
};
