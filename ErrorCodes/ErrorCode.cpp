//
// Created by maikeu on 17/07/2020.
//

#include "ErrorCode.h"

const ErrorCodeItem ErrorCodeItem::Invalid{255, "Invalid"};
std::list<ErrorCodeItem> ErrorCode::items{};