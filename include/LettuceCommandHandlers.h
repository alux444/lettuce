#pragma once
#include <string>
#include <vector>
#include "LettuceDatabase.h"

std::string handlePing(const std::vector<std::string>&, LettuceDatabase&);
std::string handleEcho(const std::vector<std::string>&, LettuceDatabase&);
std::string handleFlushAll(const std::vector<std::string>&, LettuceDatabase&);
std::string handleSet(const std::vector<std::string>&, LettuceDatabase&);
std::string handleGet(const std::vector<std::string>&, LettuceDatabase&);
std::string handleKeys(const std::vector<std::string>&, LettuceDatabase&);
std::string handleType(const std::vector<std::string>&, LettuceDatabase&);
std::string handleDel(const std::vector<std::string>&, LettuceDatabase&);
std::string handleExpire(const std::vector<std::string>&, LettuceDatabase&);
std::string handleRename(const std::vector<std::string>&, LettuceDatabase&);

std::string handleLlen(const std::vector<std::string>&, LettuceDatabase&);
std::string handleLpush(const std::vector<std::string>&, LettuceDatabase&);
std::string handleRpush(const std::vector<std::string>&, LettuceDatabase&);
std::string handleLpop(std::vector<std::string>&, LettuceDatabase&);
std::string handleRpop(std::vector<std::string>&, LettuceDatabase&);
std::string handleLrem(const std::vector<std::string>&, LettuceDatabase&);
std::string handleLindex(const std::vector<std::string>&, LettuceDatabase&);
std::string handleLset(const std::vector<std::string>&, LettuceDatabase&);