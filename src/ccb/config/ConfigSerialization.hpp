#pragma once

#define CCB_WIDEN(x) L ## x
#define CCB_SERIALIZE(ar, field)  (ar).Serialize((field), CCB_WIDEN(#field))
#define CCB_SERIALIZE_DEFAULT(ar, field, def)  (ar).Serialize((field), CCB_WIDEN(#field), (def))

