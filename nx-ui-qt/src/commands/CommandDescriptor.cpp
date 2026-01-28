#include "CommandDescriptor.h"

std::vector<std::string> getCommandArgs(CommandId id) {
    switch (id) {
        case CommandId::Version:
            return {"nx", "--version"};
        case CommandId::Help:
            return {"nx", "--help"};
    }
    return {};
}