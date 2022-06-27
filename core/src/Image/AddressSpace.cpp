#include "Core/Image/AddressSpace.h"
#include "Core/Image/Image.h"
#include "Core/Context.h"

using namespace sda;

AddressSpace::AddressSpace(Context* context, Object::Id* id, const std::string& name)
    : ContextObject(context, id, name)
{
    m_context->getAddressSpaces()->add(std::unique_ptr<AddressSpace>(this));
}

void AddressSpace::setImages(const std::list<Image*>& images) {
    m_context->getCallbacks()->onObjectModified(this);
    m_images = images;
}

const std::list<Image*>& AddressSpace::getImages() const {
    return m_images;
}

Image* AddressSpace::getImageAt(std::uintptr_t address) const {
    for (auto image : m_images) {
        if (image->contains(address)) {
            return image;
        }
    }
    return nullptr;
}

void AddressSpace::serialize(boost::json::object& data) const {
    ContextObject::serialize(data);
    data["collection"] = Collection;

    // serialize the list of images
    auto imagesIds = boost::json::array();
    for (auto image : m_images)
        imagesIds.push_back(image->serializeId());
    data["images"] = imagesIds;
}

void AddressSpace::deserialize(boost::json::object& data) {
    ContextObject::deserialize(data);

    // deserialize the list of images
    m_images.clear();
    const auto& imagesIds = data["images"].get_array();
    for (const auto& imageId : imagesIds) {
        auto image = m_context->getImages()->get(imageId);
        m_images.push_back(image);
    }
}

void AddressSpace::destroy() {
    m_context->getAddressSpaces()->remove(this);
}