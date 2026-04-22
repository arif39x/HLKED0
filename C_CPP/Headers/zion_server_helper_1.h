#pragma once

#include <ntddk.h>
#include <windef.h>
#include <memory.h>
#include "../zion_proxy_dll/zion_ssdt_hook.h"
#include "zion_ssdt_hook_impl.h"

#include "zion/server/zion_common.h"
#include "zion/zion_cmd_packet.h"

ULONG Safe_CopyMemFromUser(PVOID ptrBufferAddr,
						   ULONG size,
						   PVOID ptrDest);