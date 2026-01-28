#pragma once

enum class CommandId {
    Version,
    Help
};

class Command {
public:
    explicit Command(CommandId id);
    CommandId getId() const;
    
private:
    CommandId m_id;
};

Command getCommand(CommandId id);