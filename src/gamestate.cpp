#include "gamestate.h"

GameState::GameState(const QString& path)
    : _path(path)
{

}

const QString& GameState::getPath() const
{
    return _path;
}

