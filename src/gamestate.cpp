#include "gamestate.h"

GameState::GameState(const QString& filePath)
    : _filePath(filePath)
{
}

const QString& GameState::getFilePath() const
{
    return _filePath;
}

