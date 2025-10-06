#include "../values.hpp"
#include "../../../context/positions.hpp"
#include <string>
#include <unordered_map>

namespace interpreter {
    Type::Type(const context::Context ctx, const std::string type, const std::unordered_map<std::string, bool> inheritence) {
        this->ctx = ctx;
        this->data_type = "type";
        this->type = type;
        this->inheritence = inheritence;
    }
}