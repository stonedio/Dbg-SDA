#pragma once
#include "Core/DataType/DataType.h"

namespace sda
{
    class TypedefDataType : public DataType
    {
        DataType* m_pointedType;
    public:
        static inline const std::string Type = "typedef";

        TypedefDataType(
            Context* context,
            Object::Id* id = nullptr,
            const std::string& name = "",
            DataType* pointedType = nullptr);

        void setPointedType(DataType* pointedType);

        DataType* getPointedType() const;

        size_t getSize() const override;

        void serialize(boost::json::object& data) const override;

        void deserialize(boost::json::object& data) override;
    };
};