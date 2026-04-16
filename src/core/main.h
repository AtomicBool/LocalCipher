#pragma once

#include <string>
#include <cstdint>
#include <tchar.h>

// include windows.h without status
#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS

#include <ntstatus.h> 
