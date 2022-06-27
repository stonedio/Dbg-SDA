#include "Core/Symbol/StructureFieldSymbol.h"

using namespace sda;

StructureFieldSymbol::StructureFieldSymbol(
    Context* context,
    Object::Id* id,
    const std::string& name,
    DataType* dataType)
    : Symbol(context, id, name, dataType)
{}

void StructureFieldSymbol::serialize(boost::json::object& data) const {
    Symbol::serialize(data);
    data["type"] = Type;
}

void StructureFieldSymbol::deserialize(boost::json::object& data) {
    Symbol::deserialize(data);
}