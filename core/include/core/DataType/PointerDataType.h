#pragma once
#include "Core/DataType/DataType.h"

namespace sda
{
    class PointerDataType : public DataType
    {
        DataType* m_pointedType;
    public:
        static inline const std::string Type = "pointer";

        PointerDataType(
            Context* context,
            Object::Id* id = nullptr,
            DataType* pointedType = nullptr);

        DataType* getPointedType() const;

        size_t getSize() const override;

        void serialize(boost::json::object& data) const override;

        void deserialize(boost::json::object& data) override;
    };
};