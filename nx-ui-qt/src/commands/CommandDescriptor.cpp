#include "CommandDescriptor.h"
#include <vector>
#include <string>

Command::Command(CommandId id) : m_id(id) {
}

CommandId Command::getId() const {
    return m_id;
}

Command getCommand(CommandId id) {
    return Command(id);
}

std::vector<std::string> resolveCommandArgs(CommandId id) {
    switch (id) {
        case CommandId::Version:
            return {"nx", "--version"};
        case CommandId::Help:
            return {"nx", "--help"};
    }
    return {};
}